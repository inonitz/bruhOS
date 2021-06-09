#include "smp.h"
#include "../acpi/lapic.h"
#include "../interrupts/gdt.h"
#include "../interrupts/idt.h"
#include "../interrupts/tss.h"
#include "../mem/virt/paging.h"
#include "../device/timer.h"



static void*              lapic_address;
static GDT_DESCRIPTOR*    globalGDT;
static idtreg_t*          globalIDT;
static volatile uint32_t  activeAPs = 0;


uint8_t* setup_trampoline()
{
    // essentially asks the pmm for a reserved VERY LOW ADDRESS memory region.
    return nullptr;
}


bool_t wakeup_processor(uint8_t lapic_id, uint8_t* trampoline_start)
{
    lapic_write_reg(lapic_address, LAPIC_ERRSTTS, 0); // clear errors.


    // send an INIT IPI trigger to the desired ap processor
    uint32_t tmp1 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0x00ffffff) | (lapic_id << 24);
    uint32_t tmp0 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0xfff00000) | (ICR_DESTMODE_INIT << 8) | (0x3 << 14);
    lapic_write_reg(lapic_address, LAPIC_INTCOMMND1, tmp1);
    lapic_write_reg(lapic_address, LAPIC_INTCOMMND0, tmp0);
    while(lapic_read_reg(lapic_address, LAPIC_INTCOMMND0) & ICR_INT_PENDING) {
        __asm__ volatile("pause"); // wait for receivement of the interrupt.
    }


    // send an INIT de-assert to the desired AP processor.
    tmp1 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0x00ffffff) | (lapic_id << 24);
    tmp0 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0xfff00000) | (ICR_DESTMODE_INIT << 8) | (0x2 << 14);
    lapic_write_reg(lapic_address, LAPIC_INTCOMMND1, tmp1);
    lapic_write_reg(lapic_address, LAPIC_INTCOMMND0, tmp0);
    while(lapic_read_reg(lapic_address, LAPIC_INTCOMMND0) & ICR_INT_PENDING) {
        __asm__ volatile("pause"); // wait for receivement of the interrupt.
    }
    count(10); // wait 10 ms for the processor.


    for(uint8_t i = 0; i < 2; ++i)
    {
        lapic_write_reg(lapic_address, LAPIC_ERRSTTS, 0);       // clear errors.
        tmp1 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0x00ffffff) | (lapic_id << 24);
        tmp0 = (lapic_read_reg(lapic_address, LAPIC_INTCOMMND1) & 0xfff00000) | (ICR_DESTMODE_SIPI << 8) | ((uint64_t)trampoline_start >> 12);
        lapic_write_reg(lapic_address, LAPIC_INTCOMMND1, tmp1); // write the output to the respective registers.
        lapic_write_reg(lapic_address, LAPIC_INTCOMMND0, tmp0); // ^^^^
        countns(200000);                                     // count 200 microseconds. 
        while(lapic_read_reg(lapic_address, LAPIC_INTCOMMND0) & ICR_INT_PENDING) {
            __asm__ volatile("pause");                       // wait for receivement of the interrupt.
        }
    }


    return TRUE; // success
}




void smp_init(volatile MADT_t* madt, uint8_t* trampoline_start)
{

}


void smp_main() 
{
    // copy gdt, ldt, tss, pml4 to new processor.
    // read the lapic id register to find the APICID of the current AP processor, and assign it an ID.
    // hlt the cpu after printing a friendly message.
    __asm__ volatile("lgdt (%0)"     : : "r"(globalGDT));
    __asm__ volatile("lidt (%0)"     : : "r"(globalIDT));
    __asm__ volatile("mov %0, %%cr3" : : "r"(getKernelCR3()));
    lapic_enable(lapic_address);
    
    // allocate a new TSS and initialize it in the GDT
    //
    //
    printk("HELLO FROM CPU %u\n", lapic_read_reg(lapic_address, LAPIC_ID));

    __asm__ volatile("hlt");
}