#include "percpu.h"
#include "../mem/map_ptr.h"
#include "../mem/phys/pmm.h"



static percpu_management_t percpu_data;


void init_percpu(volatile MADT_t* madt)
{
    uint64_t                 tableSize  = madt->hdr.length - sizeof(madt->hdr) - 8;
    volatile int_cnt_header* end        = (int_cnt_header*)(tableSize + (uint8_t*)&madt->intcont[0]);
    LAPIC_t*                 clapic;

    bool_t   good = TRUE;
    uint16_t cpus = 0;
    uint32_t tmpVar = 0;

    for(volatile int_cnt_header* table = &madt->intcont[0]; table < end; table = (int_cnt_header*)((uint8_t*)table + table->length))
    { 
        if(table->int_type == INT_CONTROLLER_PROCESSOR_LOCAL_APIC) {
            // if a cpu is enabled, or online capable, we'll add it to the current amount of CPUs.
            clapic = (LAPIC_t*)table;
            good   = boolean(clapic->flags & 0x3);
            // lapicid[cpus] = good * ((clapic->apic_id << 8) | ( clapic->flags & 0x1) ) + !good * lapicid[cpus];
            cpus  += good;
        }
    }
    percpu_data.processorCount = cpus;
    

    tmpVar = (uint32_t)cpus * sizeof(__percpu_t);
    tmpVar = (tmpVar & 0xFFF) * 0x1000 + (tmpVar >> LOG2_PAGE_SIZE);                       // turn tmpVar into pages - tmpVar = (tmpVar % 4096) * 4096 + tmpVar / 4096.
    percpu_data.procStructs = VIRT_TYPE(pfa_alloc_pages(tmpVar, DMA_NORMAL), __percpu_t*); // allocate pages for the structure.
    memset(percpu_data.procStructs, 0x00, (uint64_t)tmpVar * 4096);                        // set to zero.


    // initialize all lapic id's inside the percpu_data structure.
    tmpVar = 0;
    for(volatile int_cnt_header* table = &madt->intcont[0]; table < end; table = (int_cnt_header*)((uint8_t*)table + table->length))
    {
        if(table->int_type == INT_CONTROLLER_PROCESSOR_LOCAL_APIC) {
            clapic = (LAPIC_t*)table;
            percpu_data.procStructs[tmpVar].lapicid.isEnabled = clapic->flags & 0x1;
            percpu_data.procStructs[tmpVar].lapicid.lapicID   = clapic->apic_id;
            ++tmpVar;
        }
    }

}


uint16_t processorCount()
{
    return percpu_data.processorCount;
}


uint16_t currentLAPICID()
{
    return atomic_load_u16(&percpu_data.currentlapicID);
}


void setCurrentID(uint16_t cpuid)
{
    atomic_store_u16(&percpu_data.currentlapicID, &cpuid);
    return;
}