#pragma once
#ifndef __GRAPHICS_CONSOLE__
#define __GRAPHICS_CONSOLE__
#include "window.h"
#include "basic.h"
#include "fonts/Consolas16_bmp.h"


// structs
typedef struct console {
	window_t      canvas;
	vec2us      cursor_pos;
	uint32_t    background_col;
	uint32_t    text_col;
	fontStyle_t style;
} console;



static inline write_bm8rgb_console(
	IN console* cons,
	IN bitmap8* ch,
	IN uint32_t col
) {
	return write_bm8rgb_framebuffer(
		&cons->canvas.buffers.front,
		&cons->cursor_pos, 
		ch, 
		col
	);
}


static inline uint64_t write_newline_console(
	IN console* cons
) {
	cons->cursor_pos.y += bitmap8y;
	cons->cursor_pos.x  = cons->canvas.winoff.x;
}

// will set up a re-useable bitmap8 that will print every char in s
uint64_t write_string_console(
	IN console* 	 cons,
	IN const char_t* s
);

// will set up a re-useable bitmap8 that will print every char in s
uint64_t write_substring_console(
	IN console*       cons,
	IN const char_t*  s,
	IN uint32_t 	  begin,
	IN uint32_t 	  end
);


uint64_t clear_console_rgb(
	IN console* cons,
	IN uint32_t color
);

uint64_t clear_local_console_rgb(
	IN uint32_t color
);

uint64_t clear_console(
	IN console* cons
);

uint64_t clear_local_console(
);




uint64_t print_bitmap8_local_console(
	IN bitmap8* bm8
);

uint64_t print_char8_local_console(
	IN char_t c
);

uint64_t print_newline_local_console(
);

uint64_t print_string_local_console(
	IN const char_t* s
);

uint64_t print_substring_local_console(
	IN const char_t*  s,
	IN       uint32_t b,
	IN       uint32_t e
);



#endif 