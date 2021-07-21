#include "vas.h"
#include "../phys/pmm.h"
#include "../map_ptr.h"



void vas_init(vas_man_t* man, PML4* paging_table)
{
    if(man == nullptr) return;
    if(paging_table == nullptr) {
        paging_table = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), PML4*);
        memset(man->table, 0x00, sizeof(*man->table));
    }

    man->table = paging_table;
    man->alloc = create_node_list();  
    man->root  = alloc_node(man->alloc);

    return;
}


void* vas_alloc(vas_man_t* vas, void* address, uint32_t pages, paging_flags_t flags)
{
    if(unlikely(vas == nullptr)) return nullptr;

    vas_node_t* new = alloc_node(vas->alloc);
    vas_node_t* tmp;
    new->record = (vm_region_t){
        .address = (uint64_t)address,
        .extent  = pages,
        .wrk_set = 0,
        .flags   = flags
    };

    tmp = insert_node(&vas->root, new); // insert the allocation node.
    if(tmp) {
        free_node(vas->alloc, tmp);
        return nullptr;
    }

    toVirtualFlag(address, pages, flags.qword.u64); // map the allocation to the virtual address space.
    return address;
}


void vas_free(vas_man_t* vas, void* ptr)
{
    if(unlikely(vas == nullptr)) return;
    
    vas_node_t* to_del = lookup_by_address(vas->root, (uint64_t)ptr);
    if(!to_del) // if ptr doesn't exist in the vas_manager then that's sus - probably just exit the program with a fault.
        return; 
    
    remove_node(&vas->root, to_del);       // remove the node from the tree.
    unVirtual(ptr, to_del->record.extent); // unmap the region owned by the node.
    free_node(vas->alloc, to_del);         // free the region node, 'precise-lier': recycle it.

    // TODO: 
    // Pages that aren't global (Meaning, allocated from user space) 
    // need to be TLB Shotdown across ALL CPUS. (IPI to all CPU's to do invlpg)
    // This is (As far as I can see) the only thing that is missing here.
    return;
}