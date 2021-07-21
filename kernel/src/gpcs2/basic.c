#include "basic.h"



// Not useful for now, will not implement yet.
uint64_t write_array_to_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us         positions[],
	IN uint32_t       colors[],
	IN vec2us         pixel_offset
)
{
	return NOT_IMPLEMENTED;
}



uint64_t write_bm8rgb_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us*        offset,
	IN bitmap8*       c,
    IN uint32_t       color
)
{
	uint32_t*     pixel_address = (uint32_t*)fb->start + (uint32_t)(offset->x + fb->dims.x * offset->y);
	uint8_t       pix;

	for (int8_t i = 12; i >= 0; --i)
	{
		// drawing a single row of bits (8 bits) from the 13 in the current char bitmap
		pix = c->bits[i];
		*(pixel_address     ) = color * (pix & 0x80u);
		*(pixel_address + 1u) = color * (pix & 0x40u);
		*(pixel_address + 2u) = color * (pix & 0x20u);
		*(pixel_address + 3u) = color * (pix & 0x10u);
		*(pixel_address + 4u) = color * (pix & 0x08u);
		*(pixel_address + 5u) = color * (pix & 0x04u);
		*(pixel_address + 6u) = color * (pix & 0x02u);
		*(pixel_address + 7u) = color * (pix & 0x01u);
		pixel_address += (uint32_t)fb->dims.x; // goto next line in the framebuffer
	}
	offset->x += c->dims.x + charOffsetX;
	return SUCCESS;
}



uint64_t write_bm8_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us*        offset,
	IN bitmap8*       c
)
{
	return write_bm8rgb_framebuffer(fb, offset, c, WHITE);
}



vec2us framebufferWriteCharBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
	IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,
    IN char_t         ch
) 
{
	uint32_t  masks[8] = {0};
	uint32_t* pixel_address = (uint32_t*)fb->start + (uint32_t)(offset.x + fb->dims.x * offset.y); 
	uint8_t*  chline        = &style->GlyphBitmaps[(uint32_t)style->GlyphBytesWidth * style->GlyphHeight * (uint32_t)(ch - 32)]; // get character bitmap

	// draw the charcter.
	for(int8_t y = 0; y < style->GlyphHeight; ++y) // for each line in bitmap
	{
		for(int8_t x = 0; x < style->GlyphBytesWidth; ++x) // draw every line
		{
			for(uint8_t i = 0; i < 8; ++i)
			{
				masks[i]  = !!(*chline & (0x80u >> i));
				masks[i] *= color;
				masks[i] += !masks[i] * backColor;
			}
			memcpy(pixel_address, masks, sizeof(masks));
			++chline;							   // goto next line in the char
			pixel_address += (uint32_t)fb->dims.x; // goto next line in the framebuffer
		}
	}
	offset.x += style->GlyphBytesWidth * 8 + charOffsetX; // increment offset
	offset.x  = offset.x * (offset.x < maxoff.x) + startoff.x * (offset.x >= maxoff.x);
	offset.y += (style->GlyphHeight + charOffsetY) * (offset.x == startoff.x);
	return offset;
}



vec2us framebufferWriteStringBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
	IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,
    IN const char_t*  str
) {
	// if(unlikely(fb == nullptr))
	// {
	// 	return INVALID_INPUT;
	// }
	// else if(unlikely(style->FixedWidth == FALSE))
	// {
	// 	return NOT_IMPLEMENTED;
	// } else if (unlikely(offset.x > fb->dims.x || offset.y > fb->dims.y))
	// {
	// 	return INVALID_INPUT;
	// } 
	if(str == nullptr)
		return (vec2us){0xFFFF, 0xFFFF};

	uint32_t  masks[8] align(16) = {0};
	uint32_t  GlyphIdx      = (uint32_t)style->GlyphBytesWidth * style->GlyphHeight;
	uint32_t* pixel_address = 0;
	uint8_t*  chline        = 0;

	for(const char_t* c = str; *c != '\0'; ++c)
	{
		pixel_address = (uint32_t*)fb->start + (uint32_t)fb->dims.x * offset.y + offset.x;
		chline 		  = &style->GlyphBitmaps[GlyphIdx * (uint32_t)(*c - 32)]; // get current character

		// draw the charcter.
		for(int8_t y = 0; y < style->GlyphHeight; ++y) // for each line in bitmap
		{
			for(int8_t x = 0; x < style->GlyphBytesWidth; ++x) // draw every line
			{
				for(uint8_t i = 0; i < 8u; ++i)
				{
					masks[i]  = !!(*chline & (0x80u >> i)); // get bit at i.
					masks[i] *= color;						// set as color if true.
					masks[i] += !!!masks[i] * backColor;	// set as backColor if false (pixel_isnt_on == background color)
				}
				memcpy(pixel_address, masks, sizeof(masks));
				++chline;							   // goto next line in the char
				pixel_address += (uint32_t)fb->dims.x; // goto next line in the framebuffer
			}
		}
		offset.x += style->GlyphBytesWidth * 8 + charOffsetX; 								// increment x offset
		offset.x  = offset.x * (offset.x < maxoff.x) + startoff.x * (offset.x >= maxoff.x); // limit the active x offset to the extent set by the args. 
		offset.y += (style->GlyphHeight + charOffsetY) * (offset.x == startoff.x);   		// if we reached the extent, then we need to increment to a new line.
		// masks[0]  = bufferOff.y >= extent.y;												// we use masks[0] as a temporary.
		// bufferOff.x *= !masks[0];														// if the active y offset has reached the extent, we need to go back to 'offset'.
		// bufferOff.x += offset.x * masks[0];
		// bufferOff.y *= !masks[0];
		// bufferOff.y += offset.y * masks[0];

		// ^ a fancy way of doing this 
		// (I really am trusting the compiler do optimize this, although I'm fairly skeptical):
		if(offset.y >= maxoff.y) {
			offset = startoff;
		}
	}

	return offset;
}



vec2us framebufferWriteSubStringBmFontRGB(	
    IN framebuffer_t* fb,
    IN fontStyle_t*   style,
	IN vec2us         startoff,
    IN vec2us         offset,
	IN vec2us         maxoff,
    IN uint32_t       color,
	IN uint32_t       backColor,	
    IN char_t*        str,
    IN uint32_t       begin,
    IN uint32_t       end
) 
{
	if(str == nullptr)
		return (vec2us){0xFFFF, 0xFFFF};

	uint32_t  GlyphIdx      = (uint32_t)style->GlyphBytesWidth * style->GlyphHeight;
	uint32_t* pixel_address = 0;
	uint8_t*  chline        = 0;
	uint32_t  masks[8] align(16) = {0};

	str += begin;
	while(begin < end)
	{
		pixel_address = (uint32_t*)fb->start + (uint32_t)fb->dims.x * offset.y + offset.x;
		chline 		  = &style->GlyphBitmaps[GlyphIdx * (uint32_t)(*str - 32)]; // get current character bitmap

		// draw the charcter.
		for(int8_t y = 0; y < style->GlyphHeight; ++y) // for each line in bitmap
		{
			for(int8_t x = 0; x < style->GlyphBytesWidth; ++x) // draw every line
			{
				for(uint8_t i = 0; i < 8u; ++i)
				{
					masks[i]  = !!(*chline & (0x80u >> i));
					masks[i] *= color;
					masks[i] += !masks[i] * backColor;
				}
				memcpy(pixel_address, masks, sizeof(masks));
				++chline;							   // goto next line in the char
				pixel_address += (uint32_t)fb->dims.x; // goto next line in the framebuffer
			}
		}
		
		offset.x += style->GlyphBytesWidth * 8 + charOffsetX; 								
		offset.x  = offset.x * (offset.x < maxoff.x) + startoff.x * (offset.x >= maxoff.x); 
		offset.y += (style->GlyphHeight + charOffsetY) * (offset.x == startoff.x);   		
		if(offset.y >= maxoff.y) {
			offset = startoff;
		}
		
		++begin;
		++str;
	}

	return offset;
}




uint64_t batch_bitmap8_array(
	IN  bitmap8  bitmaps[],
	IN  uint16_t bitmap_spacing,
	OUT bitmap8* result,
	OUT vec2us*  result_dims
)
{
	return NOT_IMPLEMENTED;
}




/* 
Unused code, or just code that took forever to write that in a case where I need to reuse it I'll look here: 

uint64_t write_bitmap32_to_framebuffer(
	IN framebuffer_t* fb,
	IN vec2us*        offset,
	IN bitmap32*      c
)
{
	uint32_t* pixel_address      = (uint32_t*)fb->start + (uint32_t)(offset->x + fb->dims.x * offset->y);
	uint32_t  char_current_pixel = 0u;
	uint16_t  charOffsetX        = 10u; // spacing between every char that is printed.
	//UINT32  charOffsetY        = 10u; // when printing newline ('\n') you need to take this in mind
	
	// I want to perform multiple pixel draws per loop
	// the remainder is the amount that didn't divide into the pixelsPerLoop variable, and needs to run in the other loop.
	const uint8_t pixelsPerLoop  = 8u;
	uint32_t      remainder      = c->dims.x % pixelsPerLoop; 
	uint32_t      loopConst      = c->dims.x / pixelsPerLoop;

	for (uint32_t j = 0; j < c->dims.y; ++j)
	{
		for (uint32_t i = 0; i < loopConst; ++i)
		{
			*(pixel_address     ) = c->buffer[i     ];
			*(pixel_address + 1u) = c->buffer[i + 1u];
			*(pixel_address + 2u) = c->buffer[i + 2u];
			*(pixel_address + 3u) = c->buffer[i + 3u];
			*(pixel_address + 4u) = c->buffer[i + 4u];
			*(pixel_address + 5u) = c->buffer[i + 5u];
			*(pixel_address + 6u) = c->buffer[i + 6u];
			*(pixel_address + 7u) = c->buffer[i + 7u];
			  pixel_address      += 8u;
			  char_current_pixel += 8u;
		}
		for (uint32_t i = 0; i < remainder; ++i)
		{
			*(pixel_address + i) = c->buffer[i];
		}
		char_current_pixel += remainder;
		pixel_address      += remainder + (uint32_t)fb->dims.x;
	}
	offset->x += c->dims.x + charOffsetX;
	
	return NOT_IMPLEMENTED;
}
*/


/* 
	// outside loops
	uint32_t align(16) bColor[8] = {
		backColor, backColor, backColor, backColor, 
		backColor, backColor, backColor, backColor
	};
	uint32_t align(16) dest[8]   = {
		color, color, color, color, 
		color, color, color, color
	};

	// in main loop.
	uint32_t mask[8] = {
		!!(tc & 0x80u),
		!!(tc & 0x40u),
		!!(tc & 0x20u),
		!!(tc & 0x10u),
		!!(tc & 0x08u),
		!!(tc & 0x04u),
		!!(tc & 0x02u),
		!!(tc & 0x01u)
	};

	store_u32_epi8mask((__m128*)dest, 	  (__m128*)bColor, 	   (__m128*)mask	);
	store_u32_epi8mask((__m128*)&dest[4], (__m128*)&bColor[4], (__m128*)&mask[4]);
	memcpy(pixel_address, dest, sizeof(dest));
*/

/* 
	maskBuffer[0]  = color 	   *  !!(tc & 0x80u); 
	maskBuffer[1]  = color 	   *  !!(tc & 0x40u);
	maskBuffer[2]  = color 	   *  !!(tc & 0x20u);
	maskBuffer[3]  = color 	   *  !!(tc & 0x10u);
	maskBuffer[4]  = color 	   *  !!(tc & 0x08u);
	maskBuffer[5]  = color 	   *  !!(tc & 0x04u);
	maskBuffer[6]  = color 	   *  !!(tc & 0x02u);
	maskBuffer[7]  = color 	   *  !!(tc & 0x01u);
	maskBuffer[0] += backColor * !!!(maskBuffer[0]); 
	maskBuffer[1] += backColor * !!!(maskBuffer[1]);
	maskBuffer[2] += backColor * !!!(maskBuffer[2]);
	maskBuffer[3] += backColor * !!!(maskBuffer[3]);
	maskBuffer[4] += backColor * !!!(maskBuffer[4]);
	maskBuffer[5] += backColor * !!!(maskBuffer[5]);
	maskBuffer[6] += backColor * !!!(maskBuffer[6]);
	maskBuffer[7] += backColor * !!!(maskBuffer[7]);

*/