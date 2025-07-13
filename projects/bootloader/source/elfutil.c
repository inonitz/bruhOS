#include <bootloader/elfutil.h>
#include <bootloader/err.h>
#include <bootloader/debug_print.h>
#include <bootloader/string.h>
#include <bootloader/bool_macro.h>
#include <bootloader/file.h>
#include <bootloader/alloc.h>


BOOLEAN verifyElfHeader(Elf64_Ehdr* header) {
    unsigned char magic[4] = { 0x7f, 'E', 'L', 'F' };
    uint64_t cmp = (memcmp(header->e_ident, magic, 4UL));

    debug_printb(L"Header Debug Info: \n\r\
    MAGIC       == { 0x7f, 'ELF' }: %u\n\r\
    ei_class    == ELFCLASS64     : %u\n\r\
    ei_data     == ELFDATA2LSB    : %u\n\r\
    ei_version  == EV_CURRENT     : %u\n\r\
    e_type      == ET_EXEC        : %u\n\r\
    e_machine   == EM_X86_64      : %u\n\r\
    e_version   == EV_CURRENT     : %u\n\r",
        cmp,
        header->e_ident[EI_CLASS]   == ELFCLASS64 ,
        header->e_ident[EI_DATA]    == ELFDATA2LSB,
        header->e_ident[EI_VERSION] == EV_CURRENT,
        header->e_type              == ET_EXEC,
        header->e_machine 		  	== EM_X86_64,
        header->e_version 		  	== EV_CURRENT
    );
    return !blboolean(cmp) &&
		header->e_ident[EI_CLASS]   == ELFCLASS64  &&
		header->e_ident[EI_DATA]    == ELFDATA2LSB &&
		header->e_ident[EI_VERSION] == EV_CURRENT  &&
		header->e_type 			    == ET_EXEC     &&
		header->e_machine 			== EM_X86_64   &&
		header->e_version 			== EV_CURRENT;
}


void elfSectionHeaderTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    OUT Elf64_Shdr**   outHeaderTable
) {
    Elf64_Xword offset = header->e_shoff;
    Elf64_Xword sectionHeaderTableSize = elfSectionHeaderTableSizeBytes(header);
    Elf64_Xword actuallyRead = 0;
    Elf64_Shdr* sectionHeaderTable = (Elf64_Shdr*)allocatePool(sectionHeaderTableSize);


    if(sectionHeaderTable == NULL) {
        debug_printb(L"    elfSectionHEaderTable() -> Did not read table - allocation failed");
        *outHeaderTable = (Elf64_Shdr*)NULL;
        return;
    }


    /* Read the whole section-header table to */
    readFile(
        file,
        offset,
        sectionHeaderTableSize,
        (uint8_t*)sectionHeaderTable,
        &actuallyRead
    );
    CHECKERRFMT_NOEXIT(actuallyRead != sectionHeaderTableSize ? -1 : 1, 
        L"elfSectionHeaderTable() -> Couldn't read Elf Section-Header"
    );
    *outHeaderTable = (actuallyRead == sectionHeaderTableSize) ? sectionHeaderTable : (Elf64_Shdr*)NULL;
    return;
}


Elf64_Xword elfSectionHeaderTableEntrySize(Elf64_Ehdr* header)
{
    return header->e_shentsize;
}

Elf64_Xword elfSectionHeaderTableEntryCount(Elf64_Ehdr* header)
{
    return header->e_shnum;
}

Elf64_Xword elfSectionHeaderTableSizeBytes(Elf64_Ehdr* header)
{
    return (Elf64_Xword)header->e_shentsize * header->e_shnum;
}

Elf64_Xword elfProgramHeaderTableEntrySize(Elf64_Ehdr* header)
{
    return header->e_phentsize;
}

Elf64_Xword elfProgramHeaderTableEntryCount(Elf64_Ehdr* header)
{
    return header->e_phnum;
}

Elf64_Xword elfProgramHeaderTableSizeBytes(Elf64_Ehdr* header)
{
    return (Elf64_Xword)header->e_phentsize * header->e_phnum;
}


void elfSectionHeaderStringTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    IN Elf64_Shdr*     sectionHeaderTableBegin,
    OUT char**         stringTable
) {
    Elf64_Shdr* strtableheader = &sectionHeaderTableBegin[header->e_shstrndx];
    Elf64_Xword offset         = strtableheader->sh_offset;
    Elf64_Xword bytestoread    = strtableheader->sh_size;
    Elf64_Xword actuallyRead   = 0;
    char*       strtable       = (char*)allocatePool(strtableheader->sh_size);


    if(header->e_shstrndx == SHN_UNDEF || strtable == NULL) {
        *stringTable = (char*)NULL;
        return;
    }


    /* Read the whole string table to a single buffer, return the buffer in 'stringTable' */
    readFile(
        file,
        offset,
        bytestoread,
        (uint8_t*)strtable,
        &actuallyRead
    );
    CHECKERRFMT_NOEXIT(actuallyRead != bytestoread ? -1 : 1, 
        L"elfStringTable() -> Couldn't Read Elf String-Table"
    );
    *stringTable = strtable;
    return;
}


void elfProgramHeaderTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    OUT Elf64_Phdr**   outHeaderTable
) {
    Elf64_Xword offset = header->e_phoff;
    Elf64_Xword programHeaderTableSize = elfProgramHeaderTableSizeBytes(header);
    Elf64_Xword actuallyRead = 0;
    Elf64_Phdr* programHeaderTable = (Elf64_Phdr*)allocatePool(programHeaderTableSize);


    if(programHeaderTable == NULL) {
        debug_printb(L"    elfProgramHeaderTable() -> Did not read table - allocation failed");
        *outHeaderTable = (Elf64_Phdr*)NULL;
        return;
    }


    /* Read the whole section-header table to */
    readFile(
        file,
        offset,
        programHeaderTableSize,
        (uint8_t*)programHeaderTable,
        &actuallyRead
    );
    CHECKERRFMT_NOEXIT(actuallyRead != programHeaderTableSize ? -1 : 1, 
        L"elfSectionHeaderTable() -> Couldn't read Elf Section-Header"
    );
    *outHeaderTable = (actuallyRead == programHeaderTableSize) ? programHeaderTable : (Elf64_Phdr*)NULL;
    return;
}
