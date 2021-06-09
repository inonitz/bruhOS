#pragma once
#ifndef __INTERRUPT_HANDLERS__
#define __INTERRUPT_HANDLERS__
#include "../std/debug.h"



/* 
    * after an interrupt/trap gate has been called,
    * these registers are pushed onto the stack in this fashion:
    * (see Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Combined Volumes,
    *  Vol. 1 Section 6.5.1 Figure 6-7 + Section 6.5.6)

    address x:    stack before push ->
                                       ss
                                       rsp
                                       rflags
                                       cs
                                       rip
    address x-48: stack after push  -> (OPTIONAL) error code
    * NOTE: the asm interrupt handler pushes a dummy error code incase it isn't pushed by the cpu.

    * in the assembly interrupt handler, I give the address to error code in rdi.
    * because the stack grows downwards, 
    * and because a C struct with a base address Y adds N bytes to access each element,
    * that means that the stack_state struct should be ordered in the opposite manner
    * of the one shown in the stack. 
*/
typedef struct pack __stack_state
{
    // control registers
    uint64_t cr8;
    uint64_t cr4;
    uint64_t cr3;
    uint64_t cr2;
    uint64_t cr0;
    // scratch registers
    uint64_t r11;  
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
    // gets pushed automatically during interrupt by cpu. (besides irqn)
    uint64_t irqn;  // which interrupt called this.
    uint64_t ecode; 
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} stack_state;


//                                RDI (System V ABI x86_64)     
void __noinline interrupt_handler(stack_state* sst);




extern void inoe_handler0();
extern void inoe_handler1();
extern void inoe_handler2();
extern void inoe_handler3();
extern void inoe_handler4();
extern void inoe_handler5();
extern void inoe_handler6();
extern void inoe_handler7();
extern void ie_handler8();
extern void ie_handler10();
extern void ie_handler11();
extern void ie_handler12();
extern void ie_handler13();
extern void ie_handler14();
extern void inoe_handler16();
extern void ie_handler17();
extern void inoe_handler18();
extern void inoe_handler19();
extern void inoe_handler20();
extern void ie_handler29();
extern void ie_handler30();




extern void inoe_handler32();
extern void inoe_handler33();
extern void inoe_handler34();
extern void inoe_handler35();
extern void inoe_handler36();
extern void inoe_handler37();
extern void inoe_handler38();
extern void inoe_handler39();
extern void inoe_handler40();
extern void inoe_handler41();
extern void inoe_handler42();
extern void inoe_handler43();
extern void inoe_handler44();
extern void inoe_handler45();
extern void inoe_handler46();
extern void inoe_handler47();
extern void inoe_handler48();
extern void inoe_handler49();
extern void inoe_handler50();
extern void inoe_handler51();
extern void inoe_handler52();
extern void inoe_handler53();
extern void inoe_handler54();
extern void inoe_handler55();
extern void inoe_handler56();
extern void inoe_handler57();
extern void inoe_handler58();
extern void inoe_handler59();
extern void inoe_handler60();
extern void inoe_handler61();
extern void inoe_handler62();
extern void inoe_handler63();
extern void inoe_handler64();
extern void inoe_handler65();
extern void inoe_handler66();
extern void inoe_handler67();
extern void inoe_handler68();
extern void inoe_handler69();
extern void inoe_handler70();
extern void inoe_handler71();
extern void inoe_handler72();
extern void inoe_handler73();
extern void inoe_handler74();
extern void inoe_handler75();
extern void inoe_handler76();
extern void inoe_handler77();
extern void inoe_handler78();
extern void inoe_handler79();
extern void inoe_handler80();
extern void inoe_handler81();
extern void inoe_handler82();
extern void inoe_handler83();
extern void inoe_handler84();
extern void inoe_handler85();
extern void inoe_handler86();
extern void inoe_handler87();
extern void inoe_handler88();
extern void inoe_handler89();
extern void inoe_handler90();
extern void inoe_handler91();
extern void inoe_handler92();
extern void inoe_handler93();
extern void inoe_handler94();
extern void inoe_handler95();
extern void inoe_handler96();
extern void inoe_handler97();
extern void inoe_handler98();
extern void inoe_handler99();
extern void inoe_handler100();
extern void inoe_handler101();
extern void inoe_handler102();
extern void inoe_handler103();
extern void inoe_handler104();
extern void inoe_handler105();
extern void inoe_handler106();
extern void inoe_handler107();
extern void inoe_handler108();
extern void inoe_handler109();
extern void inoe_handler110();
extern void inoe_handler111();
extern void inoe_handler112();
extern void inoe_handler113();
extern void inoe_handler114();
extern void inoe_handler115();
extern void inoe_handler116();
extern void inoe_handler117();
extern void inoe_handler118();
extern void inoe_handler119();
extern void inoe_handler120();
extern void inoe_handler121();
extern void inoe_handler122();
extern void inoe_handler123();
extern void inoe_handler124();
extern void inoe_handler125();
extern void inoe_handler126();
extern void inoe_handler127();
extern void inoe_handler128();
extern void inoe_handler129();
extern void inoe_handler130();
extern void inoe_handler131();
extern void inoe_handler132();
extern void inoe_handler133();
extern void inoe_handler134();
extern void inoe_handler135();
extern void inoe_handler136();
extern void inoe_handler137();
extern void inoe_handler138();
extern void inoe_handler139();
extern void inoe_handler140();
extern void inoe_handler141();
extern void inoe_handler142();
extern void inoe_handler143();
extern void inoe_handler144();
extern void inoe_handler145();
extern void inoe_handler146();
extern void inoe_handler147();
extern void inoe_handler148();
extern void inoe_handler149();
extern void inoe_handler150();
extern void inoe_handler151();
extern void inoe_handler152();
extern void inoe_handler153();
extern void inoe_handler154();
extern void inoe_handler155();
extern void inoe_handler156();
extern void inoe_handler157();
extern void inoe_handler158();
extern void inoe_handler159();
extern void inoe_handler160();
extern void inoe_handler161();
extern void inoe_handler162();
extern void inoe_handler163();
extern void inoe_handler164();
extern void inoe_handler165();
extern void inoe_handler166();
extern void inoe_handler167();
extern void inoe_handler168();
extern void inoe_handler169();
extern void inoe_handler170();
extern void inoe_handler171();
extern void inoe_handler172();
extern void inoe_handler173();
extern void inoe_handler174();
extern void inoe_handler175();
extern void inoe_handler176();
extern void inoe_handler177();
extern void inoe_handler178();
extern void inoe_handler179();
extern void inoe_handler180();
extern void inoe_handler181();
extern void inoe_handler182();
extern void inoe_handler183();
extern void inoe_handler184();
extern void inoe_handler185();
extern void inoe_handler186();
extern void inoe_handler187();
extern void inoe_handler188();
extern void inoe_handler189();
extern void inoe_handler190();
extern void inoe_handler191();
extern void inoe_handler192();
extern void inoe_handler193();
extern void inoe_handler194();
extern void inoe_handler195();
extern void inoe_handler196();
extern void inoe_handler197();
extern void inoe_handler198();
extern void inoe_handler199();
extern void inoe_handler200();
extern void inoe_handler201();
extern void inoe_handler202();
extern void inoe_handler203();
extern void inoe_handler204();
extern void inoe_handler205();
extern void inoe_handler206();
extern void inoe_handler207();
extern void inoe_handler208();
extern void inoe_handler209();
extern void inoe_handler210();
extern void inoe_handler211();
extern void inoe_handler212();
extern void inoe_handler213();
extern void inoe_handler214();
extern void inoe_handler215();
extern void inoe_handler216();
extern void inoe_handler217();
extern void inoe_handler218();
extern void inoe_handler219();
extern void inoe_handler220();
extern void inoe_handler221();
extern void inoe_handler222();
extern void inoe_handler223();
extern void inoe_handler224();
extern void inoe_handler225();
extern void inoe_handler226();
extern void inoe_handler227();
extern void inoe_handler228();
extern void inoe_handler229();
extern void inoe_handler230();
extern void inoe_handler231();
extern void inoe_handler232();
extern void inoe_handler233();
extern void inoe_handler234();
extern void inoe_handler235();
extern void inoe_handler236();
extern void inoe_handler237();
extern void inoe_handler238();
extern void inoe_handler239();
extern void inoe_handler240();
extern void inoe_handler241();
extern void inoe_handler242();
extern void inoe_handler243();
extern void inoe_handler244();
extern void inoe_handler245();
extern void inoe_handler246();
extern void inoe_handler247();
extern void inoe_handler248();
extern void inoe_handler249();
extern void inoe_handler250();
extern void inoe_handler251();
extern void inoe_handler252();
extern void inoe_handler253();
extern void inoe_handler254();
extern void inoe_handler255();


#endif


