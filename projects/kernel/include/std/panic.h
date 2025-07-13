#ifndef __KERNEL_C_DEFINITION_STD_NONSTANDARD_PANIC__
#define __KERNEL_C_DEFINITION_STD_NONSTANDARD_PANIC__
#include <std/int.h>


/* 
    * will turn the kernel into panic mode, after which it won't be able to continue execution.
    * kpanic will cause the OS to halt/shutdown (depending on future implementation).
    * NOTE: This function can only be called AFTER the graphics library has been initialized.
    * 
    * const char_t* msg - the message that will be printed to the screen.
*/
void kpanic(const char_t* msg);


#endif /* __KERNEL_C_DEFINITION_PANIC__ */
