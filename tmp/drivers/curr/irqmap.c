#include "irqmap.h"
#include "pic.h"


static ISA_TO_GSI ISAmap;
static GSI_TO_IRQ GSImap;
 

void irqmap_info()
{
    const char* trigger_str[4] = 
    {
        "CONFORMING     ",
        "Edge  triggered",
        "",
        "Level triggered"
    };
    const char* polarity_str[4] = 
    {
        "CONFORMING ",
        "Active High",
        ""
        "Active Low "
    };
    
    
    printk_align_format(2);
    for(uint8_t i = 0; i < 30; ++i)
    {
        printk("GSI %u (IOAPIC_ID %u, register %u) -> IRQ %u. %s, %s\n",
            i,
            GSImap[i].apicid,
            GSImap[i].apicreg,
            GSImap[i].irqn,
            trigger_str[GSImap[i].trigmode],
            polarity_str[GSImap[i].polarity]
        );
    }
}


void ioapic_init(MADT_t* madt, bool_t dis_pic)
{
    int_cnt_header* table     = &madt->intcont[0];
    uint64_t        madt_size = madt->hdr.length - sizeof(acpi_table_header_t) - 8;
    uint32_t        reg       = 0;
    uint16_t        ioapic    = 0;

    INT_SRC_OVERRIDE_t* isop;
    IOAPIC_t*           ioapicp;
    intredirectEntry_t  ioapic_reg;


    if(dis_pic) {
        disable_pic(); // disable legacy pic controller
    }


    // init tables.
    memset(&GSImap, 0, sizeof(GSI_TO_IRQ));
    memset(&ISAmap, 0, sizeof(ISA_TO_GSI));
    for(uint8_t i = 0; i < 16; ++i)
    {
        ISAmap[i] = (itgentry_t){
            .enabled  = 0,
            .reserved = 0,
            .polarity = ACTIVE_HIGH,
            .trigmode = EDGE,
            .gsin     = i
        };
    }

    // find all Interrupt Source Overrides and change the appropriate isa_to_gsi entries
    for(uint16_t offset = 0; offset < madt_size; offset += table->length)
    {
        if(table->int_type == INT_CONTROLLER_INTERRUT_SOURCE_OVERRIDE)
        {
            isop = (INT_SRC_OVERRIDE_t*)table;
            ISAmap[isop->irq_src] = (itgentry_t){
                .enabled  = 1,
                .reserved = 0,
                .polarity = (isop->flags & 0x3) + !(isop->flags & 0x3) * ACTIVE_HIGH,
                .trigmode = (isop->flags & 0x3) + !(isop->flags & 0x3) * EDGE,
                .gsin     = isop->GSI
            };
        }
        ioapic += table->int_type == INT_CONTROLLER_IO_APIC;
        table = (int_cnt_header*)((uint8_t*)table + table->length);
    }


    // for(uint8_t i = 0; i < 16; ++i)
    // {
    //     printk("%b\n", ISAmap[i]);
    // }


    // transfer info from iso_to_gsi
    for(uint8_t i = 0; i < 16; ++i)
    {
        if(ISAmap[i].enabled) {
            GSImap[ISAmap[i].gsin] = (gtientry_t){ 
                .enabled  = ISAmap[i].enabled, 
                .polarity = ISAmap[i].polarity,
                .trigmode = ISAmap[i].trigmode, 
                .irqn     = i
                // rest of the members stay 0, for now.
            };
        }
    }
    
    // printk("\nGSI:\n");
    // for(uint8_t i = 0; i < 20; ++i)
    // {
    //     printk("%b\n", GSImap[i]);
    // }


    // find all I/O ACPI tables, and initialize the gsi entries that we find.
    table = &madt->intcont[0];
    for(uint16_t offset = 0; offset < madt_size; offset += table->length)
    {
        if(table->int_type == INT_CONTROLLER_IO_APIC) // we found an I/O APIC Table
        {
            ioapicp = (IOAPIC_t*)table;
            reg     = 1 + (ioapic_read_reg(ioapicp->address, IOAPICVER_REG) >> 16); // maximum irq's

            // set each gsi entry using the ioapic redirection table registers
            for(uint8_t i = 0; i < reg; ++i)
            {
                ioapic_reg = (intredirectEntry_t) { 
                    .lower = ioapic_read_reg(ioapicp->address, IOREDTBL_OFFSET_LOW(i)),
                    .upper = ioapic_read_reg(ioapicp->address, IOREDTBL_OFFSET_HIGH(i)) 
                };
                ioapic_reg.u64 |= (1 << 16);
                // ioapic_reg.bitmask = 1; // disable this irq and write it to the actual register

                ioapic_write_reg(ioapicp->address, IOREDTBL_OFFSET_LOW(i), ioapic_reg.lower);
                GSImap[i + ioapicp->GSIbase].apicid      = ioapicp->io_apic_id;
                GSImap[i + ioapicp->GSIbase].apicreg     = i;
                GSImap[i + ioapicp->GSIbase].apicaddress = ioapicp->address;
            }
        }
        table = (int_cnt_header*)((uint8_t*)table + table->length);
    }
}


void install_irq_vector(
    IN uint8_t            irqn, 
    IN IDT_ENTRY_PRIORITY priority,
    IN enum pin_polarity  activate_hl, 
    IN enum triggerMode   triggerOnEdgeOrLevel,
    IN IDT_ACCESS         idt_gate_flags
)
{
    uint8_t idx = request_vector_min_pri(priority);
    if(unlikely(idx == 0)) printk("couldn't find a suitable idt gate entry for the irqn.\n");
    
    // when you need to implement pci, add a PCImap struct uptop so you can map PCI devices (not only ISA).
    
    if(irqn > 15) { // probably PCI device 
        printk("PCI devices can't be initialized yet - not supported\n");
        return;
    }

    // this is probably an ISA device
    bool_t  entry_empty = !ISAmap[irqn].enabled;
    uint8_t gsidx       = ISAmap[irqn].gsin;

    if(entry_empty)
    {
        ISAmap[irqn].polarity = activate_hl;
        ISAmap[irqn].trigmode = triggerOnEdgeOrLevel;
        ISAmap[irqn].enabled  = TRUE;
    }
    GSImap[gsidx].irqn     = idx;
    GSImap[gsidx].polarity = activate_hl;
    GSImap[gsidx].trigmode = triggerOnEdgeOrLevel;
    GSImap[gsidx].enabled  = TRUE;

    intredirectEntry_t reg = {
        .irqn        = GSImap[gsidx].irqn,
        .delivMode   = FIXED,
        .destMode    = PHYSICAL,
        .pinPolarity = GSImap[gsidx].polarity,
        .triggerMode = GSImap[gsidx].trigmode,
        .bitmask     = 0,
        .destination = 0 // send this to cpu0 for now; later make sure to make this more dynamic.
    };
    
    
    ioapic_write_reg((uint64_t)GSImap[gsidx].apicaddress, IOREDTBL_OFFSET_LOW (GSImap[gsidx].apicreg), reg.lower);
    ioapic_write_reg((uint64_t)GSImap[gsidx].apicaddress, IOREDTBL_OFFSET_HIGH(GSImap[gsidx].apicreg), reg.upper);
    printk("INSTALL_IRQ_VECTOR ==> i: %u *** ptr: %X *** flags: %b *** CS: %x\n", idx, getHandler(idx), idt_gate_flags.u8, (uint8_t)CS_KERNEL);
    set_idt_gate(idx, (uint64_t)getHandler(idx), idt_gate_flags.u8, CS_KERNEL);
}


inline void enable_interrupts()  { __asm__ ("sti"); }
inline void disable_interrupts() { __asm__ ("cli"); }