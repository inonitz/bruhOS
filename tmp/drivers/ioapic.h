#ifndef __IO_APIC__
#define __IO_APIC__
#include "../std/debug.h"


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
            uint8_t irqn        : 8;
            uint8_t delivMode   : 3;
            uint8_t destMode    : 1;
            uint8_t delivStatus : 1;
            uint8_t pinPolarity : 1;
            uint8_t remoteIRR   : 1;
            uint8_t triggerMode : 1;
            uint8_t bitmask     : 1;
            uint64_t reserved   : 39;
            uint8_t destination : 8;
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


static inline uint32_t ioapic_read_reg(uint64_t apic_base, uint8_t register_index)
{
    *((uint32_t* volatile)apic_base) = register_index;
    return *((uint32_t* volatile)apic_base + 4);
}


static inline void ioapic_write_reg(uint64_t apic_base, uint8_t register_index, uint32_t value)
{
    *((uint32_t* volatile)apic_base)     = register_index;
    *((uint32_t* volatile)apic_base + 4) = value;
    return;
}


static inline uint32_t ioapic_max_int(uint64_t apic_base)
{
    // read the IOAPICVER[16:23]
    IOAPICVER_reg tmp; tmp.u32 = (volatile uint32_t)ioapic_read_reg(apic_base, 1);
    return tmp.maximumRedirectionEntries;
}


void read_register_table(uint64_t tableaddress);
void redirect_apic_port(uint8_t n, intredirectEntry_t entry);
void init_lapic();
void init_ioAPIC();



#endif
