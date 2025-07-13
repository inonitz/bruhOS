#include <std/halt.h>


__asm__("\
.global pauseloop   \n\t\
.global idleloop    \n\t\
.global hang        \n\t\
.global busyloop    \n\t\
.global abshalt     \n\t\
                    \n\t\
                    \n\t\
pauseloop:          \n\t\
    __here0:        \n\t\
        pause       \n\t\
        jmp __here0 \n\t\
                    \n\t\
                    \n\t\
idleloop:           \n\t\
    __here1:        \n\t\
        hlt         \n\t\
        jmp __here1 \n\t\
                    \n\t\
                    \n\t\
hang:               \n\t\
    call idleloop   \n\t\
                    \n\t\
                    \n\t\
busyloop:           \n\t\
    jmp busyloop    \n\t\
                    \n\t\
                    \n\t\
abshalt:            \n\t\
    cli             \n\t\
    hlt             \n\t\
");


/* [NOTE] halt.c:28: NMI and SMM interrupts can still wakeup the CPU core. */