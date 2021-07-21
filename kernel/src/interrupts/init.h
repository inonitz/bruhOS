#ifndef __INIT_INTERRUPTS__
#define __INIT_INTERRUPTS__
#include "../../../shared/def.h"



/* 
    * initializes a GDT, IDT, handle Table for the IDT, 
    * and ACPI Related devices / data structures.
    * ROXSD_t* acpi - a pointer to the Extended Root System Descriptor Pointer in memory. (XSDT)
*/
void init_interrupts(kernel_header_t* khdr);

#endif