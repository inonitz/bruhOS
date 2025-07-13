#ifndef __KERNEL_C_DEFINITION_STD_BITS_HEADER__
#define __KERNEL_C_DEFINITION_STD_BITS_HEADER__
#include <stdint.h>


/* least significant bit for integers of size uint32_t - index starts at 1, not 0! */
uint32_t lsbui(uint32_t bits);

/* least significant bit for integers of size uint64_t - index starts at 1, not 0! */
uint64_t lsbul(uint64_t bits);

/* least significant bit for integers of size int32_t - index starts at 1, not 0! */
int32_t lsbi(int32_t bits);

/* least significant bit for integers of size int64_t - index starts at 1, not 0! */
int64_t lsbl(int64_t bits);


#endif /* __KERNEL_C_DEFINITION_STD_BITS_HEADER__ */