#include <bootloader/bool_macro.h>
#include <bootloader/string.h>
#include <bootloader/err.h>
#include <bootloader/file.h>
#include <bootloader/video.h>
#include <bootloader/acpi.h>
#include <bootloader/loadkernel.h>
#include <bootloader/memorymap.h>
#include <bootloader/debug_print.h>
#include <bootloader/pause.h>
#include <bootloader/exitbootservices.h>
#include <bootloader/exitloader.h>
#include <preboot/kheader.h>


EFI_API EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS status = EFI_SUCCESS;
    // array of efi Protocols
    uint64_t    handlecount;
    EFI_HANDLE* handlebuf;

    uint64_t virtualOffset;

    // filesystem handles
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesys        = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*)NULL;
    EFI_FILE_HANDLE                  rootdirhandle  = (EFI_FILE_HANDLE)NULL;
    EFI_FILE_HANDLE                  kernelhandle   = (EFI_FILE_HANDLE)NULL;
    EFI_FILE_HANDLE                  tmphandle      = (EFI_FILE_HANDLE)NULL;
    kernel_header_t                  initialKernelHdr;
    loadedElf64Program               kernelConfigInfo;
    RSDPdescriptor20*                rsdpaddr       = (RSDPdescriptor20*)NULL;


    InitializeLib(ImageHandle, SystemTable);


    /*
        Required to debug the Bootloader
    */
    ondebug({
        EFI_GUID                   loadedImageProtocolGUID = EFI_LOADED_IMAGE_PROTOCOL_GUID;
        EFI_LOADED_IMAGE_PROTOCOL* loadedImageInfo         = (EFI_LOADED_IMAGE_PROTOCOL*)NULL;
        status = SystemTable->BootServices->HandleProtocol(
            ImageHandle, 
            &loadedImageProtocolGUID, 
            (void **)&loadedImageInfo
        );
        CHECKERRFMT_NOEXIT(status, L"Couldn't Retrieve LoadedImage Protocol");
        debug_printb(L"    [DEBUG] UEFI Bootloader Base Address: %p\n\r", loadedImageInfo->ImageBase);


        // Write image base and marker for GDB
        /* This shit surprisingly works... no vscode extensions for now... */
        volatile uint64_t* marker_ptr     = (uint64_t *)0x10000;
        volatile uint64_t* image_base_ptr = (uint64_t *)0x10008;
        *image_base_ptr = (uint64_t)loadedImageInfo->ImageBase;
        *marker_ptr = 0xDEADBEEF;
        debug_printb(L"Hello, world!\n");
    })


    status = uefi_call_wrapper(ST->ConOut->Reset, 2, 
        ST->ConOut, FALSE
    ); 
    status = uefi_call_wrapper(ST->ConOut->ClearScreen, 1, 
        ST->ConOut
    );
    status = uefi_call_wrapper(ST->ConOut->SetCursorPosition, 1, 
        ST->ConOut, 
        0, 
        0
    );
    // SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_TEXT_ATTR(EFI_YELLOW, EFI_BLUE));
    CHECKERRFMT(status, L"Could not Reset COUT\n\t", getErrorStr(status));

    uefi_call_wrapper(gBS->SetWatchdogTimer, 4, 0, 0, 0, (CHAR16*)NULL); // disable the firmware Watchdog timer

    debug_printb(L"Querying Currently Available Text Modes:\n\r");
    for(int32_t i = 0; i < SystemTable->ConOut->Mode->MaxMode; ++i)
    {
        UINTN max_cols = 0;
        UINTN max_rows = 0;
        uefi_call_wrapper(ST->ConOut->QueryMode, 4, 
            SystemTable->ConOut, 
            SystemTable->ConOut->Mode->Mode, 
            &max_cols, 
            &max_rows
        );

        debug_printb(L"id=%u -> Attrib %x | Cursor [visible ?=%u] <Row, Col> = <%u, %u> | <RowMax, ColMax> = <%d, %d>\n\r",
            SystemTable->ConOut->Mode->Mode,
            SystemTable->ConOut->Mode->Attribute,
            SystemTable->ConOut->Mode->CursorVisible,
            SystemTable->ConOut->Mode->CursorRow,
            SystemTable->ConOut->Mode->CursorColumn,
            max_rows,
            max_cols
        );
    }



    /* Actual Relevant Bootloader starts here */
    /* Search through all available volumes for the kernel elf */
    status = uefi_call_wrapper(gBS->LocateHandleBuffer, 
        5,
        ByProtocol, 
        &gEfiSimpleFileSystemProtocolGuid, 
        NULL, 
        &handlecount, 
        &handlebuf
    );
	CHECKERR(status)
    for (uint64_t i = 0; i < handlecount; ++i)
	{
		// retrieve file system Protocol for current handle
		status = uefi_call_wrapper(gBS->OpenProtocol, 
            6,
			handlebuf[i],
			&gEfiSimpleFileSystemProtocolGuid,
			(VOID**)&filesys,
			ImageHandle,
			NULL,
			EFI_OPEN_PROTOCOL_GET_PROTOCOL
		);
		CHECKERR(status)

        uefi_call_wrapper(filesys->OpenVolume, 2, filesys, &rootdirhandle);
        tmphandle    = findFile(rootdirhandle, (const CHAR16*)L"kernel.elf"); // find the file we need.
        kernelhandle = (tmphandle == NULL) ? kernelhandle : tmphandle;
        rootdirhandle->Close(rootdirhandle);
    }
    CHECKERRFMT(!blboolean(kernelhandle), L"Couldn't find the kernel to be loaded")


    initialize_screenbuffer(0, 0, &initialKernelHdr.screen);
    get_verify_acpi_rsdp(&rsdpaddr);

    BOOLEAN rsdpmodern = (rsdpaddr->first.revision == 2);
    initialKernelHdr.acpi = (ACPIinitialConfig){
        .address   = rsdpmodern ? rsdpaddr->xsdtAddress : rsdpaddr->first.rsdtAddress,
        .extended  = rsdpmodern,
        .padding   = {0},
        .procCount = get_core_count()
    };


    // load the kernel executable sections into memory, and get a pointer to the entry.
    // we also have a bunch of parameters to initialize,
    // as all required memory is allocated in a single contiguous block
    __kernel_entry __attribute__((unused)) sysvabi_func = (__kernel_entry)loadKernel(
        kernelhandle, 
        initialKernelHdr.acpi.procCount, 
        &kernelConfigInfo
    );
    (void)(sysvabi_func);
    debug_printb(L"\
Output From loadElf64:\n\r\
    kpstart, kvstart: %p, %p (%X Bytes)\n\r\
    kstackp: %p (%X Bytes)\n\r\
    percpuRBP   : %p\n\r\
    percpuTSS   : %p\n\r\
    GDT         : %p\n\r\
    hHalf Header: %p\n\r\
    voffset     : %p\n\r\
    \n",
        kernelConfigInfo.startp, kernelConfigInfo.startv, kernelConfigInfo.loadSize,
        kernelConfigInfo.stackSection, kernelConfigInfo.stackSectionSize,
        kernelConfigInfo.percpuStackOffset,
        kernelConfigInfo.percpuTSSOffset,
        kernelConfigInfo.GDTOffset,
        kernelConfigInfo.postbootServicesHeader,
        kernelConfigInfo.startv - kernelConfigInfo.startp
    );


    /* Modify Memconfig to include more data from loadedElf64Program */
    debug_printb(L"initialKernelHdr.memcfg @%p\n\r", &initialKernelHdr.memcfg);
    virtualOffset = kernelConfigInfo.startv - kernelConfigInfo.startp;
    memsetzerovoid(&initialKernelHdr.memcfg, sizeof(kernel_initial_memory_config));
    // initialKernelHdr.memcfg = (kernel_initial_memory_config){
    //     .kstartp                  = kernelConfigInfo.startp,
    //     .kstartv                  = kernelConfigInfo.startv,
    //     .krbptopv                 = kernelConfigInfo.stackSection + kernelConfigInfo.stackSectionSize,
    //     .kpercpu_rbpv             = VIRTUAL kernelConfigInfo.percpuStackOffset + virtualOffset,
    //     .kpercpu_tssv             = VIRTUAL kernelConfigInfo.percpuTSSOffset   + virtualOffset,
    //     .kgdtv                    = VIRTUAL kernelConfigInfo.GDTOffset         + virtualOffset,
    //     .efi_rtp                  = (physical_address)SystemTable->RuntimeServices,
    //     .pml4                     = 0,
    //     .bl_ripv                  = 0,
    //     .bl_rip_pagetable_buf     = 0,
    //     .krbpsize                 = (uint32_t)kernelConfigInfo.stackSectionSize,
    //     .pml4size                 = 0,
    //     .bl_rip_pagetable_bufsize = 0,
    //     .reserved1                = 0,
    //     .kendoffset               = (uint32_t)kernelConfigInfo.endOffset
    // };
    /* This worked on 16:48 PM @ 13/7/2025 */
    initialKernelHdr.memcfg.kstartp                  = kernelConfigInfo.startp;
    initialKernelHdr.memcfg.kstartv                  = kernelConfigInfo.startv;
    initialKernelHdr.memcfg.krbptopv                 = kernelConfigInfo.stackSection + kernelConfigInfo.stackSectionSize;
    initialKernelHdr.memcfg.kpercpu_rbpv             = VIRTUAL kernelConfigInfo.percpuStackOffset + virtualOffset;
    initialKernelHdr.memcfg.kpercpu_tssv             = VIRTUAL kernelConfigInfo.percpuTSSOffset   + virtualOffset;
    initialKernelHdr.memcfg.kgdtv                    = VIRTUAL kernelConfigInfo.GDTOffset         + virtualOffset;
    initialKernelHdr.memcfg.efi_rtp                  = (physical_address)SystemTable->RuntimeServices;
    initialKernelHdr.memcfg.pml4                     = 0;
    initialKernelHdr.memcfg.bl_ripv                  = 0;
    initialKernelHdr.memcfg.bl_rip_pagetable_buf     = 0;
    initialKernelHdr.memcfg.krbpsize                 = (uint32_t)kernelConfigInfo.stackSectionSize;
    initialKernelHdr.memcfg.pml4size                 = 0;
    initialKernelHdr.memcfg.bl_rip_pagetable_bufsize = 0;
    initialKernelHdr.memcfg.reserved1                = 0;
    initialKernelHdr.memcfg.kendoffset               = (uint32_t)kernelConfigInfo.endOffset;
    initialKernelHdr.acpi.address += virtualOffset;
    initialKernelHdr.screen.start = (uint8_t*)(initialKernelHdr.screen.start) + virtualOffset;
    /* 
        Data left to modify/init in the kernel_header_t: 
        INIT initialKernelHdr->map
        INIT initialKernelHdr->memcfg.pml4
        INIT initialKernelHdr->memcfg.pml4size
        INIT initialKernelHdr->memcfg.identity_mapped_bootloader_ripv
        INIT initialKernelHdr->memcfg.identity_mapped_bootloader_ripp
        INIT initialKernelHdr->memcfg.identity_mapped_bootloader_ripsize
        [MAP_BUF_VIRTUAL] initialKernelHeader.map
        [MAP_BUF_VIRTUAL] initialKernelHeader.screen.start
    */
    UINTN memoryMapKey = initializeKernelMemoryMap(&initialKernelHdr.map);

    /* Map the kernel to the desired higher half address; Switch Page Directories, and jump to the kernel entry. */
    enablePagingJumpToEntry(
        &initialKernelHdr,
        (kernel_header_t*)kernelConfigInfo.postbootServicesHeader,
        sysvabi_func,
        kernelConfigInfo.startp,
        kernelConfigInfo.startv,
        ImageHandle,
        memoryMapKey
    );


    return status;
}