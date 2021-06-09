#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <random>

#define TREE_DEPTH	     16ull
#define TREE_SIZE	     (-1 + (1ull << (TREE_DEPTH + 1ull)))
#define boolean(arg)     (!!(arg))
 
extern "C" 
{

uint64_t retrieve_lvl_ht(uint64_t offset)
{
    uint8_t lvl = 0;
    while( 
        !boolean(
            (uint64_t)offset & ((4096ul << lvl) + 1)
        )
    )
    {
        ++lvl;
    }
    return TREE_DEPTH - lvl;
}


uint64_t retrieve_lvl(uint64_t offset)
{
    uint64_t lvl   = 0;
    uint64_t align = 4096ul;
    uint64_t  b     = offset % align;
    while(!(b))
    {
        ++lvl;
        align <<= 1;
        b = offset % align;
    }
    return TREE_DEPTH - lvl;
}

}

int main()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::chrono::steady_clock::time_point begin, end;
    uint64_t bruh = 0;
    uint64_t avg0 = 0, avg1 = 0, avg2 = 0, avg3 = 0;
    uint64_t iterations = 10000000;
    uint64_t rand = 0;

    for(uint64_t i = 0; i < iterations; ++i)
    {
        rand = mt() & 0xfffffff;
        begin = std::chrono::steady_clock::now();
        bruh  = retrieve_lvl(0x1ffFF000ul + 4096ul * rand); 
        end   = std::chrono::steady_clock::now();
        avg0 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        avg1 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    }
    std::cout << "difference [v0norm]= " << avg0 / iterations << "[µs]" << std::endl;
    std::cout << "difference [v0norm]= " << avg1 / iterations << "[ns]" << std::endl;


    for(uint64_t i = 0; i < iterations; ++i)
    {
        rand = mt() & 0xfffffff;
        begin = std::chrono::steady_clock::now();
        bruh  = retrieve_lvl_ht(0x1ffFF000ul + 4096ul * rand); 
        end   = std::chrono::steady_clock::now();
        avg2 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        avg3 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    }
    std::cout << "difference [v1optm]= " << avg2 / iterations << "[µs]" << std::endl;
    std::cout << "difference [v1optm]= " << avg3 / iterations << "[ns]" << std::endl;
    /* 
        * Tested with GCC 9.3 and 10.2.
        * With GCC 9.3:
        *   -O1:
        *       v0norm is faster (by about 150%->200%)
        *   -O2:
        *       v1optm is faster (by 1.5->4 times)
        * 
        * With GCC 10.2:
        *   -O1:
        *       same performance (deviations of 1%->5%)
        *   -O2:
        *       v1optm is faster (by about 150%->400%)
        * 
        * Bottom Line: 
        *   When debugging, use preferable to use the first version.
        *   When running in release, preferable to use the second version.
        * Why did I test this: 
        *   this function is run in pfa_free() every de-allocation
        *   if this is not fast, it'll slow down the general performance of the system.
        *   I found out that modulus (%) operations are generally slow (because they use a div instruction).
        *   for n = 2^m, in k % n, there is a faster way to perform a % using bit operations: m % n == m & (n - 1)
        *   Tested this, and generally this optimization is helpful. 
    */

}
