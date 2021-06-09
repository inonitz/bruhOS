#pragma once
#ifndef __STD_IO__
#define __STD_IO__
#include "../shared/int.h"



/* 
	* Returns length of char array - loops until it finds '\0'.
*/
size_t strlen(char_t* string);


/* 
	* Copies source str to dest
	* length(source) must be less or equal to length(dest)
	* returns dest
	* unsafe - doesn't check buffer length.
*/
char_t* strcpy(char_t* source, char_t* dest);


/* 
	* initializes memory with val
	* size is the amount of bytes to initialize
	* size must be less or equal to the sizeof(location)
*/
void* memset(void* location, int32_t val, size_t size);


/* 
	* compares two distinct memory regions.
	* the pointers CANNOT overlap.
	* returns 0 if successful.
*/
uint8_t memcmp(char_t* a, char_t* b, int64_t size);


/* 
	* copies amount bytes from src to dst
	* dst 	 - where memory will be placed
	* src 	 - where memory will be copied from
	* amount - how many bytes are copied
	* unsafe - we don't know the extent of each array.
*/
void memcpy(void* dst, void* src, uint64_t amount);


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
void printk(char_t* string, ...);


/* 
	* Same as printk, except that it supports colored printing.
	* uint64_t foreback - expects the foreGround in the High 32 bits,
							  and     the backGround in the Low  32 bits.
							  Ex. [(63) foreGround, backGround (0)]
*/
#define FOREBACK(foreground, background) (foreground | (background << 32))
void printkcol(uint64_t foreback, char_t* string, ...);


/*
	* Aligns Format strings by 'padding_amount' spaces.
	* number/string formats that are bigger than padding_amount will ignore it.
*/
void printk_align_format(uint16_t padding_amount);


/* 
	* returns the current alignment value.
*/
uint16_t printk_get_align();


/* 
	* prints string to the local console.
	* no format specifier (including '\n', '\b', ...)
*/
void puts(char_t* string);


/* 
	* Prints a newline to the console (it's just incrementing the cursor in the console.)
*/
void putln();


inline uint64_t round2(uint64_t n) // round n to closest power of 2
{
	--n; 
	n |= n >> 1; n |= n >> 4; n |= n >> 8; n |= n >> 2; n |= n >> 16; n |= n >> 32;
	++n;
	return n;
}

// n = 2^M + K, where K > 0. return 2^M. K = 0 is a special case that is not handled.
inline uint64_t ppow2(uint64_t n)
{
	return round2(n) >> 1;
}


static inline uint32_t log2ui (uint32_t b)  { return ((unsigned) (8 * sizeof(unsigned int      ) - __builtin_clz  ((b)) - 1)); } // b = 2^n, n >= 0. returns n.
static inline uint64_t log2ul (uint64_t b)  { return ((unsigned) (8 * sizeof(unsigned long     ) - __builtin_clzl ((b)) - 1)); } // b = 2^n, n >= 0. returns n.
static inline size_t   log2ull(size_t   b)  { return ((unsigned) (8 * sizeof(unsigned long long) - __builtin_clzll((b)) - 1)); } // b = 2^n, n >= 0. returns n.
static inline uint32_t lsbui  (uint32_t b)  { return __builtin_ffs((b));   } // least significant bit. index starts at 1, not 0!
static inline uint64_t lsbul  (uint64_t b)  { return __builtin_ffsl((b));  } // least significant bit. index starts at 1, not 0!
static inline size_t   lsbull (size_t   b)  { return __builtin_ffsll((b)); } // least significant bit. index starts at 1, not 0!



#endif






