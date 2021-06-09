#pragma once
#ifndef __ACPI__
#define __ACPI__
#include "../std/io.h"



#define INT_CONTROLLER_PROCESSOR_LOCAL_APIC         0
#define INT_CONTROLLER_IO_APIC                      1
#define INT_CONTROLLER_INTERRUT_SOURCE_OVERRIDE     2
#define INT_CONTROLLER_NON_MASKABLE_INTERRUPT_SRC   3
#define INT_CONTROLLER_LOCAL_APIC_NMI               4
#define INT_CONTROLLER_LOCAL_APIC_ADDRESS_OVERRIDE  5
#define INT_CONTROLLER_IO_SAPIC                     6
#define INT_CONTROLLER_LOCAL_SAPIC                  7
#define INT_CONTROLLER_PLATFORM_INTERRUPT_SOURCES   8
#define INT_CONTROLLER_PROCESSOR_LOCAL_X2APIC       9
#define INT_CONTROLLER_LOCAL_X2APIC_NMI             10
#define INT_CONTROLLER_GIC_CPU_INTERFACE            11
#define INT_CONTROLLER_GIC_DISTRIBUTOR              12
#define INT_CONTROLLER_GIC_MSI_FRAME                13
#define INT_CONTROLLER_REDISTRIBUTER                14
#define INT_CONTROLLER_INTERUPT_TRANSLATION_SERVICE 15
#define INT_CONTROLLER_MULTIPROCESSOR_WAKEUP        16
#define INT_CONTROLLER_RESERVED_START               17
#define INT_CONTROLLER_RESERVED_END                 127
#define INT_CONTROLLER_RESERVED_OEM_START           128
#define INT_CONTROLLER_RESERVED_OEM_END             255


enum deliveryMode    { FIXED, LOWEST_PRIORITY, SMI_INT, RESERVED, NON_MASKABLE_INT, INIT_ASSERT, INIT_DEASSERT, START_UP };
enum destinationMode { 
    PHYSICAL,
    LOGICAL
};
enum deliveryStatus { 
    IDLE,
    PENDING
};
enum triggerMode  {
    TCONFORMING = 0x0, 
    EDGE        = 0x1, 
    LEVEL       = 0x3
};
enum pin_polarity { 
    PCONFORMING = 0x0,
    ACTIVE_HIGH = 0x1, 
    ACTIVE_LOW  = 0x3 
};


typedef struct pack __acpi_description_table_header
{
	char_t   signature[4];
	uint32_t length;
	uint8_t  revision;
	uint8_t  checksum;
	uint8_t  OEM_ID[6];
	uint8_t  OEM_TABLE_ID[8];
	uint32_t OEM_REV;
	uint32_t creatorID;
	uint32_t creatorREV;
} acpi_table_header_t;


// Note: This struct can't be used directly, only convert pointers to XSDT_t*
typedef struct pack __ACPI_extended_root_system_description_table_type
{
    acpi_table_header_t hdr;
    uint64_t            table[1];
} XSDT_t;


// Note: This struct can't be used directly, only convert pointers to MADT_t*
typedef struct pack __interrupt_controller_header
{ 
    uint8_t int_type;
    uint8_t length;
} int_cnt_header;


// array of tables, each of which has info about different system capabillities 
typedef struct pack __ACPI_multiple_acpi_description_table
{
    acpi_table_header_t hdr;
    uint32_t            lapic_begin; // bsp local interrupt controller pointer
    uint32_t            flags;
    int_cnt_header      intcont[1];
} MADT_t;


// essentially 'marks' a CPU in the system, delivers info on whether it can be used or not.
typedef struct pack __processor_local_apic_descriptor {
    int_cnt_header hdr;
    uint8_t        puid;
    uint8_t        apic_id;
    uint32_t       flags;
} LAPIC_t;


// I/O APIC Descriptor. Use ioapic_read_reg() with address to read/write-to the I/O APIC's Registers. 
typedef struct pack __input_output_apic
{
    int_cnt_header hdr;     
    uint8_t        io_apic_id; // id
    uint8_t        reserved;   // always 0
    uint32_t       address;    // IOAPIC_t address
    uint32_t       GSIbase;    // global sys interrupt base (address).
} IOAPIC_t;


// overrides an interrupt number N for a legacy device.
// we can use bus_src, irq_src, and GSI to find out where this override resides
typedef struct pack __interrupt_source_override
{
    int_cnt_header hdr;
    uint8_t        bus_src;
    uint8_t        irq_src;
    uint32_t       GSI;
    uint16_t       flags;
} INT_SRC_OVERRIDE_t;



typedef struct pack __local_non_maskable_interrupts
{
    int_cnt_header hdr;
    uint8_t        pid;  // to which cpu this belongs. 0xff is all
    uint16_t       flags;
    uint8_t        LINT; // which LINT - 1 or 0
} LNMI_t;


static inline bool_t tableChecksum(acpi_table_header_t* tableHeader)
{
    uint8_t sum = 0;
    for (int i = 0; i < tableHeader->length; i++)
    {
        sum += ((char_t*)tableHeader)[i];
    }
    return sum == 0;
}



#endif
