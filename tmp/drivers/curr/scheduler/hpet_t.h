#pragma once
#ifndef __HPET_TIMER__
#define __HPET_TIMER__
#include "../acpi/acpi.h"


// register offsets
#define HPET_GENERAL_CAPABILITIES_ID_REG   0x00
#define HPET_GENERAL_CONFIG_REG            0x10
#define HPET_GENERAL_INT_STATUS_REG        0x20
#define HPET_MAIN_COUNTER_VALUE_REG        0xF0
#define HPET_TIMER_N_CAPABILITES_ID_REG(n) 0x100 + 0x20 * n          
#define HPET_TIMER_N_COMPARATOR_VAL_REG(n) 0x108 + 0x20 * n
#define HPET_TIMER_N_FSB_INT_ROUTE_REG(n)  0x110 + 0x20 * n

// get info from registers
#define HPET_GCIREG_READ_COUNTER_CLOCK_PERIOD(reg) ((reg >> 32) & 0xFFFFFFFF)
#define HPET_GCIREG_READ_VENDOR_ID(reg)            ((reg >> 16) & 0xffff)
#define HPET_GCIREG_READ_LEG_ROUTE(reg)            ((reg >> 15) & 1)
#define HPET_GCIREG_READ_COUNTSIZE64(reg)          ((reg >> 13) & 1)
#define HPET_GCIREG_READ_TIMER_COUNT(reg)          ((reg >> 8) & 0x1f)

#define HPET_CONFIGREG_ISENABLED(reg)              (reg & 1)
#define HPET_CONFIGREG_LEGACY_ROUTE(reg)           (reg >> 1) & 1
#define HPET_INTN_ISACTIVE_REG(reg, timer_idx)     (reg & (1 << timer_idx))

#define HPET_TN_CONFIGCAPREG_READ_INT_TYPE(reg)    boolean(reg & (1 << 1))
#define HPET_TN_CONFIGCAPREG_READ_INT_ENABLED(reg) boolean(reg & (1 << 2))
#define HPET_TN_CONFIGCAPREG_READ_PERIODIC(reg)    boolean(reg & (1 << 3))
#define HPET_TN_CONFIGCAPREG_READ_PER_CAPABLE(reg) boolean(reg & (1 << 4))
#define HPET_TN_CONFIGCAPREG_READ_TIMER64(reg)     boolean(reg & (1 << 5))
#define HPET_TN_CONFIGCAPREG_READ_SET_PER_VAL(reg) boolean(reg & (1 << 6))
#define HPET_TN_CONFIGCAPREG_READ_FORCE32(reg)     boolean(reg & (1 << 8))
#define HPET_TN_CONFIGCAPREG_READ_IOAPIC_ROUTE(reg)((reg >> 8) & 0x1f)
#define HPET_TN_CONFIGCAPREG_READ_FSB_ENABLE(reg)  ((reg >> 14) & 1)
#define HPET_TN_CONFIGCAPREG_READ_FSB_INT_DEL(reg) ((reg >> 15) & 1)
#define HPET_TN_CONFIGCAPREG_READ_IOAPIC_CAN_ROUTE(reg) ((reg >> 32) & 0xFFFFFFFF)


// write info to register at bits N-M ( N <= M)
#define HPET_CONFIGREG_ENABLE_TIMER(reg)                  (reg |= 1)
#define HPET_CONFIGREG_DISABLE_TIMER(reg)                 (reg &= ~1)
#define HPET_CONFIGREG_ENABLE_LEGACY_IRQ (reg)            (reg |= 2)
#define HPET_CONFIGREG_DISABLE_LEGACY_IRQ(reg)            (reg &= ~2)

#define HPET_INTN_CLEAR_LEVEL_TRIGGER_REG(reg, timer_idx) (reg |= (1 << timer_idx))


typedef struct pack __hpet_address_structure
{
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} hpet_address;


typedef struct pack __high_precision_event_timer
{
    acpi_table_header_t hdr;
    uint8_t      hardware_rev_id;
    uint8_t      flags;         // struct definition below, DONT CONVERT TO IT. BIT FIELDS ARE EVIL. ONLY BITMASKS.
    uint16_t     pci_vendor_id;
    hpet_address tableaddr;
    uint8_t      hpet_number;
    uint16_t     min_tick;
    uint8_t      page_protection;
} HPET_t;

typedef struct pack __hpet_flags
{
    uint8_t comparators  : 5;
    uint8_t counter64    : 1;
    uint8_t reserved     : 1;
    uint8_t hasLegacyISA : 1; // can IRQ's be connected by replacing legacy interrupts. 
} HPET_FLAGS;



void     hpet_init(HPET_t* hpet);
uint64_t hpet_read_reg (uint64_t hpet_table_base, uint16_t offset);
void     hpet_write_reg(uint64_t hpet_table_base, uint16_t offset, uint64_t data);

#endif