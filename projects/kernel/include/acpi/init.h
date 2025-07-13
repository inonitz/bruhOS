#ifndef __ACPI_INIT__
#define __ACPI_INIT__
#include <preboot/acpi.h>


/* 
    * Initializes the HPET Timer, the I/O APIC's to a usable interface (install_vector(), ...)
    * and all the detected LAPIC's (enables all CPU's detected).
*/
void init_acpi(ACPIinitialConfig* acpi_config);


#endif
