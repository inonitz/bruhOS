#include <device/hpet.h>
#include <interrupts/router.h>
#include <interrupts/idt.h>
#include <std/io.h>
#include <std/bit.h>
#include <acpi/lapic.h>
#include <mem/map_ptr.h>



typedef struct pack __hpet_timer_struct
{
    union {
        struct pack {
            bool_t   halted          : 1;
            bool_t   legacy_routed   : 1;
            bool_t   legacy_routable : 1;
            bool_t   mainCounter64   : 1;
            bool_t   reserved0       : 4;
            uint8_t  timerCount;     // the timers that are available are from index 0 -> timerCount
            uint16_t mintick;
            uint32_t reserved1;
            uint64_t frequency;
            uint64_t regtable;
        };
        uint64_t u64[3];
    };
} HPETtimer_t;


typedef struct pack __hpet_timer_comparator
{
    bool_t  hw_enabled         : 1;
    bool_t  periodic           : 1;
    bool_t  periodic_supported : 1;
    uint8_t irqRoute           : 5;
} COMP_TIMER;


static HPETtimer_t      hpet_timer;
static volatile bool_t  polled = BOOLEAN_FALSE;



__force_inline uint64_t hpet_read_reg(uint64_t reg_base, uint16_t offset)
{
    return *(uint64_t volatile*)(reg_base + (uint64_t)offset);
}


__force_inline void hpet_write_reg(uint64_t reg_base, uint16_t offset, uint64_t data)
{
    *(uint64_t volatile*)(reg_base + (uint64_t)offset) = data;
}


static __force_inline void hpetDisable(); // define it for the handler.
void hpet_timer_handler()
{
    hpetDisable();
    hpet_write_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG, 0); // reset main counter.
    polled = BOOLEAN_TRUE;
}




void HPET_init(volatile HPET_t* table)
{
    uint64_t regtable  = VIRT(table->registers.address);
    toVirtualFlag((void*)table->registers.address, 1, 0x800000000000000b); // 1 51*0 000 0 0 00 0 1 0 1 1

    uint64_t cap       = hpet_read_reg(regtable, HPET_CAPABILITIES_ID_REG);
    uint64_t cfg       = hpet_read_reg(regtable, HPET_CONFIG_REG         );
    uint64_t tmp       = 0;
    uint64_t frequency = ONE_OVER_FEMTOSECOND / (cap >> 32);
    uint16_t mintick   = table->minimum_tick;
    uint8_t  timers    = 1 + ((cap >> 8) & HPET_COMP_COUNT_MASK);
    bool_t   mcount64  = (cap >> 13) & 1;


    // update configuration register.
    cfg &= ~CONFIG_REG_ENABLE_TIMER;                      // enable_timer   = BOOLEAN_FALSE
    cfg &= ~CONFIG_REG_REPLACE_LEGACY;                    // legacy_replace = BOOLEAN_FALSE
    hpet_write_reg(regtable, HPET_CONFIG_REG, cfg);       // update config for normal operation.
    hpet_write_reg(regtable, HPET_MAIN_COUNT_VAL_REG, 0); // reset main counter.
    

    // disable all timers, and set their comparators to 0.
    for(uint8_t i = 0; i < timers; ++i)
    {
        tmp  = hpet_read_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(i));
        tmp &= ~TIMER_ENABLE_INTERRUPT;
        hpet_write_reg(regtable, TIMERN_COMPARATOR_VALUE_REG(i),    0); 
        hpet_write_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(i), tmp);
    }


    // init timer0
    uint64_t timer0      = hpet_read_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(0));
    uint64_t ioapic_irq0 = lsbul(timer0 >> 32) - 1;
    
    timer0 &= ~TIMER_ENABLE_FSB_INT;                                            // disable fsb
    timer0 |= ioapic_irq0 << 9;                                                 // set ioapic_route, using the least significant bit in the available routes.
    timer0 &= ~TIMER_PERIODIC;                                                  // set timer to 1shot mode.
    timer0 |= TIMER_ENABLE_INTERRUPT;                                           // enable interrupts on timer0.
    hpet_write_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(0), timer0        );  // update timer0 register
    hpet_write_reg(regtable, TIMERN_COMPARATOR_VALUE_REG(0) , frequency * 10);  // update timer0 comparator to 100ms (100000000000000u)
    // send to cpu0 (BSP) timer interrupts.
    installISA(ioapic_irq0, (uint8_t)lapic_get_id(), IDT_GATE_PRI_LVL0, (void*)(uint64_t)hpet_timer_handler);  


    // init static HPETtimer_t
    hpet_timer = (HPETtimer_t){
        .halted          = BOOLEAN_FALSE,
        .legacy_routed   = BOOLEAN_FALSE,
        .legacy_routable = (cap >> 15) & 1,
        .mainCounter64   = mcount64,
        .reserved0       = 0,
        .timerCount      = timers,
        .mintick         = mintick,
        .reserved1       = 0,
        .frequency       = frequency,
        .regtable        = regtable
    };
    printk("       [ HPET Timer Initializer    ] Timer Frequency is %ufs (femtoseconds)\n", frequency);
}



static __force_inline void hpetEnable()
{
    uint64_t cfg = (hpet_timer.u64[0] & 0x2) | CONFIG_REG_ENABLE_TIMER;
    hpet_write_reg(hpet_timer.regtable, HPET_CONFIG_REG, cfg);
    hpet_timer.u64[0] |= 1ul;
}


static __force_inline void hpetDisable()
{
    uint64_t cfg = hpet_timer.u64[0] & 0x2; 
    hpet_write_reg(hpet_timer.regtable, HPET_CONFIG_REG, cfg);
    hpet_timer.u64[0] &= ~1ul;
}


/* 
    * millisecond conversion:
    * counter_period = hpet_timer.frequency = x * femtoseconds (max = 100nanoseconds);
    * 1. lets call counter_period x_femto.
    * 2. x_femto * y = 1_milli; 
    * y is the number that converts from femtoseconds to milliseconds
    * ==> y = 1_milli / x_femto 
    *       = 1_milli / ( x * 10^(-12)_milli  )
    *       = 10^(12) / x
    * 
    * apparently multiplying the original equality by 10 makes it go to milliseconds.
    * I don't know, bug in QEMU maybe? maybe I'm stupid? Probably the latter.
    * anyway, it works. No need to ask anymore questions.
*/
#define CONSTANT_JUST_WORKS 10
#define MILLI_TO_FEMTO 0xe8d4a51000
#define NANO_TO_FEMTO  0xf4240
void hcount(uint64_t milliseconds)
{
    hpetDisable();
    milliseconds *= CONSTANT_JUST_WORKS * MILLI_TO_FEMTO;
    milliseconds /= hpet_timer.frequency;
    hpet_write_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG       , 0           ); // reset main counter.
    hpet_write_reg(hpet_timer.regtable, TIMERN_COMPARATOR_VALUE_REG(0), milliseconds); // update timer0 comparator to N(ms).
    hpetEnable();
    while(!polled) { __asm__ volatile("pause"); }
    polled = BOOLEAN_FALSE;
}


void hcountns(uint64_t nanoseconds)
{
    hpetDisable();
    nanoseconds *= CONSTANT_JUST_WORKS * NANO_TO_FEMTO;
    nanoseconds /= hpet_timer.frequency;
    hpet_write_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG       , 0          );         
    hpet_write_reg(hpet_timer.regtable, TIMERN_COMPARATOR_VALUE_REG(0), nanoseconds);
    hpetEnable();
    while(!polled) { __asm__ volatile("pause"); }
    polled = BOOLEAN_FALSE;
}


void hpet_timer_prepmill(uint64_t milliseconds)
{
    hpetDisable();
    milliseconds *= CONSTANT_JUST_WORKS * MILLI_TO_FEMTO;
    milliseconds /= hpet_timer.frequency;
    hpet_write_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG       , 0           );
    hpet_write_reg(hpet_timer.regtable, TIMERN_COMPARATOR_VALUE_REG(0), milliseconds);
    return;
}


void hpet_timer_prepnano(uint64_t nanoseconds)
{
    hpetDisable();
    nanoseconds *= CONSTANT_JUST_WORKS * NANO_TO_FEMTO;
    nanoseconds /= hpet_timer.frequency;
    hpet_write_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG       , 0          );         
    hpet_write_reg(hpet_timer.regtable, TIMERN_COMPARATOR_VALUE_REG(0), nanoseconds);
    return;
}


void hpet_start()
{
    hpetEnable();
    while(!polled) { __asm__ volatile("pause"); }
    polled = BOOLEAN_FALSE;
}


void hpet_stop()
{
    hpetDisable();
}




uint64_t hpet_poll_count()
{
    return hpet_read_reg(hpet_timer.regtable, HPET_MAIN_COUNT_VAL_REG);
}


bool_t timerPolled() {
    return polled;
}



/*
uint64_t cap       = hpet_read_reg(regtable, HPET_CAPABILITIES_ID_REG);
uint64_t cfg       = hpet_read_reg(regtable, HPET_CONFIG_REG         );
uint64_t frequency = ONE_OVER_FEMTOSECOND / (cap >> 32);
uint16_t mintick   = table->minimum_tick; (table = HPET_t*)
uint8_t  timers    = 1 + ((cap >> 8) & HPET_COMP_COUNT_MASK);
bool_t   mcount64  = (cap >> 13) & 1;

printk("\nHPET Info:\nMain Counter 64bits: %u\n%u timers\nClock frequency is: %u\nminimum tick is %b\n\n", 
    mcount64,
    timers, 
    frequency, 
    mintick
);



tmp  = hpet_read_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(i));
printk("Timer %u:\nINT_TYPE: %u\nINT_ENABLED: %u\nPERIODIC: %u\nPERIODIC
        CAPABLE: %u\nTIMER_SIZE64BITS: %u\nFORCE32: %u\nI/O APIC ROUTING: %u\nFSB MAPPING: <Enabled=%u, Supported=%u>\nIOAPIC ROUTES: %b\n\n",
        i, 
        boolean(tmp & TIMER_INTERRUPT_TYPE), 
        boolean(tmp & TIMER_ENABLE_INTERRUPT), 
        boolean(tmp & TIMER_PERIODIC), 
        boolean(tmp & TIMER_PERIODIC_SUPPORTED),
        boolean(tmp & TIMER_IS64BIT), 
        boolean(tmp & TIMER_FORCE32BIT), 
        (tmp >> 9) & TIMER_IOAPIC_ROUTE_MASK, 
        boolean(tmp & TIMER_ENABLE_FSB_INT), 
        boolean(tmp & TIMER_FSB_INT_SUPPORTED), 
        tmp >> 32
);


tmp = hpet_read_reg(regtable, TIMERN_CONFIG_CAPABILITY_REG(0));
printk("Timer %u:\nINT_TYPE: %u\nINT_ENABLED: %u\nPERIODIC: %u\nPERIODIC CAPABLE: %u\nTIMER_SIZE64BITS: %u\nFORCE32: %u\nI/O APIC ROUTING: %u\nFSB MAPPING: <Enabled=%u, Supported=%u>\nIOAPIC ROUTES: %b\n\n",
            0, 
            boolean(timer0 & TIMER_INTERRUPT_TYPE), 
            boolean(timer0 & TIMER_ENABLE_INTERRUPT), 
            boolean(timer0 & TIMER_PERIODIC), 
            boolean(timer0 & TIMER_PERIODIC_SUPPORTED),
            boolean(timer0 & TIMER_IS64BIT), 
            boolean(timer0 & TIMER_FORCE32BIT), 
            (timer0 >> 9) & TIMER_IOAPIC_ROUTE_MASK, 
            boolean(timer0 & TIMER_ENABLE_FSB_INT), 
            boolean(timer0 & TIMER_FSB_INT_SUPPORTED), 
            timer0 >> 32
);

uint64_t milliseconds = 100 * 0xe8d4a51000ul;                                      // 1ms = 10^(12)fs
uint64_t bruh         = 0;
hpet_write_reg(hpet_timer.regtable, TIMERN_COMPARATOR_VALUE_REG(0), milliseconds); // update timer0 comparator to N(ms).
enableTimer();

bruh = hpet_poll_count();
while(hpet_poll_count() < 0x51dac207a000) {}
hang();
for(uint64_t i = 0; i < 200; ++i)
{
    for(uint64_t j = 0; j < 10000; ++j) { __asm__ volatile("pause"); }
    printk("%X ", hpet_poll_count());
    if((1+i) % 10 == 0) {
        putln();
    }
}
*/