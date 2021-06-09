#pragma once
#ifndef __KERNEL_VMM__
#define __KERNEL_VMM__
#include "../../std/debug.h"
#include "../../ds/linked_page_buffer.h"
#include "vas.h"



/* 
Kernel & Process Virtual Address Space:		
----------------------------------------------------------------R----------------------------------------------------------------
|	Process	  |			Process			|     Process		    |												   |	  |  |	|
|  Executable |      Heap (Grow >>)     |  Stack (Grow <<)      |												   | KERN |KS|KR|
|			  |							|					    |												   |	  |  |  |
----------------------------------------------------------------R----------------------------------------------------------------
^			  ^							^						^												   ^ 		  	^
0			  Exec END                  Heap END  	 			128TiB (Stack TOP)          					   240TiB       256 TiB


0TiB   -> 128TiB - Dedicated to the Process being executed:
	0		 -> Exec END   : Process Executable [ADDR(.text + .data + .bss + ...)	   ]
	Exec END -> Heap END   : Process Heap       [Heap END is equivalent to brk	in UNIX]
	Heap END -> Stack TOP  : Process Stack      [Stack Grows Downwards from 128TiB	   ] 

128TiB -> 240TiB - Undefined:
	Stack TOP -> 240 TiB   : Unmapped & Undefined (Un-Accessible) Addresses.

240TiB -> 256TiB - Kernel Address Space:
	ADDR(KERN) -> ADDR(KS) : The Executable Section of the Kernel. Most of the kernel is loaded here.
	ADDR(KS)   -> ADDR(KR) : The Stack of the kernel. its size is 32KiB.
	ADDR(KR)   -> 256 TiB  : Reserved for the kernel. 

	* NOTE: 
        Physical Memory is mapped to the 'higher half' at 240TiB, including the kernel.
		The only detail that will stay fixed throughout development will be the kernel location and mapping.
		The rest might change to due inconvenience/learning new concepts and realizing I was wrong.


Description:
    * As far as I'm concerned, the memory has 4 tasks:
        1. Create and Destroy (Manage, essentially) Address Spaces.
        2. Manage the Address spaces from processes.
        3. Switch Page Tables (for Task switch) (CR3 Register).
        4. Figure out where page faults come from, so we can take care of them in the appropriate address space.
           (This is known by probing the VAS that is assigned to the current Address Space)
        
    * Moreover, the vmm_t structure is essentially an array of per_cpu objects,
    * where each cpu core manages its own list of address spaces, depending on the work that its doing.

*/


/* 
    * Initializes the virtual memory manager defined in vmm.c
*/
void       vmm_init();


/* 
    * Creates a new Virtual Address Space Manager.
    * uint16_t procid - the processor that requested the virtual memory manager.
    * returns pointer to the allocated Address Space Manager.
*/
vas_man_t* vmm_create_address_space (uint16_t procid);


/* 
    * Destroys a Virtual Address Space Manager by returning it to its allocator,
    * and freeing its data back to the Physical Memory Manager.
    * vas_man_t* manager - pointer to an existing Virtual Address Space Manager.
*/
void       vmm_destroy_address_space(vas_man_t* manager);


/* 
    * Switches The Active Virtual Address Space Manager and the CR3 Register
    * for the CPU requesting the switch.
    * vas_man_t* target - the new address space that the cpu core will change to.
*/
void       vmm_switch_address_space (vas_man_t* target );


typedef vas_man_t* vas_man_ptr_t;


DEFINE_LINKED_PAGES_OF_OBJECTS_INTERFACE(vas_man_t)
typedef linked_vas_man_t_pages_t man_node_alloc_t;


typedef struct __virtual_memory_manager_quote_unquote
{
    man_node_alloc_t* listAlloc;   // allocates data for the listhead.
    uint64_t          usedMem;     // total used memory by all virtual memory related work.
    uint64_t          totalMem;    // total currently allocated memory for all virtual memory workloads.
} vmm_t;

#endif