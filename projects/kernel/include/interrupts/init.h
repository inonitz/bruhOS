#ifndef __INIT_INTERRUPTS__
#define __INIT_INTERRUPTS__


#if !defined(__KERNEL_HEADER_TYPE_ALREADY_DEFINED)
typedef struct __kernel_header_t kernel_header_t;
#endif


/* 
    * initializes a GDT, IDT, handle Table for the IDT, 
    * and ACPI Related devices / data structures.
    * ROXSD_t* acpi - a pointer to the Extended Root System Descriptor Pointer in memory. (XSDT)
*/
void init_interrupts(kernel_header_t* khdr);


#endif
