#pragma once
#ifndef __LINKER_DEFINED_VARIABLES__
#define __LINKER_DEFINED_VARIABLES__



typedef unsigned char uint8_t;


extern uint64_t __mem_virtual_offset; // not defined in the linker, but in entry.h
extern uint8_t __kernel_start;
extern uint8_t __kernel_vma;
extern uint8_t __kernel_bss_start;
extern uint8_t __kernel_bss_end;
extern uint8_t __kernel_stack_bottom;
extern uint8_t __kernel_stack_top;
extern uint8_t __kernel_end;
extern uint8_t STACK_SIZE;
#endif