#include "lapic.h"

// defaults to this (Found from Intel Vol.3 Programming Manual Section 10). can be changed.
static uint64_t lapic_start = 0xFEE00000; 



uint32_t read_lapic_reg(void* lapic_base, uint16_t reg_offset)
{
    return *(uint32_t volatile*)((uint8_t*)lapic_base + reg_offset);
}

void write_lapic_reg(void* lapic_base, uint16_t offset, uint32_t data)
{
    uint32_t volatile* reg = (uint32_t volatile*)((uint8_t*)lapic_base + offset);
    *reg = data;
    return;
}


void set_lapic_address(uint64_t lapic_begin_address) {
    lapic_start = lapic_begin_address;
}


void cpu_startup_init(uint8_t lapic_id)
{
    ICR_t* reg = (ICR_t*)(lapic_start + LAPIC_INTCOMMND);
    printk("Interrupt Command Register %u : %x", reg->vecnum, reg->u32);

    // write_lapic_reg((void*)lapic_start, LAPIC_INTCOMMND, );

}
