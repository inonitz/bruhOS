#pragma once
#ifndef __KERNEL_PANIC__
#define __KERNEL_PANIC__
#include "int.h"



/* 
    * will turn the kernel into panic mode, after which it won't be able to continue execution.
    * kpanic will cause the OS to halt/shutdown (depending on future implementation).
    * NOTE: This function can only be called AFTER the graphics library has been initialized.
    * 
    * const char_t* msg - the message that will be printed to the screen.
*/
void kpanic(const char_t* msg);

#endif