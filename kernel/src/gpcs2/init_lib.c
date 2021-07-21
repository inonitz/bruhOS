#include "init_lib.h"
#include "console/console_instance.h"



// defining the local console
console_t local_console =
{
    .canvas = {
        .buffers = {
            .front = { NULL, { 0, 0 } },
            .back  = { NULL, { 0, 0 } }
        },
        .dims = { 0, 0 },
        .winoff = 0,
    },
    .cursor_pos     = { 0, 0 },
    .background_col = BLACK,
    .text_col       = WHITE,
    .style          = nullptr
};


void init_graphics_lib(IN framebuffer_t* init_front_buffer)
{
    if(isnull(init_front_buffer))
        return;


    vec2us win_offset = { 0, 0 };
    setup_window(
        init_front_buffer, 
        win_offset,
        &local_console.canvas
    );
    local_console.cursor_pos = win_offset;
    local_console.style      = &FontStyle_Consolas12;
    
    
    system_status(SUCCESS, "graphics initialization Succeeded");    
    printk(". Screen is %ux%u pixels (width X height)\n", 
        init_front_buffer->dims.x, 
        init_front_buffer->dims.y
    );
    return;
}