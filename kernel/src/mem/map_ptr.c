#include "map_ptr.h"
#include "../../../shared/lvars.h"
#include "virt/paging.h"



uint8_t* getKernelStart() { return &__kernel_start;      }
uint8_t* getKernelEnd  () { return &__kernel_end;        }
uint64_t virtualOffset()  { return __mem_virtual_offset; }


// legacy; every call to this function should be replaced with toVirtualFlag calls.
// should NOT be used!
void toVirtual(void* address, uint64_t pages)
{
    PML4* cr3 = getKernelCR3();
    for(uint64_t i = 0; i < pages; ++i) {
        mapVirtualToPhysical(
            cr3, 
            (void*)( VIRT(address)     + i * 4096lu ), 
            (void*)( (uint64_t)address + i * 4096lu )
        );
    }
    return;
}


void toVirtualFlag(void* address, uint32_t pages, uint64_t flags)
{
    PML4* cr3 = getKernelCR3(); // will be replaced very soon.
    for(uint32_t i = 0; i < pages; ++i) {
        map_vtop(
            cr3, 
            (void*)( VIRT(address)     + (uint64_t)i * 4096lu ), 
            (void*)( (uint64_t)address + (uint64_t)i * 4096lu ),
            flags
        );
    }
    return;
}


void vtopFlag(void* virt, void* phys, uint32_t pages, uint64_t flags)
{
    PML4* cr3 = getKernelCR3();
    for(uint32_t i = 0; i < pages; ++i) {
        map_vtop(
            cr3, 
            (void*)((uint64_t)virt + (uint64_t)i * 4096lu ), 
            (void*)((uint64_t)phys + (uint64_t)i * 4096lu ),
            flags
        );
    }
    return;
}


void unVirtual(void* virtaddr, uint64_t pages)
{
    PML4* cr3 = getKernelCR3();
    for(uint64_t i = 0; i < pages; ++i) {
        unmapVirtual(cr3, (void*)( (uint64_t)virtaddr + i * 4096lu ));
    }
    return;
}


uint64_t getCR3()
{
    return (uint64_t)getCurrentCR3();
}