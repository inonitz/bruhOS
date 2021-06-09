#pragma once
#ifndef __GRAPHICS_CONSOLE__
#define __GRAPHICS_CONSOLE__
#include "../basic.h"
#include "../window.h"
#include "../fonts/Consolas12_bmp.h"



// structs
typedef struct __console {
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
static inline void consoleWipe(IN console_t* cons) {
    uint32_t col = cons->background_col;
    cons->background_col = BLACK;
    consoleClear(cons);
    cons->background_col = col;
}


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
    IN console_t* cons,
    IN char_t*    s,
	IN uint32_t   b,
	IN uint32_t   e
);


/* 
    * Write an ASCII character to a console given its handle.
    * IN console_t* cons - the console to which the character will be printed
    * IN char_t     ch   - the ASCII character to print
*/
vec2us consoleWriteChar(
    IN console_t* cons,
    IN char_t     ch
);


/* 
    * Moves the Console Cursor to a new line given the windows' offset in the main framebuffer.
*/
static inline void consoleNewline(IN console_t* cons) {
    cons->cursor_pos.y +=  cons->style->GlyphHeight + charOffsetY;
    cons->cursor_pos.y  = cons->cursor_pos.y * (cons->cursor_pos.y < cons->canvas.dims.y) + cons->canvas.winoff.y * (cons->cursor_pos.y >= cons->canvas.dims.y);
    cons->cursor_pos.x  = cons->canvas.winoff.x;
}

#endif