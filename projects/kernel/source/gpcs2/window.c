#include <gpcs2/window.h>
#include <std/macro.h>
#include <std/error_macro.h>



uint64_t setup_window(
	IN  framebuffer_t* initialized_front_buf,
	IN  vec2us		   window_offset,  
	OUT window_t* 	   to_init
)
{
	to_init->buffers.front = (framebuffer_t){ 
		initialized_front_buf->start, 
		initialized_front_buf->dims , 
		{ 0, 0 }
	};
	to_init->buffers.back  = (framebuffer_t){
		NULLPTR,
		{ 0, 0 }, 
		{ 0, 0 }
	};
	to_init->dims   = initialized_front_buf->dims;
	to_init->winoff = window_offset;
	return KERNEL_SUCCESS;	
}


uint64_t getFrameBufferSize(framebuffer_t* buf)
{
	/* See UEFI Spec - Graphics Output Protocol for more info */
	uint64_t result = (uint64_t)buf->dims.x * (uint64_t)buf->dims.y;
	return __KERNEL_CONSOLE_RGB32_UNION_TYPE_SIZE_BYTES * result;
}
