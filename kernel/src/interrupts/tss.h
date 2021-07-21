#pragma once
#ifndef __TSS_SEGMENT__
#define __TSS_SEGMENT__
#include "../std/io.h"



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

#endif