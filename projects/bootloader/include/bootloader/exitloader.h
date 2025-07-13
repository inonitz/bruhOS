#ifndef __BOOTLOADER_C_DEFINITION_EXIT_BOOTLOADER__
#define __BOOTLOADER_C_DEFINITION_EXIT_BOOTLOADER__
#include <bootloader/api.h>
#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <preboot/kheader.h>
#undef __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__


/* 
    This function does a lot of things:
    1. Paging:
        creates a PML4 Table and maps the whole address space,
        using on the virtual start of the kernel (that was retrieved from loadElf64)
        NOTE: This creates a temporary identity mapping, which needs to be removed when inside the Elf File entry point.
    
    2. Identity maps the functions' stack and paging tables.
    3. Switches to the new PML4.
    4. Jumps to the Kernel Entry Point.
    
    oldHeader - the initially configured header that is in boot-services physical memory 
    newHeader - the updated header that'll be mapped to virt-mem & used by the kernel.
    entry     - main func of the kernel
    physStart - the physical start of the Kernel Elf64 File. (Where it was loaded in memory)
    virtStart - the virtual  start of the Kernel Elf64 File. (The vaddr of the first PT_LOAD Section) 
*/
/* 
    * This function does a lot of things, lets break it down:
    * 1. Paging:
    *       creates a PML4 Table and maps the whole address space,
    *       using on the virtual start of the kernel (that was retrieved from loadElf64)
    *       NOTE: This creates a temporary identity mapping, which needs to be removed when inside the Elf File entry point.
    * 
    * 2. Identity maps the functions' stack and paging tables.
    * 
    * 3. Switches to the new PML4.
    * 
    * 4. Jumps to the Kernel Entry Point.
    * 
    * kernel_header_t* header    - a pointer to where the kernel header resides in the previous stack. We'll supply the kernel with the info in this header.
    * kernel_header_t* hHalfHdr  - a pointer to where the kernel header that will be read by the kernel, reside.
    * __kernel_entry   entry     - the pointer to the starting address of the kernel (in physical memory)
    * uint64_t         physStart - the physical start of the Elf64 File. (Where it was loaded in memory)
    * uint64_t         virtStart - the virtual  start of the Elf64 File. (The vaddr of the first PT_LOAD Section) 
*/
EFI_API void enablePagingJumpToEntry(
    kernel_header_t* oldHeader,
    kernel_header_t* newHeader,
    __kernel_entry   entry, 
    uint64_t         physStart, 
    uint64_t         virtStart,
    EFI_HANDLE       ImageHandle,
    UINTN            memoryMapKey
);


#endif /* __BOOTLOADER_C_DEFINITION_EXIT_BOOTLOADER__ */
