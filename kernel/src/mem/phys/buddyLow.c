#include "buddyLow.h"
#include "../../std/io.h"


#define TREE_SIZE (-1ll + (1ll << (BUDDYLOW_TREE_DEPTH + 1ull)))
#define LEVEL_START(lvl) (-1 + (1ull << (lvl)))
#define LEVEL_END(lvl)   (2 * LEVEL_START(lvl))
#define ELEM_COUNT(lvl)  (1ull << lvl)


#define BLOCK_SIZE(lvl)   (1ull << (BUDDYLOW_TREE_DEPTH + 12ul - lvl))
#define PTR(START, OFFSET) ((node_t*)(START + OFFSET))
#define OFFSET(START, POINTER) ((uint32_t)(POINTER - START))
#define LEVELS(mman)     mman->levels
#define LINKEDMAP(mman)  mman->linkedmap
#define NODE_AT(mman, i) mman->linkedmap[i]
#define HEAD(mman)       mman->levels.head
#define FREE(mman)		 mman->levels.free
#define SIZE(mman)		 mman->levels.size


#define MPTR(mman, OFF) ((node_t*)(mman->linkedmap + OFF))
#define MOFFSET(mman, pointer) ((uint32_t)(pointer - &mman->linkedmap[0]))


static inline uint8_t  get_level (node_t*l, node_t* linkedmap)			    { return (uint8_t)log2ui(1 + (uint32_t)(l - linkedmap)); }
static inline uint32_t get_offset(node_t*l, node_t* linkedmap)			    { return (uint32_t)(l - linkedmap);					  	 }
static inline uint32_t get_index (node_t*l, node_t* linkedmap, uint8_t lvl) { return (uint32_t)(l - linkedmap) + 1u - (1u << lvl);   }
static inline uint32_t get_buddy (uint32_t offset			  )             { return offset + 2 * (offset & 1) - (offset != 0);      }



static __force_inline void* getAddress(BuddyAllocatorLowMem* mman, node_t* bud) {
	uint8_t lvl = get_level(bud, mman->linkedmap);
	return mman->start + get_index(bud, mman->linkedmap, lvl) * BLOCK_SIZE(lvl);
}


static __force_inline node_t* getBuddy(node_t* linkedmap, uint32_t offset) {	
	return &linkedmap[get_buddy(offset)];
}


static void remove_entry(BuddyAllocatorLowMem* mman, node_t* entry) {
	entry->state = USED;
    MPTR(mman, entry->prev)->next = entry->next;
    MPTR(mman, entry->next)->prev = entry->prev;
	
    --SIZE(mman)[get_level(entry, mman->linkedmap)];
    return;
}


static void push_entry(BuddyAllocatorLowMem* mman, uint8_t lvl, node_t* entry) {
	node_t* last_free = FREE(mman)[lvl];

	entry->state = FREE;
	entry->next  = last_free->next;
	entry->prev  = MOFFSET(mman, last_free);

	last_free->next		           = MOFFSET(mman, entry);
	MPTR(mman, entry->next)->prev  = MOFFSET(mman, entry);

	FREE(mman)[lvl] = entry;
	++SIZE(mman)[lvl];
	return;
}


static node_t* pop_entry(BuddyAllocatorLowMem* mman, uint8_t lvl)
{
	if (SIZE(mman)[lvl] == 0) return &HEAD(mman)[lvl];
	node_t* entry = FREE(mman)[lvl];
	remove_entry(mman, entry);

	FREE(mman)[lvl] = MPTR(mman, entry->prev);
	return entry;
}


static void try_merge(BuddyAllocatorLowMem* mman, uint32_t offset, uint8_t lvl)
{
	node_t* buddy = getBuddy(mman->linkedmap, offset);
	bool_t  cond = (bool_t)buddy->state;
	while (cond && lvl > 0)
	{
		remove_entry(mman, buddy);
		pop_entry(mman, lvl);

		offset = (offset - 1) / 2;
		push_entry(mman, lvl - 1, &NODE_AT(mman, offset));
		
		--lvl;
		buddy = getBuddy(mman->linkedmap, offset);
		cond = buddy->state && NODE_AT(mman, offset).state;
	}
}


static void dump_freelist(BuddyAllocatorLowMem* mman, uint8_t lvl)
{
	node_t* begin = MPTR(mman, HEAD(mman)[lvl].next);
	uint32_t off  = 0;
	while (begin != &HEAD(mman)[lvl])
	{
		off = get_offset(begin, LINKEDMAP(mman));
        printk("[%u]->", off);
		begin = MPTR(mman, begin->next);
	}
	putln();
}


static void dump_levels(BuddyAllocatorLowMem* mman)
{
	for (uint8_t i = 0; i < BUDDYLOW_TREE_DEPTH + 1; ++i)
	{
		printk("lvl %u: ", i);
		dump_freelist(mman, i);
	}
	putln();
}


void lowmem_buddy_init(BuddyAllocatorLowMem* mman, void* start)
{
	mman->start = start;
	memset(mman->linkedmap, 0x00, ((1 << BUDDYLOW_TREE_DEPTH) -1) * sizeof(node_t));
	for (uint8_t i = 0; i < BUDDYLOW_TREE_DEPTH + 1; ++i)
	{
		mman->levels.head[i] = (list_head){ 
			MOFFSET(mman, &mman->levels.head[i]),
			MOFFSET(mman, &mman->levels.head[i]), 
			USED 
		};
		mman->levels.free[i] = &mman->levels.head[i];
		mman->levels.size[i] = 0;
	}
	push_entry(mman, 0, &mman->linkedmap[0]);
}




struct mem_zone {
	physical_address mem;
	uint64_t         pages;
}; 


void lowmem_buddy_init_free_regions(
    BuddyAllocatorLowMem* mman, 
    void*                 start, 
    struct mem_zone*      free_memory_regions, 
    uint16_t              count
) {
	uint32_t 		 lm_offset = 0;
	struct mem_zone* zone 	   = free_memory_regions;
	node_t*          tmp;


	lowmem_buddy_init(mman, start); // initialize the buddy allocator as usual
	pop_entry(mman, 0);			    // there isn't TOTAL_MEM available, so we get rid of this block.


	// set the available pages at level TREE_DEPTH to FREE.
	for(uint16_t i = 0; i < count; ++i)
	{
		lm_offset = ((zone[i].mem - (uint64_t)mman->start) >> LOG2_PAGE_SIZE) + LEVEL_START(BUDDYLOW_TREE_DEPTH);
		for(uint64_t j = 0; j < zone[i].pages; ++j)
		{
			NODE_AT(mman, lm_offset + j).state = FREE;
		}
	}


	// go through every level and mark parent blocks as FREE if both their children are FREE
	for(int8_t i = BUDDYLOW_TREE_DEPTH - 1; i >= 0; --i)
	{
		for(int32_t offset = (int32_t)LEVEL_END(i); offset >= (int32_t)LEVEL_START(i); --offset)
		{
			tmp 	   = &NODE_AT(mman, offset);
			tmp->state = NODE_AT(mman, offset * 2 + 1).state && NODE_AT(mman, offset * 2 + 2).state; 
			NODE_AT(mman, offset * 2 + 1).state *= !tmp->state;  // if the parent is free, mark both children as USED
			NODE_AT(mman, offset * 2 + 2).state *= !tmp->state;  // if the parent is free, mark both children as USED
		}
	}


	count = 0;
	for(int64_t i = 0; i < TREE_SIZE; ++i) {
		count += NODE_AT(mman, i).state;
	}


	uint32_t indices[count + 1];
	uint32_t lg2idx [count + 1];
	uint32_t idx = 0;
	for(uint32_t i = 0; i < TREE_SIZE; ++i)
	{
		indices[idx] = i;
		lg2idx[idx]  = log2ui(i + 1u);
		idx 		+= NODE_AT(mman, i).state;
	}
	for(uint32_t i = 0; i < count; ++i)
	{
		// printk("idx: %u lg2: %u\n", indices[i], lg2idx[i]);
		push_entry(mman, lg2idx[i], &NODE_AT(mman, indices[i]));
	}
	return;
}


void* lowmem_alloc_pages(BuddyAllocatorLowMem* mman, uint32_t count)
{
	count = (uint32_t)round2(count);
	if (count > (1 << BUDDYLOW_TREE_DEPTH) || count == 0) return nullptr;

	int8_t   lvl	= BUDDYLOW_TREE_DEPTH - (int8_t)log2ui(count);
	node_t*  block  = LEVELS(mman).free[lvl];
	int32_t  splits = 0;
	uint32_t index  = 0;


	// try to find an available block in the previous levels.
	// keeps track of the amount of times we need to split it
	for (; lvl > 0 && (block == &HEAD(mman)[lvl]); --lvl) {
		block = pop_entry(mman, lvl - 1);
		++splits;
	}
	if (block == &HEAD(mman)[lvl]) return nullptr;


	// split the block we just got
	index = get_offset(block, mman->linkedmap);
	for (int32_t i = lvl; i < lvl + splits; ++i)
	{
		push_entry(mman, i + 1, &NODE_AT(mman, index * 2 + 2));
		push_entry(mman, i + 1, &NODE_AT(mman, index * 2 + 1));
		
		block = pop_entry(mman, i + 1);
		index = get_offset(block, mman->linkedmap);
	}
	if (splits == 0)
		block = pop_entry(mman, lvl);


	return getAddress(mman, block);
}


uint8_t lowmem_free_pages(BuddyAllocatorLowMem* mman, void* ptr, uint8_t lvl)
{
	if (ptr == nullptr 			     || 
		(char*)ptr < mman->start     || 
		((uint64_t)ptr & 0xfff) != 0 ||
		lvl > BUDDYLOW_TREE_DEPTH
	) {
		return INVALID_INPUT;
	}

	uint32_t offset = LEVEL_START(lvl) + (((char*)ptr - mman->start) >> (LOG2_PAGE_SIZE + BUDDYLOW_TREE_DEPTH - lvl));
	push_entry(mman, lvl, &NODE_AT(mman, offset));
	try_merge(mman, offset, lvl);

	return SUCCESS;
}


void*   lowmem_alloc_page(BuddyAllocatorLowMem* mman)            { return lowmem_alloc_pages(mman, 1);              }
uint8_t lowmem_free_page (BuddyAllocatorLowMem* mman, void* ptr) { return lowmem_free_pages(mman, ptr, BUDDYLOW_TREE_DEPTH); }


uint64_t buddyLow_freeMemory(BuddyAllocatorLowMem* mman)
{
	uint64_t S = 0;
	for(uint8_t i = 0; i < BUDDYLOW_TREE_DEPTH + 1; ++i)
	{
		S += LEVELS(mman).size[i] * BLOCK_SIZE(i);
	}
	return S;
}


uint64_t buddyLow_maxBlockReq(BuddyAllocatorLowMem* mman)
{
	uint8_t i = 0;
	for(; (SIZE(mman)[i] == 0) && i < BUDDYLOW_TREE_DEPTH + 1; ++i);
	return BLOCK_SIZE(i) * (i != BUDDYLOW_TREE_DEPTH + 1); 
}


bool_t buddyLow_isFull(BuddyAllocatorLowMem* mman)
{
	bool_t budFull = TRUE;
	for(uint8_t i = 0; (i < BUDDYLOW_TREE_DEPTH) && budFull; ++i) 
    {	
		budFull = budFull && (mman->levels.size[i] == 0);
	}
	return budFull;
}


void buddyLow_info(BuddyAllocatorLowMem* mman)
{
	printk("Low-Mem BuddyAllocator:\n");
	printk("Free-lists from lvl %u to %u:\n", 0, (uint32_t)BUDDYLOW_TREE_DEPTH);
	dump_levels(mman);
}