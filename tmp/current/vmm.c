#include "vmm.h"
#include "../phys/pmm.h"
#include "../../shared/percpu.h"



static vmm_t virtualMemoryManager;




void vmm_init()
{
    virtualMemoryManager.listAlloc = create_vas_man_t_page_list();
    virtualMemoryManager.totalMem  = 4096; // the allocator uses at first only 1 page.
    virtualMemoryManager.usedMem   = 0;
}


vas_man_t* vmm_create_address_space(uint16_t procid)
{
    vas_man_ptr_t out = alloc_vas_man_t_node(virtualMemoryManager.listAlloc);
    vas_init(out, nullptr);
    return out;
}


void vmm_destroy_address_space(vas_man_t* manager)
{
    if(!manager) return;
    virtualMemoryManager.usedMem -= ( manager->tablePages + manager->alloc->totalPages );
    // free manager allocator && PML4 Table.
    free_vas_man_t_node(virtualMemoryManager.listAlloc, manager);
}


void vmm_switch_address_space(vas_man_t* target)
{

}


