#ifndef __KERNEL_C_DEFINITION_TASK_STATE_SEGMENT_HEADER__
#define __KERNEL_C_DEFINITION_TASK_STATE_SEGMENT_HEADER__
#include <std/int.h>
#include <std/macro.h>


typedef struct pack __task_state_segment
{
    uint32_t reserved0;
    QWORD    rsp[3];
    QWORD    reserved1;
    QWORD    ist[7];
    QWORD    reserved2;
    uint32_t reserved3;
    uint32_t IOPBoffset;
} TSS_t;


#define __KERNEL_PERCPU_TSS_x86_64_SIZE sizeof(TSS_t)
#define __KERNEL_PERCPU_TSS_ENTRY_SIZE 0x08


#endif /* __KERNEL_C_DEFINITION_TASK_STATE_SEGMENT_HEADER__ */
