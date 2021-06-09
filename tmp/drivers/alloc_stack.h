#pragma once
#ifndef __ALLOCATOR_STACK_WRAP__
#define __ALLOCATOR_STACK_WRAP__
#include "../../ds/finite_stack.h"



#define ALLOCATOR_STACK_ELEMENTS 128
typedef alloc_list_t* fs_data;


DEFINE_FINITE_STACK_INTERFACE(alloc_stack_t, fs_data, 128, allocs)


typedef struct __percpu_finite_physical_allocator_stack
{
    alloc_stack_t stack;
    uint64_t      active;
} pmm_stack_t;


static __force_inline void alloc_stack_push(pmm_stack_t* man, fs_data* data)
{
    push_fs_data_allocs((finite_stack_fs_data_128_t*)man, data);
    ++man->active;
}


static __force_inline fs_data alloc_stack_pop(pmm_stack_t* man)
{
    fs_data out = pop_fs_data_allocs((finite_stack_fs_data_128_t*)man);
    man->active -= (out != nullptr);
    return out;
}


static __force_inline fs_data alloc_stack_top(pmm_stack_t* man)
{
    return top_fs_data_allocs((finite_stack_fs_data_128_t*)man);
}


static __force_inline void alloc_stack_info(pmm_stack_t* man) {
    printk("Finite Stack, T = alloc_list*. Info:\nActive Used: %u\nStack Size (Elements):\n%u Stack Element Size (bytes): %x\n\n",
        man->active,
        ALLOCATOR_STACK_ELEMENTS,
        sizeof(fs_data)
    );
}

#endif