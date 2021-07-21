#pragma once
#ifndef __KERNEL_PMM__
#define __KERNEL_PMM__
#include "buddy.h"
#include "buddyLow.h"
#include "../../std/atomic.h"




#define DMA_LOW    0
#define DMA_NORMAL 1
typedef struct __memory_type_flags
{
    union {
        struct pack {
            uint8_t mem_location : 2; // low, normal or high.
            uint8_t request_id   : 6; // who is requesting the memory (future idea, not used yet).
        };
        uint8_t u8;
    };
} request_type_t;


typedef struct __memory_zone
{
    physical_address mem;   // the starting address of the memory zone
    uint64_t         pages; // amount of pages (used or free, depending on context) in the memory zone
} mem_zone;


typedef struct __managed_zone
{
    BuddyAllocator* manager;   // manages total memory zone
    uint64_t        total;     // amount of free pages in this zone.
    uint64_t        maxMemReq; // if lower than request, we can't this allocator.
    atomic8u_t      using;     // if a thread is using this allocator it'll set this variable.
} managed_zone;


typedef struct __low_memory_managed_zone
{
    BuddyAllocatorLowMem* manager;
    uint16_t              total;
    uint16_t              maxMemReq;
    atomic8u_t            using;
} lowmem_zone;


typedef struct __PageFrameAllocator
{
    lowmem_zone   lowZoneAllocator; // allocator for managing memory under 4MiB.
    managed_zone* zoneAllocators;   // an array containing metadata for each buddy allocator, and metadata for it.
    uint16_t      zoneCount;        // Amount of allocators (total amount in alloc_list)
    uint64_t      free;             // free memory     (bytes)
    uint64_t      reserved;         // reserved memory (bytes)
} PageFrameAllocator;



/*  
    ========================================
                Helper Functions
    ========================================
*/

/* 
    * PrintMemMapBasicInfo()   -> Prints the Size, Address, Amount of Descriptors And Size of Descriptor in the memory map.
*/
void PrintMemMapBasicInfo  (efi_memory_map* map);


/* 
    * PrintMemMapFreeRegions() -> Prints all the available memory regions in the memory map in a table format.
*/
void PrintMemMapFreeRegions(efi_memory_map* map);


/*     
    * PrintMemMapUsedRegions() -> Prints all the used      memory regions in the memory map in a table format.
*/
void PrintMemMapUsedRegions(efi_memory_map* map);


/*
    * read the efi_memory_map and prints its info to the screen
*/
void PrintMemMapDescriptors(efi_memory_map* map);


/* 
    * Prints info about the Page Frame Allocator.
*/
void PrintPMMInfo();


/* 
    * Prints the description of every buddy allocator in the alloc_list.
*/
void PrintBuddyDescriptors();


/* 
    * Same thing as PrintBuddyDescriptors(), but for a single buddy.
    * uint16_t buddyID - the index of the buddy in the zone array.
      0            - Low    Memory Buddy Alloc
      1->zoneCount - Normal memory Buddy Alloc
*/
void PrintBuddyDescriptor(uint16_t buddyID);


/* 
    * Initializes the Page Frame Allocator.
    * efi_memory_map* map: a pointer to the efi_memory_map retrieved from the Bootloader.
*/
void  pfa_init(IN efi_memory_map* map);


/* 
    * Reserves 'count' contiguous 4KiB Pages from the Page Frame Allocator.
    * The Page Frame Allocator searches for a given allocator to satisfy the request.
    * If the manager doesn't find an allocator, nullptr will be returned.
    * 
    * IN uint32_t count        - the amount of pages that are requested.
    * IN uint8_t  request_type - uint8_t in the form of request_type_t describing what type of allocation is requested.
*/
void* pfa_alloc_pages(IN uint32_t count, IN uint8_t request_type);


/* 
    * same as pfa_request_pages(), but for the special case of 1 page only (Doesn't require a block header)
    * returns a pointer to a contiguous 4KiB block in 'physical' memory.
*/
void* pfa_alloc_page(IN uint8_t request_type);


/* 
    * Returns an allocated memory region to the Page Frame Allocator. 
    * 
    * ptr:            pointer to a memory region previously allocated from the Page Frame Allocator.
    * blockSizePages: The amount of pages that were allocated for this region. 
    *                 essentially this is 'count' that was passed from pfa_alloc_pages().
*/
bool_t  pfa_free(IN void* ptr, uint32_t blockSizePages);


/* 
    * Returns a Previously allocated page to the Page Frame Allocator.
    * ptr: the starting address of the page to be freed.
*/
bool_t pfa_free_page(IN void* ptr);


/* 
    * returns the total amount of memory detected by
    * the UEFI Memory map.
    * This function should be called only afer calling pfa_init()
    * before pfa_init(), it'll return 0.
*/
uint64_t totalDetectedMemory();


/* 
    * returns the amount of currently available memory (bytes).
*/
uint64_t totalFreeMemory();


/* 
    * returns the amount of currently used      memory (bytes).
*/
uint64_t totalUsedMemory();


/* 
    * Calculates a ratio that describes the 'memory pressure' in the system.
    * A lower ratio means memory is scarce and the system needs to return data to the PMM,
    * and put more data on swap files in the disk.
    * When The Pressure Ratio is 1, 100% of memory (RAM) is used. 
    * In such a scenario, the system will try to free as much data as possible,
    * in order to elavate the pressure. 
*/
uint64_t MemoryPressure();

#endif