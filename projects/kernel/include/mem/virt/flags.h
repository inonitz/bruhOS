#ifndef __PAGING_TABLE_ENTRY_FLAGS__
#define __PAGING_TABLE_ENTRY_FLAGS__
#include <std/int.h>
#include <std/macro.h>


// modifiable flags
#define PRESENT             0x1
#define ALSO_WRITEABLE      0x2
#define USER_ACCESSIBLE     0x04
#define WRITE_THROUGH       0x08
#define UNCACHEABLE         0x10
#define PAGING_ATTRIB_TABLE 0x80
#define NOFLUSH_GLOBAL      0x100
#define NO_EXECUTION        0x8000000000000000

// read only flags
#define ACCESSED            0x20   
#define DIRTY               0x40




typedef struct __pageTableEntry_bitFlags
{
    union
    {
        struct pack 
        {
            uint8_t present       : 1; 
            uint8_t writeable     : 1; // readable by default.
            uint8_t userAccess    : 1;
            uint8_t always_latest : 1; //      TRUE  (write-through) - data modified in cache is always updated also in main memory.
            uint8_t cacheNoAccess : 1; // ^^^^ FALSE (write-back   ) - data in main memory is updated when the data is cache evicted.
            uint8_t padding0      : 2;
            uint8_t attrib_tbl    : 1; // uses the Paging Attribute Table.
            uint8_t tlb_noflush   : 1; // global attribute - data won't be evicted from the TLB Cache on CR3 Change.
            uint8_t available     : 3; // available for the OS.
            uint8_t padding1[6];       // address + ignore0 + protkey. should never be set.
            uint8_t padding2      : 3; // address + ignore0 + protkey. should never be set.
            uint8_t noExecution   : 1; // TRUE - can't execute code in this page.
        };
        QWORD qword;
    };
} paging_flags_t;


/* 
    * clears the paging_flags from unwanted bits.
    * Because the flags are 1:1 from the paging tables,
    * some attributes should NOT be set by paging_flags_t,
    * which means we need to clear them.
    * NOTE:
    * 0x8000000000000f9f == 0b1000000000000000000000000000000000000000000000000000111110011111.
    * the bits im keeping are:
    *   present
    *   writeable
    *   userAccess
    *   always_latest
    *   cacheNoAccess
    * 
    *   attrib_tbl 
    *   tlb_noflush
    *   available
    *   
    *   noExecution
    * 
*/
static __force_inline uint64_t sanitize_flags(uint64_t flags)
{
    return flags & 0x8000000000000f9f;
}


#endif
