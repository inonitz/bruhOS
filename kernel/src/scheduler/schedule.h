#pragma once
#ifndef __KERNEL_SCHEDULER__
#define __KERNEL_SCHEDULER__
#include "../std/io.h"



void schedule();


void block();


extern void context_switch();

#endif