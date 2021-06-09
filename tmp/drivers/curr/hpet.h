#ifndef __HPET_TIMER_IMPL__
#define __HPET_TIMER_IMPL__
#include "../std/debug.h"
#include "../acpi/acpi.h"



#define RO // read-only  access
#define RW // read-write access
#define ONE_OVER_FEMTOSECOND 0x38d7ea4c68000 // 10^15

typedef struct pack __hpet_address
{
    uint8_t  address_space_id; // 0 - system memory. 1 - system I/O.
    uint8_t  reg_width;
    uint8_t  reg_offset;
    uint8_t  reserved;
    uint64_t address;
} hpet_address;

typedef struct pack __high_precision_event_timer
{
    acpi_table_header_t hdr;
    uint8_t             hardware_rev_id;
    uint8_t             comparator_count   : 5;
    uint8_t             counter_size       : 1;
    uint8_t             reserved           : 1;
    uint8_t             legacy_replacement : 1;
    uint16_t            pci_vendor_id;
    hpet_address        regtable;
    uint8_t             hpet_number;
    uint16_t            minimum_tick;
    uint8_t             page_protection;
} HPET_t;


// problem: bit fields are wrong when reading from them.
// solution: use bit shifts, and NEVER USE THIS STRUCT.
// ONLY use this as a reference for the register structure.
typedef struct pack __general_capabilities_and_id_register
{
    union {
        struct pack {
            uint8_t  rev_id;
            uint8_t  tcount   : 5; // available timers.
            uint8_t  cmode64  : 1;
            uint8_t  reserved : 1;
            uint8_t  lgcy_map : 1;
            uint16_t vendor_id;
            uint32_t clock_period; // in femto-seconds (10^-15). 0 < clock_period <= 0x05F5E100 (100ns)
        };
        struct pack {
            uint32_t low;
            uint32_t high;
        };
    };
} GENCAP_ID_REG;

typedef struct pack __general_configuration_register
{
    uint8_t  enable_cnf : 1; // enable this timer.
    uint8_t  leg_rt_cnf : 1; // legacy replacement mapping
    uint64_t reserved  : 62;
} CONFIGREG;

typedef struct pack __general_interrupt_status_register
{
    uint32_t timer_int_status;
    uint32_t reserved;
} GINTSREG;

typedef struct pack __main_counter_value_register
{
    // write only when ENABLE_CNF = 0.
    // if 64 - return main_cval, else return lower 32 bits.
    union {
        uint64_t cntval;
        struct pack {
            uint32_t cnt_lower;
            uint32_t cnt_upper;
        };
    };
} COUNT_REG;

typedef struct pack __timer_n_configuration_and_capability_register
{
    union {
        struct pack {
            uint8_t  reserved0    : 1;
            uint8_t  int_type     : 1;
            uint8_t  int_enb_cnf  : 1;
            uint8_t  periodic_t   : 1;
            uint8_t  period_cap   : 1;
            uint8_t  size64       : 1;
            uint8_t  val_set      : 1;
            uint8_t  reserved1    : 1;
            uint8_t  force32      : 1;
            uint8_t  ioapic_route : 5;
            uint8_t  fsb_map      : 1;
            uint8_t  fsb_cap      : 1;
            uint16_t reserved;
            uint32_t apic_irq_bitx;
        };
        uint64_t u64;
    };
} TN_CONFIG_CAP_REG;

typedef struct pack __timer_n_comparator_value_register
{
    union {
        uint64_t u64;
        struct pack { // if the timer is 32bit ONLY USE THIS!
            uint32_t u32;
            uint32_t reserved;
        };
    };
} TN_CMPVAL_REG;


// ONLY convert pointers to HPET_REG_TABLE_t* when they have the base address for this struct
typedef struct pack __hpet_registers_table
{
    GENCAP_ID_REG RO capabilites_id_reg; // 0x000 -> 0x007
    uint64_t         pad0;               // 0x008 -> 0x010
    CONFIGREG     RW config_reg;         // 0x010 -> 0x017
    uint64_t         pad1;               // 0x018 -> 0x020
    GINTSREG      RW int_stts_reg;       // 0x020 -> 0x027
    uint64_t         pad2[25];           // 0x28  -> 0x0F0
    COUNT_REG     RW main_counter_reg;   // 0x0F0 -> 0x0F7
    uint64_t         pad3;
    struct pack {
        TN_CONFIG_CAP_REG cpr;
        TN_CMPVAL_REG     comp;
        uint64_t          fsb_irr; // fsb interrupt route register, unused.
        uint64_t          padN;
    } TimerN[1]; // there are N (N > 0) timers, not 1. using this to access specific ones.
} HPET_REG_TABLE_t;



typedef uint8_t TN_INT_ROUTE;

typedef struct pack __hpet_timer_struct
{
    bool_t            lcntrs    : 1; // is the main counter 64bit.
    bool_t            enabled   : 1; // if this timer is enabled.
    bool_t            config    : 2; // ISA, Normal Or PCI configurated. [enum in hpet.c]
    uint8_t           reserved0 : 4; 
    uint8_t           frtimer;      // offset of a free timer in the register table
    uint16_t          tickmin;      // 
    uint32_t          period;       // period = 10^15 / clock_period = seconds per full cycle 
    TN_INT_ROUTE      irqs[32];     // irq connections to each timer in the table. there is a hardware limit of 32 timers (min = 3)
    HPET_REG_TABLE_t* registers;    // the HPET registers' address
    HPET_t*           table;        // the actual table (address)
    uint64_t          reserved1;
} hpet_timer;


/* 
    * Initializes the HPET with the relevant information in order to send interrupts.
*/
void hpet_init(HPET_t* hpet);


/*
    * Enables the HPET Timer.
*/
void hpet_enable();


/* 
    * Disables the HPET Timer (Doesn't send interrupts)
*/
void hpet_disable();



/* 
    * resets the HPET counters.
*/
void hpet_restart();



//////////////////////////// LOCAL TIMER //////////////////////////// 
static hpet_timer timer;
//////////////////////////// LOCAL TIMER ////////////////////////////

#endif