#ifndef __KERNEL_C_DEFINITION_STD_STRING_HEADER__
#define __KERNEL_C_DEFINITION_STD_STRING_HEADER__
#include <stddef.h>
#include <std/int.h>
#include <std/macro.h>


/* 
	* Returns length of char array - loops until it finds '\0'.
*/
uint64_t strlen(char_t* string);


/* 
	* Copies source str to dest
	* length(source) must be less or equal to length(dest)
	* returns dest
	* unsafe - doesn't check buffer length.
*/
char_t* strcpy(char_t* source, char_t* dest);


/* 
	* initializes memory[i] with val
	* size is the amount of bytes to initialize
	* size must be less or equal to the sizeof(location)
*/
void* memset_byte(uint8_t* location, uint8_t val, uint64_t size);
void* memset(void* location, uint8_t val, uint64_t size);


/* 
	* compares two distinct memory regions.
	* the pointers CANNOT overlap.
	* returns 0 if successful.
*/
uint8_t memcmp(const char_t* a, const char_t* b, int64_t size);


/* 
	* copies amount bytes from src to dst
	* dst 	 - where memory will be placed
	* src 	 - where memory will be copied from
	* amount - how many bytes are copied
	* unsafe - we don't know the extent of each array.
*/
void* memcpy(
	void* 		__restrict __dest, 
	const void* __restrict __src,
	size_t __n
);



#endif /* __KERNEL_C_DEFINITION_STD_STRING_HEADER__ */