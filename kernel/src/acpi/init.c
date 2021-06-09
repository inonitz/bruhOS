#include "init.h"
#include "lapic.h"
#include "../device/timer.h"
#include "../interrupts/router.h"
#include "../interrupts/handlers.h"
#include "../interrupts/lapicrouter.h"
#include "../mem/map_ptr.h"



void init_acpi(ACPI_DESC_TABLE* acpi_rsdp_descriptor)
{
    KERNEL_ERROR_IF(acpi_rsdp_descriptor->extended == FALSE, NOT_IMPLEMENTED, TRUE, "ACPI 1.0 and below isn't supported. Exiting...\n");

    XSDT_t*              xsdt_table  = (XSDT_t*)acpi_rsdp_descriptor->address; // already updated to virtual offset in the kernel header
    uint32_t             entries     = (xsdt_table->hdr.length - sizeof(acpi_table_header_t)) >> 3;
    bool_t               found       = FALSE;
    bool_t               lgcy_irqs   = FALSE; // if this is true, disable the PIC.
    char_t               hdrname[5]  = "    ";
    char_t               madt_sig[4] = "APIC";
    char_t               hpet_sig[4] = "HPET";
    acpi_table_header_t* hdr;
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
    

    lapic = VIRT_TYPE(madt->lapic_begin, volatile LAPIC_t*); 
    toVirtual((void*)(uint64_t)madt->lapic_begin, 1);


    init_lapicrouter(madt, lapic->apic_id);
    set_lapic(lapic->apic_id, (void*)lapic); // set BSP.
    lapic_enable((void*)lapic);


    init_irqRouter(madt);
    HPET_init(hpet);    
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