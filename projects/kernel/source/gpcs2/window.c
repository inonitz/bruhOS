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
		initialized_front_buf->m_baseAddress, 
		initialized_front_buf->m_width,
		initialized_front_buf->m_height,
		initialized_front_buf->m_pixelsPerScanLine,
		initialized_front_buf->m_pixelElementSizeBytes,
	};
	to_init->buffers.back = (framebuffer_t){ 
		NULLPTR,
		0, 0,
		0,
		0
	};
	to_init->dims = (vec2us){ initialized_front_buf->m_width, initialized_front_buf->m_height };
	to_init->winoff = window_offset;
	return KERNEL_SUCCESS;	
}


uint64_t getFrameBufferSize(framebuffer_t* buf)
{
	/* See UEFI Spec - Graphics Output Protocol for more info */
	uint64_t result = (uint64_t)buf->m_pixelElementSizeBytes;
	result *= buf->m_pixelsPerScanLine;
	result *= buf->m_height;
	return result;
}
