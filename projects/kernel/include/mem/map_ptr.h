#ifndef __POINTER_MAPPER__
#define __POINTER_MAPPER__
#include <mem/virt/flags.h>
#include <stdint.h>


#define PHYS(VIRTUAL_POINTER)  ((uint64_t)(VIRTUAL_POINTER ) - virtualOffset())
#define VIRT(PHYSICAL_POINTER) ((uint64_t)(PHYSICAL_POINTER) + virtualOffset())
#define PHYS_TYPE(VIRTUAL_POINTER,  TYPE) ( (TYPE)( PHYS(VIRTUAL_POINTER) ) )
#define VIRT_TYPE(PHYSICAL_POINTER, TYPE) ( (TYPE)( VIRT(PHYSICAL_POINTER) ) )


/* 
    * Return the addresses in which the kernel is placed, as described by the linker script.
*/
uint8_t* getKernelStart(void);
uint8_t* getKernelEnd(void);


/* 
    * get the virtual address offset, which we retrieve from the memory map.
    * NOTE: *Most* of physical memory is mapped to the higher half by a constant offset, 
    *        which is dependant on the physical location of the kernel.
    * This value is set in __init_kernel_basic(), at entry.h
*/
uint64_t virtualOffset(void);


/* 
    * Brings a physical address to the virtual address space, if it wasn't already mapped.
    * void*    address - the address we'll map in the current PML4.
    * uint64_t pages   - the extent of the memory region we're mapping.
    * NOTE: Virtual pointer isn't needed since virtual_addr = phys_addr + offset.
    * NOTE: This can be called ONLY AFTER a paging table exists in the kernel's PML4 Manager, at src/virt/paging.c
*/
void toVirtual(void* address, uint64_t pages);


/* 
    * Does (almost) the same thing as toVirtual().
    * uint64_t flags - attributes that'll be set for every allocated page table/table entries.
    *                  look in paging.h for more info about available flags, etc...
*/
void toVirtualFlag(void* address, uint32_t pages, uint64_t flags);


/* 
    * Does the same thing as map_vtop(). this is a wrapper around map_vtop().
*/
void vtopFlag(void* virt, void* phys, uint32_t pages, uint64_t flags);


/* 
    * Removes a page entry from the Current Page Directory.
    * Same as toVirtual(), but does the reverse.
*/
void unVirtual(void* virtaddr, uint64_t pages);


/* 
    * wrapper function for getCurrentCR3() in paging.h
*/
uint64_t getCR3(void);


#endif
