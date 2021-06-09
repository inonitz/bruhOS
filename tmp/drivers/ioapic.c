#include "ioapic.h"



void read_register_table(uint64_t tableaddress)
{
    intredirectEntry_t tmp0;
    uint32_t maxirqs = ioapic_max_int(tableaddress); 
    printk("  irqn delivMode destMode delivStatus pinPolarity remoteIRR triggerMode bitmask destination\n\n");
    printk_align_format(3);
    for(uint8_t i = 0; i < maxirqs; i += 2)
    {
        tmp0 = (intredirectEntry_t){ 
            .lower = ioapic_read_reg(tableaddress, IOREDTBL_OFFSET_LOW(i)),
            .upper = ioapic_read_reg(tableaddress, IOREDTBL_OFFSET_HIGH(i)) 
        };
        // printk("%b\n", 
        //     tmp0.u64
        // ); 
        // printk("%u  %u       %u %u %u %u %u %u %u\n",
        //     tmp0.irqn,
        //     tmp0.delivMode,
        //     tmp0.destMode,
        //     tmp0.delivStatus,
        //     tmp0.pinPolarity,
        //     tmp0.remoteIRR,
        //     tmp0.triggerMode,
        //     tmp0.bitmask,
        //     tmp0.destination
        // );
    }
}


void init_lapic() {}
void init_ioAPIC() {}