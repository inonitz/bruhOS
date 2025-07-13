#include <mem/virt/vas_node_alloc.h>
#include <mem/phys/pmm.h>
#include <mem/map_ptr.h>

#include <std/string.h>
#include <std/io.h>


#define NODELIST_CAST(pointer) ((vas_nodelist_t*)(pointer))




vas_nodelist_t* create_node_list()
{
    vas_nodelist_t* head = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), vas_nodelist_t*);
    head->nextPage   = NULLSTR;
    head->first      = &head->rest[0];
    head->free       = head->first;
    head->totalNodes = 63u;
    head->usedNodes  = 0;
    head->totalPages = 1; // the first page in which we manage ourselves counts.

    for(uint8_t i = 1; i < 63; ++i) {
        head->all[i].nextNode = &head->all[i + 1]; 
    }
    head->all[63].nextNode = NULLPTR_TYPE(struct nodew_t);

    return head;
}


void destroy_node_list(vas_nodelist_t* head)
{
    for(char_t* pageAtIdx = head->nextPage; 
        pageAtIdx != NULLPTR; 
        pageAtIdx = ((vas_nodelist_t*)pageAtIdx)->nextPage
    ) {
        pfa_free_page(PHYS_TYPE(pageAtIdx, void*));
    }
    pfa_free_page(PHYS_TYPE(head, void*));
    return;
}





vas_node_t* alloc_node(vas_nodelist_t* head)
{
    vas_node_t* out = &head->free->node;

    head->free = head->free->nextNode;
    ++head->usedNodes;

    // if we still have available nodes, we don't need a new page.
    if(head->usedNodes < head->totalNodes) {
        return out;
    }


    // we have no nodes left:
    //  if we didn't expand already:
    //      ...
    //  else (we already expanded atleast once):
    //      ...
    if(head->nextPage == NULLPTR)
    {
        head->nextPage = head->lastPage = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), char_t*);
    } else { 
        NODELIST_CAST(head->lastPage)->nextPage = VIRT_TYPE(pfa_alloc_page(DMA_NORMAL), char_t*);
        head->lastPage = NODELIST_CAST(head->lastPage)->nextPage;
    }

    // initialize the new nodelist
    memset(
        NODELIST_CAST(head->lastPage)->rest, 
        0x00, 
        sizeof(vas_nodelist_t)
    );
    for(uint8_t i = 0; i < 63; ++i) {
        NODELIST_CAST(head->lastPage)->rest[i].nextNode = &NODELIST_CAST(head->lastPage)->rest[i + 1]; 
    }
    NODELIST_CAST(head->lastPage)->rest[62].nextNode = NULLPTR_TYPE(struct nodew_t);


    // because usedNodes == totalNodes, this means that head->free is NULLPTR.
    // because we allocated new nodes, we simply link them to head->free. 
    head->free        = &NODELIST_CAST(head->lastPage)->rest[0];
    head->totalNodes += 63; // (page_size(4KiB) - 8bytes) / 64bytes
    ++head->totalPages;

    return out;
}


void free_node(vas_nodelist_t* head, vas_node_t* node)
{
    if(unlikely(head->usedNodes == 0))
        return;

    nodew_t* actual_node  = (nodew_t*)node;
    actual_node->nextNode = head->free;
    head->free            = actual_node;
    --head->usedNodes;

    return;
}


void vas_nodelist_info(vas_nodelist_t* head)
{
    printk("Nodelist Allocator Info:\nTotal Nodes managed: %u\nTotal Nodes Allocated: %u\nTotal Amount of Pages Used: %u\nMemory Consumption (Used/Total): %u/%u\n",
        head->totalNodes, 
        head->usedNodes,
        head->totalPages,
        sizeof(nodew_t) * head->usedNodes,
        sizeof(nodew_t) * head->totalNodes
    );
}


