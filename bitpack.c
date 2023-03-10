#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "bitpack.h"
#include "assert.h"
#include "except.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

static const unsigned MAX_BYTES = 64;

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
        assert(lsb + width <= MAX_BYTES);
        uint64_t aligned = shift_left(word, (MAX_BYTES - (lsb + width)));
        uint64_t extracted = shift_right_logical(aligned, (MAX_BYTES - width));
        return extracted;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BYTES);
        assert(lsb + width <= MAX_BYTES);
        int64_t aligned = shift_right_arith(word, 
                                                (MAX_BYTES - (lsb + width)));
        uint64_t extracted = shift_left(aligned, (MAX_BYTES - width));
        return extracted;
}


uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
        unsigned shift_high = lsb + width;
        unsigned shift_low = MAX_BYTES - lsb;
        assert(shift_high <= 64);
        assert(width <= 64);
        if (!(Bitpack_fitsu(value, width))) {
                RAISE(Bitpack_Overflow);
        }
        /*  right-shifts the word by the index of the MSB to align the bits 
        of the field with the most significant bits of the word */
        uint64_t align_high = shift_right_logical(word, shift_high);
        /*  left-shifts the aligned bits to the leftmost bits of the word, so 
        that they are ready to be combined with the value */
        uint64_t high_end = shift_left(align_high, shift_high);
        /* left-shifts the bits that are not included in the field being written 
        to the leftmost bits of the word, so that they are ready to be combined 
        with the value */
        uint64_t align_low = shift_left(word, shift_low);
        /* right-shifts the left-shifted bits to align them with the LSB's of 
        the word */
        uint64_t low_end = shift_right_logical(align_low, shift_low);
        /* combines the aligned bits of the field with the most significant 
        bits of the word (high_end), the value to be written, left-shifted by 
        the least significant bit (value << lsb), and the aligned bits of the 
        field with the least significant bits */
        uint64_t results = high_end | (value << lsb) | low_end;

        return results;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        if(!(Bitpack_fitss(value, width))) { 
                RAISE(Bitpack_Overflow);
        }
        uint64_t extracted_value = Bitpack_getu(value, width, 0);
        return Bitpack_newu(word, width, lsb, extracted_value);
}