#ifndef __KERNEL_C_DEFINITION_LINKER_SCRIPT_VARIABLES__
#define __KERNEL_C_DEFINITION_LINKER_SCRIPT_VARIABLES__
#include <stdint.h>

#if !defined(__KERNEL_HEADER_TYPE_ALREADY_DEFINED)
typedef struct __kernel_header_t kernel_header_t;
#endif


extern uint64_t         __attribute__((section(".data")))   __mem_virtual_offset;
extern kernel_header_t* __attribute__((section(".data")))   __kheader;
extern uint32_t         __attribute__((section(".rodata"))) __trampoline_size_bytes;
extern uint8_t          __kernel_start;
extern uint8_t          __kernel_vma;
extern uint8_t          __kernel_bss_start;
extern uint8_t          __kernel_bss_end;
extern uint8_t          __kernel_stack_bottom;
extern uint8_t          __kernel_stack_top;
extern uint8_t          __kernel_end;
extern uint8_t          __kernel_STACK_SIZE;

#endif /* __KERNEL_C_DEFINITION_LINKER_SCRIPT_VARIABLES__ */