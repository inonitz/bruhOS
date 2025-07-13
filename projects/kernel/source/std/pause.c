#include <std/pause.h>
#include <std/macro.h>


void pausewhile(volatile uint8_t condition)
{
	while(boolean(condition)) {
		__asm__ volatile("pause" : : : "memory");
	}
	return;
}
