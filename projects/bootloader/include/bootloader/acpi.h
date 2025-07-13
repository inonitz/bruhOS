#ifndef __BOOTLOADER_C_DEFINITION_ACPI_UTIL__
#define __BOOTLOADER_C_DEFINITION_ACPI_UTIL__
#include <bootloader/api.h>
#include <bootloader/funcargtype.h>
#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <preboot/acpi.h>
#undef __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__


/* 
    * Finds any RSDP Structures in memory (ACPI_2.0+ / ACPI_1.0)
    * Verifies their checksum
    * on Failure, the bootloader will exit with EFI_UNSUPPORTED
    * on Success, the bootloader will continue, 'dsc' will be set to the adress of the 'RSDPdescriptor20' struct
*/
EFI_API void get_verify_acpi_rsdp(
    OUT RSDPdescriptor20** dsc
);


/* 
    * Finds the amount of CPU cores available.
    * on Failure, returns 0.
    * On Success, returns the amount of CPU cores available in the system.
*/
EFI_API uint32_t get_core_count(void);


#endif /* __BOOTLOADER_C_DEFINITION_ACPI_UTIL__ */