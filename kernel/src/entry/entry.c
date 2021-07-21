#include "entry.h"



// physical_address + virtualOffset = virtual_address (FOR ALL MEMORY!)
uint64_t         __mem_virtual_offset __attribute__((section(".data"))); 
kernel_header_t* __kheader            __attribute__((section(".data")));