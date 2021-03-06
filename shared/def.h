#pragma once
#ifndef __KERNEL_TYPES__
#define __KERNEL_TYPES__
#include <stdint.h>



#define IN 
#define OUT
#define BOOT_ELF_MEM_TYPE    0x80696969
#define BOOT_PAGING_MEM_TYPE 0x80AAAAAA
#define BOOT_MEMORY_MAP_TYPE 0xBEEFBEEF
#define PERCPU_STACK_SIZE    0x8000


typedef unsigned long long size_t;
typedef uint64_t           physical_address;
typedef uint64_t           virtual_address;

#ifndef __UINT8_AS_BOOL
#define __UINT8_AS_BOOL
    typedef uint8_t bool_t;
#endif


typedef struct __attribute__((packed)) __root_system_description_pointer_descriptor
{
    char     signature[8];
    uint8_t  checksum;
    char     OEMID[6];
    uint8_t  revision;
    uint32_t rsdtAddress;
} RSDPdescriptor10;


typedef struct __attribute__((packed)) __root_system_description_pointer_descriptor20
{
    RSDPdescriptor10 first;
    uint32_t         length;
    uint64_t         xsdtAddress;
    uint8_t          exchecksum;
    uint8_t          reserved[3];
} RSDPdescriptor20;


typedef struct __attribute__((packed)) __multiple_apic_description_table
{
    uint8_t  signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    uint8_t  OEM_ID[6];
    uint64_t OEM_TABLE_ID;
    uint32_t OEM_REVISION;
    uint32_t creator_id;
    uint32_t creator_rev;
    uint32_t LICA; // local interrupt controller address - The Address in memory (physical) of the BSP LAPIC
    uint32_t flags;

} MADT;


typedef struct __efi_memory_descriptor {
    uint32_t          memtype;
    uint32_t          pad;
    physical_address  physAddr;
    virtual_address   virtAddr;
    uint64_t          pages;
    uint64_t          attribute;
} efi_mem_descriptor;


typedef struct __efi_memory_map {
    efi_mem_descriptor*    mmap;
    uint64_t               used_size;
    uint64_t			   map_size;
    uint64_t			   entry_size;
} efi_memory_map;


#ifndef __GRAPHICS_TYPES__
// to make sure that whenever we include the graphics library, we won't have conflicting typedef's
// include this file SECOND!!!
typedef struct __attribute__((packed)) __vec2us {
	uint16_t x, y;
} vec2us;


typedef struct __attribute__((packed, aligned(16))) __framebuffer_t {
	void*   start;
	vec2us  dims;
    // bytes per pixel in uefi GOP is automatically 4 (byte for each channel), so I'm not using it.
} framebuffer_t;

#endif


typedef struct __advanced_configuration_and_power_interface_config
{
    uint64_t        address;  
    bool_t          extended;   // address == RSDT / XSDT
    uint8_t         padding[3];
    uint32_t        procCount;  // cores on the CPU
} acpiCfg;


typedef struct __attribute__((packed, aligned(8))) __memory_related_data
{
    uint64_t physicalAddress; // the location in which the kernel was allocated.
    uint64_t virtualAddress;  // the location in which the kernel is mapped in the virtual address space.
    void*    addressPML4;     // the physical address of the page tables in memory. CR3 register Value, if you will
    uint64_t sizePML4;        // the size in bytes (total) of the paging tables used.
    uint64_t rbpAddress;      // the location of the kernel stack in virtual Memory.
    uint64_t rbpSize;         // the size of the kernel stack.
    void*    ident_rip;       // the address that was identity mapped. look below for more info.
    void*    ident_rip_addr;  // the physical address of the pages used for mapping RIP in the bootloader. kernel should remove these pages. 
    uint64_t ident_rip_size;  // the amount of pages that were used for the RIP pointer.
    void*    percpuRBP;       // the (physical) start of all per-cpu stacks. every Stack is 32KiB. (macro defined at top)
    void*    percpuTSS;       // the (physical) start of all per-cpu TSS's.
} memConfig;


typedef struct __kernel_header_t {
	efi_memory_map        map;
    memConfig             memcfg;
	framebuffer_t         screen;
    acpiCfg               acpi;
    void*                 gdt;    // convert to GDT* to use in gdt.c
    void*                 efi_rt; // convert to Efi_runtime_services* to use.
} kernel_header_t;

typedef int (*__kernel_entry)(kernel_header_t header) __attribute__((noreturn)) __attribute__((sysv_abi));

#endif