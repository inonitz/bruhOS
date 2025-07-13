#include <smp/smp.h>
#include <smp/symbols.h>  

#include <acpi/acpi.h>
#include <acpi/lapic.h>

#include <device/hpet.h>
#include <device/lapict.h>
#include <device/msr.h>

#include <mem/map_ptr.h>
#include <mem/phys/pmm.h>

#include <std/error.h>
#include <std/error_macro.h>
#include <std/halt.h>
#include <std/math.h>
#include <std/pause.h>
#include <std/string.h>


static MADT_t*    localMADT;
static uint16_t   ncpus = 0;
static atomic8u_t done  = 0;


void set_madt(MADT_t* madt)
{
    localMADT = madt;
}