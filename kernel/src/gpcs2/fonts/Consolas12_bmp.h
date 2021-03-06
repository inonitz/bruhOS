
//
// Bitmap font C header file generated by bmfont2c.py 
// 

#ifndef FONTLIBRARY_H_
#define FONTLIBRARY_H_
#include "../../std/debug.h"


typedef struct
{
    char_t   GlyphCount;
    char_t   FirstAsciiCode;
    char_t   GlyphBytesWidth;
    char_t   GlyphHeight;
    char_t   FixedWidth;
    uint8_t* GlyphWidth;
    uint8_t* GlyphBitmaps;
} fontStyle_t;


extern fontStyle_t FontStyle_Consolas12;

#endif /* FONTLIBRARY_H_ */
