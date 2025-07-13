#ifndef __KERNEL_C_DEFINITION_STD_NONSTANDARD_HALT_DIRECTIVES__
#define __KERNEL_C_DEFINITION_STD_NONSTANDARD_HALT_DIRECTIVES__
#include <std/macro.h>


extern void __noreturn pauseloop(void);
extern void __noreturn idleloop(void);
extern void __noreturn hang(void);
extern void __noreturn busyloop(void);
extern void __noreturn abshalt(void);


#endif /* __KERNEL_C_DEFINITION_STD_NONSTANDARD_HALT_DIRECTIVES__ */