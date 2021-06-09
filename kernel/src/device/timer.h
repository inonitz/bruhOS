#pragma once
#ifndef __HPET_TIMER__
#define __HPET_TIMER__
#include "../acpi/ioapic.h"
#include "../acpi/acpi.h"



//
#define ONE_OVER_FEMTOSECOND               0x38d7ea4c68000 // 10**15
#define HPET_CAPABILITIES_ID_REG           0x00
#define HPET_CONFIG_REG                    0x10
#define HPET_INTERRUPT_STATUS_REG          0x20
#define HPET_MAIN_COUNT_VAL_REG            0xF0
#define TIMERN_CONFIG_CAPABILITY_REG(n)   (0x100 + 0x20 * (n))
#define TIMERN_COMPARATOR_VALUE_REG(n)    (0x108 + 0x20 * (n))
#define TIMERN_FSB_INTERRUPT_ROUTE_REG(n) (0x110 + 0x20 * (n))

#define HPET_COMP_COUNT_MASK             0x1f
#define HPET_COUNTER64_CAPABLE           (1 << 5)
#define HPET_LEGACY_REPLACEABLE          (1 << 1)
#define CONFIG_REG_ENABLE_TIMER          (1     )
#define CONFIG_REG_REPLACE_LEGACY        (1 << 2)
#define INTERRUPT_STATUS(n)              (1 << (n))
     
#define TIMER_INTERRUPT_TYPE             (1 << 1)
#define TIMER_ENABLE_INTERRUPT           (1 << 2)
#define TIMER_PERIODIC                   (1 << 3)
#define TIMER_PERIODIC_SUPPORTED         (1 << 4)
#define TIMER_IS64BIT                    (1 << 5)
#define TIMER_CAN_SET_ACCUMULATOR        (1 << 6)
#define TIMER_FORCE32BIT                 (1 << 8)
#define TIMER_IOAPIC_ROUTE               (1 << 9)
#define TIMER_IOAPIC_ROUTE_MASK          0x1f
#define TIMER_ENABLE_FSB_INT             (1 << 14)
#define TIMER_FSB_INT_SUPPORTED          (1 << 15)
#define TIMER_IOAPIC_CAPABLE_ROUTES      (1 << 32)
#define TIMER_IOAPIC_CAPABLE_ROUTES_MASK 0xffffffff

typedef struct pack __hpet_address_structure
{
    uint8_t  address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  reserved;
    uint64_t address;
} address_structure;


typedef struct pack __high_precision_event_timer
{
    acpi_table_header_t hdr;
    uint8_t             hardware_rev_id;
    uint8_t             flags;
    uint16_t            pci_vendor_id;
    address_structure   registers;
    uint8_t             hpet_number;
    uint16_t            minimum_tick;
    uint8_t             page_protection;
} HPET_t;

typedef struct pack __hpet_flags
{
    uint8_t comparator_count   : 5;
    uint8_t counter_size       : 1;
    uint8_t reserved           : 1;
    uint8_t legacy_replacement : 1;
} HPET_FLAGS_t;


typedef struct __general_capabilities_and_id_register
{
    uint8_t  revid;
    uint8_t  flags;
    uint16_t vendorid;
    uint32_t counter_clock_period;
} GCI_R;

typedef struct pack __GCI_REGISTER_flags
{
    uint8_t timers_minus1      : 5;
    uint8_t capable64count     : 1;
    uint8_t reserved           : 1;
    uint8_t legacy_replaceable : 1;
} GCIR_FLAGS_t;


typedef struct pack __general_configuration_register
{
    uint8_t  enable_timer   : 1;
    uint8_t  legacy_replace : 1;
    uint64_t reserved       : 62;
} GC_R;


typedef struct __general_interrupt_status_register
{
    uint32_t interrupt_status; // for level triggered interrupts - if timer[bit n] is active, software can clear this by writing 1 to this.
    uint32_t reserved;
} GIS_R;


typedef uint64_t MCV_R;


typedef struct __timer_n_configuration_capabilities_register
{
    uint16_t flags;
    uint16_t reserved;
    uint32_t int_routable; // each bit corresponds to an IRQ that this timer can be routed to.
} TN_CC_R;

typedef struct pack __TNCCR_flags
{
    uint8_t reserved0     : 1;
    uint8_t int_type      : 1;
    uint8_t enable_int    : 1;
    uint8_t periodic      : 1;
    uint8_t periodic_sup  : 1;
    uint8_t is64bit       : 1;
    uint8_t can_set_accum : 1;
    uint8_t reserved1     : 1;
    uint8_t settimer32    : 1;
    uint8_t ioapic_route  : 5;
    uint8_t fsb_int_map   : 1;
    uint8_t fsb_int_sup   : 1;
} TN_CCR_FLAGS;


typedef uint64_t TN_COMP_VAL_R;


// only convert pointers to a pointer of this.
typedef struct pack __hpet_register_table
{
    GCI_R    capabilites_id;
    uint64_t pad0;
    GC_R     config;
    uint64_t pad1;
    GIS_R    int_status;
    uint64_t pad2[25];
    MCV_R    main_counter;
    uint64_t pad3;
    struct pack {
        TN_CC_R       config_cap;
        TN_COMP_VAL_R comparator;
        uint64_t      FSB_INT_ROUTE_REG;
        uint64_t      padN;
    } timer[1];
} HPET_REG_TABLE_t;



uint64_t hpet_read_reg   (uint64_t reg_base, uint16_t offset               );
void     hpet_write_reg  (uint64_t reg_base, uint16_t offset, uint64_t data);


/* 
    * Initialize the HPET timer in hardware, and the struct that manages it.
    * volatile HPET_t* table - the HPET_t* table found in the ACPI MADT table.
*/
void     HPET_init       (volatile HPET_t* table                           );


/* 
    * Enable/Disable the Timer struct that controls 
    * the hardware HPET Main Counter Register and an available Comparator Register
*/
void     enableTimer     (                                                 );
void     disableTimer    (                                                 );


/* 
    * Will return from the function when it finished counting the 'milliseconds'.
    * uint64_t milliseconds - the amount of milliseconds the timer will count.
*/
void     count           (uint64_t milliseconds                            );


/* 
    * Will return from the function when it finished counting the 'nanoseconds'.
    * uint64_t nanoseconds - the amount of milliseconds the timer will count.
*/
void     countns         (uint64_t nanoseconds                             );


/* 
    * Returns the current value in the Main Counter Register.
    * Simply, it returns the current Count of the counter.
*/
uint64_t poll_timer_count(                                                 );


/* 
    * Returns the status of the timer - inactive/finished counting, or currently counting.
*/
bool_t   timerPolled     (                                                 );

#endif