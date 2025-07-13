#ifndef __KERNEL_C_DEFINITION_PREBOOT_KERNEL_HEADER_TYPE_AND_ENTRY__
#define __KERNEL_C_DEFINITION_PREBOOT_KERNEL_HEADER_TYPE_AND_ENTRY__


#ifdef __KERNEL_USE_NO_DEPENDENCY_VERSION
#include <std/static_assert.h>

KERNEL_STATIC_ASSERT(
    sizeof(unsigned long long) == 8, 
    kHeaderMakeSureUnsignedLongLongIsntAccidentlyLessThan64Bits, 
    "kheader.h -> Pointer Size validity check"
)

typedef unsigned long long physical_address;
typedef unsigned long long virtual_address;
typedef int kernel_header_t[168]; /* Inline definition of same size without any dependency */
typedef int (*__kernel_entry)(kernel_header_t header) __attribute__((noreturn)) __attribute__((sysv_abi));

#elif !defined(__KERNEL_HEADER_TYPE_ALREADY_DEFINED)
#   include <preboot/memmap.h>
#   include <preboot/memconfig.h>
#   include <preboot/framebuffer.h>
#   include <preboot/acpi.h>
#   include <std/static_assert.h>
#   define __KERNEL_HEADER_TYPE_ALREADY_DEFINED


typedef struct __kernel_header_t {
	efi_memory_map    map;
    kernel_memcfg     memcfg;
	framebuffer_t     screen;
    ACPIinitialConfig acpi;
} kernel_header_t;


typedef int (*__kernel_entry)(kernel_header_t header) __attribute__((noreturn)) __attribute__((sysv_abi));


KERNEL_STATIC_ASSERT(offsetof(kernel_header_t, map) == 0 && sizeof(efi_memory_map) == 32, 
    __kernelHeaderType_MemoryMap_OffsetAndSizeExpectationCheck, 
    "Offset & Type Size Check - Headers Shared across modules Must conform to the same expectations, \
    even under different toolchains\n"
);
KERNEL_STATIC_ASSERT(offsetof(kernel_header_t, memcfg) == 32 && sizeof(kernel_memcfg) == 96, 
    __kernelHeaderType_KernelMemoryConfig_OffsetAndSizeExpectationCheck, 
    "Offset & Type Size Check - Headers Shared across modules Must conform to the same expectations, \
    even under different toolchains\n"
);
KERNEL_STATIC_ASSERT(offsetof(kernel_header_t, screen) == 128 && sizeof(framebuffer_t) == 16, 
    __kernelHeaderType_ScreenFramebuffer_OffsetAndSizeExpectationCheck, 
    "Offset & Type Size Check - Headers Shared across modules Must conform to the same expectations, \
    even under different toolchains\n"
);
KERNEL_STATIC_ASSERT(offsetof(kernel_header_t, acpi) == 144 && sizeof(ACPIinitialConfig) == 16, 
    __kernelHeaderType_ACPI_Datatable_OffsetAndSizeExpectationCheck, 
    "Offset & Type Size Check - Headers Shared across modules Must conform to the same expectations, \
    even under different toolchains\n"
);


#endif /* __KERNEL_USE_NO_DEPENDENCY_VERSION */


#endif /* __KERNEL_C_DEFINITION_PREBOOT_KERNEL_HEADER_TYPE_AND_ENTRY__ */
