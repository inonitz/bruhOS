#include "efibind.h"
#include <bootloader/exitbootservices.h>
#include <bootloader/err.h>
#include <bootloader/lib.h>
#include <bootloader/pause.h>


void exitBootServices(EFI_HANDLE imgHandle, UINTN memoryMapKey)
{
    EFI_STATUS status = uefi_call_wrapper(gBS->ExitBootServices, 2, imgHandle, memoryMapKey);
    CHECKERRFMT(status, L"Couldn't Exit boot services\n\r");
    return;
}
