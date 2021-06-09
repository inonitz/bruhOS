#include "../gpcs2/init_lib.h"
#include "../gpcs2/console/user_ifce.h"

#include "../../../shared/uefi_runtime.h"
#include "entry.h"

#include "../mem/map_ptr.h"
#include "../mem/phys/pmm.h"
#include "../mem/virt/paging.h"

#include "../interrupts/init.h"
#include "../device/timer.h"
#include "../gpcs2/console/console_instance.h"



int noret __naked _start(kernel_header_t* data)
{
    __kernel_init_basic(data);        // init stack pointer, bss, ...
    init_graphics_lib(&data->screen); // init graphics


    system_status(SUCCESS, "Kernel Stack loaded Successfully\n");
    printk("Stack top is at %p. Stack Size is %u bytes\n", data->stack.rbpAddress, data->stack.size);
    DEBUG(
        printk("Kernel loaded at %p. Kernel is at %p in virtual memory == %p\n", 
            getKernelStart() - virtualOffset(), 
            getKernelStart()
        );
    )


    // initialize page frame allocator.
    pfa_init(&data->map);
    system_status(SUCCESS, "Physical Memory Manager Initialization Succeeded\n");
    // PrintPMMInfo();


    // we already enabled paging in the bootloader. we handoff the table from the bootloader.
    handover_paging(data->pml4.pml4_addr);
    system_status(SUCCESS, "Kernel Virtual Memory Initialization Succeeded\n");


    // create_kernel_process(data->pml4.pml4_addr, BSP_ID);


    init_interrupts(&data->acpi);
    system_status(SUCCESS, "Interrupt Managers Initialized Successfully\n");


    printk("Testing Millisecond Timer for 5 seconds\n");
    count(5000);
    printk("Millisecond Timer Works!\n");
    printk("Testing Nanosecond Timer for 5 seconds\n");
    countns(5000000000llu);
    printk("Nanosecond Timer Works!\n");


    // smp
    

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
    * use I/O APIC Abstraction & HPET timer to wakeup all processors in the system
    * refactor PMM for SMP.
    * Lend each processor it's own stack and set it up for long mode (trampoline code) (and enable it)
    * Write the keyboard, Mouse and Disk Drivers
    * VMM
    * Scheduler
    * FUCKOFF IM NOT WRITING A FILESYSTEM! IM SO FUCKING DONE
*/
