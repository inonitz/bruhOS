#ifndef __BOOTLOADER_C_DEFINITION_EXIT_BOOT_SERVICES__
#define __BOOTLOADER_C_DEFINITION_EXIT_BOOT_SERVICES__
#include <bootloader/api.h>


EFI_API void exitBootServices(
    EFI_HANDLE imgHandle, 
    UINTN      memoryMapKey
);


#endif /* __BOOTLOADER_C_DEFINITION_GET_BOOT_SERVICES_MEMORY_MAP__ */