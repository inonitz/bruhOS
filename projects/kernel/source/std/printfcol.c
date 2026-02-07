#include <std/printfcol.h>
#include <stdarg.h>
#include <std/printf.h>
#include <gpcs2/console/console_instance.h>



int printfcol_(uint64_t foreback, const char* format, ...) {
    uint32_t front = local_console.text_col;
    uint32_t back  = local_console.background_col;

	foreback &= 0x00FFFFFF00FFFFFFu; 			     // arg safety - see gpcs2/gdef.h 
	localConsoleSetBackColor(foreback & 0x00FFFFFF); // low  bits
	localConsoleSetTextColor(foreback >> 32);		 // high bits
	
    va_list va;
    va_start(va, format);
    char buffer[1];
    const int ret = vprintf(format, va);
    va_end(va);
    
	localConsoleSetBackColor(back);
	localConsoleSetTextColor(front);
    return ret;
}