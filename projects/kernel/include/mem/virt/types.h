#ifndef __KERNEL_C_DEFINITION_PAGING_TYPES__
#define __KERNEL_C_DEFINITION_PAGING_TYPES__
#include <std/stdint.h>
#include <std/macro.h>
#include <std/static_assert.h>


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

typedef struct pack alignsz(PAGE_SIZE) __PageTable
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

typedef struct alignsz(PAGE_SIZE) __PageDirectory
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

typedef struct alignsz(PAGE_SIZE) __PageDirectoryPointerTable
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

typedef struct alignsz(PAGE_SIZE) __PageMapLevel4
{
    PML4E ptrtables[512];
} PML4;


typedef struct pack __virtualAddress_4KiB_Page_Size
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


typedef struct alignsz(PAGE_SIZE) __genericPageTable
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


KERNEL_STATIC_ASSERT(sizeof(PageTable) == PAGE_SIZE, 
    __genericPageTableSizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(TableEntry) == sizeof(uint64_t), 
    __GenericTableEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PML4) == PAGE_SIZE, 
    __PageMapLevel4SizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PML4E) == sizeof(uint64_t), 
    __PageMapLevel4EntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PDPT) == PAGE_SIZE, 
    __PageDirectoryPointerTableSizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
); 
KERNEL_STATIC_ASSERT(sizeof(PDPTE) == sizeof(uint64_t), 
    __PageDirectoryPointerTableEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PD) == PAGE_SIZE, 
    __PageDirectorySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PDE) == sizeof(uint64_t), 
    __PageDirectoryEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PT) == PAGE_SIZE, 
    __PageTableSizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PTEntry) == sizeof(uint64_t), 
    PageTableEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);


#endif /* __KERNEL_C_DEFINITION_PAGING_TYPES__ */
