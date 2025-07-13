#ifndef __BOOTLOADER_C_DEFINITION_UEFI_APP_API__
#define __BOOTLOADER_C_DEFINITION_UEFI_APP_API__
#include <bootloader/lib.h>

#ifndef EFIAPI
#   define EFIAPI __attribute__((ms_abi))
#endif
#ifndef EFI_API
#   define EFI_API EFIAPI
#endif

#endif /* __BOOTLOADER_C_DEFINITION_UEFI_APP_API__ */
