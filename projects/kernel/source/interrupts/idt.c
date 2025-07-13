#include <interrupts/idt.h>
#include <interrupts/gdt.h>
#include <interrupts/handlers.h>
#include <interrupts/htable.h>
#include <std/string.h>
#include <std/io.h>
#include <std/error.h>
// #include "../mem/map_ptr.h"



static IDT  kidt;
static idtreg_t idt_reg;



void initialize_idt(void)
{
    IDT_ACCESS flags = { 
        .gate_type   = GATE_TYPE_INTERRUPT32,
        .storage_seg = 0,
        .access_lvl  = 0,
        .present     = 1  
    };
    

    memset(&kidt, 0, sizeof(IDT));
    // init all handler wrappers. irq, exception, and general interrupt handlers are defined in more detail in handlers.c
    set_idt_gate(0x00, (uint64_t)inoe_handler0 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x01, (uint64_t)inoe_handler1 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x02, (uint64_t)inoe_handler2 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x03, (uint64_t)inoe_handler3 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x04, (uint64_t)inoe_handler4 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x05, (uint64_t)inoe_handler5 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x06, (uint64_t)inoe_handler6 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x07, (uint64_t)inoe_handler7 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x08, (uint64_t)ie_handler8   , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x0a, (uint64_t)ie_handler10  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x0b, (uint64_t)ie_handler11  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x0c, (uint64_t)ie_handler12  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x0d, (uint64_t)ie_handler13  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x0e, (uint64_t)ie_handler14  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x10, (uint64_t)inoe_handler16, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x11, (uint64_t)ie_handler17  , flags.u8+0, GDT_CS_KERNEL);
    set_idt_gate(0x12, (uint64_t)inoe_handler18, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x13, (uint64_t)inoe_handler19, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x14, (uint64_t)inoe_handler20, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x1e, (uint64_t)ie_handler30  , flags.u8+1, GDT_CS_KERNEL);


    set_idt_gate(0x20,(uint64_t)inoe_handler32 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x21,(uint64_t)inoe_handler33 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x22,(uint64_t)inoe_handler34 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x23,(uint64_t)inoe_handler35 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x24,(uint64_t)inoe_handler36 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x25,(uint64_t)inoe_handler37 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x26,(uint64_t)inoe_handler38 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x27,(uint64_t)inoe_handler39 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x28,(uint64_t)inoe_handler40 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x29,(uint64_t)inoe_handler41 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2a,(uint64_t)inoe_handler42 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2b,(uint64_t)inoe_handler43 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2c,(uint64_t)inoe_handler44 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2d,(uint64_t)inoe_handler45 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2e,(uint64_t)inoe_handler46 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x2f,(uint64_t)inoe_handler47 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x30,(uint64_t)inoe_handler48 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x31,(uint64_t)inoe_handler49 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x32,(uint64_t)inoe_handler50 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x33,(uint64_t)inoe_handler51 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x34,(uint64_t)inoe_handler52 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x35,(uint64_t)inoe_handler53 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x36,(uint64_t)inoe_handler54 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x37,(uint64_t)inoe_handler55 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x38,(uint64_t)inoe_handler56 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x39,(uint64_t)inoe_handler57 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3a,(uint64_t)inoe_handler58 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3b,(uint64_t)inoe_handler59 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3c,(uint64_t)inoe_handler60 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3d,(uint64_t)inoe_handler61 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3e,(uint64_t)inoe_handler62 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x3f,(uint64_t)inoe_handler63 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x40,(uint64_t)inoe_handler64 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x41,(uint64_t)inoe_handler65 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x42,(uint64_t)inoe_handler66 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x43,(uint64_t)inoe_handler67 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x44,(uint64_t)inoe_handler68 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x45,(uint64_t)inoe_handler69 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x46,(uint64_t)inoe_handler70 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x47,(uint64_t)inoe_handler71 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x48,(uint64_t)inoe_handler72 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x49,(uint64_t)inoe_handler73 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4a,(uint64_t)inoe_handler74 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4b,(uint64_t)inoe_handler75 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4c,(uint64_t)inoe_handler76 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4d,(uint64_t)inoe_handler77 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4e,(uint64_t)inoe_handler78 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x4f,(uint64_t)inoe_handler79 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x50,(uint64_t)inoe_handler80 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x51,(uint64_t)inoe_handler81 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x52,(uint64_t)inoe_handler82 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x53,(uint64_t)inoe_handler83 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x54,(uint64_t)inoe_handler84 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x55,(uint64_t)inoe_handler85 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x56,(uint64_t)inoe_handler86 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x57,(uint64_t)inoe_handler87 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x58,(uint64_t)inoe_handler88 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x59,(uint64_t)inoe_handler89 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5a,(uint64_t)inoe_handler90 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5b,(uint64_t)inoe_handler91 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5c,(uint64_t)inoe_handler92 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5d,(uint64_t)inoe_handler93 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5e,(uint64_t)inoe_handler94 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x5f,(uint64_t)inoe_handler95 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x60,(uint64_t)inoe_handler96 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x61,(uint64_t)inoe_handler97 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x62,(uint64_t)inoe_handler98 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x63,(uint64_t)inoe_handler99 , flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x64,(uint64_t)inoe_handler100, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x65,(uint64_t)inoe_handler101, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x66,(uint64_t)inoe_handler102, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x67,(uint64_t)inoe_handler103, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x68,(uint64_t)inoe_handler104, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x69,(uint64_t)inoe_handler105, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6a,(uint64_t)inoe_handler106, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6b,(uint64_t)inoe_handler107, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6c,(uint64_t)inoe_handler108, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6d,(uint64_t)inoe_handler109, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6e,(uint64_t)inoe_handler110, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x6f,(uint64_t)inoe_handler111, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x70,(uint64_t)inoe_handler112, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x71,(uint64_t)inoe_handler113, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x72,(uint64_t)inoe_handler114, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x73,(uint64_t)inoe_handler115, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x74,(uint64_t)inoe_handler116, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x75,(uint64_t)inoe_handler117, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x76,(uint64_t)inoe_handler118, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x77,(uint64_t)inoe_handler119, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x78,(uint64_t)inoe_handler120, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x79,(uint64_t)inoe_handler121, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7a,(uint64_t)inoe_handler122, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7b,(uint64_t)inoe_handler123, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7c,(uint64_t)inoe_handler124, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7d,(uint64_t)inoe_handler125, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7e,(uint64_t)inoe_handler126, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x7f,(uint64_t)inoe_handler127, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x80,(uint64_t)inoe_handler128, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x81,(uint64_t)inoe_handler129, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x82,(uint64_t)inoe_handler130, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x83,(uint64_t)inoe_handler131, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x84,(uint64_t)inoe_handler132, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x85,(uint64_t)inoe_handler133, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x86,(uint64_t)inoe_handler134, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x87,(uint64_t)inoe_handler135, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x88,(uint64_t)inoe_handler136, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x89,(uint64_t)inoe_handler137, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8a,(uint64_t)inoe_handler138, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8b,(uint64_t)inoe_handler139, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8c,(uint64_t)inoe_handler140, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8d,(uint64_t)inoe_handler141, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8e,(uint64_t)inoe_handler142, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x8f,(uint64_t)inoe_handler143, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x90,(uint64_t)inoe_handler144, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x91,(uint64_t)inoe_handler145, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x92,(uint64_t)inoe_handler146, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x93,(uint64_t)inoe_handler147, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x94,(uint64_t)inoe_handler148, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x95,(uint64_t)inoe_handler149, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x96,(uint64_t)inoe_handler150, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x97,(uint64_t)inoe_handler151, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x98,(uint64_t)inoe_handler152, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x99,(uint64_t)inoe_handler153, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9a,(uint64_t)inoe_handler154, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9b,(uint64_t)inoe_handler155, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9c,(uint64_t)inoe_handler156, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9d,(uint64_t)inoe_handler157, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9e,(uint64_t)inoe_handler158, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0x9f,(uint64_t)inoe_handler159, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa0,(uint64_t)inoe_handler160, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa1,(uint64_t)inoe_handler161, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa2,(uint64_t)inoe_handler162, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa3,(uint64_t)inoe_handler163, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa4,(uint64_t)inoe_handler164, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa5,(uint64_t)inoe_handler165, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa6,(uint64_t)inoe_handler166, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa7,(uint64_t)inoe_handler167, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa8,(uint64_t)inoe_handler168, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xa9,(uint64_t)inoe_handler169, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xaa,(uint64_t)inoe_handler170, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xab,(uint64_t)inoe_handler171, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xac,(uint64_t)inoe_handler172, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xad,(uint64_t)inoe_handler173, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xae,(uint64_t)inoe_handler174, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xaf,(uint64_t)inoe_handler175, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb0,(uint64_t)inoe_handler176, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb1,(uint64_t)inoe_handler177, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb2,(uint64_t)inoe_handler178, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb3,(uint64_t)inoe_handler179, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb4,(uint64_t)inoe_handler180, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb5,(uint64_t)inoe_handler181, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb6,(uint64_t)inoe_handler182, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb7,(uint64_t)inoe_handler183, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb8,(uint64_t)inoe_handler184, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xb9,(uint64_t)inoe_handler185, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xba,(uint64_t)inoe_handler186, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xbb,(uint64_t)inoe_handler187, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xbc,(uint64_t)inoe_handler188, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xbd,(uint64_t)inoe_handler189, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xbe,(uint64_t)inoe_handler190, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xbf,(uint64_t)inoe_handler191, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc0,(uint64_t)inoe_handler192, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc1,(uint64_t)inoe_handler193, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc2,(uint64_t)inoe_handler194, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc3,(uint64_t)inoe_handler195, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc4,(uint64_t)inoe_handler196, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc5,(uint64_t)inoe_handler197, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc6,(uint64_t)inoe_handler198, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc7,(uint64_t)inoe_handler199, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc8,(uint64_t)inoe_handler200, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xc9,(uint64_t)inoe_handler201, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xca,(uint64_t)inoe_handler202, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xcb,(uint64_t)inoe_handler203, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xcc,(uint64_t)inoe_handler204, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xcd,(uint64_t)inoe_handler205, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xce,(uint64_t)inoe_handler206, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xcf,(uint64_t)inoe_handler207, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd0,(uint64_t)inoe_handler208, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd1,(uint64_t)inoe_handler209, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd2,(uint64_t)inoe_handler210, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd3,(uint64_t)inoe_handler211, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd4,(uint64_t)inoe_handler212, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd5,(uint64_t)inoe_handler213, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd6,(uint64_t)inoe_handler214, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd7,(uint64_t)inoe_handler215, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd8,(uint64_t)inoe_handler216, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xd9,(uint64_t)inoe_handler217, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xda,(uint64_t)inoe_handler218, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xdb,(uint64_t)inoe_handler219, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xdc,(uint64_t)inoe_handler220, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xdd,(uint64_t)inoe_handler221, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xde,(uint64_t)inoe_handler222, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xdf,(uint64_t)inoe_handler223, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe0,(uint64_t)inoe_handler224, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe1,(uint64_t)inoe_handler225, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe2,(uint64_t)inoe_handler226, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe3,(uint64_t)inoe_handler227, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe4,(uint64_t)inoe_handler228, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe5,(uint64_t)inoe_handler229, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe6,(uint64_t)inoe_handler230, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe7,(uint64_t)inoe_handler231, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe8,(uint64_t)inoe_handler232, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xe9,(uint64_t)inoe_handler233, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xea,(uint64_t)inoe_handler234, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xeb,(uint64_t)inoe_handler235, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xec,(uint64_t)inoe_handler236, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xed,(uint64_t)inoe_handler237, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xee,(uint64_t)inoe_handler238, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xef,(uint64_t)inoe_handler239, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf0,(uint64_t)inoe_handler240, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf1,(uint64_t)inoe_handler241, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf2,(uint64_t)inoe_handler242, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf3,(uint64_t)inoe_handler243, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf4,(uint64_t)inoe_handler244, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf5,(uint64_t)inoe_handler245, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf6,(uint64_t)inoe_handler246, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf7,(uint64_t)inoe_handler247, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf8,(uint64_t)inoe_handler248, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xf9,(uint64_t)inoe_handler249, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xfa,(uint64_t)inoe_handler250, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xfb,(uint64_t)inoe_handler251, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xfc,(uint64_t)inoe_handler252, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xfd,(uint64_t)inoe_handler253, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xfe,(uint64_t)inoe_handler254, flags.u8+1, GDT_CS_KERNEL);
    set_idt_gate(0xff,(uint64_t)inoe_handler255, flags.u8+1, GDT_CS_KERNEL);
}


void load_idt(void)
{
    idt_reg = (idtreg_t){ (uint16_t)(sizeof(IDT) - 1), (uint64_t)&kidt };
    __asm__ volatile("lidt %0" : : "m" (idt_reg));
}


idtreg_t* getKernelIDTR(void)
{
    return &idt_reg;
}


void set_idt_gate(uint8_t idx, uint64_t handler, uint8_t access_flags, uint8_t selector)
{
    DEBUG(       
        printk_align_format(2);
        printk("SET_IDT_ENTRY ==> i: %u *** ptr: %X *** flags: %b *** CS: %x\n", idx, handler, access_flags, selector);
        printk_align_format(0);
    )
    kidt[idx].offset0   =  handler        & 0xffff;
    kidt[idx].offset1   = (handler >> 16) & 0xffff;
    kidt[idx].offset2   = (handler >> 32) & 0xffffffff;
    kidt[idx].selector  = selector;
    kidt[idx].istoff    = 0;
    kidt[idx].type_attr = access_flags;
    return;
}


uint8_t request_vector(uint8_t pri)
{
    uint8_t  c       = 0;
    bool_t   free    = 0;
    uint16_t i       = PRI_LVL_START(pri);

    KERNEL_ERROR_IF(i > 255, 0, BOOLEAN_TRUE, "request_vector(...) => overflow on input variable 'uint8_t pri' ");
    for(; i < PRI_LVL_END(pri) && !free; ++i)
    {
        free = isHandleFree(i); // uninitialized (000..) entry is free. memcmp = 0 if true. 
        c    = i * free;        // keep i if it is free.
    }
    if(unlikely(c == 0)) {
        const char_t* pri_strs[14] = {
            "PRI_LVL0 ", "PRI_LVL1 ", "PRI_LVL2 ", "PRI_LVL3 ", "PRI_LVL4 ", "PRI_LVL5 ", "PRI_LVL6 ",
            "PRI_LVL7 ", "PRI_LVL8 ", "PRI_LVL9 ", "PRI_LVL10", "PRI_LVL11", "PRI_LVL12", "PRI_LVL13"
        };
        printk("Can't allocate interrupt vector [priority %s]: No available slots.\n", pri_strs[pri]);
        return 0x00; // invalid entry number, 0 is intel reserved.
    }

    return c;
}


uint8_t request_vector_min(uint8_t pri)
{
    bool_t  found   = BOOLEAN_FALSE;
    uint8_t slotidx = 0;

    for(uint8_t i = pri; i <= IDT_GATE_PRI_LVL13 && !found; ++i)
    {
        for(uint8_t vec = PRI_LVL_START(pri); vec < PRI_LVL_END(pri) && !found; ++vec)
        {
            found   = isHandleFree(vec);
            slotidx = vec * found; 
        }
    }
    if(unlikely(!found))
    {
        static const char_t* pri_strs[14] = {
            "PRI_LVL0 ", "PRI_LVL1 ", "PRI_LVL2 ", "PRI_LVL3 ", "PRI_LVL4 ", "PRI_LVL5 ", "PRI_LVL6 ",
            "PRI_LVL7 ", "PRI_LVL8 ", "PRI_LVL9 ", "PRI_LVL10", "PRI_LVL11", "PRI_LVL12", "PRI_LVL13"
        };
        printk("Can't allocate interrupt vectors from %s's and up. \n", pri_strs[pri]);
        return 0xFF;
    }

    return slotidx;
}


void free_vector(uint8_t n) {
    set_handler(n, NULLPTR);
}
