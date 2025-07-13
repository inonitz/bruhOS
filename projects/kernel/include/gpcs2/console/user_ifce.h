#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_USER_INTERFACE__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_USER_INTERFACE__
#include <stdint.h>


/* 
    * sets the screen of the current active console with the color set by consoleSetBackgroundColor
*/
void consoleClearScreen(void);


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
uint64_t consoleFramebufferSize(void);


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_USER_INTERFACE__ */
