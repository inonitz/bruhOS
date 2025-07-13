#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_WINDOW__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_WINDOW__
#include <std/argprefix.h>
#include <gpcs2/enable_types.h>
#include <gpcs2/types.h>


// structs
typedef vec2us relative_pixel_offset;


typedef struct __window_buffers 
{
	framebuffer_t front;
	framebuffer_t back;
} window_buffers;


typedef struct __window 
{
	window_buffers        buffers;
	vec2us                dims;
	relative_pixel_offset winoff;
	// uint8_t               reserved[8];
} window_t;



/* 
	* initializes a window object with a front buffer, and an offset in the screen buffer.
	* IN  framebuffer_t* initialized_front_buf - a framebuffer that is set to the front of the window (the one actively seen)
	* IN  vec2us		 window_offset         - the offset of the window in the screen buffer.
	* OUT window_t* 	 to_init			   - the window object to initialize.
*/
uint64_t setup_window(
	IN  framebuffer_t* initialized_front_buf,
	IN  vec2us		   window_offset,  
	OUT window_t* 	   to_init
);


/*
	* Calculate the total size (bytes) of a framebuffer
	* buf - the framebuffer to calculate the size of.
	* Note: due to the fact that using GOP (read EFI_GRAPHICS_OUTPUT_... in UEFI documentation) is almost mandatory,
	*       I'm not going to spend more time supporting other types of frame buffers.
*/
uint64_t getFrameBufferSize(framebuffer_t* buf);


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_WINDOW__ */
