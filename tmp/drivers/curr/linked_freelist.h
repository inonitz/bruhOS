#pragma once
#ifndef __LINKED_PAGE_FREELIST_ALLOCATOR__
#define __LINKED_PAGE_FREELIST_ALLOCATOR__
#include "../std/io.h"
#include "../mem/phys/pmm.h"
#include "../mem/map_ptr.h"



typedef int typename_t;


typedef struct typename_t_node_t
{
    typename_t         data;
    typename_t_node_t* nextNode;
} typename_t_node_t;


typedef struct __allocation_page
{
    struct __allocation_page* nextPage;
    typename_t_node_t         data[4088 / sizeof(typename_t_node_t)];
} __allocation_page;


typedef struct __linked_list_of_pages
{
    __allocation_page* pagelist;   // the list of pages that were allocated.
    __allocation_page* lastPage;   // the last allocated page for objects.
    typename_t_node_t* first;      // the first object in the freelist
    typename_t_node_t* free;       // the first free object in the freelist
    uint32_t           totalNodes; // used with usedNodes to measure if we need more pages.
    uint32_t           usedNodes;  // ^^^^
    uint32_t           totalPages; // total amount of pages allocated for the allocator.
} linked_freelist_t;


void init_linked_freelist(linked_freelist_t* alloc)
{
    alloc->pagelist = alloc->lastPage = VIRT_TYPE(pfa_alloc_page(), __allocation_page*);
    memset(alloc->pagelist, 0x00, PAGE_SIZE);

    uint16_t objsPerPage = 4088 / sizeof(typename_t_node_t);
    alloc->first         = &alloc->pagelist->data[0];
    alloc->free          = alloc->first;
    alloc->totalNodes    = objsPerPage;
    alloc->usedNodes     = 0;
    alloc->totalPages    = 1; // the first page in which we manage ourselves counts.

    for(uint16_t i = 0; i < objsPerPage; ++i) {
        alloc->lastPage->data[i].nextNode = &alloc->lastPage->data[i+1];
    }
   alloc->lastPage->data[objsPerPage].nextNode = (typename_t_node_t*)nullptr;
}


void destroy_linked_freelist(linked_freelist_t* alloc)
{
    for(__allocation_page* pageAtIdx = alloc->pagelist; 
        pageAtIdx != nullptr; 
        pageAtIdx = alloc->pagelist->nextPage
    ) {
        pfa_free_page(PHYS_TYPE(pageAtIdx, void*));
    }
    return;
}


typename_t* alloc_obj_lfl(linked_freelist_t* alloc)
{
    
}


void free_obj_lfl(linked_freelist_t* alloc)
{

}


/* 
*/

#endif