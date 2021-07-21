#pragma once
#ifndef __IO_APIC__
#define __IO_APIC__
#include "../std/debug.h"



#define IOREGSEL_SELECT_REG_OFFSET    0
#define IOREGWIN_READWRITE_REG_OFFSET 0x10
#define IOAPICID_REG  0
#define IOAPICVER_REG 1
#define IOREDTBL_REG  2

#define IOREDTBL_OFFSET_LOW(n)  (0x10 + ((n) << 1)    )
#define IOREDTBL_OFFSET_HIGH(n) (0x10 + ((n) << 1) + 1)




typedef struct pack __IOAPIC_version_register
{
    union {
        struct pack {
            uint8_t apic_ver;
            uint8_t reserved0;
            uint8_t maximumRedirectionEntries;
            uint8_t reserved1;
        };
        uint32_t u32;
    };
} IOAPICVER_reg;


typedef struct pack __io_interruptN_redirection_table_IOREDTBL_register
{
    union {
        struct pack {
            uint8_t  irqn        : 8;
            uint8_t  delivMode   : 3;
            uint8_t  destMode    : 1;
            uint8_t  delivStatus : 1;
            uint8_t  pinPolarity : 1;
            uint8_t  remoteIRR   : 1;
            uint8_t  triggerMode : 1;
            uint8_t  bitmask     : 1;
            uint64_t reserved    : 39;
            uint8_t  destination : 8;
        };
        struct pack {
            uint32_t lower;
            uint32_t upper;
        };
        struct pack {
            uint64_t u64;
        };
    };
} intredirectEntry_t;


extern uint32_t ioapic_read_reg    (uint64_t apic_base, uint8_t register_index);                 // read from register in I/O APIC Table 
extern void     ioapic_write_reg   (uint64_t apic_base, uint8_t register_index, uint32_t value); // write to  register in I/O APIC Table 
extern uint32_t ioapic_irq_count   (uint64_t apic_base);                                         // the amount of irq's available in the I/O APIC
extern void     disable_ioapic_irqs(uint64_t apic_base);                                         // disable all irq's
void            redirect_apic_port (uint8_t n, intredirectEntry_t entry);                        // redirect a irq to a different IDT gate entry.



#endif