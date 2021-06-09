#pragma once
#ifndef __GRAPHICS_TYPES__
#define __GRAPHICS_TYPES__
#include "../shared/int.h"



#define RGB(r, g, b) (uint32_t)(r << 24 + g << 16 + b << 8)
#define BGR(r, g, b) (uint32_t)(b << 24 + g << 16 + r << 8) 
//      Color      RES R G B   (little-endian BGR format)
#define BLACK      0x00000000	
#define WHITE	   0x00FFFFFF
#define RED		   0x00FF0000
#define BRIGHT_RED 0x00ff1439
#define ORANGE     0x00FF6600
#define LIME	   0x0000FF00
#define BLUE	   0x000000FF
#define YELLOW	   0x00FFFF00
#define CYAN 	   0x0000FFFF
#define MAGENTA    0x00FF00FF
#define SILVER	   0x00C0C0C0
#define GRAY	   0x00808080
#define MAROON	   0x00800000
#define OLIVE	   0x00808000
#define GREEN	   0x00008000
#define NEON_GREEN 0x0039ff14
#define PURPLE	   0x00800080
#define TEAL	   0x00008080
#define NAVY	   0x00000080



typedef struct pack __vec2us {
	uint16_t x, y;
} vec2us;

typedef union rgb32 {
	struct pack { uint8_t  r, g, b, res; } channels;
	uint32_t as_digit;
} rgb32;

typedef struct bitmap8 {
	uint8_t* bits;
	vec2us   dims;
} bitmap8;

typedef struct bitmap32 {
	uint32_t* buffer;
	vec2us    dims;
} bitmap32;



typedef struct alignpack(16) __framebuffer_t {
	void*   start;
	vec2us  dims;
    // bytes per pixel in uefi GOP is automatically 4 (byte for each channel), so I'm not using it.
} framebuffer_t;



#endif