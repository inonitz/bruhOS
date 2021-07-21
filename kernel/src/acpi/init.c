#include "init.h"
#include "lapic.h"
#include "../smp/smp.h"
#include "../interrupts/handlers.h"
#include "../interrupts/router.h"
#include "../device/hpet.h"
#include "../mem/map_ptr.h"



void init_acpi(acpiCfg* acpi_config)
{
    KERNEL_ERROR_IF(acpi_config->extended == FALSE, NOT_IMPLEMENTED, TRUE, "ACPI 1.0 and below isn't supported. Exiting...\n");

    XSDT_t*              xsdt_table  = (XSDT_t*)acpi_config->address; // already updated to virtual offset in the kernel header
    uint32_t             entries     = (xsdt_table->hdr.length - sizeof(acpi_table_header_t)) >> 3;
    bool_t               found       = FALSE;
    bool_t               lgcy_irqs   = FALSE; // if this is true, disable the PIC.
    char_t               hdrname[5]  = "    ";
    char_t               madt_sig[4] = "APIC";
    char_t               hpet_sig[4] = "HPET";
    acpi_table_header_t* hdr;
    paging_flags_t       flags;
    // uint64_t             tmp        = 0;
    // int_cnt_header*      controller = 0; 

    volatile MADT_t*  madt; uint32_t idxreg0 = UINT32_MAX;
    volatile HPET_t*  hpet; uint32_t idxreg1 = UINT32_MAX;
    volatile LAPIC_t* lapic;


    // find MADT & HPET
    DEBUG( printk("PARSING ACPI RXDT TABLE...\n"); )
    for(uint8_t i = 0; i < entries; ++i)
    {
        hdr   = VIRT_TYPE(xsdt_table->table[i], acpi_table_header_t*);
        memcpy(hdrname, hdr->signature, 4);
        DEBUG( printk("    Found Table %s\n", hdrname); )

        found = !memcmp(hdr->signature, madt_sig, 4); idxreg0 = found * i + idxreg0 * !found;
        found = !memcmp(hdr->signature, hpet_sig, 4); idxreg1 = found * i + idxreg1 * !found;
    }

    KERNEL_ERROR_IF(idxreg0 == UINT32_MAX, MISSING_DATA, TRUE, "    ACPI MADT       Table Not Found.");
    KERNEL_ERROR_IF(idxreg1 == UINT32_MAX, MISSING_DATA, TRUE, "    ACPI HPET Timer Table Not Found.");
    madt = VIRT_TYPE(xsdt_table->table[idxreg0], volatile MADT_t*);
    hpet = VIRT_TYPE(xsdt_table->table[idxreg1], volatile HPET_t*);
    KERNEL_ERROR_IF(tableChecksum(&((MADT_t*)madt)->hdr) != 1, BAD_DATA, TRUE, "    ACPI MADT Table is invalid.");
    KERNEL_ERROR_IF(tableChecksum(&((HPET_t*)hpet)->hdr) != 1, BAD_DATA, TRUE, "    ACPI HPET Table is invalid.");
    

    /* For SMP Use, we need to Set the MADT that the smp_init() will use */
    set_madt((MADT_t*)madt);


    /* Map LAPIC, init BSP lapic */
    lapic = VIRT_TYPE(madt->lapic_begin, volatile LAPIC_t*); 
    toVirtualFlag((void*)(uint64_t)madt->lapic_begin, 1, 0x8000000000000013); //  1 000000000000000000000000000000000000000000000000000 000 0 0 00 1 0 0 1 1

    lapic_set_base((uint64_t)lapic);
    lapic_init();

    /* Initialize the I/O APIC & IDT Manager */
    init_irqRouter(madt);
    
    
    /* Initialize the HPET Timer */
    HPET_init(hpet);    


    /* Debugging Info about irqRouter */
    // irqRouterDescription();
    // ioAPICDescription(0);
}





// // print detailed description of certain ACPI tables in the XSDT table.
// INT_SRC_OVERRIDE_t* a;
// LNMI_t*             b;
// tmp        = madt->hdr.length - sizeof(acpi_table_header_t) - 8;
// controller = &madt->intcont[0];
// for(uint16_t offset = 0; offset < tmp; offset += controller->length)
// {
//     switch(controller->int_type)
//     {
//         case 0:
//             printk("LAPIC: <%u (proc apic_id), %b (flags)>\n", ((LAPIC_t*)controller)->apic_id, ((LAPIC_t*)controller)->flags);
//             break;
//         case 1:
//             printk("IOAPIC: <%u (apic_id), %p (addr), %u (GSI offset)>\n", ((IOAPIC_t*)controller)->io_apic_id, ((IOAPIC_t*)controller)->address, ((IOAPIC_t*)controller)->GSIbase);
//             break;
//         case 2:
//             a = (INT_SRC_OVERRIDE_t*)controller;
//             printk("INT_SRC_OVERRIDE: <%u (BUS), %u (IRQ), %u (GSI), %b (flags)>\n", a->bus_src, a->irq_src, a->GSI, a->flags);
//             break;
//         case 4:
//             b = (LNMI_t*)controller;
//             printk("NMI: <%u (APIC_ID), %u (LINT), %b (flags)>\n", b->pid, b->LINT, b->flags);
//             break;
//         default:
//             printk("Found Table of Type: %u\n", controller->int_type);
//             break;
//     }
//     controller = (int_cnt_header*)((uint8_t*)controller + controller->length);
// }
// printk("%u %p %p\n", hpet->flags, hpet->tableaddr.address, lapic);


// timer_prepare(1000000000);
// timer_start();
// c = (((uint64_t)timer.registers->main_counter_reg.cnt_upper) << 32) | timer.registers->main_counter_reg.cnt_lower;
// printk("reg is at: %u\n", c);
// for(volatile uint32_t i = 0; i < 10000000; ++i);;
// c = (((uint64_t)timer.registers->main_counter_reg.cnt_upper) << 32) | timer.registers->main_counter_reg.cnt_lower;
// printk("reg is at: %u\n", c);


// __asm__("int $0x20");
// __asm__("int $0x21");
// printk_align_format(20);
// for(uint8_t i = 8; i > 0; --i) {
//     printk("%b", lapic_read_reg(a, LAPIC_INSERVICE0 + (i-1) * 0x10));
// }
// printk("\nISR: ");
// for(uint8_t i = 8; i > 0; --i) {
//     printk("%b", lapic_read_reg(a, LAPIC_INTREQ0 + (i-1) * 0x10));
// }
// printk("irq0: %b\nirq8: %b\n", ioapic_read_reg((uint64_t)bruh->address, IOREDTBL_OFFSET_LOW(2)), ioapic_read_reg((uint64_t)bruh->address, IOREDTBL_OFFSET_LOW(8)));
// IDT* aa = getIDT();
// uint64_t lower, upper;
// for(uint8_t i = 0; i < 40; ++i)
// {
//     printk("offset: {0, 1, 2} => {%u, %u, %u} selector: %x istoff: %u attributes: %b\n", 
//         (uint32_t)aa[i]->offset0, 
//         (uint32_t)aa[i]->offset1, 
//         (uint32_t)aa[i]->offset2, 
//         (uint32_t)aa[i]->selector, 
//         (uint8_t)aa[i]->type_attr
//     );
// }