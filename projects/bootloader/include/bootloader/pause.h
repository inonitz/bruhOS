#ifndef __BOOTLOADER_C_DEFINITION_PAUSE__
#define __BOOTLOADER_C_DEFINITION_PAUSE__
#include <bootloader/api.h>


EFI_API __attribute__((noreturn)) void busyloop(void);
EFI_API __attribute__((noreturn)) void halt(void);
EFI_API                           void pause(uint32_t microseconds);
EFI_API                           void pauseforinput();


#endif /* __BOOTLOADER_C_DEFINITION_PAUSE__ */