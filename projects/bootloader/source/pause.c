#include "efibind.h"
#include <bootloader/pause.h>
#include <bootloader/lib.h>


__attribute__((noreturn)) void busyloop(void)
{
	while(1) { __asm__ volatile("pause"); }
}


__attribute__((noreturn)) void halt(void)
{
	__asm__ volatile("hlt");
	busyloop();
}


void pause(uint32_t microseconds)
{
	uefi_call_wrapper(gBS->Stall, 1, microseconds);
	return;
}


void pauseforinput()
{
	EFI_INPUT_KEY key;
	for(uint8_t i = 0; i < 2; ++i) {
		while(
			uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &key)
			!=
			EFI_SUCCESS
		) {}
	}
	return;
}
