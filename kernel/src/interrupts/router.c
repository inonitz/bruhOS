#include "router.h"
#include "htable.h"
#include "../acpi/ioapic.h"
#include "../acpi/pic.h"
#include "../mem/map_ptr.h"



static IRQROUTER_t router;




void init_irqRouter(volatile MADT_t* madt_table)
{
    // we're assuming the madt has been checksummed.
    uint64_t        tableSize  = madt_table->hdr.length - sizeof(madt_table->hdr) - 8;
    int_cnt_header* end        = (int_cnt_header*)(tableSize + (uint8_t*)&madt_table->intcont[0]);
    bool_t          disablePIC = madt_table->flags & 1; 


    if(disablePIC)
        disable_pic();

    // init the router struct.
    memset(&router, 0, sizeof(IRQROUTER_t));
    for(uint8_t i = 0; i < 16; ++i)
    {
        router.re[i] = (mapped_t){ // i == global system interrupt.
            .irq       = i,
            .int_vec   = 0, // not init
            .apicID    = 0, // not init
            .masked    = 1,
            .destmode  = PHYSICAL,
            .trigger   = 0, // 0 - edge,        1 - level
            .polarity  = 0, // 0 - active high, 1 - active low
            .delivMode = FIXED, 
            .dest      = 0  // not init
        };
    }


    // any interrupt source overrides found will be overwritten in the router struct
    INT_SRC_OVERRIDE_t* iso;
    for(volatile int_cnt_header* table = &madt_table->intcont[0]; table < end; table = (int_cnt_header*)((uint8_t*)table + table->length))
    {
        if(table->int_type == INT_CONTROLLER_INTERRUT_SOURCE_OVERRIDE)
        {
            iso = (INT_SRC_OVERRIDE_t*)table;
            router.re[iso->GSI].irq      = iso->irq_src;
            router.re[iso->GSI].polarity = ( iso->flags       & 0x3) == 0x3;
            router.re[iso->GSI].trigger  = ((iso->flags >> 2) & 0x3) == 0x3;
        }   
    }    


    // initialize the rest of the entries, with the relevant info
    // from the I/O ACPI Tables in the MADT.
    IOAPIC_t*     ioapic;
    IOAPICVER_reg reg;
    uint64_t      ioapic_addr;
    uint64_t      irqs = 1;
    for(volatile int_cnt_header* table = &madt_table->intcont[0]; table < end; table = (int_cnt_header*)((uint8_t*)table + table->length))
    {
        if(table->int_type == INT_CONTROLLER_IO_APIC)
        {
            ioapic      = (IOAPIC_t*)table; 
            toVirtualFlag((void*)(uint64_t)ioapic->address, 1, 0x8000000000000013); // 1 51*0 000 0 0 00 1 0 0 1 1
            ioapic_addr = VIRT(ioapic->address);
            irqs       += (ioapic_read_reg(VIRT(ioapic->address), IOAPICVER_REG) >> 16) & 0xFF;
            for(uint8_t i = 0; i < irqs; ++i)
            {
                router.re[(uint64_t)ioapic->GSIbase + i].apicID       = ioapic->io_apic_id; 
                router.re[(uint64_t)ioapic->GSIbase + i].apicInputIdx = i; 
                // mask this ioapic irq register.
                ioapic_write_reg(ioapic_addr, IOREDTBL_OFFSET_LOW(i), (1u << 16));
            }
            // note: remember to map these addresses when enabling paging!
            router.ioapics[router.activeAPICs] = (map_addr){ ioapic->io_apic_id, ioapic_addr };
            ++router.activeAPICs;
            router.activeIRQs += irqs;
        }   
    }


}


void irqRouterDescription()
{
    printk_align_format(2);
    const char_t* set0[2] = {
        "Active High",
        "Active Low"
    };
    const char_t* set1[2] = {
        "Edge  Triggered",
        "Level Triggered"
    };


    printk("\nIRQ ROUTER DESCRIPTION TABLE:\nActive I/O APIC's: %u\nActive     IRQ's: %u\nI/O APIC ID    ==> TABLE ADDRESS:\n",
        router.activeAPICs, router.activeIRQs
    );
    for(uint8_t i = 0; i < 4; ++i) {
        printk("I/O APIC ID %u ==> %p\n", router.ioapics[i].id, router.ioapics[i].addr);
    }
    printk("\n");
    
    
    for(uint8_t i = 0; i < router.activeIRQs; ++i)
    {
        printk("GSI%u <IOAPIC ID %u, input=%u> ==> IRQ%u <interrupt_vector=%u, masked=%u> %s %s\n",
            i, 
            router.re[i].apicID, 
            router.re[i].apicInputIdx, 
            router.re[i].irq,
            router.re[i].int_vec,
            router.re[i].masked,
            set1[router.re[i].trigger], 
            set0[router.re[i].polarity] 
        );
    }
    printk_align_format(0);
}


void ioAPICDescription(uint8_t ioapic_id)
{
    if(router.ioapics[ioapic_id & 0x04].addr == 0) return;
    
    void*              ioapic = (IOAPIC_t*)router.ioapics[ioapic_id].addr;
    uint8_t            irqs   = 1 + ((ioapic_read_reg((uint64_t)ioapic, IOAPICVER_REG) >> 16) & 0xFF);
    intredirectEntry_t reg; 
    reg.u64 = 0;


    const char_t* deliveryModes[8] = {
        "FIXED           ",
        "LOWEST_PRIORITY ",
        "SMI_INT         ",
        "RESERVED        ", 
        "NON_MASKABLE_INT",
        "INIT_ASSERT     ",
        "INIT_DEASSERT   ", 
        "START_UP        "
    };
    const char_t* destMode[2] = {
        "PHYSICAL",
        "LOGICAL "
    };
    const char_t* delivStatus[2] = {
        "REST    ",
        "IRQ SENT"
    };
    const char_t* polarityMode[2] = {
        "Active High",
        "Active Low "
    };
    const char_t* triggerMode[2] = {
        "Edge  Triggered",
        "Level Triggered"
    };
    const char_t* mask[2] = {
        "ENABLED",
        "MASKED "
    };


    printk("I/O APIC TABLE (ID %u) DESCRIPTION:\n", ioapic_id);
    printk_align_format(2);
    for(uint8_t i = 0; i < irqs; ++i)
    {
        reg.lower = ioapic_read_reg((uint64_t)ioapic, IOREDTBL_OFFSET_LOW(i) );
        reg.upper = ioapic_read_reg((uint64_t)ioapic, IOREDTBL_OFFSET_HIGH(i));
        printk("IOAPIC ENTRY %u ==> IRQ%u DELIV MODE: %s | DEST MODE: %s | DELIV STATUS: %s | POLARITY: %s | TRIGGER: %s | STATE: %s | DEST %b\n",
            i,
            reg.irqn,
            deliveryModes[reg.delivMode  ],
            destMode     [reg.destMode   ],
            delivStatus  [reg.delivStatus],
            polarityMode [reg.pinPolarity],
            triggerMode  [reg.triggerMode],
            mask         [reg.bitmask    ],
            reg.destination
        );
    }
    printk_align_format(0);
}


void installISA(uint8_t gsi, uint8_t lapic_id, uint8_t priority, void* handlerAddress)
{
    if(unlikely(gsi > 15)) {
        printk("installISA Can't allocate interrupt vectors for non-ISA interrupts\nuse installIRQ instead\n");
        return;
    }
    
    
    uint8_t idx = request_vector_min(priority);
    if(idx == 0xFF) {
        printk("Couldn't install IRQ vector. try a different (higher) priority lvl\n");
        return;
    }
    printk("       [ Interrupt Installer (ISA) ] FOUND INT#%u FOR DEVICE\n", idx+32);


    set_handler(idx, handlerAddress);
    router.re[gsi].int_vec = idx+32;
    router.re[gsi].dest    = lapic_id;
    router.re[gsi].masked  = FALSE;


    uint64_t entry       = router.re[gsi].u64 & 0xff0000000000ffff;
    uint64_t ioapic_addr = 0;
    for(uint8_t i = 0; i < router.activeAPICs && ioapic_addr == 0; ++i)
    {
        ioapic_addr = (router.ioapics[i].id == router.re[gsi].apicID) * router.ioapics[i].addr;
    }
    ioapic_write_reg(ioapic_addr, IOREDTBL_OFFSET_LOW( router.re[gsi].apicInputIdx), (uint32_t)(entry      ));
    ioapic_write_reg(ioapic_addr, IOREDTBL_OFFSET_HIGH(router.re[gsi].apicInputIdx), (uint32_t)(entry >> 32));
    return;
}


void installIRQ(uint8_t gsi, uint8_t lapic_id, IRQ_FLAGS flags, uint8_t priority, void* handlerAddress)
{
    // set irq, destMode = physical, dest = lapic, flags = ...
    // AND do everything in installISA 
    if(gsi <= 15) {
        printk("installIRQ Can't allocate interrupt vectors for ISA interrupts\nuse installISA instead\n");
        return;
    }
    else if(!isMapped(gsi)) {
        printk("Can't use GSI that doesn't have I/O APIC Mapping\n");
        return;
    }

    uint8_t idx = request_vector_min(priority);
    if(idx == 0xFF) {
        printk("Couldn't install IRQ vector. try a different (higher) priority lvl\n");
        return;
    } 
    printk("       [ Interrupt Installer (IRQ) ] FOUND INT#%u FOR DEVICE\n", idx);
    set_handler(idx, handlerAddress);


    router.re[gsi].int_vec  = idx+32;
    router.re[gsi].dest     = lapic_id;
    router.re[gsi].polarity = flags.polarity;
    router.re[gsi].trigger  = flags.trigger;
    router.re[gsi].destmode = flags.destmode;
    router.re[gsi].masked   = FALSE;

    uint64_t entry       = router.re[gsi].u64 & 0xff0000000000ffff;
    uint64_t ioapic_addr = 0;
    for(uint8_t i = 0; i < router.activeAPICs && ioapic_addr == 0; ++i)
    {
        ioapic_addr = (router.ioapics[i].id == router.re[gsi].apicID) * router.ioapics[i].addr;
    }
    ioapic_write_reg(ioapic_addr, IOREDTBL_OFFSET_LOW( router.re[gsi].apicInputIdx), (uint32_t)(entry      ));
    ioapic_write_reg(ioapic_addr, IOREDTBL_OFFSET_HIGH(router.re[gsi].apicInputIdx), (uint32_t)(entry >> 32));
    return;
}


bool_t isEmpty(uint8_t idx)
{
    uint64_t reg = router.re[idx].u64 & 0xffffffff00ffffff; // apicInputIdx = 0
    return reg == 0;
}


bool_t isMapped(uint8_t idx)
{
    return router.re[idx].u64 != 0;
}




// printk("init_irqRouter:\n");
// for(int_cnt_header* table = &madt_table->intcont[0]; table < end; table = (int_cnt_header*)((uint8_t*)table + table->length))
// {
//     switch(table->int_type)
//     {
//         case INT_CONTROLLER_PROCESSOR_LOCAL_APIC:
//             printk("Found LAPIC ID %u\n", ((LAPIC_t*)table)->apic_id);
//             break;
//         case INT_CONTROLLER_IO_APIC: 
//             printk("Found IOAPIC at %x\n", ((IOAPIC_t*)table)->address);
//             break;
//         case INT_CONTROLLER_INTERRUT_SOURCE_OVERRIDE: 
//             printk("Found ISO irq%u in gsi%u\n", ((INT_SRC_OVERRIDE_t*)table)->irq_src, ((INT_SRC_OVERRIDE_t*)table)->GSI);
//             break;
//         case INT_CONTROLLER_LOCAL_APIC_NMI: 
//             printk("Found LAPIC_NMI PROC <%u, LINT=%u>, flags: %b\n", 
//                 ((LNMI_t*)table)->pid,
//                 ((LNMI_t*)table)->LINT,
//                 ((LNMI_t*)table)->flags
//             );
//             break;
//         default:
//             printk("Found table with Signature ID %u\n", table->int_type);
//     }
// }
// printk("\n\n");