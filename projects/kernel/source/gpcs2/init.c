#include <gpcs2/init.h>
#include <gpcs2/console/console.h>
#include <gpcs2/colours.h>
#include <std/error_macro.h>
#include <std/error.h>
#include <std/io.h>
#include <std/macro.h>


// defining the local console
console_t local_console =
{
    .canvas = {
        .buffers = {
            .front = { NULLPTR, { 0, 0 }, {0} },
            .back  = { NULLPTR, { 0, 0 }, {0} }
        },
        .dims   = { 0, 0 },
        .winoff = { 0, 0 },
    },
    .cursor_pos     = { 0, 0 },
    .background_col = __KERNEL_CONSOLE_BLACK,
    .text_col       = __KERNEL_CONSOLE_WHITE,
    .style          = (fontStyle_t*)NULLPTR
};


void init_graphics_lib(IN framebuffer_t* init_front_buffer)
{
    if( __kernel_isnull((init_front_buffer)) ) {
        return;
    }

    
    vec2us win_offset = { 0, 0 };
    setup_window(
        init_front_buffer, 
        win_offset,
        &local_console.canvas
    );
    local_console.cursor_pos = win_offset;
    local_console.style      = &FontStyle_Consolas12;
    
    // void* tmp = init_front_buffer->start;
    // consoleSetBackColor(&local_console, __KERNEL_CONSOLE_BLUE);
    // consoleClear(&local_console);
    // consoleWipe(&local_console);
    
    // __asm__ volatile("movq %0, %%rcx\n\t" : : "i"(0));
    // __asm__ volatile("movq %0, %%rcx\n\t" : : "i"(0));

    // __asm__ volatile("movq %0, %%rcx\n\t" : : "r"(init_front_buffer));
    // __asm__ volatile("movq %0, %%rbx\n\t" : : "r"(tmp));
    // *( ((uint32_t*)init_front_buffer->start) + 0) = 0x00FF0000; 
    // *( ((uint32_t*)init_front_buffer->start) + 1) = 0x00FF0000; 
    // printk("test\n");
    
    system_status(KERNEL_SUCCESS, "graphics initialization Succeeded");    
    printk(". Screen is %ux%u pixels (width X height)\n", 
        init_front_buffer->dims.x, 
        init_front_buffer->dims.y
    );
    return;
}
