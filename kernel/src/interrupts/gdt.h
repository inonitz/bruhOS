#pragma once
#ifndef __GDT__
#define __GDT__
#include "../shared/int.h"



#define GDT_NS_KERNEL 0x00 // null segment kernel
#define GDT_CS_KERNEL 0x08 // code segment kernel
#define GDT_DS_KERNEL 0x10 // data segment kernel
#define GDT_CS_USER   0x18 // code segment user
#define GDT_DS_USER   0x20 // data segment user
#define GDT_TS0       0x28 // task state segment
#define GDT_TSN(i)   (GDT_TS0 + i * 0x08)




typedef struct pack __Limit1AndFlags
{
    union {
        struct pack {
            uint8_t limit1     : 4;
            uint8_t not_used   : 1;
            uint8_t longmode   : 1; // if longmode != 0, size == 0
            uint8_t size       : 1;
            uint8_t granuality : 1; // if 1 -> segment size is 4KiB->4GiB. else, 1B->1MiB
        };
        uint8_t u8;
    };
} GDT_Flags;

typedef struct pack __Access_Attributes
{
    union 
    { 
        struct pack { 
            uint8_t accessed  : 1;
            uint8_t rw_able   : 1;
            uint8_t dirOrConf : 1;
            uint8_t exec      : 1;
            uint8_t desc_type : 1;
            uint8_t privilege : 2; // 0 = kernel, 3 = user
            uint8_t present   : 1;
        }; 
        uint8_t u8;
    };

} GDT_Access;


typedef struct pack __GDT_Segment
{
    uint16_t limit0     ;
    uint16_t base0      ;
    uint8_t  base1      ;
    uint8_t  access     ;
    uint8_t  lim1_flags ;
    uint8_t  base2      ;
} GDT_ENTRY;



typedef struct pack __GlobalDescriptorTable
{
    union { // ONLY USE THIS AS A GDT* because of the TSS Segments.
        struct pack {
            GDT_ENTRY null;
            GDT_ENTRY kernelCode;
            GDT_ENTRY kernelData;
            GDT_ENTRY userCode;
            GDT_ENTRY userData;
            GDT_ENTRY taskState[1];
        };
        GDT_ENTRY segments[6];
    };
} GDT;

typedef struct pack align(0x10) __gdt_descriptor_register
{
    uint16_t size;
    uint64_t base;
} GDT_DESCRIPTOR;


void            initialize_gdt(void* gdt_address);
void            load_gdt();
GDT_DESCRIPTOR* getKernelGDTP();


#endif