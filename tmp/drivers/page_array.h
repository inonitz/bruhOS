#pragma once
#ifndef __PAGE_GRANULAR_ARRAY__
#define __PAGE_GRANULAR_ARRAY__
#include "../mem/phys/pmm.h"
#include "../mem/map_ptr.h"



#define DEFINE_PAGE_GRANULAR_ARRAY(TYPENAME) \
    typedef struct __page_granular_array_##TYPENAME \
    { \
        union { \
        \
        \
            struct pack { \
                struct pack { \
                    char_t*  nextPage; \
                    char_t*  lastPage; \
                    uint64_t totalElems; \
                    uint64_t usedElems; \
                    uint32_t totalPages; \
                } header; \
                TYPENAME     Elems[4088 / sizeof(TYPENAME)]; \
                /* to get the start of the Elems Array: In the non-head pages, its a normal array access. Inside the Head*/ \
            } asHead; \
        \
        \
            struct pack { \
                char_t*  nextPage; \
                TYPENAME Elems[4088 / sizeof(TYPENAME)]; \
            } asNode; \
        \
        \
            uint8_t asPage[4096]; \
        }; \
    } linked_##TYPENAME##_buffer_t; \
    \
    \
    \
    \
    linked_##TYPENAME##_buffer_t* create_linked_##TYPENAME##_buffer_t() \
    { \
        linked_##TYPENAME##_buffer_t* head = VIRT_TYPE(pfa_alloc_page(), linked_##TYPENAME##_buffer_t*); \
        head->nextPage = nullptr; \
        head->totalElems = 
    }





#endif