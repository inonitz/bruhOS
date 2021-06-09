#include "lapicrouter.h"
#include "../std/atomic.h"



static align(8) LAPICMAP_t lapics;
static          uint8_t    currentLAPIC;




void  init_lapicrouter(volatile MADT_t* madt, uint8_t bsp_lapic) {
    memset(lapics, 0x00, sizeof(LAPICMAP_t));
    atomic_store_u8(&currentLAPIC, &bsp_lapic);
}


void  set_lapic(uint8_t idx, void* address) {
    atomic_store_u64(&lapics[idx], (uint64_t*)&address);
}


void* get_lapic(uint8_t idx) {
    return (void*)atomic_load_u64(&lapics[idx]);
}


// needs to switch depending on the processor. basically should be multi-threading safe.
uint8_t getCurrentLAPIC()
{
    return atomic_load_u8(&currentLAPIC);
}