#include <std/bit.h>
#include <std/static_assert.h>


KERNEL_STATIC_ASSERT(
    sizeof(uint32_t) == sizeof(int), 
    
    Kernel_lsbui_MakeSure_LeastSignificantBit_BitsAreComparableInSize, 
    "lsbui - sizeof(uint32_t) != sizeof(int) Types are not comparable in size, whatever was compiled may exhibit undefined behaviour"
);
KERNEL_STATIC_ASSERT(
    sizeof(uint64_t) == sizeof(long long),
    
    Kernel_lsbul_MakeSure_LeastSignificantBit_BitsAreComparableInSize, 
    "lsbui - sizeof(uint64_t) != sizeof(long) Types are not comparable in size, whatever was compiled may exhibit undefined behaviour"
);
KERNEL_STATIC_ASSERT(
    sizeof(int32_t) == sizeof(int), 
    
    Kernel_lsbi_MakeSure_LeastSignificantBit_BitsAreComparableInSize, 
    "lsbui - sizeof(int32_t) != sizeof(int) Types are not comparable in size, whatever was compiled may exhibit undefined behaviour"
);
KERNEL_STATIC_ASSERT(
    sizeof(int64_t) == sizeof(long long),
    
    Kernel_lsbl_MakeSure_LeastSignificantBit_BitsAreComparableInSize, 
    "lsbui - sizeof(int64_t) != sizeof(long) Types are not comparable in size, whatever was compiled may exhibit undefined behaviour"
);


uint32_t lsbui(uint32_t bits) {
	int temp = (int32_t)bits;
	
	return (uint32_t)__builtin_ffs(temp);
}

uint64_t lsbul(uint64_t bits) { 
	long long temp = (long long)bits;

	return (uint64_t)__builtin_ffsll(temp);
}

int32_t lsbi(int32_t bits)
{
    return (int32_t)__builtin_ffs(bits);
}

int64_t lsbl(int64_t bits)
{
    return (int64_t)__builtin_ffsll(bits);
}


