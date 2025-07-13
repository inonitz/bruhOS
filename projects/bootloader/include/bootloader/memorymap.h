#ifndef __BOOTLOADER_C_DEFINITION_GET_BOOT_SERVICES_MEMORY_MAP__
#define __BOOTLOADER_C_DEFINITION_GET_BOOT_SERVICES_MEMORY_MAP__
#include <bootloader/api.h>

#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#   include <preboot/memmap.h>
#undef __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__


EFI_API UINTN initializeKernelMemoryMap(efi_memory_map* map);
EFI_API void  dumpMemoryMap(efi_memory_map* map);


#endif /* __BOOTLOADER_C_DEFINITION_GET_BOOT_SERVICES_MEMORY_MAP__ */