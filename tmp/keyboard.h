#pragma once
#ifndef __PS2_KEYBOARD_DRIVER__
#define __PS2_KEYBOARD_DRIVER__
#include "../interrupts/router.h"



// NOTE: I'm not reading the ACPI tables to determine if there is actually a PS2 controller,
// as I'm currently using QEMU and it 100% has a PS2 Keyboard emulation layer.
typedef struct __keyboard_buffer
{
    uint8_t  keyQueue[1024];
    uint16_t queueHead;
    uint16_t queueTail;
    uint8_t  lastPress;
    uint8_t  currentPress;
} keyboard_buffer_t;




void    keyboard_handler();             // will be called on keyboard interrupt.
void    keyboard_init();                // initializes the static keyboard_buffer_t structure
uint8_t scancode_to_ascii(uint8_t key); // converts a ps2 keyboard scan-code into a readable ascii.
void    readKey();                      // updates the keys buffer.
uint8_t lastKey();                      // returns the last key in the keyboard buffer.



#endif