#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitpack.h"
#include "assert.h"

const unsigned MAX_BYTES = 64;

static uint64_t shift_left(uint64_t word, unsigned bits)
{
        assert(bits <= MAX_BYTES);
        if (bits == MAX_BYTES) {
                return 0;
        } else {
                return word << bits;
        }
}

static uint64_t shift_right_logical(uint64_t word, unsigned bits)
{
        assert(bits <= MAX_BYTES);
        if (bits == MAX_BYTES) {
                return 0;
        } else {
                return word >> bits;
        }
}

static int64_t shift_right_arith(uint64_t word, unsigned bits)
{
        assert(bits <= MAX_BYTES);
        if (bits == MAX_BYTES) {
                bits = 63;
        }
        return ((int64_t) word) >> bits; 
}

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= MAX_BYTES);
        uint64_t result = shift_right_logical(n, width);
        return result == 0;
}

bool Bitpack_fitss(int64_t n, unsigned width) 
{                   
        assert(width <= MAX_BYTES);
        uint64_t shifted_left = shift_left(n, MAX_BYTES - width);
        int64_t result = shift_right_arith(shifted_left, MAX_BYTES - width);

        return result == n; 
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BYTES);
        assert(lsb + width);
        uint64_t shifted_right = shift_right_logical(word, 
                                                (MAX_BYTES - (lsb + width)));
        uint64_t result = shift_left(shifted_right, (MAX_BYTES - width));
        return result;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BYTES);
        assert(lsb + width);
        int64_t shifted_right = shift_right_arith(word, 
                                                (MAX_BYTES - (lsb + width)));
        uint64_t result = shift_left(shifted_right, (MAX_BYTES - width));
        return result;
}


uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                                uint64_t value)
{
        assert(width <= MAX_BYTES);
        assert(lsb + width);
        unsigned shift_diff = lsb + width;
}