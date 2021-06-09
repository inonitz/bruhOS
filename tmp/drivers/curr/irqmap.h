#pragma once
#ifndef __INTERRUPT_VECTOR_MANAGER__
#define __INTERRUPT_VECTOR_MANAGER__
#include "../interrupts/idt.h"
#include "../interrupts/gdt.h"
#include "acpi.h"
#include "ioapic.h"




typedef struct pack __isa_to_gsi_entry
{
    union {
        struct pack {
            uint8_t  enabled  : 1;
            uint8_t  polarity : 2;
            uint8_t  trigmode : 2;
            uint16_t reserved : 3;
            uint8_t  gsin     : 8;
        };
        uint16_t u16;
    };
} itgentry_t;

// typedef struct pack __map_ISA_irq_to_GSI
// {
//     itgentry_t isa[16]; // 16 is the amount of legacy ISA irq's
// } ISA_TO_GSI;
typedef itgentry_t ISA_TO_GSI[16];


typedef struct pack __gsi_to_irq_entry
{
    union {
        struct pack {
            uint8_t  enabled  : 1;
            uint8_t  polarity : 2;
            uint8_t  trigmode : 2;
            uint8_t  reserved : 3;
            uint8_t  apicid;      // which ioapic table
            uint8_t  apicreg;     // which register is this gsi pointing to
            uint8_t  irqn;        // which irq this is mapped to
            uint32_t apicaddress; // the apic table address, since I don't know how to find it with the ID.
        };
        uint64_t u64;
    };
} gtientry_t;

// typedef struct pack __map_GSI_to_IRQ
// {
//     gtientry_t gsi[256]; // this is an arbitrarely chosen number 
// } GSI_TO_IRQ;
typedef gtientry_t GSI_TO_IRQ[256];



/* 
    * prints info about the GSImap and the ISAmap.
*/
void irqmap_info();

/* 
    * initializes local I/O APIC managers (tables)
    * MADT_t* madt - requires the madt in order to find the ioapic, and initialize the local tables.
*/
void ioapic_init(MADT_t* madt, bool_t disable_pic);


/*
    * this function does the following:
    * find an IDT entry in the local IDT, based on priority;
    * map the irqn to an available entry in i/o apic (if available):
    *   if irqn is already in the local tables:
    *       update the tables to connect to the IDT entry
    *   else:
    *       search for a free entry that can be used (in the local tables);
    *       and connect the IDT entry to the entry found;
    *       if a free entry is not found, print error message; return;
    * 
    *   write changes to the I/O APIC Table found in the MADT table;
    *   update the IDT entry found with given idt_gate_flags and a handler_address.
    * return; 
*/
void install_irq_vector(
    IN uint8_t            irqn, 
    IN IDT_ENTRY_PRIORITY priority,
    IN enum pin_polarity  activate_hl, 
    IN enum triggerMode   triggerOnEdgeOrLevel,
    IN IDT_ACCESS         idt_gate_flags
);


// both of these are self explanatory.
void enable_interrupts(); 
void disable_interrupts();



#endif