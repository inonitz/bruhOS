#include "lapic.h"
#include "../mem/virt/paging.h"
#include "../mem/map_ptr.h"



uint32_t lapic_read_reg(void* apic_base, uint16_t offset)
{
    return *(uint32_t volatile*)((uint8_t*)apic_base + offset); 
}


void lapic_write_reg(void* apic_base, uint16_t offset, uint32_t data)
{
    *(uint32_t volatile*)((uint8_t*)apic_base + offset) = data;
    return;
}


void lapic_enable(void* apic_base_virt)
{
    __asm__("cli");
    
    uint32_t tmp = lapic_read_reg(apic_base_virt, LAPIC_SPURINTVEC) | 0x1FF;
    lapic_write_reg(apic_base_virt, LAPIC_DESTFORMAT, 0xFFFFFFFF); // set destination format to flat.
    lapic_write_reg(apic_base_virt, LAPIC_LOGICDEST ,    1 << 24);
    lapic_write_reg(apic_base_virt, LAPIC_SPURINTVEC,        tmp); // enable the lapic and set the spurious int vec to idt gate 0xff.
    lapic_write_reg(apic_base_virt, LAPIC_TASKPRI   ,       0x00); // accept all irq's by changing the task priority reg.

    __asm__("sti");
}


void send_interrupt(void* apic_base, uint8_t target_apic, uint8_t vector)
{
    uint32_t tmp1 = lapic_read_reg(apic_base, LAPIC_INTCOMMND1) | ((target_apic & 0x3) << 24);
    uint32_t tmp0 = lapic_read_reg(apic_base, LAPIC_INTCOMMND0) | vector;
    lapic_write_reg(apic_base, LAPIC_INTCOMMND1, tmp1);
    lapic_write_reg(apic_base, LAPIC_INTCOMMND0, tmp0);
    return;
}


void lapic_send_eoi(void* apic_base)
{
    lapic_write_reg(apic_base, LAPIC_EOI, 0);
}

