#pragma once
#ifndef __SYSCALL_ENTRY__
#define __SYSCALL_ENTRY__
#include "../shared/int.h"
#include "../device/msr.h"



typedef struct __system_call_function_table
{
    uint64_t opcall[512];
} syscall_table_t;


/* 
    * Will select the appropriate syscall based on call_id.
    * uint16_t call_id - selects the appropriate 
*/
static void syscall_entry(uint16_t call_id);


#endif