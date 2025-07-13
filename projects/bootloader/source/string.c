#include <bootloader/string.h>


int32_t memcmp(
	const unsigned char* dest,
    const unsigned char* src, 
	uint64_t             amount
) {
    while ((amount--)) {
        if (*dest != *src) {
            return (*dest) - *src;
        }

        dest += 1;
        src += 1;
    }
    return 0;
}


int32_t memcmp16(
	const CHAR16* dest, 
	const CHAR16* src, 
	uint64_t      amount
) {
    while ((amount--)) {
        if (*dest != *src) {
            return (*dest) - *src;
        }

        dest += 1;
        src += 1;
    }
    return 0;
}


int32_t strcmp16(
    const CHAR16* first,
    const CHAR16* second
) {
    while (*first) {
        if (*first != *second) {
            break;
        }
        first  += 1;
        second += 1;
    }

    return *first - *second;
}


int32_t strcmp(
    const char* first,
    const char* second
) {
    while (*first) {
        if (*first != *second) {
            break;
        }
        first  += 1;
        second += 1;
    }

    return *first - *second;
}


void memsetzero(
    uint8_t* buffer,
    uint32_t bytes
) {
    for(uint32_t i = 0; i < bytes; ++i) {
        buffer[i] = 0x00;
    }
    return;
}


void memsetzerovoid(
    void*     buffer,
    uint32_t size
) {
    memsetzero((uint8_t*)buffer, size);
    return;
}


void memset(
    uint8_t* buffer,
    uint32_t size,
    uint8_t  value
) {
    for(uint32_t i = 0; i < size; ++i) {
        buffer[i] = value;
    }
    return;
}


void memsetw(
    uint16_t* buffer,
    uint32_t  size,
    uint16_t  value
) {
    for(uint32_t i = 0; i < size; ++i) {
        buffer[i] = value;
    }
    return;
}


void memcpy(
    void*    destination,
    void*    source,
    uint32_t bytes
) {
    uint8_t* dest = (uint8_t*)destination;
    uint8_t* src  = (uint8_t*)source;
    for(uint32_t i = 0; i < bytes; ++i) {
        dest[i] = src[i]; 
    }
    return;
}