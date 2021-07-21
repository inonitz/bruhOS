#pragma once
#ifndef __SMP_LINKER_VARIABLE_SYMBOLS__
#define __SMP_LINKER_VARIABLE_SYMBOLS__
#include "../shared/int.h"
#include "../interrupts/gdt.h"
#include "../interrupts/idt.h"
#include "../interrupts/tss.h"
#include "../std/lock.h"



typedef struct align(4) __far_pointer_for_protected_mode
{
    uint32_t ptr;
    uint16_t segment;
} far_ptr32_t;


typedef struct align(8) __far_pointer_for_long_mode
{
    uint32_t ptr;
    uint16_t segment;
} far_ptr64_t;


typedef struct pack __trampoline_data
{
    bool_t          ready;  // boolean flag
    spinlock_t      lock;   // printk lock
    uint16_t        tssOff; // iterated with every new AP core
    uint32_t        pml4;   // shared kernel PML4
    uint64_t        rbp64;  // iterated with every new AP core
    GDT_DESCRIPTOR* gdt64;  // shared kernel GDT
    idtreg_t*       idt64;  // shared kernel IDT
    uint64_t        entry;  // AP core main function.
} trampoline_data;


extern uint8_t __trampoline_start;
extern uint8_t __trampoline_end;
extern uint8_t local_targs;
extern uint8_t farPointer32;
extern uint8_t farPointer64;
extern uint8_t ApStartGdt; 
extern uint8_t ApStartGdtCs_48_55; // < The flag part of the GDT. updated to 0xA0, so we need to revert it.

#endif