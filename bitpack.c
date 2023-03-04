#include <stdio.h>
#include <stdlib.h>

#include "bitpack.h"
#include "assert.h"



static uint64_t shift_left(uint64_t word, unsigned bits)
{
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

static uint64_t shift_right(uint64_t word, unsigned bits)
{
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

static int64_t arith_shift_right(uint64_t word, unsigned bits)
{
        if (bits == 64)
                bits = 63;
        return ((int64_t) word) >> bits; 
}