#ifndef __IDT__
#define __IDT__
#include "../shared/int.h"


#define IDT_TASK_GATE      0x05
#define IDT_INTERRUPT_GATE 0x0E
#define IDT_TRAP_GATE      0x0F


typedef struct pack __idt_register
{
    uint16_t size;
    uint64_t base;
} idtreg;

typedef struct pack __idt_type_and_attributes
{
    union {
        struct pack {
            uint8_t gateType    : 4;
            uint8_t storage_seg : 1; // 0 for interrupt & trap gates
            uint8_t dplvl       : 2; // descriptor privilege lvl - min lvl from which this gate can be called.
            uint8_t present     : 1;
        };
        uint8_t u8;
    };    
} idt_access;

typedef struct pack __idt_descriptor
{
    // offset - pointer to the descriptor
    // cs_sel - the code segement offset inside an active GDT/LDT
    // istoff - interrupt stack table offset
    // access  - details at top (struct definition)
    uint16_t offset0;
    uint16_t cs_sel;
    uint8_t  istoff;
    uint8_t  access;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t zero1;
} IDT_ENTRY;


typedef struct pack __interrupt_descriptor_table
{
    union 
    {
        // struct pack {
        //     // ...?
        // };
        IDT_ENTRY gate[256];
    };
} IDT;


inline void set_entry_offset(IDT_ENTRY* e, uint64_t off)
{
    e->offset0 = off & 0xFFFF;
    e->offset1 = (off >> 16) & 0xFFFF;
    e->offset2 = (off >> 32) & 0xFFFFFFFF;
    return;
}


inline uint64_t entry_offset(IDT_ENTRY* e)
{
    return ((uint64_t)(e->offset2) << 32) | 
           ((uint64_t)(e->offset1) << 16) |
            (uint64_t)e->offset0;
}


void init_idt_register(idtreg* d);


void set_idt_entry(
    IN       uint8_t    idx, 
    IN       uint64_t   offset, 
    IN       uint16_t   code_segment, 
    IN       idt_access flags, 
    OPTIONAL uint8_t    tss_stack_table_off
);


void init_idt();




#endif