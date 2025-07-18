#ifndef __KERNEL_C_DEFINITION_INTERRUPT_DESCRIPTOR_TABLE_HEADER__
#define __KERNEL_C_DEFINITION_INTERRUPT_DESCRIPTOR_TABLE_HEADER__
#include <stdint.h>
#include <std/macro.h>



#define GATE_TYPE_INTERRUPT32 0x0E 
#define GATE_TYPE_TRAP32      0x0F
#define GATE_TYPE_TASK32      0x05


#define PRI_LEVELS     14
#define PRI_LVLN_COUNT 16
#define PRI_LVL_START(lvl) (lvl << 4)
#define PRI_LVL_END(lvl)  ((lvl << 4) + PRI_LVLN_COUNT - 1)
#define IDT_GATE_PRI_LVL0  0x00 // Highest Priority
#define IDT_GATE_PRI_LVL1  0x01
#define IDT_GATE_PRI_LVL2  0x02
#define IDT_GATE_PRI_LVL3  0x03
#define IDT_GATE_PRI_LVL4  0x04
#define IDT_GATE_PRI_LVL5  0x05
#define IDT_GATE_PRI_LVL6  0x06
#define IDT_GATE_PRI_LVL7  0x07
#define IDT_GATE_PRI_LVL8  0x08
#define IDT_GATE_PRI_LVL9  0x09
#define IDT_GATE_PRI_LVL10 0x0A
#define IDT_GATE_PRI_LVL11 0x0B
#define IDT_GATE_PRI_LVL12 0x0C
#define IDT_GATE_PRI_LVL13 0x0D // Lowest Priority


typedef struct pack __idt_register
{
    uint16_t size;
    uint64_t address;
} idtreg_t;


typedef struct pack __descriptor_entry
{
    uint16_t offset0;   // offset bits 0..15
    uint16_t selector;  // a code segment selector in a GDT or LDT
    uint8_t  istoff;    // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attr; // type and attributes
    uint16_t offset1;   // offset bits 16..31
    uint32_t offset2;   // offset bits 32..63
    uint32_t zero;      // reserved
} IDT_ENTRY;



typedef struct __descriptor_type_attributes
{
    union {
        struct {
            uint8_t gate_type   : 4;
            uint8_t storage_seg : 1; // 0 for interrupts and trap gates
            uint8_t access_lvl  : 2; // 0 - kernel, 3 - user. minimum access for this interrupt
            uint8_t present     : 1; // active for a used interrupt 
        };
        uint8_t u8;
    };
} IDT_ACCESS;

typedef enum {
    KERNEL     = 0,
    SUPERVISOR = 1,
    UNDEFINED  = 2,
    USER       = 3
} ACCESS_LVL;



typedef struct pack __interrupt_stack_table_off
{
    union {
        struct {
            uint8_t offset : 3;
            uint8_t zero   : 5;    
        };
        uint8_t u8;
    };
} IST;


// typedef struct pack __interrupt_descriptor_table 
// {
//     IDT_ENTRY handles[256];
// } IDT;
typedef IDT_ENTRY IDT[256];


/* 
    * initializes all relevant entries of the idt. (exception handlers)
*/
void initialize_idt(void);


/* 
    * loads the idt pointer into the idtr register
*/
void load_idt(void);       


/* 
    * returns a pointer to the idtreg_t* used by the requesting Processor.
*/
idtreg_t* getKernelIDTR(void);


/* 
    * will print a message to the screen, 
    * informing the developer if an interrupt has been overriden properly \ has been enabled.
    * uint8_t n - the interrupt vector number which will be tested.
*/
void test_vector_at(uint8_t n);


/* 
    * allocates an IDT entry based on its interrupt priority. (returns an index to the idt entry)
    * algorithm:
    * looks at priority lvl N for a free IDT entry. if not found, exits with error message.
    * IDT_ENTRY_PRIORITY pri - the priority of the vector (defined in idt.h)
*/
uint8_t request_vector(uint8_t pri); 


/*
    * same as request vector, with 1 key difference:
    * if not found, will look at lvl N+1, N+2, ... , PRI_LEVEL_13 until it finds a free entry
    * if still not found, will exit with error message "Can't allocate interrupt vector from lvl N and up"
    * IDT_ENTRY_PRIORITY pri - the priority of the vector (defined in idt.h)
*/
uint8_t request_vector_min(uint8_t pri); 


/* 
    * will Free an entry at index n in the IDT
    * uint8_t n - the idt gate that will be cleared and disabled.
*/
void free_vector(uint8_t n);


/* 
    Note: This is how I map the interrupt handlers in the IDT:
        0 ->31      - Intel-Reserved Exception interrupts
        32->32+16*n - lvl N Priority interrupts; 0 <= N <= 13
        description: the higher lvl N (higher position\index in IDT) is, the lower it's priority in the IDT 
*/


/* 
    * initializes an entry in the IDT based on the given parameters.
    * uint8_t  idx         - the entry to initialize.
    * uint64_t haddress    - the address in memory of the function that handles the interrupt
    * uint8_t access_flags - gate_type  : which type of idt gate this is.
    *                        storage_seg: 0 for interrupt & trap gates. (didn't find the purpose of this yet)
    *                        access_lvl : the minimum (or lower) privilege level that cpu must have when calling this idt gate.
    *                        present    : if this is an active idt gate entry.
    *                        ** more info in the IDT_ACCESS struct 
    * 
    * uint8_t selector     - the offset to a Code segment in an active GDT/LDT (Kernel/User Code segment)
*/
void set_idt_gate(uint8_t idx, uint64_t haddress, uint8_t access_flags, uint8_t selector);


#endif /* __KERNEL_C_DEFINITION_INTERRUPT_DESCRIPTOR_TABLE_HEADER__ */
