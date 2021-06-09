#pragma once
#ifndef __BUDDY_ALLOC_LOW_MEM__
#define __BUDDY_ALLOC_LOW_MEM__
#include "../../shared/int.h"



#define BUDDYLOW_TREE_DEPTH	 10ull


#ifndef MEMSIZE_SPECIFIC_STRUCTURES
typedef enum __bstate
{
	USED, FREE
} BlockState;


typedef struct pack __node_t
{
	uint16_t   prev  : 15;
	uint16_t   next  : 15;
	BlockState state : 2;
} node_t;


typedef node_t list_head;
#endif
#define MEMSIZE_SPECIFIC_STRUCTURES


#define TREE_SIZE (-1ll + (1ll << (BUDDYLOW_TREE_DEPTH + 1ull)))
typedef struct align(PAGE_SIZE) __BuddyAllocatorUnder4MiB
{
	char_t*       start;
	node_t linkedmap[TREE_SIZE];
	struct    
	{
		list_head head[BUDDYLOW_TREE_DEPTH + 1]; // +1 for node at top.
		node_t*   free[BUDDYLOW_TREE_DEPTH + 1];
		uint32_t  size[BUDDYLOW_TREE_DEPTH + 1];
	} levels;
} BuddyAllocatorLowMem;
#undef TREE_SIZE


void     lowmem_buddy_init   (BuddyAllocatorLowMem* mman, void*    start		   );
void*    lowmem_alloc_pages  (BuddyAllocatorLowMem* mman, uint32_t count		   );
bool_t   lowmem_free_pages   (BuddyAllocatorLowMem* mman, void*    ptr, uint8_t lvl);
void*    lowmem_alloc_page   (BuddyAllocatorLowMem* mman				 		   );
bool_t   lowmem_free_page    (BuddyAllocatorLowMem* mman, void*    ptr			   );
uint64_t buddyLow_freeMemory (BuddyAllocatorLowMem* mman						   );
uint64_t buddyLow_maxBlockReq(BuddyAllocatorLowMem* mman                           );
bool_t   buddyLow_isFull	 (BuddyAllocatorLowMem* mman			               );
void     buddyLow_info		 (BuddyAllocatorLowMem* mman						   );

#endif