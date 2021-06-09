#include <stdint.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <random>
#include <iostream>
#include <immintrin.h>
#include <thread>

typedef char char_t;
struct __attribute__((packed, aligned(16))) vec4
{
    int64_t v[2];
};

void memcpy_v0(char_t* dst, char_t* src, uint64_t amount)
{
    for(uint64_t i = 0; i < amount >> 4; ++i)
	{
		((vec4*)src)[i] = ((vec4*)dst)[i];
	}
    
	amount = amount & 15;
	for(uint8_t i = 0; i < amount; ++i)
	{
		((char*)src)[i] = ((char*)dst)[i];
	}
}

void memcpy_v1(char_t* dst, char_t* src, uint64_t amount)
{
    for(uint64_t i = 0; i < amount >> 3; ++i)
	{
		((uint64_t*)src)[i] = ((uint64_t*)dst)[i];
	}
    
	amount &= 7;
	for(uint8_t i = 0; i < amount; ++i)
	{
		((char*)src)[i] = ((char*)dst)[i];
	}
}


int main()
{
    using namespace std::chrono_literals;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::chrono::steady_clock::time_point begin, end;
    uint64_t avg0 = 0, avg1 = 0, avg2 = 0, avg3 = 0, avg4 = 0, avg5 = 0;
    uint64_t iterations = 1000;
    uint64_t alloc_size  = mt();
    uint64_t alloc_limit = 10000;
    uint64_t num_tests  = 20;
    char_t* a = (char_t*)malloc(alloc_size);
    char_t* b = (char_t*)malloc(alloc_size);

    alloc_size = (alloc_size < alloc_limit) * alloc_size + (alloc_size >= alloc_limit) * alloc_limit;
    for(uint64_t i = 0; i < alloc_size; ++i)
    {
        a[i] = mt();
        b[i] = mt();
    }

    std::cout.width(20);
    std::cout << "[µs][ns]  [µs][ns]  [µs][ns]\n";

    for(uint64_t j = 0; j < num_tests; ++j)
    {
        for(uint64_t i = 0; i < iterations; ++i)
        {
            begin = std::chrono::steady_clock::now();
            memcpy_v0(a, b, alloc_size);
            end   = std::chrono::steady_clock::now();

            avg0 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            avg1 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
        }
        std::this_thread::sleep_for(50ms);

        for(uint64_t i = 0; i < iterations; ++i)
        {
            begin = std::chrono::steady_clock::now();
            std::memcpy(a, b, alloc_size);
            end   = std::chrono::steady_clock::now();
            avg2 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            avg3 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
        }

        std::this_thread::sleep_for(50ms);

        for(uint64_t i = 0; i < iterations; ++i)
        {
            begin = std::chrono::steady_clock::now();
            memcpy_v1(a, b, alloc_size);
            end   = std::chrono::steady_clock::now();
            avg4 += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            avg5 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
        }
        std::cout << avg0 / iterations << "   " << avg1 / iterations << "      "
                  << avg2 / iterations << "   " << avg3 / iterations << "      "
                  << avg4 / iterations << "   " << avg5 / iterations << "\n";

        std::this_thread::sleep_for(50ms);

        avg0 = 0;
        avg1 = 0;
        avg2 = 0;
        avg3 = 0;
        avg4 = 0;
        avg5 = 0;
    }

}