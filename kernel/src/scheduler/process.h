#pragma once
#ifndef __PROCESS_STRUCTURE__
#define __PROCESS_STRUCTURE__
#include "../shared/int.h"
#include "../shared/percpu.h"
#include "../shared/kpanic.h"
#include "../mem/virt/vas.h"
#include "thread.h"



#define PROCESS_STATUS_STOPPED   0 
#define PROCESS_STATUS_RUNNING   1
#define PROCESS_STATUS_SUSPENDED 2
// this isn't a macro, but rather a variable that will be determined after intializing SMP 
#define SYS_PROCESSOR_COUNT
#define PROCESS_THREAD_LIMIT SYS_PROCESSOR_COUNT 


typedef struct __process_access_flags
{
    union {
        struct pack {
            uint8_t reserved : 7;
            uint8_t isUser   : 1; // 0 - kernel level process.
        };
        uint8_t u8;
    };
} proc_access_flags_t;


typedef proc_access_flags_t access_bits_t;


typedef struct __process_control_block_type
{
    uint16_t                pid;
    char_t*                 name;
    uint8_t                 state;
    uint8_t                 priority;
    access_bits_t           flags;

    uint8_t*                executable;
    uint64_t                exec_size;

    thread_control_block_t* thread_array; // element tcb_t array. 
    vas_man_t               addressSpace;

    uint64_t                rbp_kernel;
    uint64_t                rbp_user;


} process_control_block_t;




#endif