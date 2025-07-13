#ifndef __BOOTLOADER_DEFINITION_UTILITY_FUNCTIONS__
#define __BOOTLOADER_DEFINITION_UTILITY_FUNCTIONS__
#include <bootloader/api.h>


/* 
    * Modified and Taken from Kernel Source (kernel/mem/phys/pmm.h): Prints the EFI Memory Map.
*/
void PrintMemMapDescriptors(efi_memory_map* map);


#endif /* __BOOTLOADER_DEFINITION_UTILITY_FUNCTIONS__ */
