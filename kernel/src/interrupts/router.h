#pragma once
#ifndef __IOAPIC_ROUTER__
#define __IOAPIC_ROUTER__
#include "../acpi/acpi.h"
#include "idt.h"



typedef struct pack __mapped_gsi_entry
{
    union {
        struct pack {
            uint8_t  int_vec;        // which interrupt vector is triggered in the IDT, when this is called.
            uint8_t  delivMode : 3;  // the type of interrupt sent to the CPU core/s. for now we use FIXED.
            uint8_t  destmode  : 1;  // physical or logical (logical = send to multiple CPU's at once)
            uint8_t  polarity  : 1;  // active high or low
            uint8_t  reserved1 : 1;  // remote IRR (I don't use this..)
            uint8_t  trigger   : 1;  // edge or level triggered.
            uint8_t  masked    : 1;  // is the entry unused in the I/O APIC
            uint8_t  irq;            // the IRQ number in the corresponding I/O APIC.
            uint8_t  apicID;         // which I/O APIC 
            uint8_t  apicInputIdx;   // which Entry in the I/O APIC
            uint16_t reserved0 : 16; // padding to align on an 8-byte boundary.
            uint8_t  dest      : 8;  // the data required to know what CPU this irq is sent to (depends on destmode)
        };
        uint64_t u64;
    };
} mapped_t;

typedef struct pack __mapped_reg_flags
{
    uint8_t destmode  : 1;
    uint8_t trigger   : 1;
    uint8_t polarity  : 1; 
    uint8_t reserved  : 5;
} IRQ_FLAGS;

typedef struct pack __ioapic_id_to_address
{
    uint8_t  id;
    uint64_t addr;
} map_addr;


typedef struct pack __map_ioapic_table_entries_to_their_gsi_and_interrupt_vector
{
    mapped_t re[256];
    uint8_t  activeIRQs;
    uint8_t  activeAPICs;
    map_addr ioapics[4]; // 4 - is temporary, when you finish vmm allocate dynamically.
    // also need array of all the I/O APIC's found, with each entry mapping: APICID -> Table Address
} IRQROUTER_t;



/* 
    * Initializes the Interrupt Vector Router.
    * MADT_t* madt_table - parses the ACPI MADT Table for devices and I/O APIC's 
*/
void   init_irqRouter(volatile MADT_t* madt_table);


/* 
    * Prints the current configuration of the IRQ_Router.
*/
void   irqRouterDescription();


/* 
    * Prints Info about a specific I/O APIC That was found when initializing the IRQ_Router.
    * uint8_t ioapic_id - the Index (0 => Amount of I/O APIC's - 1) of the I/O APIC.
*/
void   ioAPICDescription(uint8_t ioapic_id);


/* 
    * modifies the I/O APIC Table and connects to an interrupt vector in the IDT.
    * This should be called for initializing interrupts for legacy device drivers (ISA Devices, gsi <= 15).
    * uint8_t gsi                 - the interrupt that will be triggered in the first available I/O APIC. This is different from the IDT interrupt.  
    * uint8_t lapic_id            - the Local APIC (Processor) That will be notified of the interrupt. 
    * IDT_ENTRY_PRIORITY priority - the higher the index, the lower the priority.
    * void* handlerAddress        - the function that will handle the interrupt that will be triggered by the device. Basically interrupt handler.
*/
void   installISA(uint8_t gsi, uint8_t lapic_id, IDT_ENTRY_PRIORITY priority, void* handlerAddress);


/* 
    * Same as installIRQ, but for device Drivers that are non-legacy.
    * Because Legacy devices are already (probably) preconfigured in the I/O APIC (By Interrupt Service Overrides), you can't set their I/O APIC Flags again.
    * Because this function isn't meant for legacy devices, the drivers are responsible for setting their own I/O APIC Flags/Entries in the corresponding I/O APIC.
    * IRQ_FLAGS flags - additional information that a device driver needs to supply, in order to initialize the corresponding I/O APIC Entry it wants.
*/
void   installIRQ(uint8_t gsi, uint8_t lapic_id, IRQ_FLAGS flags, IDT_ENTRY_PRIORITY priority, void* handlerAddress);


bool_t isEmpty(uint8_t idx);


bool_t isMapped(uint8_t idx);


#endif