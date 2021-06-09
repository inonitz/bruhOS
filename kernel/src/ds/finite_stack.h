#pragma once
#ifndef __KERNEL_FINITE_STACK__
#define __KERNEL_FINITE_STACK__
#include "../std/io.h"



#define DEFINE_FINITE_STACK_DATA_STRUCT(STRUCTNAME, TYPENAME, ELEM_COUNT, DATA_NAME) \
typedef struct __finite_stack_structure_##TYPENAME##ELEM_COUNT \
{ \
    uint64_t  rsp; \
    TYPENAME DATA_NAME[ELEM_COUNT]; \
} finite_stack_##TYPENAME##_##ELEM_COUNT##_t; \
typedef finite_stack_##TYPENAME##_##ELEM_COUNT##_t STRUCTNAME; \




#define DEFINE_FINITE_STACK_PUSH_TYPE(TYPENAME, ELEM_COUNT, DATA_NAME) \
static __force_inline void push_##TYPENAME##_##DATA_NAME( \
    finite_stack_##TYPENAME##_##ELEM_COUNT##_t* stack, \
    TYPENAME*                                   data   \
) { \
    if(unlikely(stack->rsp == (uint64_t)(ELEM_COUNT - 1))) {\
        return; \
    } \
    stack->DATA_NAME[stack->rsp] = *data;      \
    ++stack->rsp;                              \
    return;                                    \
} \
\


#define DEFINE_FINITE_STACK_POP_TYPE(TYPENAME, ELEM_COUNT, DATA_NAME) \
static __force_inline TYPENAME pop_##TYPENAME##_##DATA_NAME(finite_stack_##TYPENAME##_##ELEM_COUNT##_t* stack) \
{ \
    if(unlikely(stack->rsp == 0)) return (TYPENAME){ 0 }; \
    --stack->rsp; \
    return stack->DATA_NAME[stack->rsp + 1];            \
} \


#define DEFINE_FINITE_STACK_TOP_TYPE(TYPENAME, ELEM_COUNT, DATA_NAME) \
static __force_inline TYPENAME top_##TYPENAME##_##DATA_NAME(finite_stack_##TYPENAME##_##ELEM_COUNT##_t* stack) \
{ \
    return stack->DATA_NAME[stack->rsp]; \
} \


#define DEFINE_FINITE_STACK_INTERFACE(STRUCTNAME, TYPENAME, ELEMENT_COUNT, INTERNAL_ARRAY_NAME) \
    DEFINE_FINITE_STACK_DATA_STRUCT(STRUCTNAME, TYPENAME, ELEMENT_COUNT, INTERNAL_ARRAY_NAME) \
    DEFINE_FINITE_STACK_PUSH_TYPE(TYPENAME, ELEMENT_COUNT, INTERNAL_ARRAY_NAME) \
    DEFINE_FINITE_STACK_POP_TYPE(TYPENAME, ELEMENT_COUNT, INTERNAL_ARRAY_NAME) \
    DEFINE_FINITE_STACK_TOP_TYPE(TYPENAME, ELEMENT_COUNT, INTERNAL_ARRAY_NAME) \
    \

#endif