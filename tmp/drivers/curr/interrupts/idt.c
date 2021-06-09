#include "idt.h"
#include "gdt.h"
#include "../std/io.h"
#include "handlers.h"



extern inline void     set_entry_offset(IDT_ENTRY* e, uint64_t off);
extern inline uint64_t entry_offset    (IDT_ENTRY* e              );


static IDT kidt = {0};


void init_idt_register(idtreg* d)
{
    d->size = sizeof(IDT) - 1;
    d->base = (uint64_t)&kidt.gate[0];
    return;
}


void set_idt_entry(
    IN       uint8_t    idx, 
    IN       uint64_t   offset, 
    IN       uint16_t   code_segment, 
    IN       idt_access flags, 
    OPTIONAL uint8_t    tss_stack_table_off
)
{
    set_entry_offset(&kidt.gate[idx], offset);
    kidt.gate[idx].cs_sel = code_segment;
    kidt.gate[idx].istoff = tss_stack_table_off & 0xb11;
    kidt.gate[idx].access = flags.u8;
    return;
}


static void load_idt (idtreg* descriptor) { __asm__("lidt (%rdi)"); }
static void store_gdt(idtreg* descriptor) { __asm__("sidt (%rdi)"); }


void init_idt()
{
    memset((void*)&kidt, 0x00, sizeof(IDT));
    // // init idtr
    // idtreg idt; 
    // init_idt_register(&idt);
    // printk("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCcc\n");


    // // init fault handlers
    // idt_access a = { .gateType = IDT_INTERRUPT_GATE, .storage_seg = 0, .dplvl = 0, .present = 1 };
    
    // set_idt_entry(0x0E, (uint64_t)ir15, CS_KERNEL, a, 0);
    // // set_idt_entry(0x03, (uint64_t)ir3,  CS_KERNEL, a, 0);

    // printk("BBBBBBBBBBBBBBBBBBBBBBb\n");
    // // finally, load the idt
    // load_idt(&idt);
}