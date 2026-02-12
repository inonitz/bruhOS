#ifndef __PORT_READ_WRITE_DEFINITION_HEADER__
#define __PORT_READ_WRITE_DEFINITION_HEADER__
#include <std/macro.h>
#include <std/stdint.h>


uint8_t  inportb(uint16_t portid);
uint16_t inportw(uint16_t portid);
uint32_t inportdw(uint16_t portid);
void outb(uint16_t port, uint8_t  value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);


#endif /* __PORT_READ_WRITE_DEFINITION_HEADER__ */