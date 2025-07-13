#ifndef __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_MACRO_DEFINITIONS__
#define __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_MACRO_DEFINITIONS__
#include <std/stdint.h>
#include <std/macro.h>


#ifdef __KERNEL_GDT_ENTRY_SIZE
#   error "Macro __KERNEL_GDT_ENTRY_SIZE Should NOT be already defined!"
#endif
#ifdef GDT_NS_KERNEL
#   error "Macro GDT_NS_KERNEL Should NOT be already defined!"
#endif
#ifdef GDT_CS_KERNEL
#   error "Macro GDT_CS_KERNEL Should NOT be already defined!"
#endif
#ifdef GDT_DS_KERNEL
#   error "Macro GDT_DS_KERNEL Should NOT be already defined!"
#endif
#ifdef GDT_CS_USER
#   error "Macro GDT_CS_USER Should NOT be already defined!"
#endif
#ifdef GDT_DS_USER
#   error "Macro GDT_DS_USER Should NOT be already defined!"
#endif
#ifdef GDT_TS0
#   error "Macro GDT_TS0 Should NOT be already defined!"
#endif
#ifdef GDT_TSN
#   error "Macro GDT_TSN Should NOT be already defined!"
#endif


typedef struct pack __GDT_Segment
{
    uint16_t limit0     ;
    uint16_t base0      ;
    uint8_t  base1      ;
    uint8_t  access     ;
    uint8_t  lim1_flags ;
    uint8_t  base2      ;
} GDT_ENTRY;


#define __KERNEL_GDT_ENTRY_SIZE sizeof(GDT_ENTRY)
#define GDT_NS_KERNEL 0x00 // null segment kernel
#define GDT_CS_KERNEL 0x08 // code segment kernel
#define GDT_DS_KERNEL 0x10 // data segment kernel
#define GDT_CS_USER   0x18 // code segment user
#define GDT_DS_USER   0x20 // data segment user
#define GDT_TS0       0x28 // task state segment
#define GDT_TSN(i)   (GDT_TS0 + i * 0x08)


#endif /* __KERNEL_C_DEFINITION_GLOBAL_DESCRIPTOR_TABLE_MACRO_DEFINITIONS__ */