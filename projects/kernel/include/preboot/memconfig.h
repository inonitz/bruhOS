#ifndef __KERNEL_C_DEFINITION_PREBOOT_INITIAL_KERNEL_MEMORY_CONFIGURATION_DESCRIPTOR__
#define __KERNEL_C_DEFINITION_PREBOOT_INITIAL_KERNEL_MEMORY_CONFIGURATION_DESCRIPTOR__
#include <interrupts/tss.h>
#include <interrupts/gdtsegment.h>
#include <mem/virt/types2.h>


/* 
    This macro should not be defined here, need to find a different place for it
    probably in a header related to SMP
*/
#ifdef __KERNEL_PERCPU_STACK_SIZE
#   error "Macro __KERNEL_PERCPU_STACK_SIZE Should NOT be already defined!"
#endif
#define __KERNEL_PERCPU_STACK_SIZE 0x10000


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


typedef struct __preboot_kernel_memory_descriptor
{
    physical_address  kstartp;
    virtual_address   kstartv;
    virtual_address   krbptopv;
    virtual_address   kpercpu_rbpv;
    virtual_address   kpercpu_tssv;
    virtual_address   kgdtv;                /* convert to GDT* to use in gdt.c */
    physical_address  efi_rtp;              /* convert to Efi_runtime_services* to use */

    physical_address  pml4;                 /* Page Tables of the Higher-Half kernel address space */
    virtual_address   bl_ripv;              /* Bootloader Identity-Mapped (phys == virt) RIP Register during enablePagingAndJumpToEntry */
    genericPageTable* bl_rip_pagetable_buf; /* Buffer used for Page Tables, mapping 'bl_rip' from physical->virtual memory */
    
    uint32_t          krbpsize;
    uint32_t          pml4size;
    uint16_t          bl_rip_pagetable_bufsize;/* Size of 'bl_rip_pagetable_buf */
    uint16_t          reserved1;
    uint32_t          kendoffset;
} kernel_initial_memory_config;


typedef kernel_initial_memory_config kernel_memcfg;


#endif /* __KERNEL_C_DEFINITION_PREBOOT_INITIAL_KERNEL_MEMORY_CONFIGURATION_DESCRIPTOR__ */