#include <std/panic.h>
#include <std/error_macro.h>
#include <std/error.h>
#include <gpcs2/console/user_ifce.h>
#include <gpcs2/graphics.h>


void kpanic(const char_t* msg)
{
    consoleSetBackgroundColor(__KERNEL_CONSOLE_RED);
    consoleClearScreen();
    consoleSetTextColor(__KERNEL_CONSOLE_BLACK);
    

    printk("KERNEL PANIC!!!\n");
    KERNEL_ERROR(KERNEL_FAILURE_SEVERE, BOOLEAN_TRUE, msg)
    return;
}
