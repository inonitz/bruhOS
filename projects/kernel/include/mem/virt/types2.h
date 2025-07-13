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
#ifdef VMM_TABLE_ENTRY_PRESENT_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_PRESENT_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_PRESENT_BIT
#   error "Macro 'VMM_TABLE_ENTRY_PRESENT_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_READWRITE_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_READWRITE_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_READWRITE_BIT
#   error "Macro 'VMM_TABLE_ENTRY_READWRITE_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_USERACCESS_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_USERACCESS_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_USERACCESS_BIT
#   error "Macro 'VMM_TABLE_ENTRY_USERACCESS_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_WRITETHROUGH_BIT
#   error "Macro 'VMM_TABLE_ENTRY_WRITETHROUGH_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_DONTCACHE_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_DONTCACHE_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_DONTCACHE_BIT
#   error "Macro 'VMM_TABLE_ENTRY_DONTCACHE_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_DIRTYBIT_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_DIRTYBIT_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_DIRTYBIT_BIT
#   error "Macro 'VMM_TABLE_ENTRY_DIRTYBIT_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_LARGEPAGE_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_LARGEPAGE_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_LARGEPAGE_BIT
#   error "Macro 'VMM_TABLE_ENTRY_LARGEPAGE_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_ADDRESS_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_ADDRESS_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_ADDRESS_BIT
#   error "Macro 'VMM_TABLE_ENTRY_ADDRESS_BIT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_NOEXECUTE_SHIFT
#   error "Macro 'VMM_TABLE_ENTRY_NOEXECUTE_SHIFT' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_NOEXECUTE_BIT
#   error "Macro 'VMM_TABLE_ENTRY_NOEXECUTE_BIT' already defined somewhere else!"
#endif

#ifdef VMM_TABLE_ENTRY_GET_PRESENT_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_PRESENT_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_READWRITE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_READWRITE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_USERACCESS_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_USERACCESS_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_WRITETHROUGH_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_WRITETHROUGH_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_DONTCACHE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_DONTCACHE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_DIRTYBIT_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_DIRTYBIT_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_LARGEPAGE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_LARGEPAGE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_NOEXECUTE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_GET_NOEXECUTE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_GET_ADDRESS
#   error "Macro 'VMM_TABLE_ENTRY_GET_ADDRESS' already defined somewhere else!"
#endif

#ifdef VMM_TABLE_ENTRY_SET_PRESENT_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_PRESENT_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_READWRITE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_READWRITE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_USERACCESS_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_USERACCESS_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_WRITETHROUGH_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_WRITETHROUGH_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_DONTCACHE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_DONTCACHE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_DIRTYBIT_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_DIRTYBIT_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_LARGEPAGE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_LARGEPAGE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_NOEXECUTE_FLAG
#   error "Macro 'VMM_TABLE_ENTRY_SET_NOEXECUTE_FLAG' already defined somewhere else!"
#endif
#ifdef VMM_TABLE_ENTRY_SET_ADDRESS
#   error "Macro 'VMM_TABLE_ENTRY_SET_ADDRESS' already defined somewhere else!"
#endif


#define VMM_TABLE_ENTRY_PRESENT_SHIFT      0 
#define VMM_TABLE_ENTRY_PRESENT_BIT        (1 << VMM_TABLE_ENTRY_PRESENT_SHIFT)
#define VMM_TABLE_ENTRY_READWRITE_SHIFT    1 
#define VMM_TABLE_ENTRY_READWRITE_BIT      (1 << VMM_TABLE_ENTRY_READWRITE_SHIFT)
#define VMM_TABLE_ENTRY_USERACCESS_SHIFT   2 
#define VMM_TABLE_ENTRY_USERACCESS_BIT     (1 << VMM_TABLE_ENTRY_USERACCESS_SHIFT)
#define VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT 3
#define VMM_TABLE_ENTRY_WRITETHROUGH_BIT   (1 << VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT)
#define VMM_TABLE_ENTRY_DONTCACHE_SHIFT    4 
#define VMM_TABLE_ENTRY_DONTCACHE_BIT      (1 << VMM_TABLE_ENTRY_DONTCACHE_SHIFT)
#define VMM_TABLE_ENTRY_DIRTYBIT_SHIFT     5 
#define VMM_TABLE_ENTRY_DIRTYBIT_BIT       (1 << VMM_TABLE_ENTRY_DIRTYBIT_SHIFT)
#define VMM_TABLE_ENTRY_LARGEPAGE_SHIFT    7 
#define VMM_TABLE_ENTRY_LARGEPAGE_BIT      (1 << VMM_TABLE_ENTRY_LARGEPAGE_SHIFT)
#define VMM_TABLE_ENTRY_ADDRESS_SHIFT      12
#define VMM_TABLE_ENTRY_ADDRESS_BIT        (1 << VMM_TABLE_ENTRY_ADDRESS_SHIFT)
#define VMM_TABLE_ENTRY_NOEXECUTE_SHIFT    63
#define VMM_TABLE_ENTRY_NOEXECUTE_BIT      (1 << VMM_TABLE_ENTRY_NOEXECUTE_SHIFT)

#define VMM_TABLE_ENTRY_GET_PRESENT_FLAG(entry)     (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_PRESENT_SHIFT     ) )
#define VMM_TABLE_ENTRY_GET_READWRITE_FLAG(entry)   (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_READWRITE_SHIFT   ) )
#define VMM_TABLE_ENTRY_GET_USERACCESS_FLAG(entry)  (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_USERACCESS_SHIFT  ) )
#define VMM_TABLE_ENTRY_GET_WRITETHROUGH_FLAG(entry)(0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT) )
#define VMM_TABLE_ENTRY_GET_DONTCACHE_FLAG(entry)   (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_DONTCACHE_SHIFT   ) )
#define VMM_TABLE_ENTRY_GET_DIRTYBIT_FLAG(entry)    (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_DIRTYBIT_SHIFT    ) )
#define VMM_TABLE_ENTRY_GET_LARGEPAGE_FLAG(entry)   (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_LARGEPAGE_SHIFT   ) )
#define VMM_TABLE_ENTRY_GET_NOEXECUTE_FLAG(entry)   (0x0000000000000001 & ( (entry) >> VMM_TABLE_ENTRY_NOEXECUTE_SHIFT   ) )
#define VMM_TABLE_ENTRY_GET_ADDRESS(entry)          (0xffffffffff       & ( (entry) >> VMM_TABLE_ENTRY_ADDRESS_SHIFT     ) )

#define VMM_TABLE_ENTRY_SET_PRESENT_FLAG(entry, flag)      (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_PRESENT_SHIFT     )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_PRESENT_SHSHIFT   )  )
#define VMM_TABLE_ENTRY_SET_READWRITE_FLAG(entry, flag)    (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_PRESENT_SHIFT     )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_PRESENT_SHIFT     )  )
#define VMM_TABLE_ENTRY_SET_USERACCESS_FLAG(entry, flag)   (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_USERACCESS_SHIFT  )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_USERACCESS_SHIFT  )  )
#define VMM_TABLE_ENTRY_SET_WRITETHROUGH_FLAG(entry, flag) (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT)) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_WRITETHROUGH_SHIFT)  )
#define VMM_TABLE_ENTRY_SET_DONTCACHE_FLAG(entry, flag)    (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_DONTCACHE_SHIFT   )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_DONTCACHE_SHIFT   )  )
#define VMM_TABLE_ENTRY_SET_DIRTYBIT_FLAG(entry, flag)     (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_DIRTYBIT_SHIFT    )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_DIRTYBIT_SHIFT    )  )
#define VMM_TABLE_ENTRY_SET_LARGEPAGE_FLAG(entry, flag)    (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_LARGEPAGE_SHIFT   )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_LARGEPAGE_SHIFT   )  )
#define VMM_TABLE_ENTRY_SET_NOEXECUTE_FLAG(entry, flag)    (  ( (entry) & (~(0x0000000000000001 << VMM_TABLE_ENTRY_NOEXECUTE_SHIFT   )) ) | ( ((flag) & 0x0000000000000001) << VMM_TABLE_ENTRY_NOEXECUTE_SHIFT   )  )
#define VMM_TABLE_ENTRY_SET_ADDRESS(entry, addr)           (  ( (entry) & (~(0xffffffffff       << VMM_TABLE_ENTRY_ADDRESS_SHIFT     )) ) | ( ((addr) & 0xffffffffff      ) << VMM_TABLE_ENTRY_ADDRESS_SHIFT     )  )


// typedef struct __page_map_level4_entry
// {
//     uint8_t  present     : 1;
//     uint8_t  rw          : 1;
//     uint8_t  userOrSuper : 1;
//     uint8_t  wthrough    : 1;
//     uint8_t  cachedis    : 1;
//     uint8_t  accessed    : 1;
//     uint8_t  ignore0     : 1;
//     uint8_t  reserved    : 1; // always 0
//     uint8_t  ignore1     : 4;
//     uint64_t address     : 40; // unused bits must be 0
//     uint16_t ignore2     : 11;
//     uint8_t  noExec      : 1;
// } PML4E;


// // generic table/entry types that can be converted to any other table\entry.
// typedef struct pack __GenericTableEntry
// {
//     union 
//     {
//         struct pack {
//             uint8_t  present     : 1;
//             uint8_t  rw          : 1;
//             uint8_t  userOrSuper : 1;
//             uint8_t  wthrough    : 1;
//             uint8_t  cachedis    : 1;
//             uint8_t  accessed    : 1;
//             uint8_t  ignore0     : 1;
//             uint8_t  pageSize    : 1; // 0 = 4KB, 1 = 4MiB
//             uint8_t  ignore1     : 4;
//             uint64_t address     : 40;
//             uint16_t ignore2     : 11;
//             uint8_t  noExec      : 1;
//         };
//         uint64_t ui64;
//     };
// } TableEntry;


typedef uint64_t __page_map_level4_entry_type;
typedef __page_map_level4_entry_type PML4E;

typedef uint64_t __generic_page_table_entry_type;
typedef __generic_page_table_entry_type genericPageTableEntry;


typedef struct alignsz(PAGE_SIZE) __PageMapLevel4_type {
    PML4E ptrtables[512];
} PML4;

typedef struct alignsz(PAGE_SIZE) __GenericPageTable_type {
    genericPageTableEntry entry[512];
} genericPageTable;




KERNEL_STATIC_ASSERT(sizeof(PML4) == PAGE_SIZE, 
    __genericPageTableSizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(PML4E) == sizeof(uint64_t), 
    __GenericTableEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(genericPageTable) == PAGE_SIZE, 
    __genericPageTableSizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);
KERNEL_STATIC_ASSERT(sizeof(genericPageTableEntry) == sizeof(uint64_t), 
    __GenericTableEntrySizeEqualityCheck, 
    "Kernel Static Assert On Unexpected Type-Size Mismatch"
);

#endif /* __KERNEL_C_DEFINITION_PAGING_TYPES__ */
