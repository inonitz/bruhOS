#ifndef __KERNEL_C_DEFINITION_STD_IO_HEADER__
#define __KERNEL_C_DEFINITION_STD_IO_HEADER__
#include <std/int.h>


/* 
	* prints string to the local console
	* format Specifier is %
	* format types:
	* d - int32_t
	* u - uint32_t
	* l - int64_t
	* z - uint64_t
	* c - char_t
	* w - char16_t (not supported YET)
	* s - string   (array of chars)
	* x - hex number      (max_size = uint32_t)
	* X - hex number      (max_size = uint64_t)
	* p - address pointer (max_size = uint64_t)
	* b - binary		  (max_size = uint64_t)
*/
void printk(const char_t* string, ...);


/* 
	* Same as printk, except that it supports colored printing.
	* uint64_t foreback - expects the foreGround in the High 32 bits,
							  and     the backGround in the Low  32 bits.
							  Ex. [(63) foreGround, backGround (0)]
*/
#define FOREBACK(foreground, background) (foreground | (background << 32))
void printkcol(uint64_t foreback, const char_t* string, ...);


/*
	* Aligns Format strings by 'padding_amount' spaces.
	* number/string formats that are bigger than padding_amount will ignore it.
*/
void printk_align_format(uint16_t padding_amount);


/* 
	* returns the current alignment value.
*/
uint16_t printk_get_align(void);


/* 
	* prints string to the local console.
	* no format specifier (including '\n', '\b', ...)
*/
void puts(const char_t* string);


/* 
	* Prints a newline to the console (it's just incrementing the cursor in the console.)
*/
void putln(void);


#endif /* __KERNEL_C_DEFINITION_STD_IO_HEADER__ */
