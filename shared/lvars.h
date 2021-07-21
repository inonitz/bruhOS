#pragma once
#ifndef __LINKER_DEFINED_VARIABLES__
#define __LINKER_DEFINED_VARIABLES__
#include "def.h"



extern uint64_t         __mem_virtual_offset;
extern kernel_header_t* __kheader;
extern uint8_t          __kernel_start;
extern uint8_t          __kernel_vma;
extern uint8_t          __kernel_bss_start;
extern uint8_t          __kernel_bss_end;
extern uint8_t          __kernel_stack_bottom;
extern uint8_t          __kernel_stack_top;
extern uint8_t          __kernel_end;
extern uint8_t          STACK_SIZE;
#endif