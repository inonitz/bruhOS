#pragma once
#ifndef __INTRINSICS__
#define __INTRINSICS__
#include "../shared/int.h"



typedef struct alignpack(16) __m128_wrapper_struct
{
    union {
        double   alignpack(16) d[2];
        float    alignpack(16) f[4];
        uint32_t alignpack(16) i[4];
        uint16_t alignpack(16) w[8];
        uint8_t                b[16];
    };
} __m128;


extern void store_u32_epi8mask(__m128* dest, __m128* src, __m128* mask);



#endif