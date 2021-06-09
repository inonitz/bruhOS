#include "paging.h"
#include "../phys/pmm.h"
#include "../map_ptr.h"



static TableManager pagingMan;


void mapVirtualToPhysical(PML4* Table, void* vaddress, void* paddress) {
    PageTable*  table      = (PageTable*)VIRT(Table);
    TableEntry* ent        = nullptr; 
    uint64_t    address    = (uint64_t)vaddress;
    uint64_t    indices[4] = {0};
    // [ no execute (1) | ignored (15) | page directory pointer (9) | page directory (9) | page table (9) | page (9) | byte offset (12)]
    // ^ format of a virtual address (64bits - uint64_t)
    address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the PageTable                                    [ PageTable[i]                 ]
    address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
    address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
    address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]


   for(uint8_t i = 0; i < 3u; ++i)
    {
        ent = &table->entry[indices[3 - i]]; // get current entry
        if(!ent->present)
        {
            table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), PageTable*); // update table for next lvl.
            memset((void*)table, 0x00, PAGE_SIZE);
            ent->address = (uint64_t)PHYS(table) >> 12; // this address needs to be physical in the table
            ent->present = TRUE;
            ent->rw      = TRUE;
        }
        else {
            table = VIRT_TYPE(ent->address << 12, PageTable*); // update table for next lvl
        }
        // printk("lvl %u, ent: %X\n", 3 - i, ent->ui64);
    }
    
    ent = &table->entry[indices[0]];
    // printk("ent before: %X\n", ent->ui64);
    ent->address     = (uint64_t)paddress >> 12;
    ent->present     = TRUE;
    ent->userOrSuper = FALSE;
    ent->rw          = TRUE;
    // printk("ent after:  %X\n", ent->ui64);
}


void map_vtop(PML4* Table, void* vaddress, void* paddress, uint16_t flags)
{
    PageTable*  table      = (PageTable*)VIRT(Table);
    TableEntry* ent        = nullptr; 
    uint64_t    address    = (uint64_t)vaddress;
    uint64_t    indices[4] = {0};
    address >>= 12; indices[0] = address & 0x1ff;
    address >>= 9;  indices[1] = address & 0x1ff;
    address >>= 9;  indices[2] = address & 0x1ff;
    address >>= 9;  indices[3] = address & 0x1ff;


   for(uint8_t i = 0; i < 3u; ++i)
    {
        ent = &table->entry[indices[3 - i]];
        if(!ent->present)
        {
            table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), PageTable*);
            memset((void*)table, 0x00, PAGE_SIZE);
            ent->address = (uint64_t)PHYS(table) >> 12;
            ent->ui64 |= (uint64_t)flags;
        }
        else {
            table = VIRT_TYPE(ent->address << 12, PageTable*); // update table for next lvl
        }
    }
    
    ent          = &table->entry[indices[0]];
    ent->address = (uint64_t)paddress >> 12;
    ent->ui64   |= (uint64_t)flags;

}


void unmapVirtual(PML4* Table, void* vaddress)
{
    PageTable*  table      = (PageTable*)Table;
    TableEntry* ent        = nullptr;
    uint64_t    address    = (uint64_t)vaddress;
    uint64_t    indices[4] = {0};
    address >>= 12; indices[0] = address & 0x1ff;
    address >>= 9;  indices[1] = address & 0x1ff;
    address >>= 9;  indices[2] = address & 0x1ff;
    address >>= 9;  indices[3] = address & 0x1ff;
    

    ent = &table->entry[indices[3]]; 
    for(uint8_t i = 1; ent && i < 3u; ++i)
    {
        table = (PageTable*)(((uint64_t)ent->address) << 12);
        ent   = &table->entry[indices[3 - i]];  
    }
    if(ent == nullptr)
        return;
    
    ent = &table->entry[indices[0]];
    ent->present = FALSE;
}


void load_cr3_reg(PML4* table_ptr)
{
    __asm__("mov %0, %%cr3\n\t" : : "r" (table_ptr));
}


void handover_paging(PML4* pml4)
{
    pagingMan.PML4Table = (PageTable*)pml4;
    return;
}


// will be removed soon - VMM will return the PML4 of the currently used address space.
// task switches will cause address space changes.
PML4* getKernelCR3()
{
    return (PML4*)pagingMan.PML4Table;
}


/* 
    Legacy PML4 Initialization Function:
    ==============================================================================================
    void init_paging(void* kernelStart, uint64_t ksize, void* framebufferStart, uint64_t fbSize)
    {
        // init paging (identity mapping for now, probably gonna go for higher half kernel)
        pagingMan = (TableManager){ (PageTable*)pfa_alloc_page() };
        memset(pagingMan.PML4Table, 0x00, PAGE_SIZE);


        // map the whole address space. (512 MiB, Or the amount specified to QEMU)
        uint64_t e = ppow2(PAGE_SIZE * totalDetectedMemory());
        for(uint64_t i = 0; i < e; i += PAGE_SIZE) { 
            mapVirtualToPhysical((PML4*)pagingMan.PML4Table, (void*)(i), (void*)i);
        }


        // map the framebuffer
        uint64_t start = (uint64_t)framebufferStart;
        uint64_t end   = start + fbSize + (PAGE_SIZE * ((fbSize & 0xfff) != 0));   
        for(; start < end; start += PAGE_SIZE) {
            mapVirtualToPhysical((PML4*)pagingMan.PML4Table, (void*)(start), (void*)start);
        }

        
        load_cr3_reg((PML4*)pagingMan.PML4Table); // load the page table
    }
    ==============================================================================================
    
*/