#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TEXT_BLIT__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TEXT_BLIT__
#include <gpcs2/enable_types.h>
#include <gpcs2/fonts/Consolas12_bmp.h>
#include <gpcs2/types.h>
#include <std/argprefix.h>



static const uint8_t charOffsetX = 2u;
static const uint8_t charOffsetY = 2u;


/* 
    * good for drawing an array of pixels directly to the framebuffer
    * The pixels are written from the start of pixel_offset
    * Needs the position and color of each pixel
    * 
    * if length(positions) + pixel_offset > fb->dimensions->x:
    *   return FRAMEBUFFER_TOO_SMALL
    * if length(positions) != length(colors)
    *   return INVALID_INPUT
    * if pixel_offset > fb->dimensions->x:
    *   return INVALID_INPUT
    * if fb is nullptr:
    *   return BAD_BUFFER
    * 
    * Note: The conditions above are NOT CHECKED! 
    * considering that the function will be probably called thousands of times,
    * the impact on performance of 3 if statments will be massive.
    * So, I'm NOT CHECKING these conditions.
*/
uint64_t write_array_to_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us         positions[],
	IN uint32_t       colors[],
	IN vec2us         pixel_offset
);



/* 
    * Writes a bitmap8 to the framebuffer, given an rgb color.
    * bitmap8 definition in console.h
    * 
    * if offset.x || y > fb->dimensions.x || y:
    *   return INVALID_INPUT
    * if fb is nullptr:
    *   return BAD_BUFFER
    * 
*/
uint64_t write_bm8rgb_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us*        offset,
	IN bitmap8*       c,
    IN uint32_t       color
);



/* 
    * same as write_bitmap8_rgb_to_framebuffer (color = WHITE)
*/
uint64_t write_bm8_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us*        offset,
	IN bitmap8*       c
);



/* 
    * Writes a bitmap-font Character using a generated bmfont2c-C-file Structure.
    * returns the new calculated offset.
    * IN framebuffer_t* fb       - the framebuffer that the string will be written to.
    * IN fontStyle_t*   style    - the style of the characters to print 
    * IN vec2us         startoff - the offset at which: when we reach maxoff, then we revert back to startoff in the framebuffer.
    * IN vec2us         offset   - the starting position of the character to be drawn. 
    * IN vec2us         maxoff   - the position that when reached, will move the draw cursor to the next line.
    * IN uint32_t       color    - the RGB of the text       that will be written to the framebuffer
    * IN uint32_t   backColor    - the RGB of the background that will be written to the framebuffer
    * IN char_t*        ch       - the char that will be printed.
    * Conditions that apply to this function (to behave correctly are explained in framebufferWriteStringBmFontRGB)
*/
vec2us framebufferWriteCharBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
    IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,
    IN char_t         ch
);



/* 
    * write a bitmap-font String using a generated bmfont2c-C-file Structure.
    * returns the new calculated offset.
    * IN framebuffer_t* fb     - the framebuffer that the string will be written to.
    * IN fontStyle_t*   style  - the style of the characters to print 
    * IN vec2us         startoff - the offset at which: when we reach maxoff, then we revert back to startoff in the framebuffer.
    * IN vec2us         offset - the starting position of the first character to be drawn. 
    * IN vec2us         maxoff - the position that when reached, will move the draw cursor to the next line.
    * IN uint32_t       color  - the RGB of the text that will be written to the framebuffer
    * IN uint32_t   backColor  - the RGB of the background that will be written to the framebuffer 
    * IN char_t*        str    - the string that will be printed.
    * 
    * Conditions for the function to behave correctly:
    * if startoff > maxoff || startoff > offset || maxoff < offset || ...:
    *   return INVALID_INPUT
    * if style->FixedWidth is not TRUE:
    *   return NOT_IMPLEMENTED.
    * if offset.x || y > fb->dimensions.x || y:
    *   return INVALID_INPUT
    * else if maxoff.x || y > fb->dimensions.x || y:
    *   return INVALID_INPUT
    * if fb is nullptr:
    *   return BAD_BUFFER
*/
vec2us framebufferWriteStringBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
    IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,
    IN const char_t*  str
);



/* 
    * Same as framebufferWriteStringBmFontRGB, except that 
    * the string will start from 'begin' and end at 'end'.
*/
vec2us framebufferWriteSubStringBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
    IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,
    IN const char_t*  str,
    IN uint32_t       begin,
    IN uint32_t       end
);


/* 
    * Batches multiple bitmap8's into one buffer of N bitmap8's
    * returns a bitmap8 buffer with the resulting batch
    * if calculated_total_buffersize > result_dims:
    *   *result_dims = calculated_total_buffersize
    *   return BUFFER_TOO_SMALL
*/
uint64_t batch_bitmap8_array(
	IN  bitmap8  bitmaps[],
	IN  uint16_t bitmap_spacing,
	OUT bitmap8* result,
	OUT vec2us*  result_dims
);


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_TEXT_BLIT__ */
