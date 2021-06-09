#pragma once
#ifndef __BUDDY_EXT__
#define __BUDDY_EXT__
#include "buddy.h"



/* 
    * Reserve a block of size 'count' that is in the region of the memory allocator.
    * This function is only exposed to the physical memory manager
    * This header is intended for use only in pmm.c and not anywhere else.
    * This function makes one assumption:
    * For a previously allocated block M, a Function Caller WILL NEVER re-reserve region\s 'N' of any size, inside block M.
*/
void* reserveBlock(
    OUT BuddyAllocator* mman, 
    IN  uint64_t        blockAddr, 
    IN  uint32_t        count
);


/* 
    * There are memory holes in the EFI memory map.
    * Some of these holes CANNOT be used for memory allocation.
    * So, the situation is that only some memory regions can actually be used, and the 
    * rest should be reserved.
    * Finally, this function initializes a buddy allocator with 'free_memory_regions',
    * and the rest of the memory is marked as 'USED'.
    * This function shouldn't be called by any other function than 'pfa_init()'
    * mman                 - the buddy allocator to be initialized.
    * free_memory_regions  - array of mem_zone elements, where each memory zone describes a contiguous part of memory which is usable
    * count                - the amount of elements in free_memory_regions.

*/
void buddy_alloc_init_free_regions(
    OUT BuddyAllocator* mman, 
    IN void*            start, 
    IN mem_zone*        free_memory_regions,  
    IN uint16_t         count
);


/* 
    * Same as buddy_alloc_init_free_regions, except that the allocator is the BuddyAllocatorLowMem for regions under 4MiB.
*/
void lowmem_buddy_init_free_regions(
    OUT BuddyAllocatorLowMem* mman, 
    IN void*                  start, 
    IN mem_zone*              free_memory_regions, 
    IN uint16_t               count
);

#endif