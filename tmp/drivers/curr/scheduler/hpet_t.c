#include "hpet_t.h"
#include "../acpi/irqmap.h"



#define RO
#define RW

uint64_t hpet_read_reg(uint64_t hpet_table_base, uint16_t offset)
{
    return *((volatile uint64_t*)(hpet_table_base + (uint64_t)offset));
}


void hpet_write_reg(uint64_t hpet_table_base, uint16_t offset, uint64_t data)
{
    *((volatile uint64_t*)(hpet_table_base + (uint64_t)offset)) = data;
}


uint64_t* hpet_reg_ptr(uint64_t hpet_table_base, uint16_t offset)
{
    return (uint64_t*)(volatile uint64_t*)(hpet_table_base + offset);
}


static inline void enableHPET(uint64_t base_reg)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_GENERAL_CONFIG_REG);
    *tmp |= 1;
    return;
}


static inline void disableHPET(uint64_t base_reg)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_GENERAL_CONFIG_REG);
    *tmp &= ~1;
    return;
}


static inline void enableLegacyINT(uint64_t base_reg)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_GENERAL_CONFIG_REG);
    *tmp |= 2;
}


static inline void disableLegacyINT(uint64_t base_reg)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_GENERAL_CONFIG_REG);
    *tmp &= ~2;
}


static inline void restartHPET(uint64_t base_reg)
{
    disableHPET(base_reg);
    hpet_write_reg(base_reg, HPET_MAIN_COUNTER_VALUE_REG, 0);
    enableHPET(base_reg);
    return;
}

static inline uint64_t readCounter(uint64_t base_reg)
{
    disableHPET(base_reg);
    uint64_t out = hpet_read_reg(base_reg, HPET_MAIN_COUNTER_VALUE_REG);
    enableHPET(base_reg);
    return out;
}


static inline void enableTimerINT(uint64_t base_reg, uint8_t timeridx)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_TIMER_N_CAPABILITES_ID_REG(timeridx));
    *tmp |= 2;
}


static inline void disableTimerINT(uint64_t base_reg, uint8_t timeridx)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_TIMER_N_CAPABILITES_ID_REG(timeridx));
    *tmp &= ~2;
}


static inline void startTimer(uint64_t base_reg, uint8_t timeridx, bool_t periodic, uint64_t time)
{
    uint64_t* tmp = hpet_reg_ptr((uint64_t)base_reg, HPET_TIMER_N_CAPABILITES_ID_REG(timeridx));

    *tmp |= 2; // enable interrupts on timer.
    *tmp |= ((HPET_TN_CONFIGCAPREG_READ_PER_CAPABLE(*tmp) && periodic) << 4); // enable periodic mode if it is capable of doing so.
    hpet_write_reg(base_reg, HPET_TIMER_N_COMPARATOR_VAL_REG(timeridx), time); // update comparator
}


void hpet_init(HPET_t* hpet)
{
    uint64_t reg_base = hpet->tableaddr.address;
    uint32_t period   = HPET_GCIREG_READ_COUNTER_CLOCK_PERIOD(reg_base);
    uint64_t mintick  = hpet->min_tick;
    uint64_t timers   = HPET_GCIREG_READ_TIMER_COUNT(reg_base);

    // uint64_t* config_reg = (uint64_t*)(reg_base + HPET_GENERAL_CONFIG_REG);
    // printk("%p %b\n", config_reg, *config_reg);
    // hpet_write_reg(reg_base, HPET_GENERAL_CONFIG_REG, 1);
    // printk("%p %b\n", config_reg, *config_reg);
    // hpet_write_reg(reg_base, HPET_GENERAL_CONFIG_REG, 0);
    // printk("%p %b\n", config_reg, *config_reg);

    printk("counter is %u: \n", readCounter(reg_base));
    disableTimerINT(reg_base, 0);
    disableTimerINT(reg_base, 1);
    restartHPET(reg_base);
    for(volatile uint64_t i = 0; i < 1000000; ++i) {
        ++i;
        --i;
    }
    printk("counter is %u: \n", readCounter(reg_base));

    disableHPET(reg_base);
    IDT_ACCESS flags = {
        .gate_type   = GATE_TYPE_INTERRUPT32,
        .storage_seg = 0,
        .access_lvl  = 0,
        .present     = 1
    };
    
    enableLegacyINT(reg_base);
    // disableLegacyINT(reg_base);
    install_irq_vector(2, PRI_LVL0, ACTIVE_HIGH, EDGE, flags);
    install_irq_vector(8, PRI_LVL0, ACTIVE_HIGH, EDGE, flags);
    // hpet_write_reg(reg_base, HPET_TIMER_N_CAPABILITES_ID_REG(12), )
    __asm__("int $0x21");
    __asm__("int $0x21");
    // __asm__("int $0x21");

    disableTimerINT(reg_base, 0);
    disableTimerINT(reg_base, 1);
    startTimer(reg_base, 0, 0, 0x2fffffff);
}