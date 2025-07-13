#include <std/math.h>
#include <std/static_assert.h>
#include <std/macro.h>


KERNEL_STATIC_ASSERT(
    __builtin_types_compatible_p(uint32_t, unsigned int),
    Kernel_log2ui_MakeSure_CountLeadingZeros_TypesAreEquivalentAndCanBeConverted, 
    "log2ui - Types uint32_t & int Are Not Equivalent and Can't be converted - may exhibit undefined behaviour"
);
KERNEL_STATIC_ASSERT(
    __builtin_types_compatible_p(uint64_t, unsigned long),
    Kernel_log2ul_MakeSure_CountLeadingZeros_TypesAreEquivalentAndCanBeConverted, 
    "log2ul - Types uint64_t & long Are Not Equivalent and Can't be converted - may exhibit undefined behaviour"
);


uint64_t roundN(uint64_t x, uint64_t n)
{
	uint64_t tmp = (x % n); 
	tmp = boolean(tmp);
	tmp += (x / n);
	tmp *= n;
	return tmp;
}

uint64_t round2(uint64_t n)
{
	--n; 
	n |= n >> 1; 
    n |= n >> 4; 
    n |= n >> 8; 
    n |= n >> 2; 
    n |= n >> 16; 
    n |= n >> 32;
	++n;
	return n;
}

uint64_t ppow2(uint64_t n)
{
	return round2(n) >> 1;
}

uint32_t log2ui(uint32_t bits) 
{ 
	int result = __builtin_clz((unsigned int)bits); /* always positive */

	return (uint32_t)( (8 * sizeof(uint32_t)) - (unsigned)result );
}


uint64_t log2ul(uint64_t bits) 
{
	int result = __builtin_clzll((unsigned long long)bits); /* always positive */

	return (uint64_t)( (8 * sizeof(uint64_t)) - (uint32_t)result );
}
