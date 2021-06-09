#pragma once
#ifndef __PAGELINKED_LIST_BUFFER__
#define __PAGELINKED_LIST_BUFFER__
#include "../std/io.h"
#include "../mem/phys/pmm.h"
#include "../mem/map_ptr.h"



#define NODELIST_CAST(pointer, TYPENAME) ((linked_##TYPENAME##_pages_t*)(pointer))

#define DEFINE_LINKED_PAGES_OF_OBJECTS_INTERFACE(TYPENAME) \
    typedef struct TYPENAME##_node_t \
    { \
        TYPENAME                  data; \
        struct TYPENAME##_node_t* nextNode; \
    } TYPENAME##_node_t; \
    \
    \
    \
    \
    typedef struct __page_granular_freelist_##TYPENAME##_node_t \
    { \
        union { \
        \
        \
            struct { \
                struct align(64) { /* Total Size is 44 bytes */ \
                    char_t*            nextPage; \
                    char_t*            lastPage; \
                    TYPENAME##_node_t* first; \
                    TYPENAME##_node_t* free; \
                    uint32_t           totalNodes; \
                    uint32_t           usedNodes; \
                    uint32_t           totalPages; \
                    uint8_t            padding[20]; \
                } header; \
                TYPENAME##_node_t Elems[( PAGE_SIZE - 64 ) / sizeof(TYPENAME##_node_t)]; \
            } asHead; \
        \
        \
            struct { \
                /* When we are one of the page-nodes, we only contain a pointer to the next page-node and objects for allocation. */ \
                char_t*           nextPage; \
                TYPENAME##_node_t Elems[4088 / sizeof(TYPENAME##_node_t)]; \
            } asNode; \
        \
        \
            uint8_t asPage[4096]; \
        }; \
    } linked_##TYPENAME##_pages_t; \
    \
    \
    \
    \
    static __force_inline linked_##TYPENAME##_pages_t* create_##TYPENAME##_page_list() \
    { \
        linked_##TYPENAME##_pages_t* head = VIRT_TYPE(pfa_alloc_page(), linked_##TYPENAME##_pages_t*); \
        head->asHead.header.nextPage      = nullptr; \
        head->asHead.header.first         = &head->asHead.Elems[0]; \
        head->asHead.header.free          = head->asHead.header.first; \
        head->asHead.header.totalNodes    = ( PAGE_SIZE - sizeof(head->asHead.header) ) / sizeof(TYPENAME##_node_t); \
        head->asHead.header.usedNodes     = 0; \
        head->asHead.header.totalPages    = 1; \
        return head; \
    } \
    \
    \
    \
    \
    static __force_inline void destroy_##TYPENAME##_page_list(linked_##TYPENAME##_pages_t* head) \
    { \
        for(char_t* pageAtIdx = head->asHead.header.nextPage; \
            pageAtIdx != (char_t*)nullptr; \
            pageAtIdx = ((linked_##TYPENAME##_pages_t*)pageAtIdx)->asNode.nextPage \
        ) { \
            pfa_free_page(PHYS_TYPE(pageAtIdx, void*)); \
        } \
        pfa_free_page(PHYS_TYPE(head, void*)); \
        return; \
    } \
    \
    \
    \
    \
    static __force_inline TYPENAME* alloc_##TYPENAME##_node(linked_##TYPENAME##_pages_t* head) \
    { \
        TYPENAME* out = &head->asHead.header.free->data; \
        \
        head->asHead.header.free = head->asHead.header.free->nextNode; \
        ++head->asHead.header.usedNodes; \
        \
        \
        /* if we still have available nodes, we don't need a new page. */ \
        if(head->asHead.header.usedNodes < head->asHead.header.totalNodes) { \
            return out; \
        } \
        \
        \
        \
        \
        /* we have no nodes left:                    */ \
        /*  if we didn't expand already:             */ \
        /*      ...                                  */ \
        /*  else (we already expanded atleast once): */ \
        /*      ...                                  */ \
        if(head->asHead.header.nextPage == nullptr) \
        { \
            head->asHead.header.nextPage = head->asHead.header.lastPage = VIRT_TYPE(pfa_alloc_page(), char_t*); \
            \
        } else { \
            NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.nextPage = VIRT_TYPE(pfa_alloc_page(), char_t*); \
            head->asHead.header.lastPage = NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.nextPage; \
        } \
        \
        \
        /* initialize the new nodelist */ \
        memset(head->asNode.Elems, 0x00, sizeof(head->asNode.Elems)); \
        for(uint16_t i = 0; i < 4088 / sizeof(TYPENAME##_node_t); ++i) { \
            NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.Elems[i].nextNode = &NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.Elems[i+1]; \
        } \
        NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.Elems[ ( 4088 / sizeof(TYPENAME##_node_t) ) - 1].nextNode = nullptr; \
        \
        \
        /* because usedNodes == totalNodes, this means that head->free is nullptr. */ \
        /* because we allocated new nodes, we simply link them to head->free.      */ \
        head->asHead.header.free        = &NODELIST_CAST(head->asHead.header.lastPage, TYPENAME)->asNode.Elems[0]; \
        head->asHead.header.totalNodes += ( 4088 / sizeof(TYPENAME##_node_t) ); /* (page_size(4KiB) - 8bytes) / 64bytes */ \
        ++head->asHead.header.totalPages; \
        \
        \
        return out; \
    } \
    \
    \
    \
    \
    static __force_inline void free_##TYPENAME##_node( \
        linked_##TYPENAME##_pages_t* head, \
        TYPENAME*                    node  \
    ) { \
        if(unlikely(head->asHead.header.usedNodes == 0)) \
            return; \
        \
        TYPENAME##_node_t* actual_node = (TYPENAME##_node_t*)node; \
        actual_node->nextNode          = head->asHead.header.free; \
        head->asHead.header.free       = actual_node; \
        --head->asHead.header.usedNodes; \
        return; \
    }

#endif