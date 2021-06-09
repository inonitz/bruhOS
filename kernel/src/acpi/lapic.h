#pragma once
#ifndef __LAPIC__
#define __LAPIC__
#include "../std/debug.h"



// register offsets
#define LAPIC_ID      	   0x20
#define LAPIC_VERSION      0x30
#define LAPIC_TASKPRI      0x80
#define LAPIC_ARBPRI       0x90
#define LAPIC_PROCPRI      0xA0
#define LAPIC_EOI     	   0xB0
#define LAPIC_RMTREAD      0xC0
#define LAPIC_LOGICDEST    0xD0
#define LAPIC_DESTFORMAT   0xE0
#define LAPIC_SPURINTVEC   0xF0
#define LAPIC_INSERVICE0   0x100
#define LAPIC_INSERVICE1   0x110
#define LAPIC_INSERVICE2   0x120
#define LAPIC_INSERVICE3   0x130
#define LAPIC_INSERVICE4   0x140
#define LAPIC_INSERVICE5   0x150
#define LAPIC_INSERVICE6   0x160
#define LAPIC_INSERVICE7   0x170
#define LAPIC_TRIGGERMODE0 0x180
#define LAPIC_TRIGGERMODE1 0x190
#define LAPIC_TRIGGERMODE2 0x1A0
#define LAPIC_TRIGGERMODE3 0x1B0
#define LAPIC_TRIGGERMODE4 0x1C0
#define LAPIC_TRIGGERMODE5 0x1D0
#define LAPIC_TRIGGERMODE6 0x1E0
#define LAPIC_TRIGGERMODE7 0x1F0
#define LAPIC_INTREQ0      0x200
#define LAPIC_INTREQ1      0x210
#define LAPIC_INTREQ2      0x220
#define LAPIC_INTREQ3      0x230
#define LAPIC_INTREQ4      0x240
#define LAPIC_INTREQ5      0x250
#define LAPIC_INTREQ6      0x260
#define LAPIC_INTREQ7      0x270
#define LAPIC_ERRSTTS      0x280
#define LAPIC_LVTCMCI      0x2F0
#define LAPIC_INTCOMMND0   0x300
#define LAPIC_INTCOMMND1   0x310
#define LAPIC_LVTTIMER     0x320
#define LAPIC_LVTSENSTHERM 0x330
#define LAPIC_LVTPERFCNT   0x340
#define LAPIC_LVTINT0      0x350
#define LAPIC_LVTINT1      0x360
#define LAPIC_LVTERR       0x370
#define LAPIC_TIMERINITCNT 0x380
#define LAPIC_TIMERCURCNT  0x390
#define LAPIC_DIVCONFIG    0x3E0

// useful register information
#define RO // read only  access
#define RW // read write access


#define ICR_DESTMODE_TYPE_PHYSICAL 0
#define ICR_DESTMODE_TYPE_LOGICAL  1
#define ICR_DESTMODE_NORMAL 0
#define ICR_DESTMODE_LOWEST 1
#define ICR_DESTMODE_SMI    2
#define ICR_DESTMODE_NMI    4
#define ICR_DESTMODE_INIT   5
#define ICR_DESTMODE_SIPI   6

#define ICR_INT_PENDING (1 << 12)




typedef struct pack __local_apic_register
{
    uint32_t reg;
    uint32_t padding[3];
} LAPIC_REG;

// I have no idea what to call this, but TMR, IRR && ISR use this.
typedef struct pack __local_apic_large_register
{
    LAPIC_REG reg0;
    LAPIC_REG reg32;
    LAPIC_REG reg64;
    LAPIC_REG reg96;
    LAPIC_REG reg128;
    LAPIC_REG reg160;
    LAPIC_REG reg192;
    LAPIC_REG reg224;
} LAPIC_LARGE_REG;

typedef struct pack __local_apic_in_service_register
{
    LAPIC_LARGE_REG reg;
} ISREG;

typedef struct pack __local_apic_trigger_mode_register
{
    LAPIC_LARGE_REG reg;
} TMREG;

typedef struct pack __local_apic_interrupt_request_register
{
    LAPIC_LARGE_REG reg;
} IRREG;

typedef struct pack __local_apic_interrupt_command_register
{
    LAPIC_REG reg0;
    LAPIC_REG reg32;
} ICREG;


typedef struct alignpack(0x10) __local_apic_type
{
    uint8_t   reserved0[32]; // 0x00 ->0x20
    LAPIC_REG ID;            // 0x20 ->0x30
    LAPIC_REG VERSION;       // 0x30 ->0x40
    uint8_t   reserved1[64]; // 0x40 ->0x80
    LAPIC_REG TSKPRI;        // 0x80 ->0x90
    LAPIC_REG ARBPRI;        // 0x90 ->0xA0
    LAPIC_REG PROCPRI;       // 0xA0 ->0xB0
    LAPIC_REG EOI;           // 0xB0 ->0xC0
    LAPIC_REG RMTREAD;       // 0xC0 ->0xD0
    LAPIC_REG LDEST;         // 0xD0 ->0xE0
    LAPIC_REG DESTFMT;       // 0xE0 ->0xF0
    LAPIC_REG SPURINTVEC;    // 0xF0 ->0x100
    ISREG     INSERVICE;     // 0x100->0x180
    TMREG     TRIGGERMODE;   // 0x180->0x200
    IRREG     INTREQ;        // 0x200->0x280
    LAPIC_REG ERRSTTS;       // 0x280->0x290
    uint8_t   reserved2[96]; // 0x290->0x2F0
    LAPIC_REG LVTCMCI;       // 0x2F0->0x300
    ICREG     INTCOMMND;     // 0x300->0x320
    LAPIC_REG LVTTIMER;      // 0x320->0x330
    LAPIC_REG LVTSENSTHERM;  // 0x330->0x340 
    LAPIC_REG LVTPERFCNT;    // 0x340->0x350
    LAPIC_REG LVTINT0;       // 0x350->0x360
    LAPIC_REG LVTINT1;       // 0x360->0x370
    LAPIC_REG LVTERR;        // 0x370->0x380
    LAPIC_REG TIMERINITCNT;  // 0x380->0x390
    LAPIC_REG TIMERCURCNT;   // 0x390->0x3A0
    uint8_t   reserved3[64]; // 0x3A0->0x3E0
    LAPIC_REG DIVCONFIG;     // 0x3E0->0x3F0
    uint8_t   reserved4[16]; // 0x3F0->0x400
} lapic_t;


typedef struct pack __local_vector_table_register
{
    union {
        struct pack {
            uint8_t  vecnum;
            uint8_t  delivMode  : 3;
            uint8_t  delivStts  : 1;
            uint8_t  polarity_t : 1;
            uint8_t  rmtirr_t   : 1; // used for fixed, level-triggered int's
            uint8_t  trigmode_t : 1; // only used when delivMode is fixed, else trigger sensitive
            uint8_t  mask       : 1;
            uint8_t  timerMode  : 2; // 0b11 is reserved
            uint16_t reserved   : 14;
        };
        uint32_t u32;
    };
} LVTR_t;


typedef struct pack __interrupt_command_register
{
    union {
        struct pack {
            uint8_t  vecnum;
            uint8_t  delivMode : 3;
            uint8_t  destMode  : 1;
            uint8_t  delvstts  : 1;
            uint8_t  reserved0 : 1;
            uint8_t  Assertlvl : 1;
            uint8_t  trigmode  : 1;
            uint8_t  reserved  : 2;
            uint8_t  destSH    : 2;
            uint8_t  reserved1 : 4;
            uint8_t  tgtprocid : 3;
            uint16_t reserved2 : 5;
        };
        uint32_t u32;
    };
} ICR_t;


typedef struct pack __spurious_interrupt_vector_register
{
    union { 
        struct pack {
            uint8_t  vecnum     : 8;
            uint8_t  apicFlag   : 1; // on or off
            uint8_t  procCheck  : 1; // focus processor checking
            uint8_t  reserved0  : 2;
            uint8_t  supressEOI : 1;
            uint32_t reserved1  : 20;
        };
        uint32_t u32;
    };
} SIVR_t;


uint32_t lapic_read_reg (void* apic_base, uint16_t offset);
void     lapic_write_reg(void* apic_base, uint16_t offset, uint32_t data);
void     lapic_enable   (void* apic_base);
void     send_interrupt (void* apic_base, uint8_t target_apic, uint8_t vector);
void     lapic_send_eoi (void* apic_base);

#endif