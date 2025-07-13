// [bits 64]


.global pauseloop
.global idleloop
.global busyloop
.global abshalt


pauseloop:
    __here0:
        pause
        jmp __here0


idleloop:
    __here1:
        hlt
        jmp __here1


busyloop:
    jmp busyloop


abshalt:
    cli
    hlt // NOTE NMI and SMM interrupts can still wakeup the CPU core. 


