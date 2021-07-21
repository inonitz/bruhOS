#include "lapict.h"
#include "../interrupts/router.h"
#include "../interrupts/htable.h"
#include "../interrupts/gdt.h"
#include "../device/hpet.h"
#include "../acpi/lapic.h"
#include "../std/atomic.h"
#include "cpu.h"
#include "msr.h"



#define LVTTIMER_MASKED_BIT (1 << 16)


static uint32_t tick = 0;




void startTimer()
{
    uint32_t reg = lapic_read_reg(LAPIC_LVTTIMER);
    if(!__always_false( (reg >> 17) & LAPIC_TIMER_STATE_TSC_DEADLINE) ) {
        lapic_write_reg(LAPIC_LVTTIMER, reg & ~(1 << 16)); // disable the mask and enable the timer.
        return;
    }


    // read the tick that we saved in the counters - doesn't work, more in prepare_lapic_timer().
    uint64_t tick = lapic_read_reg(LAPIC_TIMERINITCNT);
    tick |= ( ((uint64_t)lapic_read_reg(LAPIC_TIMERCURCNT)) << 32);
    wrmsr(IA32_MSR_ECX_TSC_DEADLINE, tick);
    return;
}


void stopTimer()
{
    LVTTR_t tmp; tmp.u32 = lapic_read_reg(LAPIC_LVTTIMER);
    if(__always_false(tmp.timerMode == LAPIC_TIMER_STATE_TSC_DEADLINE) ) { // resets the LAPIC timer on deadline mode.
        wrmsr(IA32_MSR_ECX_TSC_DEADLINE, 0);   
    } else { // resets the LAPIC timer on periodic/one-shot mode.
        lapic_write_reg(LAPIC_LVTTIMER, tmp.u32 | (1 << 16));
        lapic_write_reg(LAPIC_TIMERCURCNT, 0); 
    }
    return;
}




void lapic_timer_handler()
{
    stopTimer();
    ++tick;
    printk("\ntick %u", tick);
}




void init_lapic_timer()
{
    uint8_t  irq      = request_vector_min(IDT_GATE_PRI_LVL0) + 32;
    uint32_t lvttr    = irq | (1 << 16);
    uint64_t handler  = (uint64_t)lapic_timer_handler;

    
    set_handler(irq-32, (void*)handler);       // set IDT handler.
    lapic_write_reg(LAPIC_LVTTIMER,    lvttr); // timer configuration.
    lapic_write_reg(LAPIC_DIVCONFIG,    0x0B); // set divide/tick to 0b1011 == 1
    lapic_write_reg(LAPIC_TIMERCURCNT,  0x00); // counter        == 0
    lapic_write_reg(LAPIC_TIMERINITCNT, 0x00); // counter_target == 0
    wrmsr(IA32_MSR_ECX_TSC_DEADLINE,    0x00); // reset TSC-deadline counter (if available)
}


void set_lapic_timer_state(bool_t state)
{
    uint32_t tmp = lapic_read_reg(LAPIC_LVTTIMER);
    tmp &= ~(0x03 << 17);
    switch(state)
    {
        case LAPIC_TIMER_STATE_ONE_SHOT:
                stopTimer();
                lapic_write_reg(LAPIC_LVTTIMER, tmp);
            break;


        case LAPIC_TIMER_STATE_PERIODIC:
                stopTimer();
                tmp |= (0x01 << 17);
                lapic_write_reg(LAPIC_LVTTIMER, tmp);
            break;


        case LAPIC_TIMER_STATE_TSC_DEADLINE:
                // stopTimer();
                // tmp |= (0x02 << 17);
                // lapic_write_reg(LAPIC_LVTTIMER, tmp);
            break;

        default:
            printk("Invalid LAPIC Timer State.\n");
            break;
    }
    return;
}


void prepare_lapic_timer(uint32_t quanta_ns)
{
    // calibrate the timer to the desired quanta.
    uint32_t reg    = 0;
    uint32_t ticks  = 0;
    uint64_t tick64 = 0;
    reg = lapic_read_reg(LAPIC_LVTTIMER);


    if(__always_false( (reg >> 17) & LAPIC_TIMER_STATE_TSC_DEADLINE) ) { // if TSC-Deadline is enabled.
        lapic_write_reg(LAPIC_LVTTIMER, reg & ~(1 << 16));    // enable timer interrupts.
        wrmsr(IA32_MSR_ECX_TSC_DEADLINE, 0);                  // reset counter
        
        wrmsr(IA32_MSR_ECX_TSC_DEADLINE, 0xFFFFFFFFFFFFFFFF); // start counting
        hcountns(quanta_ns);                                  // start quanta timer
        tick64 = rdmsr(IA32_MSR_ECX_TSC_DEADLINE);            // get count after t = quanta(ns)

        wrmsr(IA32_MSR_ECX_TSC_DEADLINE, 0);                  // disarm timer.


        // TSC Deadline is untested. so until then, I'm keeping debug_printk in here.
        // debug_printk("got tick %X\n", tick64);
        tick64  = 0xFFFFFFFFFFFFFFFF - tick64;
        // debug_printk("got tick %X\n", tick64);
        tick64 += rdtsc();
        // debug_printk("got tick %X\n", tick64);


        // keep TSC-Deadline in CPU-local storage, this DOESNT WORK! 
        // (You have to change to a different state, then write the tick, writes are ignored in deadline mode)
        // Too many reads/writes for keeping track of the tick.
        lapic_write_reg(LAPIC_TIMERINITCNT, (uint32_t)(tick64      )); // keep low  32-bits in init counter
        lapic_write_reg(LAPIC_TIMERCURCNT,  (uint32_t)(tick64 >> 32)); // keep high 32-bits in main counter


    } else {
        lapic_write_reg(LAPIC_TIMERINITCNT, 0xFFFFFFFF); // reset counters
        lapic_write_reg(LAPIC_TIMERCURCNT,  0x00);       // reset counters
        hpet_timer_prepnano((uint64_t)quanta_ns);

        lapic_write_reg(LAPIC_LVTTIMER, reg & ~(1 << 16)); // enable the timer by disabling the int mask.
        hpet_start();                                      // count quanta-ns using the HPET counter.
        lapic_write_reg(LAPIC_LVTTIMER, reg);              // restore the mask and disable the timer.

        ticks = 0xFFFFFFFF - lapic_read_reg(LAPIC_TIMERCURCNT); // calculate ticks-per quanta
        lapic_write_reg(LAPIC_TIMERINITCNT, ticks);             // send int every quanta nanoseconds.
        lapic_write_reg(LAPIC_TIMERCURCNT,  0x00);              // reset counter again.
    }
    return;
}


void start_lapic_timer()
{
    startTimer();
}




#define COMMON_CODE(timer_count) \
    { \
        uint32_t reg  = lapic_read_reg(LAPIC_LVTTIMER); \
        uint32_t tick = 0xFFFFFFFF; \
\
\
        lapic_write_reg(LAPIC_TIMERINITCNT, 0xFFFFFFFF); \
        lapic_write_reg(LAPIC_LVTTIMER, reg & ~(1 << 16)); /* enable      */ \
        timer_count;                                       /* count       */ \
        lapic_write_reg(LAPIC_LVTTIMER, reg);              /* disable     */ \
\
        tick -= lapic_read_reg(LAPIC_TIMERCURCNT);         /* get time    */ \
        lapic_write_reg(LAPIC_TIMERINITCNT, tick);         /* set target  */ \
\
        lapic_write_reg(LAPIC_LVTTIMER, reg & ~(1 << 16)); /* start count */ \
    } \
\


void lcountns(uint64_t ns) 
{
    COMMON_CODE(hcountns(ns));
    return;
}


void lcountus(uint64_t us)
{
    COMMON_CODE(hcountns(us * 1000));
    return;
}


void lcountms(uint64_t ms)
{
    COMMON_CODE(hcount(ms));
    return;
}



// find the timer frequency - tsc OR bus frequency...
// register uint32_t eax __asm__("eax");
// register uint32_t ebx __asm__("ebx");
// register uint32_t ecx __asm__("ecx");
// register uint32_t edx __asm__("edx");
// uint32_t tsc_freq = 0, bus_freq = 0, ticks_ms = 0;

// eax = 0x15;
// __asm__ volatile("cpuid");
// printk("%X %X %X %X\n", eax, ebx, ecx, edx);
// tsc_freq = (ecx * ebx) / eax;

// eax = 0x16;
// __asm__ volatile("cpuid");
// bus_freq = ecx & 0xFFFF;

    

// lapic_write_reg(LAPIC_LVTTIMER, irq);            // enable the timer by disabling the int mask.
// hcount(1000);                                    // count 1s using the HPET counter.
// lapic_write_reg(LAPIC_LVTTIMER, lvttr);          // restore the mask and disable the timer.

// ticks_ms = 0xFFFFFFFF - lapic_read_reg(LAPIC_TIMERCURCNT);
// lapic_write_reg(LAPIC_LVTTIMER, lvttr | LAPIC_TIMER_STATE_PERIODIC << 17); // set to periodic mode
// lapic_write_reg(LAPIC_TIMERINITCNT, ticks_ms);                             // send int every 1s.