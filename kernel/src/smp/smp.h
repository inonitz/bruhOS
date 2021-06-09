#pragma once
#ifndef __SMP_PROCESSOR_INITIALIZATION__
#define __SMP_PROCESSOR_INITIALIZATION__
#include "../shared/int.h"
#include "../acpi/acpi.h"



/* 
    * Returns the physical address of the trampoline in memory, where address < 1MiB.
*/
uint8_t* setup_trampoline();


/* 
    * Will wakeup the processor as defined in the intel specification,
    * in real mode with a functioning lapic.
    * volatile void* lapic_base       - physical address of the bootstrap lapic
    * uint8_t        lapic_id         - the Application Processor to wakeup.
    * uint8_t*       trampoline_start - the address in which code that puts the processor into long mode is.
*/
bool_t wakeup_processor(uint8_t lapic_id, uint8_t* trampoline_start);


/* 
    * Uses trampoline code to set the processor into long mode.
*/
void   set_longmode(uint8_t lapic_id);


/* 
    * Does everything that the functions above do, but to all AP processors.
*/
void   smp_init(volatile MADT_t* madt, uint8_t* trampoline_start);


/* 
    * What every processor will execute after the bootstrapping code.
    * This function will set the new processor with the kernel GDT, IDT, PML4, and a new TSS.
*/
void smp_main();


#endif