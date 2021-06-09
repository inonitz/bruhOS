#pragma once
#ifndef __GRAPHICS_LOCAL_CONSOLE__
#define __GRAPHICS_LOCAL_CONSOLE__
#include "console.h"



extern console_t local_console;

static inline void localConsoleWipe()                                        { consoleWipe        (&local_console 				  ); }
static inline void localConsoleNewline()                                     { consoleNewline     (&local_console 				  ); }
static inline void localConsoleClear()                                       { consoleClear       (&local_console 				  ); }
static inline void localConsoleSetTextColor(IN uint32_t     tColor         ) { consoleSetFontColor(&local_console, tColor         ); }
static inline void localConsoleSetBackColor(IN uint32_t     backgroundColor) { consoleSetBackColor(&local_console, backgroundColor); }
static inline void localConsoleSetTextStyle(IN fontStyle_t* tStyle         ) { consoleSetTextStyle(&local_console, tStyle         ); }
static inline void localConsoleWriteString (IN const char_t* str)                  
{ 
	local_console.cursor_pos = consoleWriteString(&local_console, str);
	return;
}
static inline void localConsoleWriteSubstring(
    IN char_t*  str,
	IN uint32_t begin,
	IN uint32_t end
)
{ 
	local_console.cursor_pos = consoleWriteSubstring(&local_console, str, begin, end);
	return;
}
static inline void localConsoleWriteChar(IN char_t ch)
{
	local_console.cursor_pos = consoleWriteChar(&local_console, ch);
	return;
}




#endif