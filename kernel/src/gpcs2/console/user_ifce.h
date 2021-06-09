#pragma once
#ifndef __USER_EXTERNAL_INTERFACE__
#define __USER_EXTERNAL_INTERFACE__
#include "../../shared/int.h"
#include "../gdef.h"



/* 
    * sets the screen of the current active console with the color set by consoleSetBackgroundColor
*/
void consoleClearScreen();


/* 
    * sets the text color of the current active console.
    * uint32_t rgb - a color in the format bRGB, where b = reserved byte == 0.
*/
void consoleSetTextColor(uint32_t rgb);


/* 
    * sets the background color of the current active console.
    * uint32_t rgb - a color in the format bRGB, where b = reserved byte == 0.
*/
void consoleSetBackgroundColor(uint32_t rgb);


/* 
    * returns the size of the framebuffer occupied by the current console.
*/
uint64_t consoleFramebufferSize();

#endif