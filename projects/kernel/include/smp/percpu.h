#ifndef __PER_CPU_GLOBAL_VARS__
#define __PER_CPU_GLOBAL_VARS__
#include <acpi/acpi.h>
#include <acpi/lapic.h>
#include <mem/virt/vas.h>
#include <std/atomic.h>


typedef struct __per_processor_data_structure
{
    bool_t     enabled;
    uint32_t   indexID;
    uint32_t   lapicID;
    atomic8u_t apicTimerPoll;
    vas_man_t* vmm_list; // should be a thread list.
} __percpu_t;


typedef struct __cpu_management_structure
{
    uint16_t         processorCount; // and this are a total of 8 bytes.
    __percpu_t*      procStructs;    // array of cpu structures that is allocated at runtime, after parsing the MADT.
} percpu_management_t;


#endif