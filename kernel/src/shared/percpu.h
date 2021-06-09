#pragma once
#ifndef __PER_CPU_GLOBAL_VARS__
#define __PER_CPU_GLOBAL_VARS__
#include "../acpi/acpi.h"
#include "../acpi/lapic.h"
#include "../std/atomic.h"



typedef struct __per_processor_global_structure
{
    union { // incase AP processors are not numbered sequentially, like in an array.  
        struct pack { 
            uint8_t isEnabled : 1; 
            uint8_t padding0  : 7; 
            uint8_t lapicID   : 8; 
        };
        uint16_t u16;
    } lapicid;

    
} __percpu_t;


typedef struct __cpu_management_structure
{
    logical lapic_t* lapic;          // the *logical* address of the BSP lapic after parsing the MADT.
    uint8_t          padding0[4];    // this
    atomic16u_t      currentlapicID; // and this
    uint16_t         processorCount; // and this are a total of 8 bytes.
    __percpu_t*      procStructs;    // array of cpu structures that is allocated at runtime, after parsing the MADT.
} percpu_management_t;


/* 
    * Parses the MADT Table and initializes the percpu structures,
    * specifically structs related to IPI communication, LAPIC, etc... 
*/
void init_percpu(volatile MADT_t* table);


/* 
    * As the name implies, it returns the amount of detected processors in the system.
*/
uint16_t processorCount();


/* 
    * returns the current active CPU
*/
uint16_t currentLAPICID();


/* 
    * sets the current Processor LAPIC_ID atomically.
*/
void     setCurrentID(uint16_t lapic_id);

#endif