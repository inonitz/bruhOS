#include <mem/phys/buddy.h>
#include <std/error_macro.h>
#include <std/error.h>
#include <std/string.h>
#include <std/math.h>


#define TREE_SIZE	      (-1ll + (1ll << (BUDDY_TREE_DEPTH + 1ull)))
#define BLOCK_SIZE(lvl)   (1ull << (BUDDY_TREE_DEPTH + 12ul - lvl))

#define LEVEL_START(lvl) (-1 + (1ull << (lvl)))
#define LEVEL_END(lvl)   (2 * LEVEL_START(lvl))
#define ELEM_COUNT(lvl)  (1ull << lvl)


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


static void dump_freelist(BuddyAllocator* mman, uint8_t lvl)
{
	node_t* begin = MPTR(mman, HEAD(mman)[lvl].next);
	uint32_t off  = 0;
	while (begin != &HEAD(mman)[lvl])
	{
		off = get_offset(begin, LINKEDMAP(mman));
        printk("[%u]->", off);
		begin = MPTR(mman, begin->next);
	}
	printk("\n");
}


static void dump_levels(BuddyAllocator* mman)
{
	for (uint8_t i = 0; i < BUDDY_TREE_DEPTH + 1; ++i)
	{
		printk("lvl %u: ", i);
		dump_freelist(mman, i);
	}
	printk("\n\n");
}




static inline void* getAddress(BuddyAllocator* mman, node_t* bud)
{
	// start + index * blockSize.
	uint8_t lvl = get_level(bud, mman->linkedmap);
	return mman->start + get_index(bud, mman->linkedmap, lvl) * BLOCK_SIZE(lvl);
}



static inline node_t* getBuddy(node_t* linkedmap, uint32_t offset)
{	
	// always gives the buddy of a node with a given offset in the buffer.
	// at offset = 0, the given node will also be 0.
	return &linkedmap[get_buddy(offset)];
}



void remove_entry(BuddyAllocator* mman, node_t* entry)
{
	entry->state = USED;
    MPTR(mman, entry->prev)->next = entry->next;
    MPTR(mman, entry->next)->prev = entry->prev;
	
	--SIZE(mman)[get_level(entry, mman->linkedmap)];
}



void push_entry(BuddyAllocator* mman, uint8_t lvl, node_t* entry)
{
	node_t* last_free = FREE(mman)[lvl];

	// *entry = (node_t){ .prev = MOFFSET(mman, last_free), .next = last_free->next, .state = FREE };
	entry->state = FREE;
	entry->next  = last_free->next;
	entry->prev  = MOFFSET(mman, last_free);

	last_free->next		           = MOFFSET(mman, entry);
	MPTR(mman, entry->next)->prev  = MOFFSET(mman, entry);

	FREE(mman)[lvl] = entry;
	++SIZE(mman)[lvl];
	return;
}



node_t* pop_entry(BuddyAllocator* mman, uint8_t lvl)
{
	if (SIZE(mman)[lvl] == 0) return &HEAD(mman)[lvl];
	node_t* entry = FREE(mman)[lvl];
	remove_entry(mman, entry);

	FREE(mman)[lvl] = MPTR(mman, entry->prev);
	return entry;
}



void try_merge(BuddyAllocator* mman, uint32_t offset, uint8_t lvl)
{
	node_t* buddy = getBuddy(mman->linkedmap, offset);
	bool_t  cond = (bool_t)buddy->state;
	while (cond && lvl > 0)
	{
		DEBUG(dump_freelist(mman, lvl);)
		remove_entry(mman, buddy); // remove the entry we know is in the list somewhere.
								   // we can remove it like this because it is not LEVELS(mman).free[lvl].
		pop_entry(mman, lvl);	   // remove the entry we just added.
		DEBUG(dump_freelist(mman, lvl);)
		
		offset = (offset - 1) / 2;
		push_entry(mman, lvl - 1, &NODE_AT(mman, offset));
		DEBUG(dump_freelist(mman, lvl - 1);)
		
		--lvl;
		buddy = getBuddy(mman->linkedmap, offset);
		cond = buddy->state && NODE_AT(mman, offset).state;
	}
}



/* 
	* Initializes the buddy allocator with a memory region that is atleast TOTAL_MEM size (in bytes)
*/
void buddy_alloc_init(BuddyAllocator* mman, void* start)
{
	mman->start = (char*)start;
	memset(mman->linkedmap, 0x00, ((1 << 17) - 1) * sizeof(node_t));
	for (uint8_t i = 0; i < BUDDY_TREE_DEPTH + 1; ++i)
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



void* buddy_alloc_del(BuddyAllocator* mman)
{
	// theres nothing to do here since we are managing the system memory.
	// if this were to be a user-space allocator, we'd atleast return the pointer to the region allocated for it to free the memory
	return mman->start;
}



// more comprehensive description of mem_zone is in mem/pmm.h
struct mem_zone
{
	physical_address mem;
	uint64_t         pages;
}; 


void buddy_alloc_init_free_regions(BuddyAllocator* mman, void* start, struct mem_zone* free_memory_regions, uint16_t count)
{
	uint32_t 		 lm_offset = 0;
	struct mem_zone* zone 	   = free_memory_regions;
	node_t*          tmp;

	
	buddy_alloc_init(mman, start); // initialize the buddy allocator as usual
	pop_entry(mman, 0);			   // there isn't TOTAL_MEM available, so we get rid of this block.


	// set the available pages at level 16 to FREE.
	for(uint16_t i = 0; i < count; ++i)
	{
		lm_offset = ((zone[i].mem - (uint64_t)mman->start) >> LOG2_PAGE_SIZE) + LEVEL_START(16);
		for(uint64_t j = 0; j < zone[i].pages; ++j)
		{
			NODE_AT(mman, lm_offset + j).state = FREE;
		}
	}


	// go through every level and mark parent blocks as FREE if both their children are FREE
	for(int8_t i = BUDDY_TREE_DEPTH - 1; i >= 0; --i)
	{
		for(int32_t offset = (int32_t)LEVEL_END(i); offset >= (int32_t)LEVEL_START(i); --offset)
		{
			tmp 	   = &NODE_AT(mman, offset);
			tmp->state = (BlockState)(
				boolean( NODE_AT(mman, offset * 2 + 1).state ) 
				&& 
				boolean( NODE_AT(mman, offset * 2 + 2).state )
			); 
			NODE_AT(mman, offset * 2 + 1).state *= !tmp->state;  // if the parent is free, mark both children as USED
			NODE_AT(mman, offset * 2 + 2).state *= !tmp->state;  // if the parent is free, mark both children as USED
		}
	}


	// push every region that is free into the free-list (from left to right of the tree)
	// how: get the amount of free blocks, get their index (in linkedmap) and their level in the tree, and push them into the freelist
	// NO IF STATEMENTS ALLOWED!
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



void* alloc_pages(BuddyAllocator* mman, uint32_t count)
{
	count = (uint32_t)round2(count);
	if (count > (1ull << BUDDY_TREE_DEPTH) || count == 0) return NULLPTR;

	int8_t   lvl	= BUDDY_TREE_DEPTH - (int8_t)log2ui(count);
	node_t*  block  = LEVELS(mman).free[lvl];
	int32_t  splits = 0;
	uint32_t index  = 0;


	// try to find an available block in the previous levels.
	// keeps track of the amount of times we need to split it
	for (; lvl > 0 && (block == &HEAD(mman)[lvl]); --lvl) {
		block = pop_entry(mman, lvl - 1);
		++splits;
	}
	if (block == &HEAD(mman)[lvl]) return NULLPTR;


	// split the block we just got
	index = get_offset(block, mman->linkedmap);
	for (int32_t i = lvl; i < lvl + splits; ++i)
	{
		push_entry(mman, i + 1, &NODE_AT(mman, index * 2 + 2));
		push_entry(mman, i + 1, &NODE_AT(mman, index * 2 + 1));
		
		DEBUG(dump_freelist(mman, i + 1);)
		block = pop_entry(mman, i + 1);
		DEBUG(dump_freelist(mman, i + 1);)
		index = get_offset(block, mman->linkedmap);
	}
	if (splits == 0)
		block = pop_entry(mman, lvl);

	// these lines are outdated: Removed headers due to alignment issues.
	// header* out = (header*)getAddress(mman, block);
	// out->offset = get_offset(block, mman->linkedmap);

	// return (out + 1);
	return getAddress(mman, block);
}



uint8_t free_pages(BuddyAllocator* mman, void* ptr, uint8_t lvl)
{
	if (ptr == NULLPTR 			     || 
		(char*)ptr < mman->start     || 
		((uint64_t)ptr & 0xfff) != 0 ||
		lvl > BUDDY_TREE_DEPTH
	) {
		return KERNEL_INVALID_INPUT;
	}

	uint32_t offset = LEVEL_START(lvl) + (((char*)ptr - mman->start) >> (LOG2_PAGE_SIZE + BUDDY_TREE_DEPTH - lvl));
	push_entry(mman, lvl, &NODE_AT(mman, offset));
	try_merge(mman, offset, lvl);

	return KERNEL_SUCCESS;
}


void* alloc_page(BuddyAllocator* mman) 
{
	return alloc_pages(mman, 1);
}



uint8_t free_page(BuddyAllocator* mman, void* ptr) 
{
	return free_pages(mman, ptr, BUDDY_TREE_DEPTH);
}



void buddyInfo(BuddyAllocator* mman)
{
	printk("Buddy Allocator Info:\n");
	printk("Free-lists from lvl %u to %u:\n", 0, (uint32_t)BUDDY_TREE_DEPTH);
	dump_levels(mman);
}




uint64_t freeMemorySize(BuddyAllocator* mman)
{
	uint64_t S = 0;
	for(uint8_t i = 0; i < BUDDY_TREE_DEPTH + 1; ++i)
	{
		S += LEVELS(mman).size[i] * BLOCK_SIZE(i);
	}
	return S;
}



uint64_t maxBlockReq(BuddyAllocator* mman)
{
	uint8_t i = 0;
	for(; (SIZE(mman)[i] == 0) && i < BUDDY_TREE_DEPTH + 1; ++i);
	return BLOCK_SIZE(i) * (i != BUDDY_TREE_DEPTH + 1); 
	// if TREE_DEPTH + 1 == i: 
	//		we reached the end and there are no available blocks ->
	// 		return 0;
	// else: 
	// 		return blocksize(i).
}



bool_t isBuddyFull(BuddyAllocator* mman)
{
	// the requirement for a buddy to be 'full' is that 
	// there are no available blocks to allocate from ALL levels in the tree
	bool_t budFull = BOOLEAN_TRUE;
	for(uint8_t i = 0; (i < BUDDY_TREE_DEPTH) && budFull; ++i)
	{	
		budFull = budFull && (mman->levels.size[i] == 0);
	}
	return budFull;
}


void* reserveBlock(BuddyAllocator* mman, uint64_t blockAddr, uint32_t count)
{
    count = (uint32_t)round2(count);
	if (count > (1ull << (BUDDY_TREE_DEPTH)) || count == 0) return NULLPTR;

	node_t*  block  = NULLPTR_TYPE(node_t);
	uint32_t index  = 0;
	uint8_t  reqlvl	= BUDDY_TREE_DEPTH - (uint8_t)log2ui(count);
    int8_t   lvl	= reqlvl;
    uint32_t indices[reqlvl + 1];

	// translating the address of the block into an offset in the linkedmap buffer. See getAddress(), get_index(), get_offset() for more info.
    blockAddr 	 = ((blockAddr - (uint64_t)mman->start) / BLOCK_SIZE(reqlvl)) + LEVEL_START(reqlvl); 
	block     	 = &NODE_AT(mman, blockAddr);
	indices[lvl] = blockAddr;
	while(!block->state && (lvl > 0))
	{
		--lvl;
		blockAddr    = (blockAddr - 1) >> 1;
		indices[lvl] = (uint32_t)blockAddr;
		block  		 = &NODE_AT(mman, blockAddr);
	}
	if(!block->state) 
	{
		printk("couldn't reserve block - didn't find a parent block available to split.");
		return NULLPTR;
	};

	// split the furthest ancestor block we found,
	// until we get to the request block, of which we return the address, incase it may not be aligned on 0x1000 boundary.
	for (int32_t i = lvl; i < reqlvl; ++i)
	{
		index = indices[lvl];
		block = &NODE_AT(mman, index);

		// this is the only block available in the level, and we need to pop it off the list
		if(block == FREE(mman)[lvl]) {
			pop_entry(mman, lvl);
		}
		// there are more blocks in the level, we can safely remove one block inside the list.
		else if(LEVELS(mman).size[lvl] > 0) {
			remove_entry(mman, block);
		}

		index = indices[i + 1];
		push_entry(mman, i + 1, &NODE_AT(mman, get_buddy(index)));
		push_entry(mman, i + 1, &NODE_AT(mman, index)	   	    );

		DEBUG(dump_freelist(mman, i + 1);)
		block = pop_entry(mman, i + 1);
		DEBUG(dump_freelist(mman, i + 1);)
	}

	return getAddress(mman, block);
}


// decision array: if size == 1, then we pop_entry. else if size > 0, we remove_entry. else we do nothing.
// inline void do_nothing() { return; };	
// void* func_decision[3] = {
// 	&do_nothing,
// 	&remove_entry,
// 	&pop_entry,
// };
// (func_decision[(LEVELS(mman).size[lvl] > 0) + (LEVELS(mman).size[lvl] == 1)])();
// simulating a jump table like this may perform worse than 2 simple if statements (branch prediction is just unpredictable),
// so I'm not using this technique now. it is a valid alternative to an if-statement-chain tho.



/* 
	//////////////////////////////// PREVIOUS REVISIONS OF THE BUDDY ALLOCATOR: ////////////////////////////////
	//////////////////////////////// The header that was used to keep track of  ////////////////////////////////
	//////////////////////////////// the block size was very difficult to work  ////////////////////////////////
	//////////////////////////////// with. So I removed it and instead opted    ////////////////////////////////
	//////////////////////////////// for the user to input the size/lvl of the  ////////////////////////////////
	//////////////////////////////// block, and now blocks can be aligned to a  ////////////////////////////////
	//////////////////////////////// proper boundary.                           ////////////////////////////////
*/



// uint8_t free_pages(BuddyAllocator* mman, void* ptr)
// {
// 	if (ptr == NULLPTR) return BAD_INPUT;

// 	header* inf = ((header*)ptr) - 1;
// 	uint8_t lvl = (uint8_t)log2ui(inf->offset + 1u);
// 	// DEBUG(dump_freelist(mman, lvl);)
// 	push_entry(mman, lvl, &NODE_AT(mman, inf->offset));
// 	// DEBUG(dump_freelist(mman, lvl);)

// 	try_merge(mman, inf->offset, lvl);
// 	return KERNEL_SUCCESS;
// }

// void* alloc_page(BuddyAllocator* mman)
// {
// 	header* out = (header*)alloc_pages(mman, 1) - 1;
// 	out->offset = 0;
// 	return (void*)out;
// }



// uint8_t free_page(BuddyAllocator* mman, void* ptr)
// {
// 	if (ptr == NULLPTR || ((uint64_t)ptr & sizeof(header)) != 0) return BAD_INPUT;

// 	uint32_t offset = LEVEL_START(16u) + (((char*)ptr - mman->start) / PAGE_SIZE);
// 	// DEBUG(dump_freelist(mman, 16u);)
// 	push_entry(mman, 16u, &NODE_AT(mman, offset));
// 	// DEBUG(dump_freelist(mman, 16u);)

// 	try_merge(mman, offset, 16u);
// 	return KERNEL_SUCCESS;
// }