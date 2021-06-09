#include "slab.h"


typedef enum {
    MEM_PHYSICAL = 0, 
    MEM_VIRTUAL  = 1
} 
SLAB_ALLOCATOR_MANAGEMENT_STATE;


static bool_t slab_state = MEM_PHYSICAL; 