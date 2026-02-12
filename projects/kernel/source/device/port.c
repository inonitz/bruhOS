#include <device/port.h>


uint8_t inportb(uint16_t portid)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(portid));
    return ret;
}


uint16_t inportw(uint16_t portid)
{
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "d"(portid));
    return ret;
}


uint32_t inportdw(uint16_t portid)
{
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "d"(portid));
    return ret;
}


void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}


void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile ("outw %0, %1" : : "a"(value), "d"(port));
}


void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile ("outl %0, %1" : : "a"(value), "d"(port));
}