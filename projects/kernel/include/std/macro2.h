#ifndef __KERNEL_C_DEFINITION_STD_NONSTANDARD_MACRO_HEADER__
#define __KERNEL_C_DEFINITION_STD_NONSTANDARD_MACRO_HEADER__


#ifdef _MSC_VER
#   define naked			  __declspec(naked)			 // tells compiler to not setup the stack for a function. that is done manually. 
#   define noret			  __declspec(noreturn)	     // function doesn't return
#   define noinline          __declspec(noinline)		 // don't inline a function
#   define restrict          __declspec(restrict)       // tells compiler to optimize functions, which return data that is not referenced in the global scope
#   define allocator         __declspec(allocator)		 // helps msvc tell that the function allocates memory and can be data-collected (runtime).
#   define use_dataseg(name) __declspec(allocate(name)) // use a specific data segment to allocate a data-type into. 
#   define align(size)		  __declspec(align(size))    //
#   define align4kb()		  __declspec(align(4096))    //
#   define noop			  __noop					 // no-op
#   define always_inline     __forceinline              // forces compiler to inline function
#   define fastcall          __fastcall                 // todo...
#   define packed	 (		__Declaration__) __pragma(pack(push, 1	 )) __Declaration__ __pragma(pack(pop)) // don't pad data types in struct
#   define pack_mems(size, __Declaration__) __pragma(pack(push, size)) __Declaration__ __pragma(pack(pop)) // pad struct members based on size
#   pragma error "THIS PACKAGE RELIES ON THE GCC COMPILER EXTENSIONS FOR CRUCIAL FEATURES, SUCH AS LOCKS, SEMAPHORES, INLINING, ALIGNMENT, etc ... PLEASE COMPILE THIS WITH GCC"

#elif defined(__clang__) || defined(__GNUC__)
#   ifndef likely
#       define likely(cond)           __builtin_expect(!!(cond), 1)
#   endif
#   ifndef unlikely
#       define unlikely(cond)         __builtin_expect(!!(cond), 0)
#   endif
#   ifndef offsetof
#       define offsetof(type, member) __builtin_offsetof(type, member)
#   endif
#   ifndef __naked
#       define __naked                __attribute__((naked))
#   endif
#   ifndef noret
#       define noret                  __attribute__((noreturn))
#   endif
#   ifndef __noinline
#       define __noinline             __attribute__((noinline))
#   endif
#   ifndef __noopt
#       define __noopt                __attribute__((optimize("O0")))
#   endif
#   ifndef allocator
#       define allocator              __attribute__((malloc))
#   endif
#   ifndef align
#       define align(size)            __attribute__((aligned(size)))
#   endif
#   ifndef align4kb
#       define align4kb               __attribute__((aligned(4096)))
#   endif
#   ifndef noop
#       define noop                   { do { ((void)0); } while(0); }
#   endif
#   ifndef __always_inline
#       define __always_inline        __attribute__((always_inline))
#   endif
#   ifndef __force_inline
#       define __force_inline         inline __always_inline
#   endif
#   ifndef fastcall
#       define fastcall               __attribute__((fastcall))
#   endif
#   ifndef interrupt
#       define interrupt              __attribute__((interrupt))
#   endif
#   ifndef pack
#       define pack                   __attribute__((packed))
#   endif
#   ifndef alignpack
#       define alignpack(size)        __attribute__((packed, aligned(size)))
#   endif
#endif


#define KERNLE_CARRAY_LENGTH(__array) (size_t)(sizeof(__array) / sizeof(*__array)) // intended for C style arrays
#define boolean(__smnt) (!!(__smnt))
#define isnull(__var)  !(!!(__var)) // if var is 0 ==> !var = 1.
#define __always_false(condition) (FALSE)
#define __always_true(condition)  (TRUE)


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


#define TRUE  !!(1u)
#define FALSE !!(0u)
#define NULL       0
#define NULLPTR    (void*)0
#define NULLSTR    (char_t*)0
#define PAGE_SIZE      4096ull
#define LOG2_PAGE_SIZE 12ull


#endif /* __KERNEL_C_DEFINITION_STD_NONSTANDARD_MACRO_HEADER__ */