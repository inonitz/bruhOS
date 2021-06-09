#pragma once
#ifndef __GRAPHICS_LIBINIT__
#define __GRAPHICS_LIBINIT__
#include "gdef.h"



/* 
    * Initializes the local console to use for printing, drawing, etc...
    * requires a framebuffer_t instance with information about the physical buffer in memory
    * that we're printing to
    * IN framebuffer_t* init_front_buffer - the buffer that will be drawn on.
*/
void init_graphics_lib(IN framebuffer_t* init_front_buffer);

#endif