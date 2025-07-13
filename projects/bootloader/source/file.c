#include "efibind.h"
#include <bootloader/file.h>

#include <bootloader/pause.h>
#include <bootloader/err.h>
#include <bootloader/alloc.h>
#include <bootloader/string.h>

#include <bootloader/bool_macro.h>
#include <bootloader/debug_print.h>


static BOOLEAN fileSeek(
    IN EFI_FILE_HANDLE hdl,
    IN UINTN           byte_offset
);

static UINTN getFileSize(
    EFI_FILE_HANDLE hdl
);


EFI_FILE_HANDLE findFile(
    IN EFI_FILE_HANDLE Directory,
    IN const CHAR16*   filename
) {
    EFI_STATUS      status   = EFI_SUCCESS;
    EFI_FILE_HANDLE out      = (EFI_FILE_HANDLE)NULL;
    if (Directory == NULL)
    {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSys = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*)NULL;

        status = uefi_call_wrapper(gBS->LocateProtocol, 3,
            &gEfiSimpleFileSystemProtocolGuid, 
            NULL, 
            (VOID**)&FileSys
        );
        CHECKERRFMT(status, L"Couldn't find protocols for a file system\r\n");
        status = FileSys->OpenVolume(FileSys, &Directory);
        CHECKERRFMT(status, L"Couldn't open file system\r\n");
    }


    searchDirectoryRecursive(Directory, filename, &out);
    return out;
}


void searchDirectoryRecursive(
    IN  EFI_FILE_HANDLE  current_dir, 
    IN  const CHAR16*    filename, 
    OUT EFI_FILE_HANDLE* out
) {
    EFI_STATUS     status = EFI_SUCCESS;
    EFI_FILE_INFO* fileinf;
    uint64_t       fileinfsize = 1;


    while (fileinfsize != 0)
    {
        fileinfsize = 1;                                               // reset fileinfsize to retrieve the next file/dir info.
        status = current_dir->Read(current_dir, &fileinfsize, NULL);   // get size of info; if current_dir is directory: reading returns EFI_FILE_INFO to first file.
        if (fileinfsize == 0) { // fileinfsize == 0 => reache end of directory. 
            continue;
        }


        fileinf = (EFI_FILE_INFO*)allocatePool(fileinfsize);            // allocate buffer for info
        status = current_dir->Read(current_dir, &fileinfsize, fileinf); // get info in fileinf
        ondebug(
            printf((const CHAR16*)L"%s %u %u\r\n", 
                fileinf->FileName, 
                fileinfsize, 
                !!(fileinf->Attribute & EFI_FILE_DIRECTORY)
            );
            pause(125000);
        );


        if (strcmp16(fileinf->FileName, L".") == 0 || strcmp16(fileinf->FileName, L"..") == 0) {
            // . = current_dir, .. = parent_dir. we don't need to search them.
            continue;
        }
        if (!blboolean(fileinf->Attribute & EFI_FILE_DIRECTORY))
        {
            // if we found our file, return a handle to it.
            if (strcmp16(fileinf->FileName, filename) == 0)
            {
                printf((const CHAR16*)L"Found file %s! returning...\r\n", filename);
                status = current_dir->Open(current_dir, out, fileinf->FileName, EFI_FILE_MODE_READ, 0);
                CHECKERRFMT(status, L"\n Couldn't open handle to desired file!\r\n")
                freePool(fileinf);
                return;
            }
            freePool(fileinf);
            continue; // else continue to the next file
        }
        else
        {
            // search in new directory.
            EFI_FILE_HANDLE newDir;
            status = current_dir->Open(current_dir, &newDir, fileinf->FileName, EFI_FILE_MODE_READ, 0);
            CHECKERRFMT(status, L"Couldn't open directory %s\r\n", fileinf->FileName)

            status = newDir->SetPosition(newDir, 0);
            searchDirectoryRecursive(newDir, filename, out);

            status = current_dir->Close(newDir);
            CHECKERRFMT(status, L"Couldn't close file %s\r\n", fileinf->FileName)
            freePool(fileinf);
        }
    }
}



void readFile(
    IN EFI_FILE_HANDLE hdl,
    IN uint64_t        byte_offset,
    IN uint64_t        byte_count,
    OUT uint8_t*       byte_buffer,
    OUT uint64_t*      bytes_actually_read
) {
    const UINTN fileSize = getFileSize(hdl);
    EFI_STATUS status[2] = { EFI_SUCCESS, EFI_SUCCESS };
    UINTN bytesRead = 0;

    if( ( byte_offset + byte_count > fileSize ) ) { 
        printf((const CHAR16*)L"    readFile() -> Tried to read %X Bytes from offset %X. File Size is only %X Bytes\n\r",
            byte_count,
            byte_offset,
            fileSize
        );
        *bytes_actually_read = 0;
        return;
    }
    if(byte_buffer == NULL || byte_count == 0) {
        printf((const CHAR16*)L"    readFile() -> Received Invalid Arguments 'byte_buffer' = %p == NULL || 'byte_count' = %X == 0\n\r",
            byte_buffer,
            byte_count
        );
    }


    fileSeek(hdl, byte_offset);
    debug_printb(L"readFile() -> Seek to position %X\n\r", byte_offset);
    

    bytesRead = byte_count;
    status[1] = hdl->Read(hdl, &bytesRead, byte_buffer);
    CHECKERRFMT_NOEXIT(status[1], L"readFile() -> Couldn't Read %X Bytes | Managed to read only %X Bytes",
        byte_count, 
        bytesRead
    )
    debug_printb(L"readFile() -> Read %X Bytes\n\r", bytesRead);

    /* Check if Read was successful */
    CHECKERRFMT_NOEXIT(byte_count != bytesRead, 
        L"    readFile() -> Managed to read only %X/%X Bytes Requested ", 
        bytesRead,
        byte_count
    )

    /* Reset File Offset After Read Operation */
    fileSeek(hdl, 0);
    *bytes_actually_read = bytesRead;
    return;
}


static BOOLEAN fileSeek(
    IN EFI_FILE_HANDLE hdl,
    IN UINTN           byte_offset
) {
    EFI_STATUS status = EFI_SUCCESS;
    UINTN originalPosition = 0;

    status = hdl->GetPosition(hdl, &originalPosition);
    CHECKERRFMT_NOEXIT(status, L"         fileSeek() -> Couldn't Get Original File Seek-Position ")
    originalPosition = EFI_ERROR(status) ? 0 : originalPosition;

    status = hdl->SetPosition(hdl, byte_offset);
    CHECKERRFMT_NOEXIT(status, L"         fileSeek() -> Couldn't Set File Offset to 0x%X ",
        byte_offset
    )
    if(EFI_ERROR(status)) { /* If there was an error we go back to the original offset */
        status = hdl->SetPosition(hdl, originalPosition);
        CHECKERRFMT_NOEXIT(status, L"         fileSeek() -> Couldn't Reset File Offset to 0 ")
        return FALSE;
    }


    return TRUE;
}


static UINTN getFileSize(
    EFI_FILE_HANDLE hdl
) {
    EFI_STATUS status[2] = { EFI_SUCCESS, EFI_SUCCESS };
    UINTN fileEnd   = 0;
    UINTN fileBegin = 0;


    /* goto end of file: https://uefi.org/specs/UEFI/2.10/13_Protocols_Media_Access.html#id34 */
    status[0] = hdl->SetPosition(hdl, 0xFFFFFFFFFFFFFFFF);
    status[1] = hdl->GetPosition(hdl, &fileEnd);
    CHECKERRFMT(status[0] | status[1], L"     readFile() -> Couldn't Seek to EOF\n");
    status[0] = hdl->SetPosition(hdl, 0);
    status[1] = hdl->GetPosition(hdl, &fileBegin);
    CHECKERRFMT(status[0] | status[1], L"     readFile() -> Couldn't Seek to Beginning Of File\n");

    return fileEnd - fileBegin;
}