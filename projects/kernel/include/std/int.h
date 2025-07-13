#ifndef __KERNEL_C_DEFINITION_STD_INTEGER_HEADER__
#define __KERNEL_C_DEFINITION_STD_INTEGER_HEADER__
#include <std/stdint.h>


#ifndef __KERNEL_PREBOOT_TYPES_ALREADY_DEFINED
typedef char     char_t;
typedef uint64_t physical_address;
typedef uint64_t virtual_address;
typedef uint8_t __attribute__((aligned(4096))) kernel_pagebuffer_t[4096];
#endif /* __KERNEL_PREBOOT_TYPES_ALREADY_DEFINED */


typedef struct __kernel_basic_string_structure 
{ 
    uint8_t* data;

} kernel_string_t;


typedef struct __kernel_basic_generic_buffer_structure 
{ 
    void*    data;
    uint64_t size;

} kernel_buffer_t;


typedef struct __lowhigh_word
{
    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t u16;
    };
} WORD;


typedef struct __lowhigh_dword
{
    union {
        struct {
            WORD l;
            WORD h;
        };
        uint32_t u32;
    };
} DWORD;


typedef struct __lowhigh_qword
{
    union {
        struct {
            DWORD l;
            DWORD h;
        };
        uint64_t u64;
    };
} QWORD;


typedef struct __lowhigh_pword
{
    union {
        struct {
            QWORD l;
            QWORD h;
        };
        __int128_t u128;
    };
} PWORD;


#endif /* __KERNEL_C_DEFINITION_STD_INTEGER_HEADER__ */
