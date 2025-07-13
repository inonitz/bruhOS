#ifndef __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_HEADER__
#define __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_HEADER__
#include <interrupts/gdtsegment.h>


typedef struct pack __Limit1AndFlags
{
    union {
        struct {
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
        struct { 
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


typedef struct pack __GlobalDescriptorTable
{
    union { // ONLY USE THIS AS A GDT* because of the TSS Segments.
        struct {
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

typedef struct pack alignsz(0x10) __gdt_descriptor_register
{
    uint16_t size;
    uint64_t base;
} GDT_DESCRIPTOR;


void            initialize_gdt(void* gdt_address);
void            load_gdt(void);
GDT_DESCRIPTOR* getKernelGDTP(void);


#endif /* __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_HEADER__ */
