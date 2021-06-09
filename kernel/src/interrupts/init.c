#include "init.h"
#include "gdt.h"
#include "idt.h"
#include "htable.h"
#include "../acpi/init.h"



void init_interrupts(ACPI_DESC_TABLE* acpi)
{
    initialize_gdt();      // initialize the new gdt
    load_gdt();            // load the gdt
    initialize_idt();      // initialize the idt with proper interrupt handlers.
    load_idt();            // load the idt


    init_IRQHandleTable(); // handles for functions that will be called on exceptions/interrupts.
    init_acpi(acpi);       // initialize the BSP LAPIC, I/O APIC and the HPET Timer. (temporarily this is it, will add more *probably*). 
    return;
}