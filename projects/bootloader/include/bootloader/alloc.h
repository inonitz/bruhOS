#ifndef __BOOTLOADER_C_DEFINITION_BOOT_SERVICES_ALLOCATOR__
#define __BOOTLOADER_C_DEFINITION_BOOT_SERVICES_ALLOCATOR__
#include <bootloader/api.h>


EFI_API void* allocatePool(uint64_t size);
EFI_API void  freePool(void* address);


EFI_API void* allocatePages(
    EFI_ALLOCATE_TYPE alloctype,
    EFI_MEMORY_TYPE   memtype, 
    uint32_t          pageCount
);
EFI_API void freePages(
    void*    address, 
    uint32_t pageCount
);


#endif /* __BOOTLOADER_C_DEFINITION_BOOT_SERVICES_ALLOCATOR__ */
