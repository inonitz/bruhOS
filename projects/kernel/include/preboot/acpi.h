#ifndef __KERNEL_C_DEFINITION_PREBOOT_ACPI_CONFIGURATION__
#define __KERNEL_C_DEFINITION_PREBOOT_ACPI_CONFIGURATION__
#include <std/stdint.h>
#include <std/enable_bool.h>
#include <std/bool.h>


typedef struct __attribute__((packed)) __root_system_description_pointer_descriptor
{
    char     signature[8];
    uint8_t  checksum;
    char     OEMID[6];
    uint8_t  revision;
    uint32_t rsdtAddress;
} RSDPdescriptor10;


typedef struct __attribute__((packed)) __root_system_description_pointer_descriptor20
{
    RSDPdescriptor10 first;
    uint32_t         length;
    uint64_t         xsdtAddress;
    uint8_t          exchecksum;
    uint8_t          reserved[3];
} RSDPdescriptor20;


typedef struct __attribute__((packed)) __multiple_apic_description_table
{
    uint8_t  signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    uint8_t  OEM_ID[6];
    uint64_t OEM_TABLE_ID;
    uint32_t OEM_REVISION;
    uint32_t creator_id;
    uint32_t creator_rev;
    uint32_t LICA; // local interrupt controller address - The Address in memory (physical) of the BSP LAPIC
    uint32_t flags;

} MAPICDT;


typedef struct __advanced_configuration_and_power_interface_config
{
    uint64_t  address;  
    bool_t    extended;   // address == RSDT / XSDT
    uint8_t   padding[3];
    uint32_t  procCount;  // cores on the CPU
} ACPIinitialConfig;


#endif /* __KERNEL_C_DEFINITION_PREBOOT_ACPI_CONFIGURATION__ */