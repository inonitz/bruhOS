#ifndef __SMP_INITIALIZE_PROCESSORS__
#define __SMP_INITIALIZE_PROCESSORS__
#include <preboot/kheader.h>
#include <acpi/acpi.h>



/* 
    * Will attempt to initialize all available application processors in the system.
    * Prints debugging message according to each processor
    * algorithm:
    *   clear interrupts
    *   allocate trampoline for SMP
    *   identity map trampoline
    *   initialize the trampoline data structure for the AP core
    *   for all lapic_ids that are not boostrap_processor (bsp):
    *       initialize_cpu(lapic_id[i]);
    *   unmap trampoline
    *   free trampoline memory 
    *   start interrupts
*/
void smp_init(kernel_header_t* khdr);


/* 
    * If you didn't supply the MADT_t in smp_init, then call this first:
    * this function sets a static variable in smp.c that will be used as the MADT_t,
    * IF it wasn't supplied in smp_init.
    * MADT_t* madt - look in smp_init() for info.
    * 
*/
void set_madt(MADT_t* madt);


#endif


