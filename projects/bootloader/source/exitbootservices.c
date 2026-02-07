#include <x86_64/efibind.h>
#include <bootloader/exitbootservices.h>
#include <bootloader/err.h>
#include <bootloader/lib.h>
#include <bootloader/pause.h>


void exitBootServices(EFI_HANDLE imgHandle, UINTN memoryMapKey)
{
    EFI_STATUS status = uefi_call_wrapper(gBS->ExitBootServices, 2, imgHandle, memoryMapKey);
    CHECKERRFMT(status != EFI_SUCCESS ? -1 : 1, L"Couldn't Exit Boot Services\n\r");
    return;
}
