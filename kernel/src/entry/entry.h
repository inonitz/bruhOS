#pragma once
#ifndef __ENTRY__
#define __ENTRY__
#include "../../../shared/lvars.h"
#include "../../../shared/def.h"
#include "../std/io.h"



#define KERNEL_IMAGE_MEM_TYPE  BOOT_ELF_MEM_TYPE
#define KERNEL_PAGING_MEM_TYPE BOOT_PAGING_MEM_TYPE

uint64_t __mem_virtual_offset __attribute__((section(".data"))); // make sure it's in .data section.




static void __force_inline __kernel_init_basic(kernel_header_t* hdr)
{
    // change to new stack.
    __asm__ volatile("movq %0, %%rbp\n\t" : : "r"(&__kernel_stack_top));
    __asm__ volatile("movq %rbp, %rsp");

    // zero bss section.
    memset(&__kernel_bss_start, 0x00, &__kernel_bss_end - &__kernel_bss_start);
    
    // get rid of the instruction pointer mapping, of the bootloader.
    // memset(hdr->pml4.rip_ipage, 0, hdr->pml4.rip_isize * 4096);

    // set the variable responsible for the virtualOffet() [mem/map_ptr.h] function.
    __mem_virtual_offset = hdr->pml4.virtualOffset;
    

    return;
}

#endif