#include "../gpcs2/init_lib.h"
#include "../gpcs2/console/user_ifce.h"

#include "../../../shared/uefi.h"
#include "entry.h"

#include "../mem/map_ptr.h"
#include "../mem/phys/pmm.h"
#include "../mem/virt/paging.h"

#include "../interrupts/init.h"
#include "../device/hpet.h"
#include "../device/lapict.h"
#include "../smp/smp.h"



int noret __naked _start(kernel_header_t* data)
{
    __kernel_init_basic(data);        // init stack pointer, bss, ...
    init_graphics_lib(&data->screen); // init graphics

    consoleClearScreen();
    system_status(SUCCESS, NULL);
    printk("Kernel Loaded Successfully at:\n       %p (Physical)\n       %p (Virtual )\n",
        getKernelStart() - virtualOffset(),
        getKernelStart()
    );


    system_status(SUCCESS, NULL);
    printk("Kernel Stack Located at:\n       %p (Virtual )\n       %u KiB (Size)\n",
        data->memcfg.rbpAddress,
        data->memcfg.rbpSize
    );
    

    pfa_init(&data->map);
    system_status(SUCCESS, "Physical Memory Manager Initialization Succeeded\n");


    handover_paging(data->memcfg.addressPML4); // we hand-off the table initialized in the bootloader.
    system_status(SUCCESS, "Kernel Virtual Memory Initialization Succeeded\n");

    init_interrupts(data);
    system_status(SUCCESS, "Interrupt Managers Initialized Successfully\n");

    
    // pcie_init();
    // create_kernel_process(data->pml4.addressPML4, BSP_ID);


    // smp  
    smp_init(data);


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
    system_status(SUCCESS, "SYSTEM HALT. REACHED END OF KERNEL");
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
