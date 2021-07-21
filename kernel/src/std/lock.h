#pragma once
#ifndef __LOCK_STRUCTURE__
#define __LOCK_STRUCTURE__
#include "atomic.h"



/* 
    * This is an implementation of a Spinlock Data Structure.
    * Source: https://rigtorp.se/spinlock/
    * 
    * In Short (from my understanding, so far...): 
    * When Thread A acquires the lock, it will block a certain memory region. 
    * The rest of the threads will be locked out of that memory zone (waiting, doing nothing) 
    * until Thread A releases the lock.
    * Meaning, only one thread can access a certain memory region at a time.
    * 
    * Problems with spinlocks: 
    *   A. The Threads that are waiting for Thread A are doing nothing, meaning they are wasting time and OS resources.
    *      This fact tells us that we should probably assign the rest of the threads to do other work.
    *      for instance: if thread A claimed memory_zone, and thread B is ready to do a differnt task,
    *                    we'll assign B to a new task, so that we use the CPU efficiently.
    *                   ( Something like that hehe... )
    *  B. Spinlocks are pretty 'heavy'.
    * 
    * Alternatives (if are available):
    *   Atomic Variables (Probably...)
*/


#define SPINLOCK_INIT_VALUE { FALSE }
#define SPINLOCK_VAL(spinlock_type) (spinlock_type.flag)


typedef struct __spinlock_type
{
    atomic8u_t flag;
} spinlock_t;


/* 
    * Tries to lock the spinlock if it is available.
    * Otherwise, it waits until the lock is available, after which it aquires it.
    * spinlock_t* sl - the address of the spinlock to grab.
*/
static void lock(spinlock_t sl)
{
    for(;;)
    {
        if(!atomic_exchange_ret_explicit_u8(&sl.flag, TRUE, memory_model_acquire)) {
            return;
        }


        while(atomic_load_explicit_u8(&sl.flag, memory_model_relaxed)) {
            __asm__ volatile("pause" : : : "memory");
        }
    }
    return;
}


/* 
    * Tries to lock the spinlock. 
    * returns if succeeded or failed: 
    * TRUE  - Lock Acquired successfully.
    * FALSE - failed to acquire the lock, someone is using it.
*/
static __force_inline bool_t try_lock(spinlock_t sl)
{
    return !atomic_load_u8(&sl.flag) && !atomic_exchange_ret_u8(&sl.flag, TRUE);
}


static __force_inline void unlock(spinlock_t sl)
{
    atomic_cmpxchg_u8(&sl.flag, &sl.flag, FALSE);
    return;
}

#endif