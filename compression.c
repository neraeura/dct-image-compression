#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pixels.h"
#include "pnm.h"
#include "compression.h"
#include "arith40.h"


static const unsigned A_SIZE = 9;
static const unsigned B_SIZE = 5;
static const unsigned C_SIZE = 5;
static const unsigned D_SIZE = 5;
static const unsigned PR_SIZE = 4;
static const unsigned PB_SIZE = 4;
static const unsigned MAX_WORD_SIZE = A_SIZE + B_SIZE + C_SIZE + D_SIZE
                                                        + PR_SIZE + PB_SIZE;



float compute_pr_avg(Pnm_componentvid_flt_pixels block) 
{
        float sum = block.pix1.pr + block.pix2.pr + block.pix3.pr + block.pix4.pr;
        return(sum/4);
}

float compute_pb_avg(Pnm_componentvid_flt_pixels block) 
{
        float sum = block.pix1.pb + block.pix2.pb + block.pix3.pb + block.pix4.pb;
        return(sum/4);
}

int sign(int num) 
{
        if (num > 0) {
                return 1;
        }
        return -1;
}

DCT_space compute_dct_values(Pnm_componentvid_flt_pixels block)
{
        float a = (block.pix1.y + block.pix2.y + block.pix3.y + block.pix4.y) / 4.0;
        float b = (block.pix1.y + block.pix2.y - block.pix3.y - block.pix4.y) / 4.0;
        float c = (block.pix1.y - block.pix2.y + block.pix3.y - block.pix4.y) / 4.0;
        float d = (block.pix1.y - block.pix2.y - block.pix3.y + block.pix4.y) / 4.0;

        DCT_space dct = {a, b, c, d};
        return dct;

}

DCT_space_int quantize_dct_compress(DCT_space block)
{
        int scaled_a = (int) block.a;
        int scaled_b = (fabs(block.b * 50) < 0.3) ? (block.b * 50) : 0.3 * sign(block.b);
        int scaled_c = (fabs(block.c * 50) < 0.3) ? (block.c * 50) : 0.3 * sign(block.c);
        int scaled_d = (fabs(block.d * 50) < 0.3) ? (block.d * 50) : 0.3 * sign(block.d);

        DCT_space_int quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

}


Codeword fill_codeword(Codeword word, unsigned bits_left) 
{
        word = Bitpack_newu(word, A_SIZE, bits_left -= A_SIZE, word.dct.a);
        word = Bitpack_news(word, B_SIZE, bits_left -= B_SIZE, word.dct.b);
        word = Bitpack_news(word, C_SIZE, bits_left -= C_SIZE, word.dct.c);
        word = Bitpack_news(word, D_SIZE, bits_left -= D_SIZE, word.dct.d);
        word = Bitpack_newu(word, PB_SIZE, bits_left -= PB_SIZE, word.avg_pb);
        word = Bitpack_newu(word, PR_SIZE, bits_left -= PB_SIZE, word.avg_pr);
        assert(bits_left == 0);
        return word;
}

void print_codeword(Codeword word, unsigned bit_left) 
{
        putc(Bitpack_getu(word, A_SIZE, bits_left -= A_SIZE), stdout);
        putc(Bitpack_getu(word, B_SIZE, bits_left -= B_SIZE), stdout);
        putc(Bitpack_getu(word, C_SIZE, bits_left -= C_SIZE), stdout);
        putc(Bitpack_getu(word, D_SIZE, bits_left -= D_SIZE), stdout);
        putc(Bitpack_getu(word, PB_SIZE, bits_left -= PB_SIZE), stdout);
        putc(Bitpack_getu(word, PR_SIZE, bits_left -= PR_SIZE), stdout);
        assert(bits_left == 0);
}


/* TODO: ASK TA IF THIS IS MODULAR */
void compress_image(A2Methods_UArray2 original_image,
                        A2Methods_T methods, unsigned denominator)
{
        int width = methods->width(original_image);
        int height = methods->height(original_image);

        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
        }

        for (int col = 0; col < width; col+=2) { 
                for (int row = 0; row < height; row+=2) {
                        /* Getting each pixel in the 2x2 block */
                        Pnm_rgb pixel0 =
                                methods->at(original_image, col, row);
                        Pnm_rgb pixel1 =
                                methods->at(original_image, col + 1, row);
                        Pnm_rgb pixel2 =
                                methods->at(original_image, col, row + 1);
                        Pnm_rgb pixel3 =
                                methods->at(original_image, col + 1, row + 1);

                        /* Convert block of RGB scaled integers to float points */
                        Pnm_rgb_flt_pixels rgb_block = create_rgbflt_pixels(pixel0, pixel1, pixel2, pixel3, denominator);
                       
                        /* Convert from RGB to CV color space */
                        Pnm_componentvid_flt_pixels compvid_block = create_compvid_pixels(compvid_block);


                        /* Compute average PR and PB values */
                        float avgpr = compute_pr_avg(compvid_block);
                        float avgpb = compute_pb_avg(compvid_block);

                        /* Computing the chroma */
                        unsigned chroma_pr = Arith40_index_of_chroma(avgpr);
                        unsigned chroma_pb = Arith40_index_of_chroma(avgpb);

                        /* DCT */
                        DCT_space values = compute_dct_values(compvid_block); 
                        DCT_space_int DCT_int_values = quantize_dct_compress(values);

                        /* Bit packing */
                        Codeword word = {DCT_int_values, chroma_pr, chroma_pb};
                        unsigned lsb_idx = MAX_WORD_SIZE;
                        uint64_t packed_word = fill_codeword(word, lsb_idx);
                        print_codeword(packed_word, lsb_idx);
                }
        }
}

