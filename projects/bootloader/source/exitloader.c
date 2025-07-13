#include <bootloader/exitloader.h>
#include <bootloader/string.h>
#include <bootloader/bool_macro.h>
#include <bootloader/debug_print.h>
#include <bootloader/err.h>
#include <bootloader/funcargtype.h>
#include <bootloader/pause.h>
#include <bootloader/memorymap.h>
#include <bootloader/exitbootservices.h>


#define __KERNEL_HEADER_MEMMAP_ENTRY_AT(MAP, i) \
    ( (efi_mem_descriptor*)((uint8_t*)(MAP)->mmap + (i * (MAP)->entry_size)) )

#define ALLOCATE_PAGE(PAGE_STACK_TYPE_PTR, type_to_cvt) \
    (type_to_cvt*)allocatePage(PAGE_STACK_TYPE_PTR)

#define ALLOC_ZERO(PAGE_STACK_TYPE_PTR, type_to_cvt) \
    (type_to_cvt*)allocatePageAndZero(PAGE_STACK_TYPE_PTR)

#define postBootServicesAssemblyMarker(register0, register1, register2) \
    { \
        __asm__ volatile("movq %0, %%rdi\n\t" : : "i"(0)); \
        __asm__ volatile("movq %0, %%rax\n\t" : : "i"(0)); \
        __asm__ volatile("movq %0, %%rbx\n\t" : : "i"(0)); \
        __asm__ volatile("movq %0, %%rcx\n\t" : : "i"(0)); \
        __asm__ volatile("movq %0, %%rdi\n\t" : : "i"(0xC0FFEEDEADBEEF)); \
        __asm__ volatile("movq %0, %%rax\n\t" : : "r"(register0)); \
        __asm__ volatile("movq %0, %%rbx\n\t" : : "r"(register1)); \
        __asm__ volatile("movq %0, %%rcx\n\t" : : "r"(register2)); \
        halt(); \
    } \


typedef struct __generic_memory_region {
    efi_mem_descriptor* dsc;
    uint64_t            indexInMap;
} efi_memory_region_t;


typedef struct __page_stack_struct_interface {
    kernel_pagebuffer_t* addr;
    uint64_t             pages;
} page_stack_t;


EFI_API static void fixVirtualAddresses(
    IN efi_memory_map* map,
    IN uint64_t        virtual_offset
);

EFI_API static void findBiggestUsableMemoryRegion(
    IN  efi_memory_map*      map,
    OUT efi_memory_region_t* outDsc
);

EFI_API static void finalizeMemoryMap(
    IN efi_memory_map*            map,
    IN const efi_memory_region_t* biggestFreeZone,
    IN const page_stack_t*        latestUsedPageStack,
    IN uint64_t                   virtualOffset
);

EFI_API static void* allocatePage(
    IN page_stack_t* pagebuffer
);

EFI_API static void* allocatePageAndZero(
    IN page_stack_t* pagebuffer
);


static void mapMemoryMapToVirtualMemory(
    IN efi_memory_map const* map,
    IN page_stack_t*   pageStack,
    IN genericPageTable* pml4Table
);
EFI_API static void mapFramebufferToVirtualMemory(
    IN framebuffer_t*    fb,
    IN page_stack_t*     pageStack,
    IN genericPageTable* pml4Table,
    IN uint64_t          virtualOffset
);

EFI_API static void mapPages(
    IN genericPageTable* Table,
    IN page_stack_t*     pageStack,
    IN uint64_t          physStart, 
    IN uint64_t          virtStart, 
    IN uint64_t          flags
);
EFI_API static void dumpPageTables(
    IN genericPageTable* Table
);

EFI_API static void print_kernel_header(
    IN kernel_header_t* hdr
);


void enablePagingJumpToEntry(
    kernel_header_t* oldHeader,
    kernel_header_t* newHeader,
    __kernel_entry   entry, 
    uint64_t         physStart, 
    uint64_t         virtStart,
    EFI_HANDLE       ImageHandle,
    UINTN            memoryMapKey
) {
    uint64_t             virtualoffset = virtStart - physStart;
    efi_memory_region_t  biggestFreeZone;
    page_stack_t         pageStack;

    genericPageTable*    pml4;
    uint64_t             currip;
    page_stack_t         identity_map_alloc_stack;
    print_kernel_header(oldHeader);

    /* 
        1. we will use the biggest region as a stack 
        2. Later, we'll modify the biggestFreeZone Entry into 2 entries:
            * BOOT_PAGING_MEM_TYPE  - Memory region with all the paging tables allocated
            * efiConventionalMemory - The remainder of the memory left from pageStack
        
        * Technically speaking, I should do the following:
            pageCountForVirtualMemory = calculateThatSomehow(...);
            CHECKERRFMT(
                biggestFreeZone->pages < pageCountForVirtualMemory ? -1 : 1,
                L"Biggest Memory region in the Memory map not big enough for virtual mapping\n\r"
            );
        
        3. Also, we'll map the next page-worth of instructions, so that we can jump
            to the kernel properly after changing the CR3 register (page table)

        4. finally, we'll update 'newHeader' and jump to the kernel
    */
    debug_printb(L"\
    Memory Configuration:\n\t\
      kstartp                 : %p\n\t\
      kstartv                 : %p\n\t\
      krbptopv                : %p\n\t\
      kpercpu_rbpv            : %p\n\t\
      kpercpu_tssv            : %p\n\t\
      kgdtv                   : %p\n\t\
      efi_rtp                 : %p\n\t\
      pml4                    : %p\n\t\
      bl_ripv                 : %p\n\t\
      bl_rip_pagetable_buf    : %p\n\t\
      krbpsize                : %x\n\t\
      pml4size                : %x\n\t\
      bl_rip_pagetable_bufsize: %x\n\t\
      reserved1               : %u\n\t\
      kendoffset              : %x\n\t",
        oldHeader->memcfg.kstartp,
        oldHeader->memcfg.kstartv,
        oldHeader->memcfg.krbptopv,
        oldHeader->memcfg.kpercpu_rbpv,
        oldHeader->memcfg.kpercpu_tssv,
        oldHeader->memcfg.kgdtv,
        oldHeader->memcfg.efi_rtp,
        oldHeader->memcfg.pml4,
        oldHeader->memcfg.bl_ripv,
        oldHeader->memcfg.bl_rip_pagetable_buf,
        oldHeader->memcfg.krbpsize,
        oldHeader->memcfg.pml4size,
        oldHeader->memcfg.bl_rip_pagetable_bufsize,
        oldHeader->memcfg.reserved1,
        oldHeader->memcfg.kendoffset
    );
    
    
    fixVirtualAddresses(&oldHeader->map, virtualoffset);


    findBiggestUsableMemoryRegion(&oldHeader->map, &biggestFreeZone);
    pageStack = (page_stack_t){
        (kernel_pagebuffer_t*)biggestFreeZone.dsc->physAddr,
        biggestFreeZone.dsc->pages
    };
    pml4 = ALLOC_ZERO(&pageStack, genericPageTable);
    
    
    mapMemoryMapToVirtualMemory(&oldHeader->map, &pageStack, pml4);


    /* map the next Page-worth of instructions */
    /* store pageStack somewhere else to calculate how many pages we used */
    __asm__ volatile("lea (%%rip), %0" : "=r"(currip));
    identity_map_alloc_stack = pageStack;
    mapPages(pml4, &identity_map_alloc_stack, currip, currip, 0);
    
    oldHeader->memcfg.bl_ripv = currip;
    oldHeader->memcfg.bl_rip_pagetable_buf     = (genericPageTable*)identity_map_alloc_stack.addr;
    oldHeader->memcfg.bl_rip_pagetable_bufsize = (uint16_t)(identity_map_alloc_stack.pages - pageStack.pages);
    pageStack = identity_map_alloc_stack; /* update pageStack to the updated amount */


    mapFramebufferToVirtualMemory(
        &oldHeader->screen,
        &pageStack, 
        pml4, 
        virtualoffset
    );

    /* Print extra data before exitBootServices() ... */
    // debug_printb(L"entry @%p\n\rnewHeader @%p (physical)\n\r", entry, newHeader);
    // dumpMemoryMap(&oldHeader->map);
    // dumpPageTables(pml4);
    // print_kernel_header(oldHeader);
    exitBootServices(ImageHandle, memoryMapKey);

    /* BiggestFreeZone is now the memory-descriptor entry for the paging tables */
    finalizeMemoryMap(
        &oldHeader->map, 
        &biggestFreeZone,
        &pageStack,
        virtualoffset
    );


    /* 
        this is the last datapoint to update
        finalizeMemoryMap mapped 'oldHeader->map.mmap' to the virtual address space  
        Problem is, we haven't switched the page table yet, 
        so a memory access would cause a page fault
        So we do a little bit of pointer magic to read from the physical address    
    */
    oldHeader->memcfg.pml4     = (uint64_t)pml4;
    oldHeader->memcfg.pml4size = (
        (efi_mem_descriptor*)(  
            ((uint8_t*)oldHeader->map.mmap) 
            - virtualoffset 
            + (oldHeader->map.entry_size * biggestFreeZone.indexInMap)
        ))->pages;

    /* newHeader is in the memory region allocated for the kernel */
    memcpy(newHeader, oldHeader, sizeof(kernel_header_t));
    newHeader = (kernel_header_t*)( ((uint8_t*)newHeader) + virtualoffset );

    __asm__ volatile("movq %0, %%rdi\n\t" : : "r"(newHeader));
    __asm__ volatile("movq %0, %%rcx\n\t" : : "r"(entry));
    __asm__ volatile("movq %0, %%cr3\n\t" : : "r" (pml4));     
    __asm__ volatile("jmp *%rcx\n\t");
    /* 
        Set Variables RDI & RCX before switching page directories
        RDI - first argument to a function in the systemV abi
        Switch to new paging directory 
        Jump to kernel entry
    */
    halt(); /* if somehow ... */
}



static void fixVirtualAddresses(
    efi_memory_map* map,
    uint64_t        virtual_offset
) {
    const uint64_t      dscs = (map->used_size / map->entry_size);
    efi_mem_descriptor* dsc  = map->mmap;
    uint8_t             notRuntimeRelated = 0;

    for(uint64_t i = 0; i < dscs; ++i)
    {
        dsc = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, i);
        notRuntimeRelated = blboolean(
            dsc->memtype != efiRuntimeServicesCode 
            && 
            dsc->memtype != efiRuntimeServicesData
        );
        dsc->virtAddr = dsc->physAddr;
        dsc->virtAddr += blboolean(notRuntimeRelated) ? virtual_offset : 0;
    }
    return;        
}


static void findBiggestUsableMemoryRegion(
    IN  efi_memory_map*      map,
    OUT efi_memory_region_t* outDsc
) {
    const uint64_t      dscs      = (map->used_size / map->entry_size);
    efi_mem_descriptor* dsc       = map->mmap;
    efi_mem_descriptor* biFree    = map->mmap;
    uint64_t            biFreeIdx = 0;
    uint8_t             usable    = 0;
    for(uint64_t i = 0; i < dscs; ++i)
    {
        dsc = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, i);
        usable = blboolean(dsc->pages > biFree->pages) 
            && (
                (dsc->memtype == efiConventionalMemory) 
                || (dsc->memtype == efiBootServicesCode) 
                || (dsc->memtype == efiBootServicesData)
            );

        biFree = usable ? dsc : biFree;
        biFreeIdx = usable ? i : biFreeIdx;
    }


    *outDsc = (efi_memory_region_t){
        .dsc        = biFree,
        .indexInMap = biFreeIdx
    };
    return; 
}


static void finalizeMemoryMap(
    IN efi_memory_map*            map,
    IN const efi_memory_region_t* biggestFreeZone,
    IN const page_stack_t*        latestUsedPageStack,
    IN const uint64_t             virtualOffset
) {
    efi_mem_descriptor* virtualMappingPages  = (efi_mem_descriptor*)NULL;
    efi_mem_descriptor* remainingMemoryPages = (efi_mem_descriptor*)NULL;
    efi_mem_descriptor* prev = (efi_mem_descriptor*)NULL;
    efi_mem_descriptor* dsc  = (efi_mem_descriptor*)NULL;
    const uint64_t      dscs = (map->used_size / map->entry_size);    


    // Move all entries from idx biFreeIdx to the right by 1.
    for(uint64_t i = dscs; i > biggestFreeZone->indexInMap; --i)
    {
        prev = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, i - 1);
        dsc  = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, i    );
        *dsc = *prev;
    }

    // update memory-descriptor[biggestFreeZone->IndexInMap] for virtual-memory pageTables
    virtualMappingPages = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, biggestFreeZone->indexInMap);
    virtualMappingPages->memtype = BOOT_PAGING_MEM_TYPE;
    virtualMappingPages->pages   = biggestFreeZone->dsc->pages - latestUsedPageStack->pages;
    
    // update the next memory-descriptor for the left over pages inside biggestFree.
    remainingMemoryPages = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, biggestFreeZone->indexInMap+1);
    remainingMemoryPages->memtype  = EfiConventionalMemory;
    remainingMemoryPages->physAddr = virtualMappingPages->physAddr + (virtualMappingPages->pages << EFI_PAGE_SHIFT);
    remainingMemoryPages->virtAddr = remainingMemoryPages->physAddr + virtualOffset;
    remainingMemoryPages->pages    = latestUsedPageStack->pages; /* whats leftover of the stack */

    ++map->used_size; /* we split an entry to 2 */
    map->mmap = VIRTUAL (efi_mem_descriptor*)((uint8_t*)map->mmap + virtualOffset); /* update to virtual mapping */

    return;
}


static void* allocatePage(
    page_stack_t* pagebuffer
) {
    uint8_t* addr = (uint8_t*)pagebuffer->addr;
    if(EFI_ERROR(pagebuffer->pages - 1 == 0 ? -1 : 1)) {
        return NULL;
    }
    /* Should Not Happen Given Total Memory >= 256MiB  */
    CHECKERRFMT(pagebuffer->pages - 1 == 0 ? -1 : 1, 
        L"allocatePage() => Biggest Memory Region in the Memory map has been exhausted\n\r"
    );

    ++pagebuffer->addr;
    --pagebuffer->pages;
    return addr;
}


static void* allocatePageAndZero(
    page_stack_t* pagebuffer
) {
    uint8_t* out = (uint8_t*)allocatePage(pagebuffer);
    if(out) {
        memsetzero(out, PAGE_SIZE);
    }
    return out;
}


static void mapMemoryMapToVirtualMemory(
    IN efi_memory_map const* map,
    IN page_stack_t*         pageStack,
    IN genericPageTable*     pml4Table
) {
    const uint64_t      dscs = (map->used_size / map->entry_size);
    efi_mem_descriptor* dsc  = map->mmap;
    uint64_t            pageCount = 0;
    uint64_t            pageTotal = 0;
    physical_address    addrp     = 0;
    virtual_address     addrv     = 0;


    for(uint64_t i = 0; i < dscs; ++i)
    {
        dsc       = __KERNEL_HEADER_MEMMAP_ENTRY_AT(map, i);
        pageCount = dsc->pages; 
        addrp     = dsc->physAddr;
        addrv     = dsc->virtAddr;
        pageTotal += pageCount;
        for(uint64_t page = 0; page < pageCount; ++page) 
        {
            mapPages(pml4Table, pageStack, 
                addrp + (page * PAGE_SIZE), 
                addrv + (page * PAGE_SIZE), 
                0
            );
        }
    }

    debug_printb(L"mapMemoryMapToVirtualMemory() mapped %z pages\n\r", pageTotal);
    return;
}


static void mapFramebufferToVirtualMemory(
    IN framebuffer_t*    fb,
    IN page_stack_t*     pageStack,
    IN genericPageTable* pml4Table,
    IN uint64_t          virtualOffset
) {
    uint64_t fbsize = 0;
    uint64_t virt   = 0;
    uint64_t phys   = 0;
    
    /* fb->start is already a virtual address, so we need to undo the offset to get the phys_addr */
    /* see main.c:143 */
    virt   = (uint64_t)fb->start;
    phys   = (uint64_t)fb->start - virtualOffset;
    fbsize = (uint64_t)fb->dims.x * (uint64_t)fb->dims.y;
    fbsize *= __KERNEL_CONSOLE_RGB32_UNION_TYPE_SIZE_BYTES;
    fbsize >>= EFI_PAGE_SHIFT;
    debug_printb(L"Mapping %z Pages for Framebuffer @%p (Physical) -> @%p (Virtual)\n\r",
        fbsize,
        phys,
        virt
    );
    for(uint64_t i = 0; i < fbsize; ++i) 
    {
        mapPages(pml4Table, pageStack, 
            phys + (i * PAGE_SIZE), 
            virt + (i * PAGE_SIZE), 
            0
        );
    }


    return;
}




/* Debug Related Functions / Scraps - Not Necessarily Used */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#ifndef MAP_PAGES_DEFAULT_VIRTUAL_ATTRIB
#   define MAP_PAGES_DEFAULT_VIRTUAL_ATTRIB(full_pointer) \
    ( \
        0x00 \
        | VMM_TABLE_ENTRY_PRESENT_BIT \
        | VMM_TABLE_ENTRY_READWRITE_BIT \
        | ( VMM_TABLE_ENTRY_GET_ADDRESS( full_pointer ) << LOG2_PAGE_SIZE ) \
    )
#endif


static void mapPages(
    IN genericPageTable* Table,
    IN page_stack_t*     pageStack,
    IN uint64_t          physStart, 
    IN uint64_t          virtStart, 
    IN uint64_t          flags
) {
    genericPageTable*      table      = Table;
    genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
    uint64_t               address    = virtStart;
    uint64_t               indices[4] = {0, 0, 0, 0};
    uint64_t               entu64     = 0;
    (void)(flags);
    address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the genericPageTable                                    [ genericPageTable[i]                 ]
    address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
    address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
    address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]


    for(uint8_t i = 0; i < 3u; ++i)
    {
        ent    = &table->entry[indices[3 - i]];
        entu64 = *ent;

        if(entu64 & VMM_TABLE_ENTRY_PRESENT_BIT) {
            table = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(entu64) << LOG2_PAGE_SIZE );
        } else {
            table = ALLOC_ZERO(pageStack, genericPageTable);
            *ent  = 0b1 | (0b1 << 1) | ( ((((uint64_t)table) >> LOG2_PAGE_SIZE) & 0xffffffffff) << LOG2_PAGE_SIZE);
        }
    }


    table->entry[indices[0]] = 0b1 | (0b1 << 1) | ( ((physStart >> LOG2_PAGE_SIZE) & 0xffffffffff) << LOG2_PAGE_SIZE);
    return;
}


static void dumpPageTables(
    IN genericPageTable* pageTable
) {
    genericPageTable* currtable[3] = { 
        pageTable, 
        (genericPageTable*)NULL, 
        (genericPageTable*)NULL 
    };
    genericPageTableEntry* current;
    
    
    printf((const CHAR16*)L"Page Tables Dump Begin:\n\r");
    for(uint16_t i = 0; i < 512; ++i) {
        current = &pageTable->entry[i];
        if(*current == 0) {
            continue;
        }
        printf((const CHAR16*)L"  [%u] %a %a %a 0x%x\n\r",
            i,
            VMM_TABLE_ENTRY_GET_PRESENT_FLAG(*current)    ? "+P" : "-P",
            VMM_TABLE_ENTRY_GET_READWRITE_FLAG(*current)  ? "RW" : "R ",
            VMM_TABLE_ENTRY_GET_USERACCESS_FLAG(*current) ? "SU" : "S ",
            *current
        );


        currtable[0] = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(*current) << LOG2_PAGE_SIZE );
        for(uint16_t j = 0; j < 512; ++j) {
            current = &currtable[0]->entry[j];
            if(*current == 0) {
                continue;
            }
            printf((const CHAR16*)L"  >>[%u] %a %a %a | 0x%x\n\r",
                j,
                VMM_TABLE_ENTRY_GET_PRESENT_FLAG(*current)    ? "+P" : "-P",
                VMM_TABLE_ENTRY_GET_READWRITE_FLAG(*current)  ? "RW" : "R ",
                VMM_TABLE_ENTRY_GET_USERACCESS_FLAG(*current) ? "SU" : "S ",
                *current
            );


            currtable[1] = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(*current) << LOG2_PAGE_SIZE );
            for(uint16_t k = 0; k < 512; ++k) {
                current = &currtable[1]->entry[k];
                if(*current == 0) {
                    continue;
                }
                printf((const CHAR16*)L"    >>>>[%u] %a %a %a | 0x%x\n\r",
                    k,
                    VMM_TABLE_ENTRY_GET_PRESENT_FLAG(*current)    ? "+P" : "-P",
                    VMM_TABLE_ENTRY_GET_READWRITE_FLAG(*current)  ? "RW" : "R ",
                    VMM_TABLE_ENTRY_GET_USERACCESS_FLAG(*current) ? "SU" : "S ",
                    *current
                );


                currtable[2] = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(*current) << LOG2_PAGE_SIZE );
                if(i == 511 && j == 0 && k == 0) 
                {
                    for(uint16_t w = 0; w < 512; ++w) {
                        current = &currtable[2]->entry[w];
                        if(*current == 0) {
                            continue;
                        }
                        printf((const CHAR16*)L"    >>>>>>[%u] %a %a %a | 0x%x\n\r",
                            w,
                            VMM_TABLE_ENTRY_GET_PRESENT_FLAG(*current)    ? "+P" : "-P",
                            VMM_TABLE_ENTRY_GET_READWRITE_FLAG(*current)  ? "RW" : "R ",
                            VMM_TABLE_ENTRY_GET_USERACCESS_FLAG(*current) ? "S " : "SU",
                            *current
                        );
                    }
                }
            }
        }
    }
    printf((const CHAR16*)L"Page Tables Dump End\n\r");
    
    
    return;
}


static void print_kernel_header(
    IN kernel_header_t* hdr
) {
    debug_printb(L"Kernel Header Contents:\n\t");
    debug_printb(L"\
    Memory Map:\n\t\
      mmap       : %p\n\t\
      used_size  : %X\n\t\
      map_size   : %X\n\t\
      entry_size : %X\n\t",
        hdr->map.mmap,
        hdr->map.used_size,
        hdr->map.map_size,
        hdr->map.entry_size
    );
    debug_printb(L"\
    Memory Configuration:\n\t\
      kstartp                 : %p\n\t\
      kstartv                 : %p\n\t\
      krbptopv                : %p\n\t\
      kpercpu_rbpv            : %p\n\t\
      kpercpu_tssv            : %p\n\t\
      kgdtv                   : %p\n\t\
      efi_rtp                 : %p\n\t\
      pml4                    : %p\n\t\
      bl_ripv                 : %p\n\t\
      bl_rip_pagetable_buf    : %p\n\t\
      krbpsize                : %x\n\t\
      pml4size                : %x\n\t\
      bl_rip_pagetable_bufsize: %x\n\t\
      reserved1               : %u\n\t\
      kendoffset              : %x\n\t",
        hdr->memcfg.kstartp,
        hdr->memcfg.kstartv,
        hdr->memcfg.krbptopv,
        hdr->memcfg.kpercpu_rbpv,
        hdr->memcfg.kpercpu_tssv,
        hdr->memcfg.kgdtv,
        hdr->memcfg.efi_rtp,
        hdr->memcfg.pml4,
        hdr->memcfg.bl_ripv,
        hdr->memcfg.bl_rip_pagetable_buf,
        hdr->memcfg.krbpsize,
        hdr->memcfg.pml4size,
        hdr->memcfg.bl_rip_pagetable_bufsize,
        hdr->memcfg.reserved1,
        hdr->memcfg.kendoffset
    );

    debug_printb(L"\
    Framebuffer:\n\t\
      start : %p\n\t\
      dims  : (%u, %u)\n\t",
        hdr->screen.start,
        hdr->screen.dims.x,
        hdr->screen.dims.y
    );
    debug_printb(L"\
    ACPI Config: %a\n\t\
      Physical Address: %p\n\t\
      Processor Count : %z\n\t",
        hdr->acpi.extended ? "XSDT" : "RSDT",
        hdr->acpi.address,
        hdr->acpi.procCount
    );


    return;
}


// static void mapPagesWithDebug(
//     IN genericPageTable*    Table,
//     IN page_stack_t* pageStack,
//     IN uint64_t      physStart, 
//     IN uint64_t      virtStart, 
//     IN uint64_t      flags
// ) {
//     genericPageTable*      table      = Table;
//     genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
//     uint64_t               address    = virtStart;
//     uint64_t               indices[4] = {0, 0, 0, 0};
//     uint64_t               entu64 = 0;
//     (void)(flags);
//     address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the genericPageTable                                    [ genericPageTable[i]                 ]
//     address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
//     address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
//     address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]

//     debug_printb(L"%a:%u - Indices Are { %z %z %z %z }\n\r", 
//         __FILE__, 
//         __LINE__,
//         indices[0],
//         indices[1],
//         indices[2],
//         indices[3]
//     );
//     for(uint8_t i = 0; i < 3u; ++i)
//     {
//         debug_printb(L"%a:%u - indices[3-i] = %z | &entry[0] @%p | &entry[1] @%p | &entry[510] @%p = %z\n\r", 
//             __FILE__, 
//             __LINE__,
//             indices[3 - i],
//             &table->entry[0],
//             &table->entry[1],
//             &table->entry[510],
//             table->entry[510]
//         );
//         ent    = &table->entry[indices[3 - i]];
//         entu64 = *ent;

//         debug_printb(L"%a:%u - ent @%p (Val = 0x%X)\n\r", __FILE__, __LINE__, ent, entu64);
//         // if(ent & VMM_TABLE_ENTRY_PRESENT_BIT)
//         // {
//         //     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         //     table = ALLOC_ZERO(pageStack, genericPageTable);
//         //     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         //     ent->address = (uint64_t)table >> LOG2_PAGE_SIZE;
//         //     ent->present = TRUE;
//         //     ent->rw      = TRUE;
//         //     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         // }
//         // else {
//         //     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         //     table = (genericPageTable*)(((uint64_t)ent->address) << LOG2_PAGE_SIZE);
//         //     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         // }
//         // debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//         if(!(entu64 & VMM_TABLE_ENTRY_PRESENT_BIT)) {
//             debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//             table   = ALLOC_ZERO(pageStack, genericPageTable);
//             entu64  = VMM_TABLE_ENTRY_PRESENT_BIT | VMM_TABLE_ENTRY_READWRITE_BIT;
//             entu64 |= ( VMM_TABLE_ENTRY_GET_ADDRESS( (uint64_t)table ) << LOG2_PAGE_SIZE );
//             debug_printb(L"%a:%u - entu64 %X\n\r", __FILE__, __LINE__, entu64);
//             *ent = entu64;
//         } else {
//             table = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(entu64) << LOG2_PAGE_SIZE );
//             debug_printb(L"%a:%u - table %X\n\r", __FILE__, __LINE__, table);
//         }
//     }

//     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//     ent = &table->entry[indices[0]];

//     entu64  = 0;
//     entu64  = VMM_TABLE_ENTRY_PRESENT_BIT | VMM_TABLE_ENTRY_READWRITE_BIT;
//     entu64 |= ( VMM_TABLE_ENTRY_GET_ADDRESS( physStart ) << LOG2_PAGE_SIZE );
//     debug_printb(L"%a:%u - entu64 %X | physStart = %X | *ent = %X\n\r", __FILE__, __LINE__, entu64, physStart, *ent);
//     *ent = entu64;
//     debug_printb(L"%a:%u\n\r", __FILE__, __LINE__);
//     halt();
//     return;
// }


// static void mapPagesWithModernTypes(
//     IN genericPageTable* Table,
//     IN page_stack_t*     pageStack,
//     IN uint64_t          physStart, 
//     IN uint64_t          virtStart, 
//     IN uint64_t          flags
// ) {
//     genericPageTable*      table      = Table;
//     genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
//     uint64_t               address    = virtStart;
//     uint64_t               indices[4] = {0, 0, 0, 0};
//     uint64_t               entu64     = 0;
//     (void)(flags);
//     address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the genericPageTable                             [ genericPageTable[i]          ]
//     address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
//     address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
//     address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]


//     for(uint8_t i = 0; i < 3u; ++i)
//     {
//         ent    = &table->entry[indices[3 - i]];
//         entu64 = *ent;

//         if(entu64 & VMM_TABLE_ENTRY_PRESENT_BIT) {
//             table = (genericPageTable*)( VMM_TABLE_ENTRY_GET_ADDRESS(entu64) << LOG2_PAGE_SIZE );
//         } else {
//             table   = ALLOC_ZERO(pageStack, genericPageTable);
//             // entu64  = VMM_TABLE_ENTRY_PRESENT_BIT | VMM_TABLE_ENTRY_READWRITE_BIT;
//             // entu64 |= ( VMM_TABLE_ENTRY_GET_ADDRESS( (uint64_t)table ) << LOG2_PAGE_SIZE );
//             // *ent = entu64;
//             *ent = MAP_PAGES_DEFAULT_VIRTUAL_ATTRIB( (uint64_t)table );
//         }
//     }


//     // table->entry[indices[0]] = 0x00 
//     //     | VMM_TABLE_ENTRY_PRESENT_BIT 
//     //     | VMM_TABLE_ENTRY_READWRITE_BIT
//     //     | ( VMM_TABLE_ENTRY_GET_ADDRESS( physStart ) << LOG2_PAGE_SIZE );
//     table->entry[indices[0]] = MAP_PAGES_DEFAULT_VIRTUAL_ATTRIB(physStart);
//     return;
// }


#pragma GCC diagnostic pop /* -Wunused-function */
