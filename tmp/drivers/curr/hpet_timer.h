#pragma once
#ifndef __HPET_TIMER__
#define __HPET_TIMER__
#include "../std/io.h"



void timer_count  (uint64_t milliseconds);
void timer_prepare(uint64_t milliseconds);
void timer_start();
void timer_end();
void timer_periodic(uint64_t milliseconds);

#endif