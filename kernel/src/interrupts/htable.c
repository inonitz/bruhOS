#include "htable.h"



static irqHandler handleTable[224];


void init_IRQHandleTable()
{
    memset(handleTable, 0, sizeof(handleTable));
}


void set_handler(uint8_t idx, void* handler)
{
    handleTable[idx] = (irqHandler)(uint64_t)handler;
}


uint64_t get_handler(uint8_t idx)
{
    return (uint64_t)handleTable[idx];
}


bool_t isHandleFree(uint8_t idx)
{
    return (handleTable[idx] == 0);
}