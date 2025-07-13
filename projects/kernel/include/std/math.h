#ifndef __KERNEL_C_DEFINITION_STD_MATH_HEADER__
#define __KERNEL_C_DEFINITION_STD_MATH_HEADER__
#include <stdint.h>


/* Rounds 'toround' to a multiple of 'multiple_n'  */
uint64_t roundN(uint64_t toround, uint64_t multiple_n);

/* round 'value' to closest power of 2 */
uint64_t round2(uint64_t value);

/* 
    given n = 2^M + K, K > 0 
        -> return 2^M

    [NOTE]: K = 0 is a special case that is not handled.
*/
uint64_t ppow2(uint64_t n);

/* Integer log for a uint32_t-sized type */
uint32_t log2ui(uint32_t bits);

/* Integer log for a uint64_t-sized type */
uint64_t log2ul(uint64_t bits);


#endif /* __KERNEL_C_DEFINITION_STD_MATH_HEADER__ */