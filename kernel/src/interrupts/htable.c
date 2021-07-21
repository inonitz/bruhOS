#include "htable.h"



static irqHandler handleTable[224];
// IDEA: There should be a Table for owner/device strings, where each device would acquire an index,
// represnting their own string.
// Moreover, every handle should be populated with a Table Index, describing who is holding that entry.
// will be very useful for debugging!!!
// struct {
//     irqHandler hdl;
//     char_t*    owner;
// } handleTable[224];

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