#pragma once
#ifndef __VIRTUAL_ADDRESS_SPACE_MANAGER__
#define __VIRTUAL_ADDRESS_SPACE_MANAGER__
#include "vas_node_alloc.h"
#include "paging.h"



typedef struct __virtual_address_space_manager
{
    PML4*           table;      // the PML4 Table that the VAS Currently Manages.
    uint32_t        tablePages; // total amount of allocated pages for this PML4 table.
    vas_nodelist_t* alloc;      // allocates nodes for the tree.
    vas_node_t*     root;       // a pointer to the root of the tree.
} vas_man_t;




/* 
    * Initialize the structure that manages the 
    * virtual address space for a process.
*/
void  vas_init(IN vas_man_t* man, OPTIONAL PML4* paging_table);


/* 
    * Allocate a virtual memory region in the virtual address space.
    * if the memory region isn't overlapping with any other regions.
    * vas_man_t*     vas     - the virtual address space allocator to modify.
    * void*          address - the address to allocate in.
    * uint32_t       pages   - the amount of pages to allocate for the region.
    * paging_flags_t flags   - memory region paging-related flags, declared at virt/paging.h
*/
void* vas_alloc(vas_man_t* vas, void* address, uint32_t pages, paging_flags_t flags);


/* 
    * Free a virtual memory region in the current tree. 
    * vas_man_t* vas - the virtual address space allocator to modify.
    * void*      ptr - the virtual address of the region.
*/
void vas_free(vas_man_t* vas, void* ptr);

#endif