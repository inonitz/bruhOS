#pragma once
#ifndef __VAS_PHYSICAL_ALLOCATOR__
#define __VAS_PHYSICAL_ALLOCATOR__
#include "../../std/debug.h"
#include "avl/avl_wrap.h"



// vas_node_alloc_t == __virtual_address_space_avl_tree_node_allocator_type
// The memory allocated from this allocator only gets freed once
// the process that is bound to this address space is freed.
typedef struct alignpack(64) nodew_t
{
    vas_node_t      node;
    struct nodew_t* nextNode;
} nodew_t;


typedef struct vas_nodelist_t
{
    union {
        
        struct alignpack(64) {
            // first 64 bytes - metadata. list_head uses all 64bytes, the rest of the pages use only 8bytes.
            struct alignpack(64) {
                char_t*   nextPage;    // the vas_nodelist_t is managed inside a page. when this page is full, we'll allocate a new page and link it to this one
                char_t*   lastPage;    // the last page allocated for new objects.
                nodew_t*  first;       // the first object in the freelist
                nodew_t*  free;        // the first free object in the freelist
                uint32_t  totalNodes;  // used with usedNodes to measure if we need more pages.
                uint32_t  usedNodes;   // ^^^^
                uint32_t  totalPages;  // total amount of pages allocated for the allocator.
            };
            // rest of the page.
            nodew_t       rest[63];    // the rest of the nodes available in this page.
        };
        struct alignpack(64) {
            nodew_t all[64];
        };

    };
} vas_nodelist_t;




/* 
    * Creates the list head for the vas_nodelist allocator.
    * returns the starting address of the list_head.
*/
vas_nodelist_t* create_node_list();


/* 
    * frees all the pages that were allocated in the vas_nodelist allocator,
    * including the list_head.
    * vas_nodelist_t* head - the head of the node allocator.
*/
void            destroy_node_list(vas_nodelist_t* head);


/*
    * allocate a single vas_node_t from the node allocator.
    * vas_nodelist_t* head - the head of the allocator, from which the node will be allocated.
*/
vas_node_t*     alloc_node       (vas_nodelist_t* head);


/* 
    * free a single vas_node_t back to the node allocator.
    * vas_nodelist_t* head - the head of the allocator, from which the node was previously allocated.
    * vas_node_t*     node - the node that will be returned to the node allocator.
*/
void            free_node        (vas_nodelist_t* head, vas_node_t* node);


/* 
    * prints info about the node allocator, currently held by the current virtual address space
    * of the process.
    * vas_nodelist_t* head - the head of the allocator
*/
void            vas_nodelist_info(vas_nodelist_t* head);

#endif


