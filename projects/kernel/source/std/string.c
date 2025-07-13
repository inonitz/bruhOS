#include <std/string.h>
#include <stddef.h>


/* 
	* Returns length of char array - loops until it finds '\0'.
*/
uint64_t strlen(char_t* string)
{
	uint64_t r = 0;
	while (*string != '\0') { ++string; ++r; }
	return r;
}


/* 
	* Copies source str to dest
	* length(source) must be less or equal to length(dest)
	* returns dest
	* unsafe - doesn't check buffer length.
*/
char_t* strcpy(char_t* source, char_t* dest)
{
	while(*source != '\0')
	{
		*dest = *source;
		++source;
		++dest;
	}
	return dest;
}


/* 
	* initializes memory[i] with val
	* size is the amount of bytes to initialize
	* size must be less or equal to the sizeof(location)
*/
void* memset_byte(uint8_t* location, uint8_t val, uint64_t size)
{
	uint8_t* end = location + size;
    for(; location < end; ++location) {
        *location = val;
    }
    return location;
}


void* memset(void* location, uint8_t val, uint64_t size)
{
	return memset_byte((uint8_t*)location, val, size);
}


/* 
	* compares two distinct memory regions.
	* the pointers CANNOT overlap.
	* returns 0 if successful.
*/
uint8_t memcmp(const char_t* a, const char_t* b, int64_t size)
{
	--size;
	while(size >= 0 && a[size] == b[size]) {
		--size;
	}
	return boolean(size+1);
}


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
) {
	for(uint64_t i = 0; i < __n; ++i)
	{
		((uint8_t*)__dest)[i] = ((uint8_t*)__src)[i]; 
	}
    return __dest;
}