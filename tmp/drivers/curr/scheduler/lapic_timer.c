#include "lapic_timer.h"
#include "../acpi/lapic.h"



// div = div1 << 3  + div0
// 000: Divide by 2
// 001: Divide by 4
// 010: Divide by 8
// 011: Divide by 16
// 100: Divide by 32
// 101: Divide by 64
// 110: Divide by 128
// 111: Divide by 1
typedef struct __lapic_divide_configuration_register
{
    union {
        struct pack {
            uint8_t div0       : 2;
            uint8_t reserved0  : 1;
            uint8_t div1       : 1;
            uint32_t reserved1 : 28;
        };
        uint32_t u32;
    };
} dcr_t;


void init_timer(void* lapic_base)
{
    lapic_write_reg(lapic_base, LAPIC_DIVCONFIG, 0x08);
    // lapic_write_reg(lapic_base, LAPIC_LVTTIMER, );
}