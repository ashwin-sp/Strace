#include "kernel/types.h"
#include "kernel/defs.h"

int w = 1, x= 2, y= 3, z= 4;
int prng() 
{ 
    int t = x;
    t ^= t << 11U;
    t ^= t >> 8U;
    x = y; y = z; z = w;
    w ^= w >> 19U;
    w ^= t;
    w %= 1000;
    return w;
 }

int prng_limit(int limit) 
{ 
    int t = x;
    t ^= t << 11U;
    t ^= t >> 8U;
    x = y; y = z; z = w;
    w ^= w >> 19U;
    w ^= t;
    w %= 1000;
    return w;
 }