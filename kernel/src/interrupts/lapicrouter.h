#pragma once
#ifndef __LAPIC_ROUTER__
#define __LAPIC_ROUTER__
#include "../std/debug.h"
#include "../acpi/acpi.h"



// amount of lapic's will need to be dynamically allocated, 
// you never know how many cores a system might have
// this is temporarily fine.
// to make this dynamic: 
// turn this into a pointer.
// find out the amount of processors by probing the ACPI MADT Table (count local APIC's, and their corresponding id's)
// allocate the LAPICMAP_t and assign the ID's per LAPIC basis.
typedef uint64_t LAPICMAP_t[8];



void    init_lapicrouter(volatile MADT_t* madt, uint8_t bsp_lapic_id);
void    set_lapic(uint8_t idx, void* address);
void*   get_lapic(uint8_t idx);
uint8_t getCurrentLAPIC();


#endif