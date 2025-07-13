#include <mem/phys/pmm.h>
#include <mem/phys/buddy_ext.h>
#include <std/error.h>
#include <std/error_macro.h>
#include <std/math.h>
#include <std/string.h>


static PageFrameAllocator pageManager;
static uint64_t           MemoryMapPageCount = 0; 


// types
typedef enum {
    Reserved,
    LoaderCode,
    LoaderData,
    BootServicesCode, //
    BootServicesData, //
    RuntimeServicesCode,
    RuntimeServicesData,
    Conventional,     //
    Unusable,
    ACPIReclaimMemory,
    ACPIMemoryNVS,
    MemoryMappedIOPortSpace,
    PalCode,
    Persistent,
    MaxMemoryType
} efi_mem_types;



void PrintMemMapBasicInfo(efi_memory_map* map)
{
    const uint64_t dscs = map->used_size / map->entry_size;
    printk("efi_memory_map info:\nsize:              %x\nstart:             %p\nDescriptor Amount: %x\nDescriptor Size:   %x\n", 
        map->map_size,
        map->mmap, 
        dscs, 
        map->entry_size
    );
}



void PrintMemMapInfo(efi_memory_map* map, bool_t FreeOrUsed)
{
    // Free - 0
    // Used - 1
    const uint64_t        dscs              = map->used_size / map->entry_size;
    efi_mem_descriptor*   dsc               = 0;
    uint8_t               RegionIsFree[2]   = { 0, 0 };
    const char_t*         MemoryType  [2]   = {
        "Available",
        "Used"
    };
    const char_t*         print_info[4]     = {
        // 0->1: aligned print of memory regions
        // 2->3: printing newline or nothing
        "           | ", 
        "%x | ",
        "",
        "\n"
    };


    // print all available/used memory addresses.
    printk("(%s Physical Addresses)\n", MemoryType[FreeOrUsed]);
    for(uint64_t i = 0; i < dscs; ++i)
    {
        dsc = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
        RegionIsFree[i % 2] = (dsc->memtype == Conventional) || (dsc->memtype == BootServicesCode) || (dsc->memtype == BootServicesData);
        RegionIsFree[i % 2] = FreeOrUsed * !RegionIsFree[i % 2] + !FreeOrUsed * RegionIsFree[i % 2];

        printk(print_info[(i != 0 && i % 9 == 0) + 2]);
        printk(print_info[RegionIsFree[i % 2]], dsc->physAddr);
    }


    // print every available/used memory region size.
    printk("\n\n(%s Physical Addresses Sizes)\n", MemoryType[FreeOrUsed]);
    for(uint64_t i = 0; i < dscs; ++i)
    {
        dsc = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
        RegionIsFree[i % 2] = (dsc->memtype == Conventional) || (dsc->memtype == BootServicesCode) || (dsc->memtype == BootServicesData);
        RegionIsFree[i % 2] = FreeOrUsed * !RegionIsFree[i % 2] + !FreeOrUsed * RegionIsFree[i % 2];

        printk(print_info[(i != 0 && i % 9 == 0) + 2]);
        printk(print_info[RegionIsFree[i % 2]], dsc->pages);
    }
}


typedef struct __id_to_string_descriptor
{
    const uint32_t id;
    const char_t*  str;
} id_string;


const id_string* memoryTypeString(uint32_t id, const id_string* arr, uint8_t len) {
    uint8_t i = 0;
    while(arr->id != id && i < len) {
        ++arr;
        ++i;
    }
    return arr;
}
void PrintMemMapDescriptors(efi_memory_map* map)
{
    const uint64_t      dscs = map->used_size / map->entry_size;
    const uint16_t      epl  = 2; // entry-per-line
    uint16_t            rem  = dscs % epl;
    uint16_t            f    = dscs - rem;     
    efi_mem_descriptor* dsc  = 0;
    static const id_string mem_string[19] = {
        { 0,                    "EfiReservedMemoryType     " },
        { 1,                    "EfiLoaderCode             " },
        { 2,                    "EfiLoaderData             " },       
        { 3,                    "EfiBootServicesCode       " }, 
        { 4,                    "EfiBootServicesData       " }, 
        { 5,                    "EfiRuntimeServicesCode    " },
        { 6,                    "EfiRuntimeServicesData    " },
        { 7,                    "EfiConventionalMemory     " }, 
        { 8,                    "EfiUnusableMemory         " },
        { 9,                    "EfiACPIReclaimMemory      " }, 
        { 10,                   "EfiACPIMemoryNVS          " },
        { 11,                   "EfiMemoryMappedIOPortSpace" },
        { 12,                   "EfiPalCode                " },
        { 13,                   "EfiPersistentMemory       " },
        { 14,                   "EfiMaxMemoryType          " },
        { BOOT_ELF_MEM_TYPE,    "EfiOSLoaderLoadedImage    " },
        { BOOT_PAGING_MEM_TYPE, "EfiOSLoaderPagingData     " },
        { BOOT_MEMORY_MAP_TYPE, "EfiOSLoaderMemoryMap      " },
        { 0xFFFFFFFF,           "EfiMaximumMemoryType      " }
    };


    printk("EFI Memory Map Description:\n");
    printk( "idx memory type                | p          | v                | pages      ||");
    printk(" idx memory type                | p          | v                | pages      ||\n");
    for(uint16_t i = 0; i < dscs; i += epl)
    {
        for(uint16_t j = 0; j < epl; ++j) {
            dsc = (efi_mem_descriptor*)((uint64_t)map->mmap + ((i+j) * map->entry_size));
            printk_align_format(3);
            printk("%u ", i+j);
            printk_align_format(6);
            printk("%s | %x | %p | %x || ", 
                memoryTypeString(dsc->memtype, mem_string, 18)->str,
                dsc->physAddr, 
                dsc->virtAddr, 
                dsc->pages
            );
        }
        printk("\n");
    }


    // for(uint16_t i = dscs - rem; i < dscs; ++i)
    // {
    //     dsc = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
    //     printk_align_format(3);
    //     printk("%u ", i);
    //     printk_align_format(6);
    //     printk("%s | %x | %p |%u||", 
    //         memoryTypeString(dsc->memtype, mem_string, 18)->str,
    //         dsc->physAddr, 
    //         dsc->virtAddr, 
    //         dsc->pages
    //     );
    // }
    printk_align_format(0);
    return;
}



void PrintPMMInfo()
{
    printk("Physical Memory Manager Description:\n");
    printk("    Detected %u (Pages)\n    Available memory (bytes): %z\n    Used      memory (bytes): %z\n    Page Frame Allocator Data:\n", 
        totalDetectedMemory(), 
        totalFreeMemory(), 
        totalUsedMemory()
    );


    printk("        Buddy Allocator (Low Mem, 0->4MiB) | Region Start (physical): %X", pageManager.lowZoneAllocator.manager->start);
    printk_align_format(6);
    printk(" | Available Pages: %z | Max Available Block Size (pages): %z\n",
        pageManager.lowZoneAllocator.total,
        pageManager.lowZoneAllocator.maxMemReq
    );
    printk_align_format(0);


    for(uint32_t i = 0; i < pageManager.zoneCount; ++i)
    {
        printk("        Buddy Allocator (Normal Mem      ) | Region Start (physical): %X", pageManager.zoneAllocators[i].manager->start);
        printk_align_format(6);
        printk(" | Available Pages: %z | Max Available Block Size (pages): %z\n",
            pageManager.zoneAllocators[i].total,
            pageManager.zoneAllocators[i].maxMemReq
        );
        printk_align_format(0);
    }
    putln();
    return;
}



void PrintBuddyDescriptors()
{
    printk("Buddy Descriptors: \n");
    
    
    buddyLow_info(pageManager.lowZoneAllocator.manager);


    for(uint16_t i = 0; i < pageManager.zoneCount; ++i)
    {
        printk("buddy %u (normal): \n", i);
        buddyInfo(pageManager.zoneAllocators[i].manager);
    }
    return;
}


void PrintBuddyDescriptor(uint16_t buddyID)
{
    if(buddyID > pageManager.zoneCount) return;
    if(buddyID == 0) {
        buddyLow_info(pageManager.lowZoneAllocator.manager);
    }
    else {
        printk("buddy %u (normal): ", buddyID);
        buddyInfo(pageManager.zoneAllocators[buddyID - 1].manager);
    }
    return;
}



void pfa_init(efi_memory_map* map)
{
    const uint64_t        dscs        = map->used_size / map->entry_size;
    efi_mem_descriptor* dsc         = 0;
    
    efi_mem_descriptor* biggestFree = 0;
    bool_t              isGood      = 0;

    bool_t              regionFree[dscs];
    uint32_t            freeRegionCount   = 0;

    uint32_t            totalAllocators   = 0;
    uint64_t            totalAllocatorMem = 0;

    uint64_t            tmpVar            = 0;
    uintptr_t           allocator_region  = 0;

    uint16_t            under4MiB         = 0;


    // get all available regions from the memory map.
    PrintMemMapDescriptors(map);
    for(uint16_t i = 0; i < dscs; ++i)
    {
        dsc                 = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
        regionFree[i]       = (dsc->memtype == Conventional) || (dsc->memtype == BootServicesCode) || (dsc->memtype == BootServicesData);
        freeRegionCount    += regionFree[i];
        under4MiB          += regionFree[i] && (dsc->physAddr < 0x400000);
        MemoryMapPageCount += dsc->pages;
    }


    // 1. amount of allocators needed.
    // 2. total memory Consumption of memory managers, including the array for pageFrameAllocator.
    // 3. from bytes to pages.
    totalAllocators   = ((MemoryMapPageCount * PAGE_SIZE) >> (BUDDY_TREE_DEPTH + 12ull));           
    totalAllocatorMem = sizeof(BuddyAllocatorLowMem) + totalAllocators * (sizeof(BuddyAllocator) + sizeof(managed_zone));
    totalAllocatorMem = !!(totalAllocatorMem % PAGE_SIZE) + (totalAllocatorMem >> LOG2_PAGE_SIZE);
    
    // printk("ALLOCATORS (COUNT - %u) (MEMORY PAGES CONSUMED - %u)\n", totalAllocators, totalAllocatorMem);
    for(uint16_t i = 0; i < dscs; ++i) // find a region to store all buddy allocators. (memory managers).
    { 
        dsc         = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
        isGood      = (regionFree[i] && dsc->pages > totalAllocatorMem);
        biggestFree = (efi_mem_descriptor*)( isGood * (uint64_t)dsc + !isGood * (uint64_t)biggestFree );
    }
    KERNEL_ERROR_IF(biggestFree == NULLPTR, 
        KERNEL_BUFFER_TOO_SMALL, 
        BOOLEAN_TRUE, 
        "PageFrameAllocator Couldn't initialize - couldn't find enough space for the memory allocators."
    );


    // initialize variables of the pfa. update the 'biggestFree' entry 
    pageManager.zoneAllocators = (managed_zone*)(biggestFree->virtAddr + sizeof(BuddyAllocatorLowMem) + (uint64_t)totalAllocators * sizeof(BuddyAllocator));   
    pageManager.zoneCount      = totalAllocators;
    allocator_region           = biggestFree->virtAddr;
    biggestFree->physAddr     += totalAllocatorMem << LOG2_PAGE_SIZE;
    biggestFree->virtAddr     += totalAllocatorMem << LOG2_PAGE_SIZE;


    // initialize the array of allocators.
    for(uint32_t i = 0; i < totalAllocators; ++i) {
        pageManager.zoneAllocators[i].manager = (BuddyAllocator*)(allocator_region + (uint64_t)i * sizeof(BuddyAllocator));
        pageManager.zoneAllocators[i].isusing   = BOOLEAN_FALSE;
    }
    pageManager.lowZoneAllocator.manager = (BuddyAllocatorLowMem*)(allocator_region + (uint64_t)totalAllocators * sizeof(BuddyAllocator));
    pageManager.lowZoneAllocator.isusing   = BOOLEAN_FALSE;




    uint64_t  memBoundaries[totalAllocators + 1]; // array of 256 MiB Boundaries.
    mem_zone  freeZones    [freeRegionCount * 2]; // array of free memory zones; some zones may need to split in order to fit 256MiB Boundaries.
    mem_zone* tmpZone;
    uint16_t  tmpCount = 0;                       // counts the amount of free zone entries.

    memBoundaries[0] = 0;
    for(uint32_t i = 1; i < totalAllocators + 1; ++i) {
        memBoundaries[i] = i * ALLOC_MANAGED_MEM;
    }
    for(uint32_t i = 0; i < dscs; ++i) {
        dsc = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
        if(regionFree[i] && dsc->physAddr >= 0x400000) {
            freeZones[tmpCount] = (mem_zone){ 
                .mem   = dsc->physAddr, 
                .pages = dsc->pages 
            }; 
            ++tmpCount;
        }
    }


    // initialize the low-mem buddy allocator.
    // 1. get all available regions under 4MiB and put them in an array.
    mem_zone lowMemZones[under4MiB];
    uint16_t totalLowZones = 0;
    isGood = BOOLEAN_TRUE;
    for(uint16_t i = 0; isGood && i < dscs; ++i) {
       dsc    = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
       isGood = dsc->physAddr < 0x400000;

       if(isGood && regionFree[i]) {
           lowMemZones[totalLowZones] = (mem_zone){ .mem = dsc->physAddr, .pages = dsc->pages };
            ++totalLowZones;
       }
    }


    // 2. Split Regions that are on the boundaries of 4MiB, and put their remainder in the freeZones Array.
    for(uint16_t i = 0; i < totalLowZones; ++i)
    {
        if(lowMemZones[i].mem + lowMemZones[i].pages * 4096 > 0x400000)
        {
            // get the amount of pages NEEDED to be in this region.
            // initialize the freeZones with a new available zone.
            tmpVar = (0x400000 - lowMemZones[i].mem) >> LOG2_PAGE_SIZE; 
            freeZones[tmpCount] = (mem_zone){ .mem = 0x400000, .pages = lowMemZones[i].pages - tmpVar };
            ++tmpCount;
            // set the low memory zone to its actual size.
            lowMemZones[i].pages = tmpVar;
        }
    }
    // // uncomment to see debugging output
    // uint16_t idx = 0;
    // for(; idx < tmpCount / 4; ++idx)
    // {
    //     printk("%x %x %x %x || %x %x %x %x\n",
    //         freeZones[4*idx].mem,   freeZones[4*idx + 1].mem,   freeZones[4*idx + 2].mem,   freeZones[4*idx + 3].mem,
    //         freeZones[4*idx].pages, freeZones[4*idx + 1].pages, freeZones[4*idx + 2].pages, freeZones[4*idx + 3].pages
    //     );
    // }
    // for(idx *= 4; idx < tmpCount; ++idx)
    // {
    //     printk("%x %x\n",
    //         freeZones[idx].mem, freeZones[idx].pages 
    //     );
    // }


    // split them according to 256MiB boundary regions.
    for(uint16_t i = 0; i < tmpCount; ++i)
    {
        tmpZone = &freeZones[i];
        for(uint16_t j = 0; j < totalAllocators + 1; ++j)
        {
            // 1. Split the region if its on a boundary and push its other half to the top of the freeZones.
            if(tmpZone->mem < memBoundaries[j] && memBoundaries[j] < (tmpZone->mem + tmpZone->pages * PAGE_SIZE))
            {
                tmpVar = tmpZone->pages;
                tmpZone->pages      = (memBoundaries[j] - tmpZone->mem) >> LOG2_PAGE_SIZE;
                freeZones[tmpCount] = (mem_zone){ .mem = memBoundaries[j], .pages = tmpVar - tmpZone->pages }; 
                ++tmpCount;
            }
        }
    }


    uint16_t mapZoneToAllocator[tmpCount]; // map every region to a buddy allocator.
    for(uint16_t j = 0; j < tmpCount; ++j) {
        mapZoneToAllocator[j] = (freeZones[j].mem - (freeZones[j].mem % ALLOC_MANAGED_MEM)) >> (BUDDY_TREE_DEPTH + 12ull);
    }
    // // uncomment to see debugging output
    // for(uint64_t i = 0; i < tmpCount; ++i) {
    //     printk("%u | ", mapZoneToAllocator[i]);
    // }
    // printk("\nAAAAAAAAAAAAA\n");


    // every allocator will be assigned different regions according to mapZoneToAllocator.
    mem_zone zoneStack[tmpCount];
    for(uint16_t i = 0; i < totalAllocators; ++i)
    {
        // printk("gdt %X %X\n", *(uint64_t*)( ((uint64_t)&map->mmap) + 144));
        tmpVar = 0;
        for(uint16_t j = 0; j < tmpCount; ++j) { // find the free regions for this allocator.
            if(!(mapZoneToAllocator[j] == i)) 
                continue;
            zoneStack[tmpVar] = freeZones[j];
            ++tmpVar;
        }
        // printk("gdt %X\n", *(uint64_t*)( ((uint64_t)&map->mmap) + 144) );

        // printk("%p AT %p %u\n", pageManager.zoneAllocators[i].manager, (void*)(ALLOC_MANAGED_MEM * i), tmpVar);
        buddy_alloc_init_free_regions(pageManager.zoneAllocators[i].manager, (void*)(ALLOC_MANAGED_MEM * i), zoneStack, (uint16_t)tmpVar);
        memset(zoneStack, 0, sizeof(mem_zone) * tmpCount); // reset the stack.

        // printk("gdt %X\n", *(uint64_t*)( ((uint64_t)&map->mmap) + 144) );
    }
    lowmem_buddy_init_free_regions(pageManager.lowZoneAllocator.manager, 0, lowMemZones, under4MiB);


    // initialize the parameters of the normal allocators.
    for(uint16_t i = 0; i < totalAllocators; ++i)
    {
        pageManager.zoneAllocators[i].maxMemReq = maxBlockReq(pageManager.zoneAllocators[i].manager) >> LOG2_PAGE_SIZE;
        pageManager.zoneAllocators[i].total     = freeMemorySize(pageManager.zoneAllocators[i].manager) >> LOG2_PAGE_SIZE;
        pageManager.free += pageManager.zoneAllocators[i].total << LOG2_PAGE_SIZE;
    }

    // initialize the parameters of the low-mem allocator.
    pageManager.lowZoneAllocator.maxMemReq = buddyLow_maxBlockReq(pageManager.lowZoneAllocator.manager) >> LOG2_PAGE_SIZE;
    pageManager.lowZoneAllocator.total     = buddyLow_freeMemory(pageManager.lowZoneAllocator.manager) >> LOG2_PAGE_SIZE;
    pageManager.free += pageManager.lowZoneAllocator.total;

    pageManager.reserved = (ALLOC_MANAGED_MEM * totalAllocators) - pageManager.free;
    return;
}




void* pfa_alloc_pages(IN uint32_t count, uint8_t type)
{
    count = (uint32_t)round2(count);
    if(count * PAGE_SIZE > ALLOC_MANAGED_MEM || count == 0) return NULLPTR;
    // printk("REQUESTED %u\n", count);


    void*          out       = NULLPTR;
    bool_t         satisfied = BOOLEAN_FALSE;
    request_type_t req       = (request_type_t){ .u8 = type };
    if(unlikely(req.mem_location == DMA_LOW))
    {
        // if someone is isusing this low-zone allocator, we'll wait.
        while(pageManager.lowZoneAllocator.isusing) { __asm__ volatile("pause" : : : "memory"); }

        atomic_exchange_ret_u8(&pageManager.lowZoneAllocator.isusing, BOOLEAN_TRUE);     // atomic lock. 
        out = lowmem_alloc_pages(pageManager.lowZoneAllocator.manager, count); // allocate pages.
        satisfied = boolean(out);                                              // set satisfied.
        atomic_clear_u8(&pageManager.lowZoneAllocator.isusing);                  // atomic clear
        return out;
    }


    // this allocation is a DMA_NORMAL-type allocation.
    for(uint16_t i = 0; !satisfied && i < pageManager.zoneCount; ++i)
    {
        // thread A locks the allocator. the next threads will proceed to the next allocators.
        while(pageManager.zoneAllocators[i].isusing || pageManager.zoneAllocators[i].maxMemReq < count) {
            continue;
        }

        // atomic lock. 
        atomic_exchange_ret_u8(&pageManager.zoneAllocators[i].isusing, BOOLEAN_TRUE);

        // printk("BEFORE %p\n", out);
        out = alloc_pages(pageManager.zoneAllocators[i].manager, count);
        pageManager.zoneAllocators[i].maxMemReq = maxBlockReq(pageManager.zoneAllocators[i].manager) >> LOG2_PAGE_SIZE;
        // printk("BEFORE %p\n", out);
        
        satisfied = boolean(out);
        pageManager.free     -= satisfied * (uint64_t)count * PAGE_SIZE;
        pageManager.reserved += satisfied * (uint64_t)count * PAGE_SIZE;
        pageManager.zoneAllocators[i].total -= satisfied * count;

        // atomic clear.
        atomic_clear_u8(&pageManager.zoneAllocators[i].isusing);
    }
    return out;
}



void* pfa_alloc_page(uint8_t type)
{
    return pfa_alloc_pages(1, type);
}



bool_t pfa_free(IN void* ptr, uint32_t blockSize)
{
    blockSize = (uint32_t)round2(blockSize);
    if(unlikely(
        ptr == NULLPTR // bad pointer.                    
    || blockSize > (1u << BUDDY_TREE_DEPTH) // invalid free request.
    || ( ((uint64_t)ptr < 0x400000) && ( (uint64_t)ptr + (blockSize << 12) > 0x400000 ) ) // invalid low-mem free request.
    || blockSize == 0 // invalid argument
    )) { 
        return KERNEL_BAD_DATA; 
    }
    uint16_t alloc_idx = 0;
    uint32_t lvl       = 0;
    bool_t   freed     = BOOLEAN_FALSE;


    if((uint64_t)ptr < 0x400000) { // this is a low mem request.
        lvl = BUDDYLOW_TREE_DEPTH - log2ui(blockSize); // calculate the level of the block.


        while(pageManager.lowZoneAllocator.isusing) {    // if someone is already isusing this allocator, we wait for them.
            __asm__ volatile("pause" : : : "memory"); 
        }


        // atomic lock
        atomic_exchange_ret_u8(&pageManager.lowZoneAllocator.isusing, BOOLEAN_TRUE);
        
        freed = lowmem_free_pages(pageManager.lowZoneAllocator.manager, ptr, lvl);
        if(unlikely(freed != KERNEL_SUCCESS)) {
            KERNEL_ERROR(KERNEL_INVALID_INPUT, BOOLEAN_FALSE, "Failed to Free Pages - Invalid Input Passed to Function.")
            return KERNEL_INVALID_INPUT;
        }
        freed = !freed;
        pageManager.lowZoneAllocator.total    += freed * blockSize;
        pageManager.lowZoneAllocator.maxMemReq = buddyLow_maxBlockReq(pageManager.lowZoneAllocator.manager);

        // atomic clear
        atomic_clear_u8(&pageManager.lowZoneAllocator.isusing);
    }
    else {
        alloc_idx = ((uint64_t)ptr - ((uint64_t)ptr % ALLOC_MANAGED_MEM)) >> (BUDDY_TREE_DEPTH + 12ull);
        lvl       = BUDDY_TREE_DEPTH - log2ui(blockSize);
        freed     = BOOLEAN_FALSE;


        // if someone is already isusing allocator[alloc_idx], we wait for them.
        while(pageManager.zoneAllocators[alloc_idx].isusing) { 
            __asm__ volatile("pause" : : : "memory");
        }


        // atomic lock.
        atomic_exchange_ret_u8(&pageManager.zoneAllocators[alloc_idx].isusing, BOOLEAN_TRUE);  
        
        freed = free_pages(pageManager.zoneAllocators[alloc_idx].manager, ptr, lvl); // free the block.
        if(unlikely(freed != KERNEL_SUCCESS)) {
            KERNEL_ERROR(KERNEL_INVALID_INPUT, BOOLEAN_FALSE, "Failed to Free Pages - Invalid Input Passed to Function.")
            return KERNEL_INVALID_INPUT;
        }
        freed = !freed;
        pageManager.zoneAllocators[alloc_idx].total    += freed * blockSize;
        pageManager.zoneAllocators[alloc_idx].maxMemReq = maxBlockReq(pageManager.zoneAllocators[alloc_idx].manager) >> LOG2_PAGE_SIZE;

        // atomic clear.
        atomic_clear_u8(&pageManager.zoneAllocators[alloc_idx].isusing);
    }

    pageManager.free     += freed * blockSize * PAGE_SIZE;
    pageManager.reserved -= freed * blockSize * PAGE_SIZE;
    return KERNEL_SUCCESS;
}



bool_t pfa_free_page(IN void* ptr)
{
    return pfa_free(ptr, 1);
}



void PrintMemMapFreeRegions(efi_memory_map* map) { PrintMemMapInfo(map, 0); }
void PrintMemMapUsedRegions(efi_memory_map* map) { PrintMemMapInfo(map, 1); }
uint64_t totalDetectedMemory() { return MemoryMapPageCount;        }
uint64_t totalFreeMemory()     { return pageManager.free;     }
uint64_t totalUsedMemory()     { return pageManager.reserved; }
uint64_t MemoryPressure()      { return MemoryMapPageCount / pageManager.free; }


// uint64_t get_used_regions(
//     IN  efi_memory_map* map,
//     OUT mem_zone**      out, 
//     OUT uint16_t*       count
// )
// {
//     const uint64_t           dscs              = map->map_size / map->entry_size;
//     efi_mem_descriptor*    dsc               = 0;
//     uint64_t               size              = 0;
//     uint64_t               totalPages        = 0;
//     uint16_t               offset            = 0;
//     uint16_t               idx               = 0;
//     bool_t                 isFirst           = BOOLEAN_TRUE;
//     uint8_t                regionMap[dscs + 1];
//     mem_zone*              usedRegions       = (mem_zone*)localMemManagerBuffer;
    

//     // map every region in the memory map to 1 or 0.
//     for (uint8_t i = 0; i < dscs; ++i)
//     {
//         dsc          = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
//         regionMap[i] = (dsc->memtype == Conventional) || (dsc->memtype == BootServicesCode) || (dsc->memtype == BootServicesData);  
//         totalPages  += dsc->pages;
//     }
//     regionMap[dscs] = BOOLEAN_TRUE;


//     // mark every region that is used by other services.
//     for(uint8_t i = 0; i < dscs + 1; ++i)
//     {
//         dsc     = (efi_mem_descriptor*)((uint64_t)map->mmap + (i * map->entry_size));
//         size   += !regionMap[i] * dsc->pages;

//         if(isFirst & !regionMap[i])
//         {
//             idx     = i;
//             isFirst = BOOLEAN_FALSE;
//         }
//         if(regionMap[i] && (i != 0 && !regionMap[i - 1]))
//         {
//             dsc                 = (efi_mem_descriptor*)((uint64_t)map->mmap + (idx * map->entry_size));
//             usedRegions[offset] = (mem_zone){ dsc->physAddr, size };
//             size                = 0;
//             isFirst             = BOOLEAN_TRUE;
//             ++offset;
//         }
//     }

//     // debug print for sanity check.
//     // DEBUG(
//     //     printk("\nFirmware Used Memory Zones: %u\ntotal Memory: %u Pages\n", offset, totalPages);
//     //     for(uint8_t i = 0; i < offset; ++i)
//     //     {
//     //         printk("Memory Region %u : { %X, %x }\n", i, usedRegions[i].mem, usedRegions[i].pages);
//     //     }
//     // );
//     localStackPointer += sizeof(mem_zone) * (dscs + 1);

//     *out           = usedRegions;
//     *count         = offset; 
//     return totalPages;
// }
