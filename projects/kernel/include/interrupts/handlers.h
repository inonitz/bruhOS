#ifndef __INTERRUPT_HANDLERS__
#define __INTERRUPT_HANDLERS__
#include <std/stdint.h>
#include <std/macro.h>



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




extern void inoe_handler0(void);
extern void inoe_handler1(void);
extern void inoe_handler2(void);
extern void inoe_handler3(void);
extern void inoe_handler4(void);
extern void inoe_handler5(void);
extern void inoe_handler6(void);
extern void inoe_handler7(void);
extern void ie_handler8(void);
extern void ie_handler10(void);
extern void ie_handler11(void);
extern void ie_handler12(void);
extern void ie_handler13(void);
extern void ie_handler14(void);
extern void inoe_handler16(void);
extern void ie_handler17(void);
extern void inoe_handler18(void);
extern void inoe_handler19(void);
extern void inoe_handler20(void);
extern void ie_handler29(void);
extern void ie_handler30(void);




extern void inoe_handler32(void);
extern void inoe_handler33(void);
extern void inoe_handler34(void);
extern void inoe_handler35(void);
extern void inoe_handler36(void);
extern void inoe_handler37(void);
extern void inoe_handler38(void);
extern void inoe_handler39(void);
extern void inoe_handler40(void);
extern void inoe_handler41(void);
extern void inoe_handler42(void);
extern void inoe_handler43(void);
extern void inoe_handler44(void);
extern void inoe_handler45(void);
extern void inoe_handler46(void);
extern void inoe_handler47(void);
extern void inoe_handler48(void);
extern void inoe_handler49(void);
extern void inoe_handler50(void);
extern void inoe_handler51(void);
extern void inoe_handler52(void);
extern void inoe_handler53(void);
extern void inoe_handler54(void);
extern void inoe_handler55(void);
extern void inoe_handler56(void);
extern void inoe_handler57(void);
extern void inoe_handler58(void);
extern void inoe_handler59(void);
extern void inoe_handler60(void);
extern void inoe_handler61(void);
extern void inoe_handler62(void);
extern void inoe_handler63(void);
extern void inoe_handler64(void);
extern void inoe_handler65(void);
extern void inoe_handler66(void);
extern void inoe_handler67(void);
extern void inoe_handler68(void);
extern void inoe_handler69(void);
extern void inoe_handler70(void);
extern void inoe_handler71(void);
extern void inoe_handler72(void);
extern void inoe_handler73(void);
extern void inoe_handler74(void);
extern void inoe_handler75(void);
extern void inoe_handler76(void);
extern void inoe_handler77(void);
extern void inoe_handler78(void);
extern void inoe_handler79(void);
extern void inoe_handler80(void);
extern void inoe_handler81(void);
extern void inoe_handler82(void);
extern void inoe_handler83(void);
extern void inoe_handler84(void);
extern void inoe_handler85(void);
extern void inoe_handler86(void);
extern void inoe_handler87(void);
extern void inoe_handler88(void);
extern void inoe_handler89(void);
extern void inoe_handler90(void);
extern void inoe_handler91(void);
extern void inoe_handler92(void);
extern void inoe_handler93(void);
extern void inoe_handler94(void);
extern void inoe_handler95(void);
extern void inoe_handler96(void);
extern void inoe_handler97(void);
extern void inoe_handler98(void);
extern void inoe_handler99(void);
extern void inoe_handler100(void);
extern void inoe_handler101(void);
extern void inoe_handler102(void);
extern void inoe_handler103(void);
extern void inoe_handler104(void);
extern void inoe_handler105(void);
extern void inoe_handler106(void);
extern void inoe_handler107(void);
extern void inoe_handler108(void);
extern void inoe_handler109(void);
extern void inoe_handler110(void);
extern void inoe_handler111(void);
extern void inoe_handler112(void);
extern void inoe_handler113(void);
extern void inoe_handler114(void);
extern void inoe_handler115(void);
extern void inoe_handler116(void);
extern void inoe_handler117(void);
extern void inoe_handler118(void);
extern void inoe_handler119(void);
extern void inoe_handler120(void);
extern void inoe_handler121(void);
extern void inoe_handler122(void);
extern void inoe_handler123(void);
extern void inoe_handler124(void);
extern void inoe_handler125(void);
extern void inoe_handler126(void);
extern void inoe_handler127(void);
extern void inoe_handler128(void);
extern void inoe_handler129(void);
extern void inoe_handler130(void);
extern void inoe_handler131(void);
extern void inoe_handler132(void);
extern void inoe_handler133(void);
extern void inoe_handler134(void);
extern void inoe_handler135(void);
extern void inoe_handler136(void);
extern void inoe_handler137(void);
extern void inoe_handler138(void);
extern void inoe_handler139(void);
extern void inoe_handler140(void);
extern void inoe_handler141(void);
extern void inoe_handler142(void);
extern void inoe_handler143(void);
extern void inoe_handler144(void);
extern void inoe_handler145(void);
extern void inoe_handler146(void);
extern void inoe_handler147(void);
extern void inoe_handler148(void);
extern void inoe_handler149(void);
extern void inoe_handler150(void);
extern void inoe_handler151(void);
extern void inoe_handler152(void);
extern void inoe_handler153(void);
extern void inoe_handler154(void);
extern void inoe_handler155(void);
extern void inoe_handler156(void);
extern void inoe_handler157(void);
extern void inoe_handler158(void);
extern void inoe_handler159(void);
extern void inoe_handler160(void);
extern void inoe_handler161(void);
extern void inoe_handler162(void);
extern void inoe_handler163(void);
extern void inoe_handler164(void);
extern void inoe_handler165(void);
extern void inoe_handler166(void);
extern void inoe_handler167(void);
extern void inoe_handler168(void);
extern void inoe_handler169(void);
extern void inoe_handler170(void);
extern void inoe_handler171(void);
extern void inoe_handler172(void);
extern void inoe_handler173(void);
extern void inoe_handler174(void);
extern void inoe_handler175(void);
extern void inoe_handler176(void);
extern void inoe_handler177(void);
extern void inoe_handler178(void);
extern void inoe_handler179(void);
extern void inoe_handler180(void);
extern void inoe_handler181(void);
extern void inoe_handler182(void);
extern void inoe_handler183(void);
extern void inoe_handler184(void);
extern void inoe_handler185(void);
extern void inoe_handler186(void);
extern void inoe_handler187(void);
extern void inoe_handler188(void);
extern void inoe_handler189(void);
extern void inoe_handler190(void);
extern void inoe_handler191(void);
extern void inoe_handler192(void);
extern void inoe_handler193(void);
extern void inoe_handler194(void);
extern void inoe_handler195(void);
extern void inoe_handler196(void);
extern void inoe_handler197(void);
extern void inoe_handler198(void);
extern void inoe_handler199(void);
extern void inoe_handler200(void);
extern void inoe_handler201(void);
extern void inoe_handler202(void);
extern void inoe_handler203(void);
extern void inoe_handler204(void);
extern void inoe_handler205(void);
extern void inoe_handler206(void);
extern void inoe_handler207(void);
extern void inoe_handler208(void);
extern void inoe_handler209(void);
extern void inoe_handler210(void);
extern void inoe_handler211(void);
extern void inoe_handler212(void);
extern void inoe_handler213(void);
extern void inoe_handler214(void);
extern void inoe_handler215(void);
extern void inoe_handler216(void);
extern void inoe_handler217(void);
extern void inoe_handler218(void);
extern void inoe_handler219(void);
extern void inoe_handler220(void);
extern void inoe_handler221(void);
extern void inoe_handler222(void);
extern void inoe_handler223(void);
extern void inoe_handler224(void);
extern void inoe_handler225(void);
extern void inoe_handler226(void);
extern void inoe_handler227(void);
extern void inoe_handler228(void);
extern void inoe_handler229(void);
extern void inoe_handler230(void);
extern void inoe_handler231(void);
extern void inoe_handler232(void);
extern void inoe_handler233(void);
extern void inoe_handler234(void);
extern void inoe_handler235(void);
extern void inoe_handler236(void);
extern void inoe_handler237(void);
extern void inoe_handler238(void);
extern void inoe_handler239(void);
extern void inoe_handler240(void);
extern void inoe_handler241(void);
extern void inoe_handler242(void);
extern void inoe_handler243(void);
extern void inoe_handler244(void);
extern void inoe_handler245(void);
extern void inoe_handler246(void);
extern void inoe_handler247(void);
extern void inoe_handler248(void);
extern void inoe_handler249(void);
extern void inoe_handler250(void);
extern void inoe_handler251(void);
extern void inoe_handler252(void);
extern void inoe_handler253(void);
extern void inoe_handler254(void);
extern void inoe_handler255(void);


#endif


