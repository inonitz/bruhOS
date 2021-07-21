#pragma once
#ifndef __STD_INT__
#define __STD_INT__
#include <stdint.h>



#ifdef _MSC_VER
#define naked			  __declspec(naked)			 // tells compiler to not setup the stack for a function. that is done manually. 
#define noret			  __declspec(noreturn)	     // function doesn't return
#define noinline          __declspec(noinline)		 // don't inline a function
#define restrict          __declspec(restrict)       // tells compiler to optimize functions, which return data that is not referenced in the global scope
#define allocator         __declspec(allocator)		 // helps msvc tell that the function allocates memory and can be data-collected (runtime).
#define use_dataseg(name) __declspec(allocate(name)) // use a specific data segment to allocate a data-type into. 
#define align(size)		  __declspec(align(size))    //
#define align4kb()		  __declspec(align(4096))    //
#define noop			  __noop					 // no-op
#define always_inline     __forceinline              // forces compiler to inline function
#define fastcall          __fastcall                 // todo...
#define packed	 (		__Declaration__) __pragma(pack(push, 1	 )) __Declaration__ __pragma(pack(pop)) // don't pad data types in struct
#define pack_mems(size, __Declaration__) __pragma(pack(push, size)) __Declaration__ __pragma(pack(pop)) // pad struct members based on size
#pragma error "THIS PACKAGE RELIES ON THE GCC COMPILER EXTENSIONS FOR CRUCIAL FEATURES, SUCH AS LOCKS, SEMAPHORES, INLINING, ALIGNMENT, etc ... PLEASE COMPILE THIS WITH GCC"


#elif __clang__
    // havent used clang yet
    // not much different from gcc tho

    
#elif __GNUC__
#define likely(cond)           __builtin_expect(!!(cond), 1)
#define unlikely(cond)         __builtin_expect(!!(cond), 0)
#define offsetof(type, member) __builtin_offsetof(type, member)
#define __naked                __attribute__((naked))
#define noret                  __attribute__((noreturn))
#define __noinline             __attribute__((noinline))
#define __noopt                __attribute__((optimize("O0")))
#define restrict 
#define allocator              __attribute__((malloc))
#define align(size)            __attribute__((aligned(size)))
#define align4kb               __attribute__((aligned(4096)))
#define noop                   { do {} while(0); }
#ifndef __always_inline
    #define __always_inline    __attribute__((always_inline))
#endif
#define __force_inline         inline __always_inline
#define fastcall               __attribute__((fastcall))
#define interrupt              __attribute__((interrupt))
#define pack                   __attribute__((packed))
#define alignpack(size)        __attribute__((packed, aligned(size)))
#endif



// useful error types
#define SUCCESS			 0u
#define FAILURE			 1u
#define FAILURE_SEVERE   2u
#define FAILURE_CRITICAL 3u

#define MISSING_DATA     4u
#define BAD_DATA         5u
#define INVALID_INPUT    6u
#define FILE_NOT_FOUND   8u
#define BAD_INPUT        9u
#define BUFFER_TOO_SMALL 10u

#define NOT_IMPLEMENTED  18u
#define INVALID_ERROR    19u


#define IN 
#define OUT
#define OPTIONAL
#define logical  // the address is in the virtual address space and is at physical_address + known_offset.
#define physical // the address is not in the virtual address space.
#define virtual  // the address is in the virtual address space in a mapping that is not logical, probe the PML4 Table to find the physical address.
#define TRUE  !!(1u)
#define FALSE !!(0u)
#define NULL       0
#define nullptr    (void*)0
#define nullstr    (char_t*)0
#define PAGE_SIZE      4096ull
#define LOG2_PAGE_SIZE 12ull


#define LENGTH(__array) (size_t)(sizeof(__array) / sizeof(*__array)) // intended for C style arrays
#define boolean(__smnt) (!!(__smnt))
#define isnull(__var)  !(!!(__var)) // if var is 0 ==> !var = 1.
#define __always_false(condition) (FALSE)
#define __always_true(condition)  (TRUE)


// useful types (already defined in stdint.h)
// typedef signed char        char8_t ;
// typedef signed char        int8_t  ;
// typedef short              char16_t;
// typedef short              int16_t ;
// typedef signed int         int32_t ;
// typedef signed long long   int64_t ;
// typedef unsigned char      uchar8_t;
// typedef unsigned char      uint8_t ;
// typedef unsigned short     uint16_t;
// typedef unsigned int       uint32_t;
// typedef unsigned long long uint64_t;
#ifndef __UINT8_AS_BOOL
#define __UINT8_AS_BOOL
    typedef unsigned char  bool_t;
#endif
typedef unsigned char      uchar8_t;
typedef signed char        char8_t;
typedef char               char_t;
typedef unsigned long long size_t;
typedef uint64_t		   physical_address;
typedef uint64_t		   virtual_address;


// data structures
typedef struct             string { uint8_t* data;				} string;
typedef struct             buffer { void*    data; size_t size; } buffer;


typedef struct __lowhigh_word
{
    union {
        struct pack {
            uint8_t l;
            uint8_t h;
        };
        uint16_t u16;
    };
} WORD;


typedef struct __lowhigh_dword
{
    union {
        struct pack {
            WORD l;
            WORD h;
        };
        uint32_t u32;
    };
} DWORD;


typedef struct __lowhigh_qword
{
    union {
        struct pack {
            DWORD l;
            DWORD h;
        };
        uint64_t u64;
    };
} QWORD;


#ifdef __x86_64__
typedef struct __lowhigh_pword
{
    union {
        struct pack {
            QWORD l;
            QWORD h;
        };
        __int128_t u128;
    };
} PWORD;
#endif


#endif