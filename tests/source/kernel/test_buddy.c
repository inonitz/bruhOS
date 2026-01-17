#include "buddy.h"
#include <time.h>
#include <windows.h>
// this file should be run in visual studio with the msvc compiler


void shuffle(uint32_t* array, size_t n)
{
	if (n <= 1) ++n;
	size_t i;
	for (i = 0; i < n - 1; i++)
	{
		size_t j   = i + rand() / (RAND_MAX / (n - i) + 1);
		uint32_t t = array[j];
		array[j]   = array[i];
		array[i]   = t;
	}
}

#define allocs 2048u
typedef struct __4kbPage
{
	char b[4096];
} block;

int main()
{
	srand(time(NULL));

	BuddyAllocator* alloc  = malloc(sizeof(BuddyAllocator));
	void*			m	   = malloc(256 * 1024 * 1024);
	unsigned	    seed   = rand();
	block*		    mem	   [allocs];
	uint32_t		pages  [allocs];
	uint32_t	    indices[allocs];

	if (alloc == NULL)
		exit(99999999999999);

	buddy_alloc_init(alloc, m);
	//buddyInfo(alloc);
	for (uint8_t i = 0; i < TREE_DEPTH; ++i)
	{
		printf("%u: ->start: %llu, end: %llu\n", i, LEVEL_START(i), LEVEL_END(i));
	}


	for (uint32_t i = 0; i < allocs; ++i) {
		indices[i] = i;
	}

	for (uint32_t i = 0; i < allocs; ++i) {
		pages[i] = (uint32_t)(1024 * (float)rand() / RAND_MAX);
		//pages[i] >>= 12;
		mem[i]   = alloc_pages(alloc, pages[i]);
		//buddyInfo(alloc);
	}
	for (uint32_t i = 0; i < allocs; ++i) {
		printf("%u, ", pages[i]);
	}
	printf("\n");
	uint32_t i = 1, tmp = 0;
	for (; !tmp && i < allocs - 1; ++i)
	{
		tmp = !!(mem[i] != NULL && mem[i - 1] != NULL && mem[i] == mem[i - 1]);
		printf("%u: %p (%u) == %p (%u) ? %d\n", i, mem[i], pages[i], mem[i - 1], pages[i - 1], tmp);
	}
	--i;
	if (tmp == 1) {
		printf("found duplicate! %u, %p %p, %u %u", i, mem[i], mem[i - 1], pages[i], pages[i - 1]);
		exit(99999999999999);
	}

	shuffle(indices, allocs);

	for (uint32_t i = 0; i < allocs; ++i) {
		//buddyInfo(alloc);
		printf("FREE PAGE INDEX IS %u\nreturning block from index %u\n", i, indices[i]);
		free_pages(alloc, mem[indices[i]]);
		buddyInfo(alloc);
	}


}