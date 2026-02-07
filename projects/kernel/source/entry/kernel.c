#include <gpcs2/init.h>
#include <gpcs2/console/user_ifce.h>

#include <preboot/uefi.h>
#include <entry/entry.h>

#include <mem/map_ptr.h>
#include <mem/phys/pmm.h>
#include <mem/virt/paging.h>

#include <device/hpet.h>
#include <device/lapict.h>
#include <interrupts/init.h>
#include <smp/smp.h>

#include <std/error.h>
#include <std/error_macro.h>
#include <std/halt.h>


int __noreturn __naked _start(__unused kernel_header_t* data);
int __noreturn __abi_sysv actual_start(kernel_header_t* data);


int __noreturn __naked _start(__unused kernel_header_t* data)
{
    /* change to new stack */
    /* Marker incase page fault */
    __asm__ volatile("movq %0, %%rbp\n\t" : : "r"(&__kernel_stack_top));
    __asm__ volatile("movq %rbp, %rsp");
    __asm__ volatile("movq $0xDEADBEEFCCCCCCCC, %r15");
    __asm__ volatile("call actual_start");
    __asm__ volatile(
        "hlt"
        : // no outputs
        : // no inputs
        : // no clobbers
    );
}


static void print_kernel_header(
    IN kernel_header_t* hdr
) {
    printf("Kernel Header Contents:\n");
    printf("    Memory Map:\n      mmap       : %lp\n      used_size  : %lX\n      map_size   : %lX\n      entry_size : %lX\n\0",
        hdr->map.mmap,
        hdr->map.used_size,
        hdr->map.map_size,
        hdr->map.entry_size
    );
    printf("\
    Memory Configuration:\n\
      kstartp                 : %p\n\
      kstartv                 : %p\n\
      krbptopv                : %p\n\
      kpercpu_rbpv            : %p\n\
      kpercpu_tssv            : %p\n\
      kgdtv                   : %p\n\
      efi_rtp                 : %p\n\
      pml4                    : %p\n\
      bl_ripv                 : %p\n\
      bl_rip_pagetable_buf    : %p\n\
      krbpsize                : %x\n\
      pml4size                : %x\n\
      bl_rip_pagetable_bufsize: %x\n\
      reserved1               : %u\n\
      kendoffset              : %x\n",
        hdr->memcfg.kstartp,
        hdr->memcfg.kstartv,
        hdr->memcfg.krbptopv,
        hdr->memcfg.kpercpu_rbpv,
        hdr->memcfg.kpercpu_tssv,
        hdr->memcfg.kgdtv,
        hdr->memcfg.efi_rtp,
        hdr->memcfg.pml4,
        hdr->memcfg.bl_ripv,
        hdr->memcfg.bl_rip_pagetable_buf,
        hdr->memcfg.krbpsize,
        hdr->memcfg.pml4size,
        hdr->memcfg.bl_rip_pagetable_bufsize,
        hdr->memcfg.reserved1,
        hdr->memcfg.kendoffset
    );

    printf("\
    Framebuffer:\n\t\
      start : %p\n\t\
      dims  : (%u, %u)\n\t",
        hdr->screen.m_baseAddress,
        hdr->screen.m_width,
        hdr->screen.m_height
    );
    printf("\
    ACPI Config: %a\n\
      Physical Address: %p\n\
      Processor Count : %z\n",
        hdr->acpi.extended ? "XSDT" : "RSDT",
        hdr->acpi.address,
        hdr->acpi.procCount
    );


    return;
}



int __noreturn __abi_sysv actual_start(kernel_header_t* data)
{
    __kernel_init_basic(data);        // init stack pointer, bss, ...
    init_graphics_lib(&data->screen); // init graphics

    
    consoleClearScreen();
    system_status(KERNEL_SUCCESS, NULLSTR);
    print_kernel_header(data);
    // printk("test\n %X", 0xfffff800000);
    
    // printkcol(__KERNEL_CONSOLE_PURPLE, "Kernel Loaded Successfully at:\n       %p (Physical)\n       %p (Virtual )\n",
    //     getKernelStart() - virtualOffset(),
    //     getKernelStart()
    // );


    system_status(KERNEL_SUCCESS, NULLSTR);
    printf("Kernel Stack Located at:\n       %p (Virtual )\n       %u KiB (Size)\n",
        data->memcfg.krbptopv,
        data->memcfg.krbpsize
    );
    

    /*
        Memory Map Being printed is incorrect.
        This can mean 2 situations:
            1. (TODO) Looking at the memory directly, the kernel memory map itself is corrupted
            2. Else, The map itself isn't being processed properly, and is accessing invalid data
                This, in turn, causes invalid memory reads and possible a triple fault (if we use & record this data)
            3. Not to mention the code right after, that needs dynamic stack space...
        Moreover, It seems from testing that printk & the localConsoleWrite(...) have been tested properly,
        and should not be the root cause of many ongoing issues.
        Next Get SMP working.
    */
    pfa_init(&data->map);
    system_status(KERNEL_SUCCESS, "Physical Memory Manager Initialization Succeeded\n");


    handover_paging((PML4*)data->memcfg.pml4); // we hand-off the table initialized in the bootloader.
    system_status(KERNEL_SUCCESS, "Kernel Virtual Memory Initialization Succeeded\n");

    init_interrupts(data);
    system_status(KERNEL_SUCCESS, "Interrupt Managers Initialized Successfully\n");

    
    // pcie_init();
    // create_kernel_process(data->pml4.addressPML4, BSP_ID);


    // smp  
    // smp_init(data);


    init_lapic_timer();
    set_lapic_timer_state(LAPIC_TIMER_STATE_ONE_SHOT);
    // prepare_lapic_timer(0x3b9aca00);
    // start_lapic_timer();
    
    // scheduler









    /* 
        9. Keyboard & Mouse Device Drivers. (PS-2 && USB-C)
        10. Storage Driver. - unfortunately no...
        11. Ext2 FileSystem - unfortunately no...
        12. Scheduler (If you have time by any chance).
    */

    // setup_processes()       - TODO
    // setup_context_switch()  - TODO
    // setup_filesystem()      - TODO
    // ...?
    // setup_pci_devices() (keyboard & mouse)
    // setup_keyboard_driver();
    // setup_mouse_driver();
    system_status(KERNEL_SUCCESS, "SYSTEM HALT. REACHED END OF KERNEL");
    hang();
}


/* 
    TODO:
    * use I/O APIC Abstraction & HPET timer to wakeup all processors in the system - DONE
    * refactor PMM for SMP                                                         - DONE
    * Lend each processor it's own stack and set it up for long mode (trampoline code) (and enable it) - DONE
    * Init TSS Segments.
    * Activate all LAPIC timers
    * Write the keyboard, Mouse and Disk Drivers
    * VMM - Recheck allocation & management structures in mem/virt/...
    * Scheduler
    * FUCKOFF IM NOT WRITING A FILESYSTEM! IM SO FUCKING DONE
    

    printk("Testing Millisecond Timer for 5 seconds\n");
    count(5000);
    printk("Millisecond Timer Works!\n");
    printk("Testing Nanosecond Timer for 5 seconds\n");
    countns(5000000000llu);
    printk("Nanosecond Timer Works!\n");
*/
