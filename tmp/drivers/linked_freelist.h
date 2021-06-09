#pragma once
#include "../std/io.h"


// Define the variable below with a working page allocator for this to work.
// use this:
// #undef PAGE_ALLOC_FUNC
// #define PAGE_ALLOCATOR_FREE(arg)
// #define PAGE_ALLOC_FUNC 'your_alloc_here'
// #define PAGE_ALLOCATOR_FREE(arg) 'your_free_function'
// DEFINE_NODELIST_...
// ...
#define PAGE_ALLOCATOR_ALLOC
#define PAGE_ALLOCATOR_FREE(arg)


#define DEFINE_NODE_STRUCT(NODE_INTENAL_DATA_TYPE, NODE_TYPENAME, ALIGNMENT) \
    typedef struct align(ALIGNMENT) NODE_TYPENAME \
    { \
        NODE_INTENAL_DATA_TYPE data; \
        struct NODE_TYPENAME*  nextNode; \
    } NODE_TYPENAME; \



#define DEFINE_NODELIST_STRUCT(NODE_TYPENAME) \
    /* the Head structure uses 64 bytes, while the rest of the pages use only 8 bytes. */ \
    /* (In practice, 64 Bytes are always wasted) */ \
    typedef struct NODE_TYPENAME##_list_t \
    { \
        struct align(PAGE_SIZE) { \
            struct alignpack(64) { \
                char_t*        nextPage; /*   the allocator is managed inside a page. when this page is full, we'll allocate a new page and link it to this one */ \
                char_t*        lastPage; /*   the last page allocated for new objects.              */ \
                NODE_TYPENAME* first; /*      the first object in the freelist                      */ \
                NODE_TYPENAME* free; /*       the first free object in the freelist                 */ \
                uint32_t       totalNodes; /* used with usedNodes to measure if we need more pages. */ \
                uint32_t       usedNodes; /*  ^^^^                                                  */ \
                uint32_t       totalPages; /* total amount of pages allocated for the allocator.    */ \
            }; \
            /* the nodes available in this page. */ \
            NODE_TYPENAME      data[(PAGE_SIZE - 64) / (8 + sizeof(NODE_TYPENAME))]; \
        }; \
    } NODE_TYPENAME##_list_t; \


#define NODELIST_CAST(pointer, TYPENAME) ((TYPENAME*)(pointer))


#define DEFINE_NODELIST_FUNCTIONS(NODE_TYPENAME) \
    static __force_inline NODE_TYPENAME##_list_t* create_##NODE_TYPENAME##_node_list() \
    { \
        NODE_TYPENAME##_list_t* head = VIRT_TYPE(PAGE_ALLOCATOR_ALLOC, NODE_TYPENAME##_list_t*); \
        head->nextPage   = nullptr; \
        head->first      = &head->rest[0]; \
        head->free       = head->first; \
        head->totalNodes = (PAGE_SIZE - 64) / (8 + sizeof(NODE_TYPENAME)); \
        head->usedNodes  = 0; \
        head->totalPages = 1; /* the first page in which we manage ourselves counts. */ \
\
\
        for(uint8_t i = 0; i < ( (PAGE_SIZE - 64) / (8 + sizeof(NODE_TYPENAME)) ) - 1; ++i) { \
            head->data[i].nextNode = &head->data[i + 1]; \
        } \
        head->data[63].nextNode = nullptr; \
        return head; \
    } \
\
\
    static __force_inline void destroy_##NODE_TYPENAME##_node_list( \
        NODE_TYPENAME##_list_t* head \
    ) { \
        for(char_t* pageAtIdx = head->nextPage; \
            pageAtIdx != nullptr; \
            pageAtIdx = ((NODE_TYPENAME##_list_t*)pageAtIdx)->nextPage \
        ) { \
            PAGE_ALLOCATOR_FREE(PHYS_TYPE(pageAtIdx, void*)) \
        } \
        PAGE_ALLOCATOR_FREE(PHYS_TYPE(head, void*)) \
        return; \
    } \
\
\
    NODE_TYPENAME* alloc_node(NODE_TYPENAME##_list_t* head)
    {
        NODE_TYPENAME* out = &head->free->node;

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
        if(head->nextPage == nullptr)
        {
            head->nextPage = head->lastPage = VIRT_TYPE(pfa_alloc_page(), char_t*);
        } else { 
            NODELIST_CAST(head->lastPage)->nextPage = VIRT_TYPE(pfa_alloc_page(), char_t*);
            head->lastPage = NODELIST_CAST(head->lastPage)->nextPage;
        }

        // initialize the new nodelist
        memset(head->lastPage, 0x00, sizeof(NODE_TYPENAME##_list_t));
        for(uint8_t i = 1; i < 63; ++i) {
            NODELIST_CAST(head->lastPage)->all[i].nextNode = &NODELIST_CAST(head->lastPage)->all[i + 1]; 
        }
        NODELIST_CAST(head->lastPage)->all[63].nextNode = nullptr;


        // because usedNodes == totalNodes, this means that head->free is nullptr.
        // because we allocated new nodes, we simply link them to head->free. 
        head->free        = &NODELIST_CAST(head->lastPage)->rest[0];
        head->totalNodes += 63; // (page_size(4KiB) - 8bytes) / 64bytes
        ++head->totalPages;

        return out;
    }


    void free_node(NODE_TYPENAME##_list_t* head, NODE_TYPENAME* node)
    {
        if(unlikely(head->usedNodes == 0))
            return;

        nodew_t* actual_node  = (nodew_t*)node;
        actual_node->nextNode = head->free;
        head->free            = actual_node;
        --head->usedNodes;

        return;
    }


void vas_nodelist_info(NODE_TYPENAME##_list_t* head)
{
    printk("Nodelist Allocator Info:\nTotal Nodes managed: %u\nTotal Nodes Allocated: %u\nTotal Amount of Pages Used: %u\nMemory Consumption (Used/Total): %u/%u\n",
        head->totalNodes, 
        head->usedNodes,
        head->totalPages,
        sizeof(nodew_t) * head->usedNodes,
        sizeof(nodew_t) * head->totalNodes
    );
}