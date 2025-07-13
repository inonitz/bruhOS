#ifndef __KERNEL_C_DEFINITION_ENTRY__
#define __KERNEL_C_DEFINITION_ENTRY__
#include <std/macro.h>
#include <preboot/kheader.h>
#include <preboot/linker.h>
#include <std/io.h>
#include <std/string.h>


#define KERNEL_IMAGE_MEM_TYPE  BOOT_ELF_MEM_TYPE
#define KERNEL_PAGING_MEM_TYPE BOOT_PAGING_MEM_TYPE


static __force_inline void __kernel_init_basic(kernel_header_t* hdr)
{
    // set the variable responsible for the virtualOffet() [mem/map_ptr.h] function.
    __mem_virtual_offset = hdr->memcfg.kstartv - hdr->memcfg.kstartp;
    __kheader            = hdr;    
    memset(&__kernel_bss_start, 0x00, &__kernel_bss_end - &__kernel_bss_start);

    // get rid of the instruction pointer mapping, of the bootloader. (Can't do that yet...)
    // memset(hdr->memcfg.ident_rip, 0x00, hdr->memcfg.ident_rip_size * PAGE_SIZE);
    return;
}


static __force_inline void __print_header(kernel_header_t* hdr)
{
    static const char_t* out = "\
Memory Map:\n\
        Address:    %X\n\
        Used  Size: %X\n\
        Total Size: %X\n\
        Entry Size: %X\n\
Memory Config:\n\
        physical Kernel Start: %X\n\
        virtual  Kernel Start: %X\n\
        PML4 Address:          %X\n\
        PML4 Size:             %X\n\
        Kernel Stack Address:  %X\n\
        Kernel Stack Size:     %X (bytes)\n\
        RIP Identity Mapping:  %X\n\
        RIP Page-Map Address:  %X\n\
        RIP Page-Map Size:     %X\n\
        Per-Core Stacks Start: %X\n\
        Per-Core TSS Segments: %X\n\
        Framebuffer:\n\
            Virtual Start: %X\n\
            Dimensions: %ux%u\n\
ACPI Config:\n\
        XSDT: exists ? %u ->\n\
            Physical Address: %X\n\
        Processor Count:      %u\n\
        GDT Virtual Address:  %X\n\
EFI Runtime Services Virtual Address: %X\n";

    printk("Kernel Header Contents:\n");
    printk(out,
        hdr->map.mmap,
        hdr->map.used_size,
        hdr->map.map_size,
        hdr->map.entry_size,

        hdr->memcfg.kstartp,
        hdr->memcfg.kstartv,
        hdr->memcfg.pml4,
        hdr->memcfg.pml4size,

        hdr->memcfg.krbptopv,
        hdr->memcfg.krbpsize,

        hdr->memcfg.bl_ripv,
        hdr->memcfg.bl_rip_pagetable_buf,
        hdr->memcfg.bl_rip_pagetable_bufsize,

        hdr->memcfg.kpercpu_rbpv,
        hdr->memcfg.kpercpu_tssv,

        hdr->screen.start,
        hdr->screen.dims.x,
        hdr->screen.dims.y,

        hdr->acpi.extended,
        hdr->acpi.address,
        hdr->acpi.procCount,

        hdr->memcfg.kgdtv,
        hdr->memcfg.efi_rtp
    );
}


#endif
