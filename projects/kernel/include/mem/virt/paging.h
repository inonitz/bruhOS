#ifndef ___KERNEL_C_DEFINITION_PAGING_HEADER__
#define ___KERNEL_C_DEFINITION_PAGING_HEADER__
#include <mem/virt/types.h>
#include <std/argprefix.h>


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


#endif /* ___KERNEL_C_DEFINITION_PAGING_HEADER__ */