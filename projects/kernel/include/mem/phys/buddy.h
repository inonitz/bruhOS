#ifndef __BUDDY_ALLOC__
#define __BUDDY_ALLOC__
#include <std/int.h>
#include <std/enable_bool.h>
#include <std/bool.h>
#include <std/macro.h>


#define BUDDY_TREE_DEPTH   16ull
#define ALLOC_MANAGED_MEM (1ull << (BUDDY_TREE_DEPTH + 12ul))


#ifndef MEMSIZE_SPECIFIC_STRUCTURES
typedef enum __bstate
{
	USED, FREE
} BlockState;


typedef struct pack __node_t
{
	uint32_t   prev  : 31;
	uint32_t   next  : 31;
	BlockState state : 2;
} node_t;


typedef node_t list_head;
#endif
#define MEMSIZE_SPECIFIC_STRUCTURES


#define TREE_SIZE (-1ll + (1ll << (BUDDY_TREE_DEPTH + 1ull)))
typedef struct alignsz(PAGE_SIZE) __BuddyAllocator
{
	char*     	  start;
	node_t    	  linkedmap[TREE_SIZE];
	struct    
	{
		list_head head[BUDDY_TREE_DEPTH + 1]; // +1 for node at top.
		node_t*   free[BUDDY_TREE_DEPTH + 1];
		uint32_t  size[BUDDY_TREE_DEPTH + 1];
	} levels;

} BuddyAllocator;
#undef TREE_SIZE


void     buddy_alloc_init(BuddyAllocator* mman, void*    start);
void*    buddy_alloc_del (BuddyAllocator* mman			      );
void*    alloc_pages	 (BuddyAllocator* mman, uint32_t count);
uint8_t  free_pages	     (BuddyAllocator* mman, void*      ptr, uint8_t lvl);
void*    alloc_page		 (BuddyAllocator* mman				  );
uint8_t  free_page		 (BuddyAllocator* mman, void*      ptr);
void     buddyInfo 	     (BuddyAllocator* mman			      );
uint64_t freeMemorySize  (BuddyAllocator* mman				  );
uint64_t maxBlockReq	 (BuddyAllocator* mman                );
bool_t   isBuddyFull	 (BuddyAllocator* mman			      );


#endif
