#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_LIBRARY_INIT__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_LIBRARY_INIT__
#include <std/argprefix.h>
#include <gpcs2/enable_types.h>
#include <gpcs2/types.h>


/* 
    * Initializes the local console to use for printing, drawing, etc...
    * requires a framebuffer_t instance with information about the physical buffer in memory
    * that we're printing to
    * IN framebuffer_t* init_front_buffer - the buffer that will be drawn on.
*/
void init_graphics_lib(IN framebuffer_t* init_front_buffer);


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_LIBRARY_INIT__ */
