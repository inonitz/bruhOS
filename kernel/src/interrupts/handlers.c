#include "handlers.h"
#include "idt.h"
#include "htable.h"
#include "../mem/map_ptr.h"
#include "../acpi/lapic.h"
#include "lapicrouter.h"



void __noinline exception_handler(stack_state* sst); // called by interrupt handler
void __noinline irq_handler      (stack_state* sst); // called by interrupt handler




void __noinline interrupt_handler(stack_state* sst)
{
    if(sst->irqn < 32) {
        exception_handler(sst);
        return;
    }
    irq_handler(sst);
    return;
}


void __noinline exception_handler(stack_state* sst)
{  
    const uint8_t trap_gates[16] = { 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20 };
    bool_t        need_hang      = FALSE;

    printk("EXCEPTION HAS OCCURED (IRQ %u). STACK FRAME:\n", sst->irqn);
    printk("ecode:  %b\nirqn:   %u\nrsp:    %X\nrip:    %X\ncs:     %x\nrflags: %b\nss:     %b\n\n",
        sst->ecode,
        sst->irqn,
        sst->rsp,
        sst->rip,
        sst->cs,
        sst->rflags,
        sst->ss
    ); 

    for(uint64_t i = 0; !need_hang && i < sizeof(trap_gates); ++i) { need_hang = i == sst->irqn; }
    if(need_hang) {
        printk("SCRATCH REGISTERS:\nRAX %X RCX %X RDX %X\nRSI %X RDI %X r8  %X\nr9  %X r10 %X r11 %X\n", 
            sst->rax, sst->rcx, sst->rdx, 
            sst->rsi, sst->rdi, sst->r8,
            sst->r9,  sst->r10, sst->r11
        );
        printk("CONTROL REGISTERS:\nCR0 %X CR2 %X CR3 %X\nCR4 %X CR8 %X\n\n",
            sst->cr0, sst->cr2, sst->cr3, 
            sst->cr4, sst->cr8
        );
        hang(); // we hang because I still need to deal with errors by hardcoding shit out and fixing bugs.
    }
    return;
}


void __noinline irq_handler(stack_state* sst)
{
    DEBUG( 
        printk("\nIRQ HANDLER CALLED AT IDT GATE %u\n", sst->irqn);
    );
    irqHandler handler = (irqHandler)get_handler(sst->irqn - 32);
    handler();
    lapic_send_eoi(get_lapic(getCurrentLAPIC()));
    return;
}




/* 
I Don't know why, but when I try to access this array of strings with sst->irqn I always get Page Faults / General protection Faults,
When in reality this is part of the kernel. I don't know, I'll just NOT use this and print the IRQ instead...
const char_t* ErrorType[32] = {
    "DIVIDE BY 0",
    "DEBUG",
    "NMI",
    "BREAKPOINT",
    "OVERFLOW",
    "BOUND RANGE EXCEEDED",
    "INVALID OPCODE",
    "DEVICE NOT AVAILABLE",
    "DOUBLE FAULT",
    "RESERVED9",
    "INVALID TSS",
    "SEGMENT NOT PRESENT",
    "STACK SEGMENT FAULT",
    "GENERAL PROTECTION FAULT",
    "PAGE FAULT",
    "RESERVED15",
    "x87 FLOATING-POINT EXCEPTION",
    "ALIGNMENT CHECK",
    "MACHINE CHECK",
    "SIMD FLOATING-POINT EXCEPTION",
    "VIRTUALIZATION EXCEPTION",
    "RESERVED21",
    "RESERVED22",
    "RESERVED23",
    "RESERVED24",
    "RESERVED25",
    "RESERVED26",
    "RESERVED27",
    "RESERVED28",
    "RESERVED29",
    "SECURITY EXCEPTION",
    "RESERVED31"
};

*/