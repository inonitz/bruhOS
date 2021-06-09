#ifndef __ACPI__
#define __ACPI__
#include "../std/debug.h"
#include "../../../shared/def.h"
#include "ioapic.h"



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


/* 
    * Note: This struct can't be used directly, only converting pointers
    * to a XSDT_t*
*/
typedef struct pack __ACPI_extended_root_system_description_table_type
{
    acpi_table_header_t hdr;
    uint64_t            table[1];
} XSDT_t;


/* 
    * Note: This struct can't be used directly, only converting pointers
    * to a MADT_t*
*/
typedef struct pack __interrupt_controller_header
{ 
    uint8_t int_type;
    uint8_t length;
} int_cnt_header;


// delivers info about different description tables, to allow for different capabillities
typedef struct pack __ACPI_multiple_acpi_description_table
{
    acpi_table_header_t hdr;
    uint32_t            lapic_begin; // local interrupt controller pointer
    uint32_t            flags;
    int_cnt_header          intcont[1];
} MADT_t;


// essentially marks a CPU in the system, delivers info on whether it can be used or not.
typedef struct pack __processor_local_apic_descriptor {
    int_cnt_header hdr;
    uint8_t    pid;
    uint8_t    apic_id;
    uint32_t   flags;
} LAPIC_t;


// I/O APIC Descriptor. Use ioapic_read_reg() with address to read/write-to the I/O APIC's Registers. 
typedef struct pack __input_output_apic
{
    int_cnt_header hdr;     
    uint8_t    io_apic_id; // id
    uint8_t    reserved;   // always 0
    uint32_t   address;    // IOAPIC_t address
    uint32_t   GSIbase;    // global sys interrupt base (address).
} IOAPIC_t;


// overrides an interrupt number N for a legacy device.
// we can use bus_src, irq_src, and GSI to find out where this override resides
typedef struct pack __interrupt_source_override
{
    int_cnt_header hdr;
    uint8_t    bus_src;
    uint8_t    irq_src;
    uint32_t   GSI;
    uint16_t   flags;
} INT_SRC_OVERRIDE_t;



typedef struct pack __local_non_maskable_interrupts
{
    int_cnt_header hdr;
    uint8_t    pid;  // to which cpu this belongs. 0xff is all
    uint16_t   flags;
    uint8_t    LINT; // which LINT - 1 or 0
} LNMI_t;


void init_acpi(ROXSD_t* acpi_rsdp_descriptor);


#endif