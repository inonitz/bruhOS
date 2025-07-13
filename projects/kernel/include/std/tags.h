#ifndef __KERNEL_C_DEFINITION_VARIABLE_TAGS__
#define __KERNEL_C_DEFINITION_VARIABLE_TAGS__
#include <std/compiler_warning.h>


#ifdef IN
#   pragma WARN("Macro 'IN' already defined from somewhere else!")
#endif
#ifdef OUT
#   pragma WARN("Macro 'OUT' already defined from somewhere else!")
#endif
#ifdef OPTIONAL
#   pragma WARN("Macro 'OPTIONAL' already defined from somewhere else!")
#endif
#ifdef logical
#   pragma WARN("Macro 'logical' already defined from somewhere else!")
#endif
#ifdef physical
#   pragma WARN("Macro 'physical' already defined from somewhere else!")
#endif
#ifdef virtual
#   pragma WARN("Macro 'virtual' already defined from somewhere else!")
#endif
#define IN 
#define OUT
#define OPTIONAL
#define logical  // the address is in the virtual address space and is at physical_address + known_offset.
#define physical // the address is not in the virtual address space.
#define virtual  // the address is in the virtual address space in a mapping that is not logical, probe the PML4 Table to find the physical address.


#endif /* __KERNEL_C_DEFINITION_VARIABLE_TAGS__ */