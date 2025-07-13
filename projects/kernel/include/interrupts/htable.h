#ifndef __INTERRUPT_HANDLER_TABLE__
#define __INTERRUPT_HANDLER_TABLE__
#include <std/int.h>
#include <std/enable_bool.h>
#include <std/bool.h>



typedef void (*irqHandler)(void);


void     init_IRQHandleTable(void);
void     set_handler (uint8_t idx, void* handlerAddress);
uint64_t get_handler (uint8_t idx);
bool_t   isHandleFree(uint8_t idx);


#endif
