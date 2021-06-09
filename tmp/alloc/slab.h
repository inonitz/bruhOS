#pragma once
#ifndef __SLAB_ALLOCATOR__
#define __SLAB_ALLOCATOR__
#include "../../std/io.h"
#include "../../std/lock.h"



#define CACHE_LINE_SIZE 64ul


// slab_t == __slab_is_list_of_blocks_of_4kb_size
typedef struct slab_t
{

    void*          buf;        // start of the page.
    char_t*        first;      // first available block.
    uint32_t       obj_active; // total blocks active in this slab.
    struct slab_t* next;       // the next slab to walk to, if this one is full.
} slab_t;


// __slab_node_t == __linked_list_node_type
typedef struct __slab_node_t
{
    slab_t                data;
    struct __slab_node_t* next;
} __slab_node_t;


// cache_t == __cache_is_array_of_linked_list_slabs
typedef struct cache_t
{    
    char_t          name[16];

    struct cache_t* next;
    struct cache_t* prev;

    __slab_node_t   full;
    __slab_node_t   partial;
    __slab_node_t   free;

    spinlock_t      barrier;      // used for safe concurrent access.
                                  //
    uint16_t        objectSize;   // the size in bytes of the object.
    uint16_t        objectAlign;  // alignment of the object in memory.
    uint16_t        colors;       // the amount of colors 
    uint16_t        color_off;    // the offset to the next slab.
    uint16_t        pagesPerSlab; // the amount of pages required for 1 slab.
    uint32_t        obj_total;    // total amount of objects in the 'cache'
    uint32_t        obj_active;   // total of active objects in the 'cache'
    uint16_t        slab_total;   // total amount of slabs   in the 'cache'
    uint16_t        slab_active;  // total of active slabs   in the 'cache'
    uint16_t        failures;     // total of cache allocations that failed.
    uint16_t        pressure;     // A measure of memory consumption in the system.
                                  // As memory becomes more scarce, this value will go up. 
                                  // On a certain threshold, this value will tell the allocator to free some slabs.
} cache_t;


// There will only ever be once instance of this object in the system.
typedef struct __slab_allocator_allocator
{
    cache_t  fixed[8];
    cache_t  fixedDMA[8];
    cache_t* rest_custom;
} SlabAllocator_t;


// this should be defined in kmalloc.h
// The implmentation will be hidden in kmalloc.c/slab.c,
// depending on the variables and their file-scope.
// This 
void switch_management_state_virtual(); 


#endif