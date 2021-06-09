#pragma once
#ifndef __PORT_BUS__
#define __PORT_BUS__
#include "../shared/int.h"


static __always_inline uint8_t inportb(uint16_t portid)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(portid));
    return ret;
}


static __always_inline uint16_t inportw(uint16_t portid)
{
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "d"(portid));
    return ret;
}


static __always_inline uint32_t inportl(uint16_t portid)
{
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "d"(portid));
    return ret;
}


static __always_inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}


static __always_inline void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile ("outw %0, %1" : : "a"(value), "d"(port));
}


static __always_inline void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile ("outl %0, %1" : : "a"(value), "d"(port));
}




#endif