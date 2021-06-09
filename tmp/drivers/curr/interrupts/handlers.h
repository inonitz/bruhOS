#ifndef __INTERRUPTS_HANDLERS__
#define __INTERRUPTS_HANDLERS__
#include "../shared/int.h"



struct interrupt_frame;

extern void interrupt ir0 (struct interrupt_frame* frame);
extern void interrupt ir1 (struct interrupt_frame* frame);
extern void interrupt ir2 (struct interrupt_frame* frame);
extern void interrupt ir3 (struct interrupt_frame* frame);
extern void interrupt ir4 (struct interrupt_frame* frame);
extern void interrupt ir5 (struct interrupt_frame* frame);
extern void interrupt ir6 (struct interrupt_frame* frame);
extern void interrupt ir7 (struct interrupt_frame* frame);
extern void interrupt ir8 (struct interrupt_frame* frame);
extern void interrupt ir9 (struct interrupt_frame* frame);
extern void interrupt ir10(struct interrupt_frame* frame);
extern void interrupt ir11(struct interrupt_frame* frame);
extern void interrupt ir12(struct interrupt_frame* frame);
extern void interrupt ir13(struct interrupt_frame* frame);
extern void interrupt ir14(struct interrupt_frame* frame);
extern void interrupt ir15(struct interrupt_frame* frame);
extern void interrupt ir16(struct interrupt_frame* frame);
extern void interrupt ir17(struct interrupt_frame* frame);
extern void interrupt ir18(struct interrupt_frame* frame);
extern void interrupt ir19(struct interrupt_frame* frame);
extern void interrupt ir20(struct interrupt_frame* frame);


#endif