#include "mem/virt/types2.h"
#include <mem/virt/paging.h>
#include <mem/phys/pmm.h>
#include <mem/map_ptr.h>
#include <std/string.h>


static TableManager pagingMan;


// Legacy, Should be removed & NOT USED!
// void mapVirtualToPhysical(PML4* Table, void* vaddress, void* paddress) {
//     genericPageTable*      table = (genericPageTable*)VIRT(Table);
//     genericPageTableEntry* ent   = NULLPTR_TYPE(genericPageTableEntry); 
//     uint64_t    address    = (uint64_t)vaddress;
//     uint64_t    indices[4] = {0};
//     // [ no execute (1) | ignored (15) | page directory pointer (9) | page directory (9) | page table (9) | page (9) | byte offset (12)]
//     // ^ format of a virtual address (64bits - uint64_t)
//     address >>= 12; indices[0] = address & 0x1ff; // The index for the page in the genericPageTable                                    [ genericPageTable[i]                 ]
//     address >>= 9;  indices[1] = address & 0x1ff; // The index for the page table in the Page Directory table                   [ PageDirectory[i]             ]
//     address >>= 9;  indices[2] = address & 0x1ff; // the index for the page directory table in the page directory pointer table [ PageDirectoryPointerTable[i] ]
//     address >>= 9;  indices[3] = address & 0x1ff; // the index for the page directory pointer table in the PML4 table.          [ PML4[i]                      ]


//    for(uint8_t i = 0; i < 3u; ++i)
//     {
//         ent = &table->entry[indices[3 - i]]; // get current entry
//         if(!ent->present)
//         {
//             table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), genericPageTable*); // update table for next lvl.
//             memset((void*)table, 0x00, PAGE_SIZE);
//             ent->address = (uint64_t)PHYS(table) >> 12; // this address needs to be physical in the table
//             ent->present = BOOLEAN_TRUE;
//             ent->rw      = BOOLEAN_TRUE;
//         }
//         else {
//             table = VIRT_TYPE(ent->address << 12, genericPageTable*); // update table for next lvl
//         }
//         // printk("lvl %u, ent: %X\n", 3 - i, ent->ui64);
//     }
    
//     ent = &table->entry[indices[0]];
//     // printk("ent before: %X\n", ent->ui64);
//     ent->address     = (uint64_t)paddress >> 12;
//     ent->present     = BOOLEAN_TRUE;
//     ent->userOrSuper = BOOLEAN_FALSE;
//     ent->rw          = BOOLEAN_TRUE;
//     // printk("ent after:  %X\n", ent->ui64);
// }


// void map_vtop(PML4* Table, void* vaddress, void* paddress, uint64_t flags)
// {
//     genericPageTable*  table      = (genericPageTable*)VIRT(Table);
//     genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
//     uint64_t    address    = (uint64_t)vaddress;
//     uint64_t    indices[4] = {0};
//     address >>= 12; indices[0] = address & 0x1ff;
//     address >>= 9;  indices[1] = address & 0x1ff;
//     address >>= 9;  indices[2] = address & 0x1ff;
//     address >>= 9;  indices[3] = address & 0x1ff;
//     flags = sanitize_flags(flags);
//     // printk("indices: %u %u %u %u\n", indices[0], indices[1], indices[2], indices[3]);

//    for(uint8_t i = 0; i < 3u; ++i)
//     {
//         ent = &table->entry[indices[3 - i]];
//         if(!ent->present)
//         {
//             table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), genericPageTable*);
//             // printk("allocated at %X\n", PHYS(table));
//             memset((void*)table, 0x00, PAGE_SIZE);
//             ent->ui64    = flags;                       // set 1:1 flags;
//             ent->address = (uint64_t)PHYS(table) >> 12; // set address.
//         }
//         else {
//             table = VIRT_TYPE(ent->address << 12, genericPageTable*); // update table for next lvl
//             // printk("available at %X\n", PHYS(table));
//         }
//     }
    

//     ent          = &table->entry[indices[0]];
//     ent->ui64    = flags;                    // set 1:1 flags;
//     ent->address = (uint64_t)paddress >> 12; // set physical address/
//     // printk("%p\n", ent->ui64);
// }


// void unmapVirtual(PML4* Table, void* vaddress)
// {
//     genericPageTable*  table      = (genericPageTable*)VIRT(Table);
//     genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
//     uint64_t    address    = (uint64_t)vaddress;
//     uint64_t    indices[4] = {0};
//     address >>= 12; indices[0] = address & 0x1ff;
//     address >>= 9;  indices[1] = address & 0x1ff;
//     address >>= 9;  indices[2] = address & 0x1ff;
//     address >>= 9;  indices[3] = address & 0x1ff;
    

//     ent = &table->entry[indices[3]]; 
//     for(uint8_t i = 1; ent && i < 3u; ++i)
//     {
//         table = VIRT_TYPE(ent->address << 12, genericPageTable*);
//         ent   = &table->entry[indices[3 - i]];  
//     }
//     if(ent == NULLPTR) {
//         return;
//     }


//     ent = &table->entry[indices[0]];
//     ent->present = BOOLEAN_FALSE;
//     return;
// }




void map_vtop(PML4* Table, void* vaddress, void* paddress, uint64_t flags)
{
    genericPageTable*      table = (genericPageTable*)VIRT(Table);
    genericPageTableEntry* ent   = NULLPTR_TYPE(genericPageTableEntry); 
    uint64_t    address    = (uint64_t)vaddress;
    uint64_t    indices[4] = {0};
    address >>= 12; indices[0] = address & 0x1ff;
    address >>= 9;  indices[1] = address & 0x1ff;
    address >>= 9;  indices[2] = address & 0x1ff;
    address >>= 9;  indices[3] = address & 0x1ff;
    flags = sanitize_flags(flags);
    // printk("indices: %u %u %u %u\n", indices[0], indices[1], indices[2], indices[3]);

    for(uint8_t i = 0; i < 3u; ++i)
    {
        ent = &table->entry[indices[3 - i]];
        if(!VMM_TABLE_ENTRY_GET_PRESENT_FLAG(*ent))
        {
            table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), genericPageTable*);
            // printk("allocated at %X\n", PHYS(table));
            memset((void*)table, 0x00, PAGE_SIZE);
            *ent = flags;                                                           // set 1:1 flags;
            *ent = VMM_TABLE_ENTRY_SET_ADDRESS(*ent, (uint64_t)PHYS(table) >> 12 ); // set address.
        }
        else {
            table = VIRT_TYPE( VMM_TABLE_ENTRY_GET_ADDRESS(*ent) << 12, genericPageTable*); // update table for next lvl
            // printk("available at %X\n", PHYS(table));
        }
    }
    

    ent = &table->entry[indices[0]];
    *ent = flags; // set 1:1 flags;
    *ent = VMM_TABLE_ENTRY_SET_ADDRESS(*ent, (uint64_t)paddress >> 12 ); // set physical address
    // printk("%p\n", ent->ui64);
}


void unmapVirtual(PML4* Table, void* vaddress)
{
    genericPageTable*  table      = (genericPageTable*)VIRT(Table);
    genericPageTableEntry* ent        = NULLPTR_TYPE(genericPageTableEntry); 
    uint64_t    address    = (uint64_t)vaddress;
    uint64_t    indices[4] = {0};
    address >>= 12; indices[0] = address & 0x1ff;
    address >>= 9;  indices[1] = address & 0x1ff;
    address >>= 9;  indices[2] = address & 0x1ff;
    address >>= 9;  indices[3] = address & 0x1ff;
    

    ent = &table->entry[indices[3]]; 
    for(uint8_t i = 1; ent && i < 3u; ++i)
    {
        table = VIRT_TYPE(VMM_TABLE_ENTRY_GET_ADDRESS(*ent) << 12, genericPageTable*);
        ent   = &table->entry[indices[3 - i]];  
    }
    if(ent == NULLPTR) {
        return;
    }


    ent = &table->entry[indices[0]];
    *ent = VMM_TABLE_ENTRY_SET_PRESENT_FLAG(*ent, BOOLEAN_FALSE);
    return;
}




void load_cr3_reg(PML4* table_ptr)
{
    __asm__("mov %0, %%cr3\n\t" : : "r" (table_ptr));
}


void handover_paging(PML4* pml4)
{
    pagingMan.PML4Table = (genericPageTable*)pml4;
    return;
}


// will be removed soon - VMM will return the PML4 of the currently used address space.
// task switches will cause address space changes.
PML4* getKernelCR3()
{
    return (PML4*)pagingMan.PML4Table;
}


PML4* getCurrentCR3()
{
    uint64_t out;
    __asm__ volatile("mov %%cr3, %0" : "=r"(out)); 
    return (PML4*)out;
}


/* 
    Legacy PML4 Initialization Function:
    ==============================================================================================
    void init_paging(void* kernelStart, uint64_t ksize, void* framebufferStart, uint64_t fbSize)
    {
        // init paging (identity mapping for now, probably gonna go for higher half kernel)
        pagingMan = (TableManager){ (genericPageTable*)pfa_alloc_page() };
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