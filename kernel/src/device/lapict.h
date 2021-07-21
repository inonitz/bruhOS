#pragma once
#ifndef __LAPIC_TIMER__
#define __LAPIC_TIMER__
#include "../shared/int.h"



/* 
    * NOTE:
    * The current version of this timer only supports OneShot & Periodic Modes.
    * This is because I can't test TSC-Deadline Mode currently with QEMU, 
    * so I'll just have to wait for updates, and then make sure my implementation is correct.
*/
#define LAPIC_TIMER_STATE_ONE_SHOT     0x00
#define LAPIC_TIMER_STATE_PERIODIC     0x01
#define LAPIC_TIMER_STATE_TSC_DEADLINE 0x02
#define LAPIC_TIMER_STATE_RESERVED     0x03




/* 
    * Initializes the LAPIC Timer to a *default* state, depending on the AP Core calling this function.
*/
void init_lapic_timer();


/* 
    * Set the LAPIC Timer to a specific State (#define's Above)
    * After Setting the State, the state of the timer needs to be prepared.
*/
void set_lapic_timer_state(uint8_t state);


/* 
    * Prepare the LAPIC Timer to run a specific quanta of time, in nanoseconds.
    * The timer MUST BE DISABLED before calling this function!
    * uint32_t quanta_ns - the target nanoseconds for the timer to prepare for.
*/
void prepare_lapic_timer(uint32_t quanta_ns);


/* 
    * the timer will start counting based on prepare_lapic_timer().
    * The timers' state must be known & prepared before calling this function.
    * if a state was set, but not prepared, the timer will NOT work.
*/
void start_lapic_timer();




/* 
    * The following functions count for the amount of time requested.
    * They perform the preparation & enabling of the timer.
    * These functions are more efficient than preparing => starting the timer, 
    * because they perform less work. Although, you should use prepare_lapic_timer
    * in the case that absolute precision is needed.
*/


/* 
    * nanoseconds (10^-9)sec
*/
void lcountns(uint64_t ns);


/* 
    * microseconds (10^-6)sec
*/
void lcountus(uint64_t us);


/* 
    * milliseconds (10^-3)sec
*/
void lcountms(uint64_t ms);

#endif