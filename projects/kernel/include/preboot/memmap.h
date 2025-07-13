#ifndef __KERNEL_C_DEFINITION_PREBOOT_MEMORY_MAP__
#define __KERNEL_C_DEFINITION_PREBOOT_MEMORY_MAP__
#include <std/stdint.h>
#include <preboot/mem_macro.h>


#ifndef __KERNEL_PREBOOT_TYPES_ALREADY_DEFINED
#   define __KERNEL_PREBOOT_TYPES_ALREADY_DEFINED
typedef char     char_t;
typedef uint64_t physical_address;
typedef uint64_t virtual_address;
typedef uint8_t __attribute__((aligned(4096))) kernel_pagebuffer_t[4096];
#endif /* __KERNEL_PREBOOT_TYPES_ALREADY_DEFINED */



typedef enum {
    allocateAnyPages,
    allocateMaxAddress,
    allocateAddress,
    maxAllocateType
} efi_allocation_type;


typedef enum {
    efiReservedMemoryType,
    efiLoaderCode,
    efiLoaderData,
    efiBootServicesCode,
    efiBootServicesData,
    efiRuntimeServicesCode,
    efiRuntimeServicesData,
    efiConventionalMemory,
    efiUnusableMemory,
    efiACPIReclaimMemory,
    efiACPIMemoryNVS,
    efiMemoryMappedIO,
    efiMemoryMappedIOPortSpace,
    efiPalCode,
    efiMaxMemoryType
} efi_memory_type;


typedef struct __efi_memory_descriptor {
    uint32_t         memtype;
    uint32_t         pad;
    physical_address physAddr;
    virtual_address  virtAddr;
    uint64_t         pages;
    uint64_t         attribute;
} efi_mem_descriptor;


typedef struct __efi_memory_map {
    efi_mem_descriptor*    mmap;
    uint64_t               used_size;
    uint64_t			   map_size;
    uint64_t			   entry_size;
} efi_memory_map;


#endif /* __KERNEL_C_DEFINITION_PREBOOT_MEMORY_MAP__ */