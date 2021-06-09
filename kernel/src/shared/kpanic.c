#include "kpanic.h"
#include "../std/debug.h"
#include "../gpcs2/console/user_ifce.h"
#include "../gpcs2/gdef.h"



void kpanic(const char_t* msg)
{
    consoleSetBackgroundColor(RED);
    consoleClearScreen();
    consoleSetTextColor(BLACK);
    

    printk("KERNEL PANIC!!!\n");
    KERNEL_ERROR(FAILURE_SEVERE, TRUE, msg);
    return;
}