#include <acpi/pic.h>
#include <std/macro.h>
#include <std/error.h>



// define's taken from https://github.com/pdoane/osdev/blob/master/time/pit.c.
// I don't give enough shit about this timer that I'm going to write comprehensive
// and understandable code for it.
#define PIT_COUNTER0                    0x40
#define PIT_CMD                         0x43

#define CMD_BINARY                      0x00   // Use Binary counter values
#define CMD_MODE1                       0x02   // Hardware Retriggerable One-Shot
#define CMD_RW_BOTH                     0x30   // Least followed by Most Significant Byte
#define CMD_COUNTER0                    0x00
#define PIT_FREQUENCY                   1193182


static void __noinline outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
static uint8_t __noinline inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


extern void pic_disable_asm(void);


void disable_pic(void)
{
    /* Set ICW1 */
    outb(0x20, 0x11); // init master
    outb(0xa0, 0x11); // init slave

    /* Set ICW2 (IRQ base offsets) */
    outb(0x21, 0xe0);
    outb(0xa1, 0xe8);

    /* Set ICW3 */
    outb(0x21, 4);
    outb(0xa1, 2);

    /* Set ICW4 */
    outb(0x21, 1);
    outb(0xa1, 1);

    /* Set OCW1 (interrupt masks) */
    outb(0x21, 0xff);
    outb(0xa1, 0xff);

    // this code section configures the pit timer to mode 1 (one-shot)
    // so that it'll interrupt only once,
    // and i'll be able to not touch this timer ever again (and finally use the HPET).
    uint16_t div = PIT_FREQUENCY / 1000;
    outb(PIT_CMD, CMD_COUNTER0 | CMD_RW_BOTH | CMD_MODE1 | CMD_BINARY);
    outb(0x40, div     );
    outb(0x40, div >> 8);

    pic_disable_asm();
    return;
}
