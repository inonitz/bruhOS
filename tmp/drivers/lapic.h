#ifndef __LOCAL_APIC__
#define __LOCAL_APIC__
#include "../std/debug.h"



enum deliveryMode
{
    FIXED,
    LOWEST_PRIORITY,
    SMI_INT,
    RESERVED,
    NON_MASKABLE_INT,
    INIT_ASSERT,
    INIT_DEASSERT,
    START_UP
};

enum destinationMode
{
    PHYSICAL,
    LOGICAL
};

enum deliveryStatus
{
    IDLE,
    PENDING
};

enum triggerMode
{  
    // only used with INIT de-assert
    EDGE,
    LEVEL  
};


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
#define LAPIC_INSERVICE    0x100
#define LAPIC_TRIGGERMODE  0x180
#define LAPIC_INTREQ       0x200
#define LAPIC_ERRSTTS      0x280
#define LAPIC_LVTCMCI      0x2F0
#define LAPIC_INTCOMMND    0x300
#define LAPIC_LVTTIMER     0x320
#define LAPIC_LVTSENSTHERM 0x330
#define LAPIC_LVTPERFCNT   0x340
#define LAPIC_LVTINT0      0x350
#define LAPIC_LVTINT1      0x360
#define LAPIC_LVTERR       0x370
#define LAPIC_TIMERINITCNT 0x380
#define LAPIC_TIMERCURCNT  0x390
#define LAPIC_DIVCONFIG    0x3E0


// 1KiB
typedef struct alignpack(0x10) local_apic_type
{
    uint8_t  reserved0[16]; // 0x00 ->0x10
    uint8_t  padding0 [16]; // 0x10 ->0x20
    uint32_t ID;            // 0x20 ->0x30
    uint32_t VERSION;       // 0x30 ->0x40
    uint8_t  reserved2[48]; // 0x40 ->0x70
    uint8_t  padding1 [16]; // 0x70 ->0x80
    uint32_t TSKPRI;        // 0x80 ->0x90
    uint32_t ARBPRI;        // 0x90 ->0xA0
    uint32_t PROCPRI;       // 0xA0 ->0xB0
    uint32_t EOI;           // 0xB0 ->0xC0
    uint32_t RMTREAD;       // 0xC0 ->0xD0
    uint32_t LDEST;         // 0xD0 ->0xE0
    uint32_t DESTFMT;       // 0xE0 ->0xF0
    uint32_t SPURINTVEC;    // 0xF0 ->0x100
    uint32_t INSERVICE;     // 0x100->0x170
    uint8_t  padding2 [16]; // 0x170->0x180
    uint32_t TRIGGERMODE;   // 0x180->0x1F0
    uint8_t  padding3 [16]; // 0x1F0->0x200
    uint32_t INTREQ;        // 0x200->0x270
    uint8_t  padding4 [16]; // 0x270->0x280
    uint32_t ERRSTTS;       // 0x280->0x290
    uint8_t  reserved3[80]; // 0x290->0x2E0
    uint8_t  padding5 [16]; // 0x2E0->0x2F0
    uint32_t LVTCMCI;       // 0x2F0->0x300
    uint32_t INTCOMMND;     // 0x300->0x310
    uint8_t  padding6 [16]; // 0x310->0x320
    uint32_t LVTTIMER;      // 0x320->0x330
    uint32_t LVTSENSTHERM;  // 0x330->0x340 
    uint32_t LVTPERFCNT;    // 0x340->0x350
    uint32_t LVTINT0;       // 0x350->0x360
    uint32_t LVTINT1;       // 0x360->0x370
    uint32_t LVTERR;        // 0x370->0x380
    uint32_t TIMERINITCNT;  // 0x380->0x390
    uint32_t TIMERCURCNT;   // 0x390->0x3A0
    uint8_t  reserved4[48]; // 0x3A0->0x3D0
    uint8_t  padding7 [16]; // 0x3D0->0x3E0
    uint32_t DIVCONFIG;     // 0x3E0->0x3F0
    uint8_t  reserved5[16]; // 0x3F0->0x400
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
            uint16_t reserved1 : 12;
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


uint32_t read_lapic_reg   (void* lapic_base, uint16_t offset               );
void     write_lapic_reg  (void* lapic_base, uint16_t offset, uint32_t data);
void     set_lapic_address(uint64_t lapic_begin_address);
void     cpu_startup_init (uint8_t lapic_id);

#endif