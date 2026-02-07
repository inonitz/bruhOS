#ifndef __KERNEL_C_DEFINITION_STD_PRINTF_COLOUR_HEADER__
#define __KERNEL_C_DEFINITION_STD_PRINTF_COLOUR_HEADER__
#include <stdint.h>


/**
 * Tiny printf implementation (with console colours)
 * You have to implement _putchar if you use printf()
 * To avoid conflicts with the regular printf() API it is overridden by macro defines
 * and internal underscore-appended functions like printf_() are used
 * \param format A string that specifies the format of the output
 * \return The number of characters that are written into the array, not counting the terminating null character
 */
#define printfcol printfcol_
int printfcol_(uint64_t foreback, const char* format, ...);


#endif /* __KERNEL_C_DEFINITION_STD_PRINTF_COLOUR_HEADER__ */