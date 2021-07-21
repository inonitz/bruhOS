#pragma once
#ifndef __PAGING__
#define __PAGING__
#include "../../std/io.h"



/*  
    * File Description: Paging Type Definitions.
    * 1. Every Table struct is coupled with a struct describing its entries:
    *   A. Specific Table Entries && Tables.
    *   B. Generic Table Entry & Table for conversion and easier use.
    *      Note: Generic entries\table have the most common attributes along all entries\tables.
    *
    * There are many ways I can address pages (2MiB, 4KiB, 1GiB)
    * I don't have enough time to support all 3+ types. So, I'm using 4KiB Pages.
    * In the future, if you're still working on this, add 2MiB && 1GiB pages.
    * typedef struct pack __virtualAddress_2MiB_Page_Size
    * {
    *     uint32_t page       : 21;
    *     uint16_t pageDir    : 9;
    *     uint16_t pageDirPtr : 9;
    *     uint16_t pageMapL4  : 9;
    *     uint16_t bit47Copy  : 16;
    * } virtualAddressMB;
    *
    * 
    * typedef struct pack __virtualAddress_1GiB_Page_Size
    * {
    *     uint32_t page       : 30;
    *     uint16_t pageDirPtr : 9;
    *     uint16_t pageMapL4  : 9;
    *     uint16_t bit47Copy  : 16;
    * } virtualAddressGB;
    * 
    * 
    * 
*/


typedef struct pack PageTableEntry
{
    uint8_t  present     : 1;
    uint8_t  rw          : 1;
    uint8_t  userOrSuper : 1; // 1 - user access enabled.
    uint8_t  wthrough    : 1;
    uint8_t  cachedis    : 1;
    uint8_t  accessed    : 1;
    uint8_t  dirty       : 1;
    uint8_t  PAT         : 1;
    uint8_t  global      : 1;  // if Cr4.PGE = 1. ignore otherwise
    uint8_t  available   : 3;
    uint64_t address     : 40; // unused address bits must be 0.     
    uint8_t  ignore0     : 7;
    uint8_t  ProtKey     : 4;  // if CR4.PKE       = 1.    
    uint8_t  noExec      : 1;  // if IA32_EFFR.NXE = 1. otherwise must be 0
} PTEntry;

typedef struct pack align(PAGE_SIZE) __PageTable
{
    PTEntry page[512];
} PT;




typedef struct pack __PageDirectoryEntry
{
    uint8_t  present     : 1;
    uint8_t  rw          : 1;
    uint8_t  userOrSuper : 1;
    uint8_t  wthrough    : 1;
    uint8_t  cachedis    : 1;
    uint8_t  accessed    : 1;
    uint8_t  ignore0     : 1;
    uint8_t  pageSize    : 1; // 0 = 4KB, 1 = 4MiB
    uint8_t  ignore1     : 4;
    uint64_t address     : 40;
    uint16_t ignore2     : 11;
    uint8_t  noExec      : 1;
} PDE;

typedef struct align(PAGE_SIZE) __PageDirectory
{
    PDE dirs[512];
} PD;




typedef struct pack __PageDirectoryPointerTableEntry
{
    uint8_t  present     : 1;
    uint8_t  rw          : 1;
    uint8_t  userOrSuper : 1;
    uint8_t  wthrough    : 1;
    uint8_t  cachedis    : 1;
    uint8_t  accessed    : 1;
    uint8_t  ignore0     : 1;
    uint8_t  pageSize    : 1;
    uint8_t  ignore1     : 4;
    uint64_t address     : 40; // bits that are not used must be 0
    uint16_t ignore2     : 11;
    uint8_t  noExec      : 1;
} PDPTE;

typedef struct align(PAGE_SIZE) __PageDirectoryPointerTable
{
    PDPTE dirPtr[512];
} PDPT;




typedef struct pack __PageMapLevel4Entry
{
    uint8_t  present     : 1;
    uint8_t  rw          : 1;
    uint8_t  userOrSuper : 1;
    uint8_t  wthrough    : 1;
    uint8_t  cachedis    : 1;
    uint8_t  accessed    : 1;
    uint8_t  ignore0     : 1;
    uint8_t  reserved    : 1; // always 0
    uint8_t  ignore1     : 4;
    uint64_t address     : 40; // unused bits must be 0
    uint16_t ignore2     : 11;
    uint8_t  noExec      : 1;
} PML4E;

typedef struct align(PAGE_SIZE) __PageMapLevel4
{
    PML4E ptrtables[512];
} PML4;



typedef struct pack align(PAGE_SIZE) __virtualAddress_4KiB_Page_Size
{
    union {
        struct pack {
            uint16_t page : 12; // Page      
            uint16_t pt   : 9;  // P-Table   (PT)
            uint16_t pd   : 9;  // PT-Table  (PD)
            uint16_t pdp  : 9;  // PD-Table  (PDP)
            uint16_t pml4 : 9;  // PDP-Table (PML4)
            uint16_t scpy : 16; // copy bit 47, last bit of mapL4.
        };
        uint64_t ui64;
        void*    ptr;
    };
} virtualAddress;




// generic table/entry types that can be converted to any other table\entry.
typedef struct pack __GenericTableEntry
{
    union 
    {
        struct pack {
            uint8_t  present     : 1;
            uint8_t  rw          : 1;
            uint8_t  userOrSuper : 1;
            uint8_t  wthrough    : 1;
            uint8_t  cachedis    : 1;
            uint8_t  accessed    : 1;
            uint8_t  ignore0     : 1;
            uint8_t  pageSize    : 1; // 0 = 4KB, 1 = 4MiB
            uint8_t  ignore1     : 4;
            uint64_t address     : 40;
            uint16_t ignore2     : 11;
            uint8_t  noExec      : 1;
        };
        uint64_t ui64;
    };
} TableEntry;

typedef struct align(PAGE_SIZE) __genericPageTable
{
    TableEntry entry[512];
} PageTable;




/* 
    * Planning for this to take care of the tables of each process.
    * In progress, might delete / heavily edit later.
*/
typedef struct __VMM_PageTableManager
{
    PageTable* PML4Table;
} TableManager;



/* 
    * Map a virtual address to a physical address, given an active PML4 Paging table.
    * Table    - the PML4 table for the current proccess.
    * virtual  - the address we want to map 
    * physical - the address in 'physical' memory the virtual address will be mapped to
*/
void mapVirtualToPhysical(
    IN PML4* Table,
    IN void* vaddress,
    IN void* paddress
);


/* 
    * This function does the same as mapVirtualToPhysical, EXCEPT: 
    * This function also takes entry flags as defined above, and in the intel specification.
    * uint64_t flags - the flags for the table entries that will be created. (paging_flags_t struct)
    *                  DO NOTE that flags.bit[N] is mapped to table_entry.flag_bit[N],
*/
void map_vtop(
    IN PML4*   Table, 
    IN void*   vaddress, 
    IN void*   paddress, 
    IN uint64_t flags
);


/* 
    * unmaps a page in the PML4 Table, by setting it non-present. All other previous attributes are kept.
    * PML4* Table   - a pointer to the current PML4 Table in physical memory.
    * void* virtual - the virtual address that will be unmapped (a single page).
*/
void unmapVirtual(PML4* Table, void* vaddress);


/*
    * Load the new page table, given its address
    * this function sets the cr3 register to the pml4_address given
    * 
*/
void load_cr3_reg(PML4* table_ptr);


/* 
    * Initialize paging with an already known PML4 table.
    * This function only initializes the Table manager, nothing else.
*/
void handover_paging(PML4* pml4);


/* 
    * get the current PML4 Table address loaded in the CR3 register.
*/
PML4* getKernelCR3();


/* 
    * Returns the Requesting Processors' Loaded PML4.
*/
PML4* getCurrentCR3();

#endif