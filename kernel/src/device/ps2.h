#pragma once
#ifndef __PS2_SUPPORT__
#define __PS2_SUPPORT__
#include "port.h"



#define PS2_DATA 0x60
#define PS2_STTS 0x64 // read
#define PS2_CMD  0x64 // write


// status register masks
#define PS2_STATUS_OUTPUT_BUFFER 0x1
#define PS2_STATUS_INPUT_BUFFER  0x2
#define PS2_STATUS_SYSTEM_FLAG   0x4
#define PS2_STATUS_INPUT_IS_CMD  0x8 // if is enabled, then data in the input buffer is for the command register. else, it is for a PS2 device.
#define PS2_STATUS_TIMEOUT_ERR   0x40
#define PS2_STATUS_PARITY_ERR    0x80


// command register 
#define PS2_READ_CONTROLLER_CONFIG_BYTE_ZERO 0x20
#define PS2_READ_CONTROLLER_CONFIG_BYTE_NEXT 0x60

#define PS2_DISABLE_FIRST_DEVICE_PORT 0xAD
#define PS2_ENABLE_FIRST_DEVICE_PORT  0xAE
#define PS2_DISABLE_SECOND_DEVICE_PORT 0xA7
#define PS2_ENABLE_SECOND_DEVICE_PORT  0xA8

#define PS2_TEST_FIRST_DEVICE_PORT               0xAB
#define PS2_TEST_SECOND_DEVICE_PORT              0xA9
#define PS2_TEST_DEVICE_BYTE_GOOD                0x00
#define PS2_TEST_DEVICE_BYTE_FAIL_CLOCKLINE_LOW  0x01
#define PS2_TEST_DEVICE_BYTE_FAIL_CLOCKLINE_HIGH 0x02
#define PS2_TEST_DEVICE_BYTE_FAIL_DATALINE_LOW   0x03
#define PS2_TEST_DEVICE_BYTE_FAIL_DATALINE_HIGH  0x04


#define PS2_READ_CONTROLLER_INPUT_PORT               0xC0

#define PS2_READ_CONTROLLER_OUTPUT_PORT              0xD0
#define PS2_NEXT_BYTE_IN_OUTPUT_CONTROLLER           0xD1

#define PS2_NEXT_BYTE_IN_FIRST_DEVICE_OUTPUT_BUFFER  0xD2
#define PS2_NEXT_BYTE_IN_SECOND_DEVICE_OUTPUT_BUFFER 0xD3
#define PS2_NEXT_BYTE_IN_SECOND_DEVICE_INPUT_BUFFER  0xD4


typedef struct __ps2_controller_config_byte
{
    uint8_t dev1_int       : 1;
    uint8_t dev2_int       : 1;
    uint8_t sys_flag       : 1;
    uint8_t reserved0      : 1;
    uint8_t dev1_clock     : 1;
    uint8_t dev2_clock     : 1;
    uint8_t dev1_translate : 1;
    uint8_t reserved1      : 1;
} ps2_cont_config_t;


typedef struct __ps2_controller_output_port
{
    uint8_t sys_reset        : 1;
    uint8_t a20_gate         : 1;
    uint8_t dev2_clock       : 1;
    uint8_t dev2_data        : 1;
    uint8_t outbuf_full_dev1 : 1;
    uint8_t outbuf_full_dev2 : 1;
    uint8_t dev1_clock       : 1;
    uint8_t dev1_data        : 1;
} ps2_cont_output_t;



static bool_t ps2_controller_ready()
{
    // return inportb(PS2_);
}


void init_ps2_controller()
{
    
}



#endif