#include "hpet.h"
#include "../acpi/irqmap.h"
#include "../interrupts/handlers.h"



#define GENERAL_CAPABILITIES_ID_REG_OFFSET  0x000
#define GENERAL_CONFIGURATION_REG_OFFSET    0x010
#define GENERAL_INTERRUPT_STATUS_REG_OFFSET 0x020
#define MAIN_COUNTER_VALUE_REG_OFFSET       0x0F0
#define TIMER_CONFIGCAP_REG_OFFSET(n) (0x100 + 0x20 * n)
#define TIMER_CMPVAL_REG_OFFSET(n)    (0x108 + 0x20 * n)
#define TIMER_FSB_IRR_OFFSET(n)       (0x110 + 0x20 * n)

typedef enum { 
    HPET_TIMER_ISA, 
    HPET_TIMER_APIC, 
    HPET_TIMER_PCI 
} TIMER_CONFIG;


void config_timer_registers(HPET_REG_TABLE_t* TNstart, uint8_t count, TIMER_CONFIG config)
{
    TN_CONFIG_CAP_REG* config_reg;
    TN_CMPVAL_REG*     tncomp;
    IDT_ACCESS         flags;

    for(uint8_t i = 0; i < count; ++i)
    {
        config_reg = &TNstart->TimerN[i].cpr;
        config_reg->fsb_map    = 0; // NO FSB MAPPING!
        config_reg->periodic_t = 0; // for now, only one-shot mode
    }
    switch(config)
    {
        case HPET_TIMER_ISA:
            flags = (IDT_ACCESS){
                .gate_type   = GATE_TYPE_INTERRUPT32,
                .storage_seg = 0,
                .access_lvl  = 0, 
                .present     = 1
            };
            config_reg = &TNstart->TimerN[0].cpr;
            tncomp     = &TNstart->TimerN[0].comp;
            config_reg->int_type     = 0; // edge triggered.
            config_reg->int_enb_cnf  = 1; // activate interrupts for this timer
            config_reg->ioapic_route = 0;
            tncomp->u32 = 0;
            if(config_reg->size64)
                tncomp->u64 = 0; 

            install_irq_vector(0, PRI_LVL0, ACTIVE_HIGH, EDGE, flags, (uint64_t)HPET_Timer_Handler);     

            config_reg = &TNstart->TimerN[1].cpr;
            tncomp     = &TNstart->TimerN[1].comp;
            config_reg->int_type     = 0;
            config_reg->int_enb_cnf  = 1;
            config_reg->u64         |= (8 << 8); // set ioapic_route to active.
            // printk("BAHBAHBAHNANANANA %b\n", config_reg->u64);
            tncomp->u32 = 0;
            if(config_reg->size64)
                tncomp->u64 = 0; 

            install_irq_vector(8, PRI_LVL0, ACTIVE_HIGH, EDGE, flags, (uint64_t)HPET_Timer_Handler);     
            break;

        case HPET_TIMER_APIC:
            for(uint8_t i = 0; i < count; ++i)
            {
                config_reg               = &TNstart->TimerN[i].cpr;
                tncomp                   = &TNstart->TimerN[i].comp;
                config_reg->u64         |= (lsbui(config_reg->apic_irq_bitx) << 8); // set ioapic_route
                config_reg->int_enb_cnf  = 1;

                tncomp->u32 = 0;
                if(config_reg->size64)
                    tncomp->u64 = 0;

                install_irq_vector(
                    config_reg->u64 & (0x1f << 8),
                    PRI_LVL0, 
                    ACTIVE_LOW * config_reg->int_type + ACTIVE_HIGH * !config_reg->int_type, // if edge -> active high. if level -> active low
                    config_reg->int_type * LEVEL + !config_reg->int_type * EDGE,
                    flags,
                    (uint64_t)HPET_Timer_Handler
                );
            }
            break;
        case HPET_TIMER_PCI:
            printk("\nFSB-PCI HPET Timer is UNSUPPORTED\n");
            break;
    }
    return;
}



void hpet_init(HPET_t* hpet)
{
    HPET_REG_TABLE_t* regs   = (HPET_REG_TABLE_t*)hpet->regtable.address;
    GENCAP_ID_REG*    reg    = &regs->capabilites_id_reg;
    CONFIGREG*        config = &regs->config_reg;
    COUNT_REG*        count  = &regs->main_counter_reg;
    bool_t            m64    = hpet->counter_size;
    uint32_t          period = ONE_OVER_FEMTOSECOND / reg->clock_period;
    uint16_t          mtick  = hpet->minimum_tick; 
    uint8_t           timerc = ((reg->low >> 8)+1) & 0x1f;


    // initialize main counter    
    config->enable_cnf = 0; // halt the timer, so we can write to main counter
    count->cntval      = 0;
    timer = (hpet_timer){
        .lcntrs    = m64,
        .enabled   = config->enable_cnf,
        .config    = HPET_TIMER_ISA,
        .reserved0 = 0,
        .tickmin   = hpet->minimum_tick,
        .period    = period,
        .registers = regs,
        .table     = hpet,
        .reserved1 = 0
    };
    memset(timer.irqs, 0, sizeof(uint8_t) * 32);
    for(uint8_t i = 0; i < timerc; ++i) {
        timer.irqs[i] = (regs->TimerN[i].cpr.u64 >> 8) & 0x1f; // because regs->TimerN[i].cpr.ioapic_route doesn't work... 
    }


    // initialize comparator counters
    config_timer_registers(regs, timerc, HPET_TIMER_ISA);
    config->leg_rt_cnf = 1; // enable legacy mapping (0 -> timer #0, 8 -> timer #1).
    timer.frtimer      = 0;
    timer.enabled      = 0;


    printk("HPET Timer Capabilities: \n  64Bit Main Counter: %u\n  Legacy ISA Mappable: %u\n  Available Timers: %x\n  Clock Period (fms): %x\n\n", 
        hpet->counter_size,
        (reg->low >> 15) & 1,
        timerc,
        reg->clock_period
    );
    // for(uint8_t i = 0; i < timerc; ++i)
    // {
    //     printk("Timer %u:\n", i);
    //     printk(" Configuration: \n");
    //     printk("  64bit: %u\n  Interrupt Type: %u\n  Enabled %u\n  Periodic Mode Capable: %u (on = %u)\n", 
    //         regs->TimerN[i].cpr.size64,
    //         regs->TimerN[i].cpr.int_type,
    //         regs->TimerN[i].cpr.int_enb_cnf,
    //         regs->TimerN[i].cpr.period_cap,
    //         regs->TimerN[i].cpr.periodic_t
    //     );
    //     printk("  I/O APIC Routes that can be connected: %b\n  Currently Connected To IRQ%u\n", 
    //         (uint64_t)regs->TimerN[i].cpr.apic_irq_bitx,
    //         (regs->TimerN[i].cpr.u64 >> 8) & 0x1f
    //     );
    //     if(regs->TimerN[i].cpr.size64) {
    //         printk(" Comparator: \n  Value: %x\n", regs->TimerN[i].comp.u64);
    //     }
    // }
}



void hpet_enable()  { timer.registers->config_reg.enable_cnf = 1; }
void hpet_disable() { timer.registers->config_reg.enable_cnf = 0; }
void hpet_restart()
{
    timer.registers->config_reg.enable_cnf      = 0; // disable the timer
    timer.registers->main_counter_reg.cnt_lower = 0; // reset the counter (0 ->31).
    timer.registers->main_counter_reg.cnt_upper = 0; // reset the counter (32->63).
    timer.registers->config_reg.enable_cnf      = 1; // enable  the timer
}
