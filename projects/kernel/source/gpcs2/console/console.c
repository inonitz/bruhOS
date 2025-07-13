#include <gpcs2/console/console.h>
#include <gpcs2/colours.h>
#include <gpcs2/blit.h>



void consoleSetFontColor(IN console_t* cons, IN uint32_t     tColor         ) { cons->text_col       = tColor;          }
void consoleSetBackColor(IN console_t* cons, IN uint32_t     backgroundColor) { cons->background_col = backgroundColor; }
void consoleSetTextStyle(IN console_t* cons, IN fontStyle_t* tStyle         ) { cons->style          = tStyle;          }


void consoleClear(IN console_t* cons)
{
    framebuffer_t* tmp         = &cons->canvas.buffers.front;
	uint64_t       buffer_size = getFrameBufferSize(tmp) / sizeof(uint64_t);
	uint64_t       batch_col   = (((uint64_t)cons->background_col) << 32) + cons->background_col;

    for(uint64_t off = 0; off < buffer_size; ++off)
    {
        *((uint64_t*)tmp->start + off) = batch_col;
    }
	cons->cursor_pos = (vec2us){ 0, 0 }; // reset to start.
}


void consoleWipe(IN console_t* cons) {
    uint32_t col = cons->background_col;
    cons->background_col = __KERNEL_CONSOLE_BLACK;
    consoleClear(cons);
    cons->background_col = col;
}


vec2us consoleWriteString(
    IN console_t*    cons,
    IN const char_t* str
) {
    return framebufferWriteStringBmFontRGB(
        &cons->canvas.buffers.front, 
        cons->style,
        cons->canvas.winoff,
        cons->cursor_pos,
        cons->canvas.dims,
        cons->text_col,
        cons->background_col,
        str
    );
}


vec2us consoleWriteSubstring(
    IN console_t*    cons,
    IN const char_t* str,
	IN uint32_t      begin,
	IN uint32_t      end
) {
    return framebufferWriteSubStringBmFontRGB(
        &cons->canvas.buffers.front, 
        cons->style,
        cons->canvas.winoff,
        cons->cursor_pos,
        cons->canvas.dims,
        cons->text_col,
        cons->background_col,
        str,
        begin,
        end
    );
}


vec2us consoleWriteChar(
    IN console_t* cons,
    IN char_t     ch
) {
    return framebufferWriteCharBmFontRGB(
        &cons->canvas.buffers.front, 
        cons->style,
        cons->canvas.winoff,
        cons->cursor_pos,
        cons->canvas.dims,
        cons->text_col,
        cons->background_col,
        ch
    );
}


void consoleNewline(IN console_t* cons) {
    cons->cursor_pos.y +=  cons->style->GlyphHeight + charOffsetY;
    cons->cursor_pos.y  = cons->cursor_pos.y * (cons->cursor_pos.y < cons->canvas.dims.y) + cons->canvas.winoff.y * (cons->cursor_pos.y >= cons->canvas.dims.y);
    cons->cursor_pos.x  = cons->canvas.winoff.x;
}