#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_INSTANCE__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_INSTANCE__
#include <gpcs2/console/console.h>


extern console_t local_console;


static inline void localConsoleWipe(void)                                    { consoleWipe        (&local_console 				  ); }
static inline void localConsoleNewline(void)                                 { consoleNewline     (&local_console 				  ); }
static inline void localConsoleClear(void)                                   { consoleClear       (&local_console 				  ); }
static inline void localConsoleSetTextColor(IN uint32_t     tColor         ) { consoleSetFontColor(&local_console, tColor         ); }
static inline void localConsoleSetBackColor(IN uint32_t     backgroundColor) { consoleSetBackColor(&local_console, backgroundColor); }
static inline void localConsoleSetTextStyle(IN fontStyle_t* tStyle         ) { consoleSetTextStyle(&local_console, tStyle         ); }
static inline void localConsoleWriteString (IN const char_t* str)                  
{ 
	local_console.cursor_pos = consoleWriteString(&local_console, str);
	return;
}
static inline void localConsoleWriteSubstring(
    IN const char_t*  str,
	IN uint32_t begin,
	IN uint32_t end
) { 
	local_console.cursor_pos = consoleWriteSubstring(&local_console, str, begin, end);
	return;
}
static inline void localConsoleWriteChar(IN char_t character)
{
	local_console.cursor_pos = consoleWriteChar(&local_console, character);
	return;
}


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_INSTANCE__ */
