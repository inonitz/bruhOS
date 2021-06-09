#include "handlers.h"
#include "../std/debug.h"



void interrupt ir0 (struct interrupt_frame* frame) {

}


void interrupt ir1 (struct interrupt_frame* frame) {

}


void interrupt ir2 (struct interrupt_frame* frame) {

}


void interrupt ir3 (struct interrupt_frame* frame) {
    printk("breakpoint hit!\n");
}


void interrupt ir4 (struct interrupt_frame* frame) {

}


void interrupt ir5 (struct interrupt_frame* frame) {

}


void interrupt ir6 (struct interrupt_frame* frame) {

}


void interrupt ir7 (struct interrupt_frame* frame) {

}


void interrupt ir8 (struct interrupt_frame* frame) {

}


void interrupt ir9 (struct interrupt_frame* frame) {

}


void interrupt ir10(struct interrupt_frame* frame) {

}


void interrupt ir11(struct interrupt_frame* frame) {

}


void interrupt ir12(struct interrupt_frame* frame) {

}


void interrupt ir13(struct interrupt_frame* frame) {

}


void interrupt ir14(struct interrupt_frame* frame) {

}

// page fault
void interrupt ir15(struct interrupt_frame* frame) {
    __asm__("cli");
    printk("Page fault bruh\n");
    hang();
}


void interrupt ir16(struct interrupt_frame* frame) {

}


void interrupt ir17(struct interrupt_frame* frame) {

}


void interrupt ir18(struct interrupt_frame* frame) {

}


void interrupt ir19(struct interrupt_frame* frame) {

}


void interrupt ir20(struct interrupt_frame* frame) {

}

