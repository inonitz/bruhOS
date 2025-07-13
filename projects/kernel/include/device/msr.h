#ifndef __MODEL_SPECIFIC_REGISTERS__
#define __MODEL_SPECIFIC_REGISTERS__
#include <std/int.h>
#include <std/macro.h>


#define IA32_MSR_ECX_EFER          0xC0000080
#define IA32_MSR_ECX_STAR          0xC0000081
#define IA32_MSR_ECX_LSTAR         0xC0000082
#define IA32_MSR_ECX_CSTAR         0xC0000083
#define IA32_MSR_ECX_FMASK         0xC0000084
#define IA32_MSR_ECX_FSBASE        0xC0000100
#define IA32_MSR_ECX_GSBASE        0xC0000101
#define IA32_MSR_ECX_KERNEL_GSBASE 0xC0000102
#define IA32_MSR_ECX_PAT           0x00000277
#define IA32_MSR_ECX_TSC_DEADLINE  0x6E0
#define IA32_MSR_ECX_APIC_BASE     0x1B



static __force_inline uint64_t rdtsc()
{
    uint32_t out;
    __asm__ volatile("rdtsc" : "=A"(out));
    return out;
}
static __force_inline uint64_t rdmsr(uint64_t msr)
{
    uint32_t low;
    uint32_t high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return (( (uint64_t)high) << 32) | low;
}


static __force_inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t highedx = value >> 32;
    uint32_t loweax  = value & 0xFFFFFFFF;
    __asm__ volatile("wrmsr" 
        :
        : "c"(msr), "d"(highedx), "a"(loweax)
    );
}


#endif
