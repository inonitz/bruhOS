#pragma once
#ifndef __THREAD_STRUCTURE__
#define __THREAD_STRUCTURE__
#include "../shared/percpu.h"
#include "../std/lock.h"



typedef enum {
    START,
    READY,
    RUNNING,
    WAITING,
    DONE
} thread_state_t;


typedef struct __general_purpose_registers_context
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;  
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
} gp_context_t;


typedef struct __thread_control_block_type
{
    uint32_t       tid;       // thread id
    uint64_t       rbp;       // stack rbp
    uint64_t       rsp;       // stack rsp
    uint64_t       rip;       // current RIP
    thread_state_t state;     // 
    gp_context_t   registers; 
} thread_control_block_t;


#endif