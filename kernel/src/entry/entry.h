#pragma once
#ifndef __ENTRY__
#define __ENTRY__
#include "../../../shared/lvars.h"
#include "../../../shared/def.h"
#include "../std/io.h"



#define KERNEL_IMAGE_MEM_TYPE  BOOT_ELF_MEM_TYPE
#define KERNEL_PAGING_MEM_TYPE BOOT_PAGING_MEM_TYPE


// physical_address + virtualOffset = virtual_address (FOR ALL MEMORY!)
extern uint64_t         __mem_virtual_offset __attribute__((section(".data"))); 
extern kernel_header_t* __kheader            __attribute__((section(".data")));
 

static void __force_inline __kernel_init_basic(kernel_header_t* hdr)
{
    // change to new stack.
    __asm__ volatile("movq %0, %%rbp\n\t" : : "r"(&__kernel_stack_top));
    __asm__ volatile("movq %rbp, %rsp");

    
    // set the variable responsible for the virtualOffet() [mem/map_ptr.h] function.
    __mem_virtual_offset = hdr->memcfg.virtualAddress - hdr->memcfg.physicalAddress;
    __kheader            = hdr;
    // check both are correct (above) => procceed.
    
    // zero bss section.
    memset(&__kernel_bss_start, 0x00, &__kernel_bss_end - &__kernel_bss_start);


    // get rid of the instruction pointer mapping, of the bootloader. (Can't do that yet...)
    // memset(hdr->memcfg.ident_rip, 0x00, hdr->memcfg.ident_rip_size * PAGE_SIZE);
    return;
}


static void __force_inline __print_header(kernel_header_t* hdr)
{
    static char_t* out = "Memory Map:\n        Address:    %X\n        Used  Size: %X\n        Total Size: %X\n        Entry Size: %X\n    Memory Config:\n        physical Kernel Start: %X\n        virtual  Kernel Start: %X\n        PML4 Address:          %X\n        PML4 Size:             %X\n        Kernel Stack Address:  %X\n        Kernel Stack Size:     %X (bytes)\n        RIP Identity Mapping:  %X\n        RIP Page-Map Address:  %X\n        RIP Page-Map Size:     %X\n        Per-Core Stacks Start: %X\n        Per-Core TSS Segments: %X\n    Framebuffer:\n        Virtual Start: %X\n        Dimensions: %ux%u\n    ACPI Config:\n        XSDT(? => bool(%u)) Physical Address: %X\n        Processor Count:       %u\n\n    GDT Virtual Address:       %X\n    EFI Runtime Services Virtual Address: %X\n";
    printk("Kernel Header Contents:\n");
    printk(out,
        hdr->map.mmap,
        hdr->map.used_size,
        hdr->map.map_size,
        hdr->map.entry_size,

        hdr->memcfg.physicalAddress,
        hdr->memcfg.virtualAddress,
        hdr->memcfg.addressPML4,
        hdr->memcfg.sizePML4,

        hdr->memcfg.rbpAddress,
        hdr->memcfg.rbpSize,

        hdr->memcfg.ident_rip,
        hdr->memcfg.ident_rip_addr,
        hdr->memcfg.ident_rip_size,

        hdr->memcfg.percpuRBP,
        hdr->memcfg.percpuTSS,

        hdr->screen.start,
        hdr->screen.dims.x,
        hdr->screen.dims.y,

        hdr->acpi.extended,
        hdr->acpi.address,
        hdr->acpi.procCount,

        hdr->gdt,
        hdr->efi_rt
    );
}

#endif