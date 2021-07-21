#include "gdt.h"



extern void load_gdt_internal(GDT_DESCRIPTOR* desc); // in gdt.asm


static GDT_DESCRIPTOR kgdt_ptr;
static GDT*           kgdt;


void initialize_gdt(void* gdt_address)
{
    kgdt  = (GDT*)gdt_address;
    *kgdt = (GDT){ // initialized in the bootloader
        .segments = {
            {0, 0, 0, 0x00, 0x00, 0}, // null
            {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
            {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
            {0, 0, 0, 0x9a, 0xa0, 0}, // user code segment
            {0, 0, 0, 0x92, 0xa0, 0}, // user data segment
            // {0, 0, 0, 0x89, 0x40, 0}  // task state -> needs to be init *more properly*
        }
    };
    kgdt_ptr = (GDT_DESCRIPTOR){
        .size = (uint16_t)(sizeof(GDT) - 1),
        .base = (uint64_t)gdt_address
    };
}


void load_gdt()
{
    load_gdt_internal(&kgdt_ptr);
}


GDT_DESCRIPTOR* getKernelGDTP()
{
    return &kgdt_ptr;
}