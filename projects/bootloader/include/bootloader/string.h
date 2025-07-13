#ifndef __BOOTLOADER_C_DEFINITION_STRING___
#define __BOOTLOADER_C_DEFINITION_STRING___
#include <bootloader/api.h>


EFI_API int32_t memcmp(
	const unsigned char* first, 
	const unsigned char* second, 
	uint64_t             amount
);


EFI_API int32_t memcmp16(
	const CHAR16* first, 
	const CHAR16* second, 
	uint64_t      amount
);


EFI_API int32_t strcmp16(
    const CHAR16* first,
    const CHAR16* second
);


int32_t strcmp(
    const char* first,
    const char* second
);


EFI_API void memsetzero(
    uint8_t* buffer,
    uint32_t size
);


EFI_API void memsetzerovoid(
    void* buffer,
    uint32_t size
);


EFI_API void memset(
    uint8_t* buffer,
    uint32_t size,
    uint8_t  value
);


EFI_API void memsetw(
    uint16_t* buffer,
    uint32_t  size,
    uint16_t  value
);


EFI_API void memcpy(
    void*    dest,
    void*    src,
    uint32_t bytes
);


#endif /* __BOOTLOADER_C_DEFINITION_STRING___ */