#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_CONSOLE_DEFINITION__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_CONSOLE_DEFINITION__
#include <gpcs2/fonts/Consolas12_bmp.h>
#include <gpcs2/window.h>


// structs
typedef struct alignpk(64) __kernel_graphics_console_definition {
	window_t     canvas;
	vec2us       cursor_pos;
	uint32_t     background_col;
	uint32_t     text_col;
	fontStyle_t* style;
} console_t;


/* 
    * Set Console Attributes
*/
void consoleSetFontColor(IN console_t* cons, IN uint32_t     tColor         );
void consoleSetBackColor(IN console_t* cons, IN uint32_t     backgroundColor);
void consoleSetTextStyle(IN console_t* cons, IN fontStyle_t* tStyle         );


/* 
    * Fill the console screen with the color defined in the console struct.
*/
void consoleClear(IN console_t* cons);


/* 
    * Same as consoleClear, but color = BLACK.
*/
void consoleWipe(IN console_t* cons);


/* 
    * Writes a string to a given console.
    * IN console_t* cons - the console to which the given string will be written.
    * IN char_t*    str  - the string that is written to the console.
*/
vec2us consoleWriteString(
    IN console_t*    cons,
    IN const char_t* str
);


/* 
    * Same as consoleWriteString, except that the string printed depends on 2 offsets.
    * IN console_t* cons - the console to which the given string will be written.
    * IN char_t*    str  - the string that is written to the console.
    * IN uint32_t   b    - the index at which the console will start writing the string.
    * IN uint32_t   e    - the index at which the console will stop  writing the string.
*/
vec2us consoleWriteSubstring(
    IN console_t*    cons,
    IN const char_t* str,
	IN uint32_t      begin,
	IN uint32_t      end
);


/* 
    * Write an ASCII character to a console given its handle.
    * IN console_t* cons - the console to which the character will be printed
    * IN char_t     ch   - the ASCII character to print
*/
vec2us consoleWriteChar(
    IN console_t* cons,
    IN char_t     character
);


/* 
    * Moves the Console Cursor to a new line given the windows' offset in the main framebuffer.
*/
void consoleNewline(IN console_t* cons);


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_CONSOLE_DEFINITION__ */
