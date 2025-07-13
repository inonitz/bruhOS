#ifndef __BOOTLOADER_C_DEFINITION_ELF_UTILITIES_HEADER__
#define __BOOTLOADER_C_DEFINITION_ELF_UTILITIES_HEADER__
#include <bootloader/elf.h>
#include <bootloader/api.h>


EFI_API BOOLEAN verifyElfHeader(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfSectionHeaderTableEntrySize(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfSectionHeaderTableEntryCount(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfSectionHeaderTableSizeBytes(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfProgramHeaderTableEntrySize(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfProgramHeaderTableEntryCount(Elf64_Ehdr* header);
EFI_API Elf64_Xword elfProgramHeaderTableSizeBytes(Elf64_Ehdr* header);

EFI_API void elfSectionHeaderTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    OUT Elf64_Shdr**   outHeaderTable
);
EFI_API void elfSectionHeaderStringTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    IN Elf64_Shdr*     sectionHeaderTableBegin,
    OUT char**         stringTable
);
EFI_API void elfProgramHeaderTable(
    IN EFI_FILE_HANDLE file,
    IN Elf64_Ehdr*     header,
    OUT Elf64_Phdr**   outHeaderTable
);


#endif /* __BOOTLOADER_C_DEFINITION_ELF_UTILITIES_HEADER__ */
