#include "hpet_timer.h"
#include "hpet.h"


void timer_count(uint64_t milliseconds)
{
    timer.registers->TimerN[timer.frtimer].comp.u64 = milliseconds * ONE_OVER_FEMTOSECOND;
    timer.registers->TimerN[timer.frtimer].cpr.int_enb_cnf = 1;
    hpet_restart();
}


void timer_prepare(uint64_t milliseconds)
{
    timer.registers->TimerN[timer.frtimer].comp.u64 = milliseconds; // * ONE_OVER_FEMTOSECOND
    timer.registers->main_counter_reg.cntval = 0;
}


void timer_start()
{
    timer.registers->config_reg.enable_cnf = 1;
    timer.registers->TimerN[timer.frtimer].cpr.int_enb_cnf = 1;
    timer.enabled = 1;
}


void timer_end()
{
    timer.registers->config_reg.enable_cnf = 0;
    timer.registers->TimerN[timer.frtimer].cpr.int_enb_cnf = 0;
    timer.enabled = 0;
}


void timer_periodic(uint64_t milliseconds)
{

}