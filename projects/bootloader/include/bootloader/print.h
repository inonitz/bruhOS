#ifndef __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__
#define __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__
#include <bootloader/api.h>


/* 
	* prints string to UEFI->Bootservices->ConOut
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
EFI_API void vprintf(const CHAR16* format, va_list args);


EFI_API void printf(const CHAR16* format, ...);


#ifndef printb
#	define printb(FORMAT_STRING, ...) printf((const CHAR16*)FORMAT_STRING, ##__VA_ARGS__);
#endif


// void     printf_align_format(uint16_t padding_amount);
// uint16_t printf_get_align(void);


#endif /* __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__ */
