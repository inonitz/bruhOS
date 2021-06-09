#pragma once
#ifndef __TSS_SEGMENT__
#define __TSS_SEGMENT__
#include "../std/io.h"


typedef struct __lowhigh
{
    union {
        struct pack {
            uint32_t low;
            uint32_t high;
        };
        uint64_t u64;
    };
} QWORD;


typedef struct pack __task_state_segment
{
    uint32_t reserved0;
    QWORD    rsp0;
    QWORD    rsp1;
    QWORD    rsp2;
    QWORD    reserved1;
    QWORD    ist[7];
    QWORD    reserved2;
    uint32_t reserved3;
    uint32_t IOPBoffset;
} TSS_t;

#endif