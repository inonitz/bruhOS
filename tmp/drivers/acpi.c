#include "acpi.h"
#include "lapic.h"



static bool_t tableChecksum(acpi_table_header_t* tableHeader)
{
    uint8_t sum = 0;
    for (int i = 0; i < tableHeader->length; i++)
    {
        sum += ((char_t*)tableHeader)[i];
    }
    return sum == 0;
}


static uint64_t calculateXSDTtableSize(acpi_table_header_t* tbl_hdr)
{
    return (tbl_hdr->length - sizeof(acpi_table_header_t)) >> 3;
}


void init_acpi(ROXSD_t* acpi_rsdp_descriptor)
{
    if(acpi_rsdp_descriptor->extended == FALSE) {
        printk("ACPI 1.0 and below isn't supported. Exiting...\n");
        hang();
    }

    // find the MADT table
    XSDT_t*              xsdt_table = (XSDT_t*)acpi_rsdp_descriptor->address;
    uint32_t             entries    = (xsdt_table->hdr.length - sizeof(acpi_table_header_t)) >> 3;
    uint64_t             tmp        = 0;
    uint64_t             apicregptr = 0;
    bool_t               found      = FALSE;
    // bool_t               lgcy_irqs  = FALSE_T;
    char_t               hdrname[5] = "    ";
    int_cnt_header*          controller;
    acpi_table_header_t* hdr;


    for(uint8_t i = 0; i < entries; ++i)
    {
        hdr   = (acpi_table_header_t*)xsdt_table->table[i];
        memcpy(hdrname, hdr->signature, 4);
        printk("  Found Table %s\n", hdrname);
    }
    for(uint8_t i = 0; i < entries && !found; ++i)
    {
        hdr   = (acpi_table_header_t*)xsdt_table->table[i];
        found = !memcmp(hdr->signature, "APIC", 4);
    }
    if(!found) {
        printk("Couldn't find an ACPI MADT Table to parse. exiting... \n");
        hang();
    }


    MADT_t* madt = (MADT_t*)hdr;
    if(!tableChecksum(&madt->hdr))
    {
        printk("ACPI MADT Table is invalid.");
        hang();
    }
    INT_SRC_OVERRIDE_t* a;
    LNMI_t*              b;

    // lgcy_irqs    = madt->flags & 1;
    controller   = &madt->intcont[0];


    // search for processor local apic structures. 
    tmp = madt->hdr.length - sizeof(acpi_table_header_t) - 8;
    for(uint16_t offset = 0; offset < tmp; offset += controller->length)
    {
        switch(controller->int_type)
        {
            case 0:
                printk("LAPIC:\n  processor apic id: %u\n  flags: %b\n", ((LAPIC_t*)controller)->apic_id, ((LAPIC_t*)controller)->flags);
                break;
            case 1:
                printk("IOAPIC:\n  apic id: %u\n  addr: %p\n  GSI base: %u\n", ((IOAPIC_t*)controller)->io_apic_id, ((IOAPIC_t*)controller)->address, ((IOAPIC_t*)controller)->GSIbase);
                apicregptr = (uint64_t)((IOAPIC_t*)controller)->address;
                break;
            case 2:
                a = (INT_SRC_OVERRIDE_t*)controller;
                printk("INT_SRC_OVERRIDE:\n  BUS: %u\n  IRQ: %u\n  IRQ SIGNALS TO GSI %u\n  flags: %b\n", a->bus_src, a->irq_src, a->GSI, a->flags);
                break;
            case 4:
                b = (LNMI_t*)controller;
                printk("NMI:\n  APIC_ID: %u\n  LINT: %u\n  flags: %b \n", b->pid, b->LINT, b->flags);
                break;
            default:
                printk("Found Table of Type: %u\n", controller->int_type);
                break;
        }
        controller = (int_cnt_header*)((uint8_t*)controller + controller->length);
    }
    
    // i/o apic
    uint32_t maxirqs = ioapic_max_int(apicregptr);
    printk("\nThe current IO-APIC Table handles %u interrupts\n", maxirqs);
    read_register_table((uint64_t)apicregptr);

    set_lapic_address(madt->lapic_begin);
    cpu_startup_init(0);
}
