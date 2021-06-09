#pragma once
#ifndef __INTERRUPT_HANDLER_TABLE__
#define __INTERRUPT_HANDLER_TABLE__
#include "../std/debug.h"



typedef void (*irqHandler)();


void     init_IRQHandleTable();
void     set_handler (uint8_t idx, void* handlerAddress);
uint64_t get_handler (uint8_t idx);
bool_t   isHandleFree(uint8_t idx);


#endif