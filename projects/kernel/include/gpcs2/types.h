#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__
#include <std/macro.h>
#include <std/stdint.h>


#ifdef __KERNEL_CONSOLE_VEC2_UNSIGNED_SHORT_TYPE
typedef struct pack __unsigned_short_2_element_vector {
	uint16_t x, y;
} vec2us;
#endif

#ifdef __KERNEL_CONSOLE_FRAMEBUFFER_TYPE
typedef struct __individual_pixel_channel_format_type {
	uint8_t bitWidth;
	uint8_t bitShift;
} pixelChannelFormat_t;


typedef struct alignsz(8) __uefi_framebuffer_pixel_format_type {
	uint8_t  			 m_sizeBytes;
	uint8_t  			 m_enumFormat;
	pixelChannelFormat_t m_channelFormats[3];
} pixelMetadata_t;


typedef struct alignsz(8) __framebuffer_t {
	void*    m_baseAddress;
	uint16_t m_width;
	uint16_t m_height;
	uint16_t m_pixelsPerScanLine;
	pixelMetadata_t m_pixelInfo;
} framebuffer_t;
#endif


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__ */
