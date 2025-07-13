#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__
#include <std/macro.h>
#include <std/stdint.h>


#ifdef __KERNEL_CONSOLE_VEC2_UNSIGNED_SHORT_TYPE
typedef struct pack __unsigned_short_2_element_vector {
	uint16_t x, y;
} vec2us;
#endif

#ifdef __KERNEL_CONSOLE_RGB32_UNION_TYPE
#ifndef __KERNEL_CONSOLE_RGB32_UNION_TYPE_SIZE_BYTES
#	define __KERNEL_CONSOLE_RGB32_UNION_TYPE_SIZE_BYTES 4
#endif
typedef union rgb32 {
	struct { uint8_t  r, g, b, res; } channels;
	uint32_t as_digit;
} rgb32;
#endif

#ifdef __KERNEL_CONSOLE_BITMAP_8BIT_TYPE
typedef struct alignsz(8) bitmap8 {
	uint8_t* bits;
	vec2us   dims;
	uint8_t  reserved[4];
} bitmap8;
#endif

#ifdef __KERNEL_CONSOLE_BITMAP_32BIT_TYPE
typedef struct bitmap32 {
	uint32_t* buffer;
	vec2us    dims;
	uint8_t  reserved[4];
} bitmap32;
#endif

#ifdef __KERNEL_CONSOLE_FRAMEBUFFER_TYPE
typedef struct alignsz(8) __framebuffer_t {
	void*   start;
	vec2us  dims;
	uint8_t reserved[4];
    // bytes per pixel in uefi GOP is automatically 4 (byte for each channel), so I'm not using it.
} framebuffer_t;
#endif


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TYPES__ */
