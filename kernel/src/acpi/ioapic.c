#include "ioapic.h"



inline uint32_t ioapic_read_reg(uint64_t apic_base, uint8_t register_index)
{
    *((uint32_t volatile*)apic_base) = register_index;
    return *(uint32_t volatile*)(apic_base + 16);
}


inline void ioapic_write_reg(uint64_t apic_base, uint8_t register_index, uint32_t value)
{
    *((uint32_t volatile*)apic_base) = register_index;
    *(uint32_t  volatile*)(apic_base + 16) = value;
    return;
}


inline uint32_t ioapic_irq_count(uint64_t apic_base)
{
    // read the IOAPICVER[16:23]
    volatile IOAPICVER_reg tmp; tmp.u32 = (volatile uint32_t)ioapic_read_reg(apic_base, 1);
    return tmp.maximumRedirectionEntries;
}

inline void disable_ioapic_irqs(uint64_t apic_base)
{
    // mask all registers
    intredirectEntry_t entry;
    for(uint8_t i = 0; i < ioapic_irq_count(apic_base); ++i)
    {
        entry.lower = ioapic_read_reg(apic_base, IOREDTBL_OFFSET_LOW(i));
        entry.bitmask = 1; // disable this irq
        ioapic_write_reg(apic_base, IOREDTBL_OFFSET_LOW(i), entry.lower);
    }
}

void redirect_apic_port(uint8_t n, intredirectEntry_t entry);