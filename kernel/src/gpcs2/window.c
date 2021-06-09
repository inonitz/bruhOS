#include "window.h"



uint64_t setup_window(
	IN  framebuffer_t* initialized_front_buf,
	IN  vec2us		   window_offset,  
	OUT window_t* 	   to_init
)
{
	to_init->buffers.front = (framebuffer_t){ initialized_front_buf->start, initialized_front_buf->dims };
	to_init->buffers.back  = (framebuffer_t){ NULL,							     { 0u	, 0u   } 		};
	to_init->dims          = initialized_front_buf->dims;
	to_init->winoff        = window_offset;
	return SUCCESS;	
}


uint64_t getFrameBufferSize(framebuffer_t* buf)
{
	// why 4 bytes per pixel ? look in definition of struct framebuffer_t (header gdef.h).
	return 4ul * (uint64_t)buf->dims.x * buf->dims.y;
}
