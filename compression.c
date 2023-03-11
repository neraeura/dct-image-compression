#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

#include "pixels.h"
#include "pnm.h"
#include "compression.h"
#include "bitpack.h"
#include "arith40.h"

static const unsigned BYTE_SIZE = 8; 
static const unsigned A_SIZE = 9;
static const unsigned B_SIZE = 5;
static const unsigned C_SIZE = 5;
static const unsigned D_SIZE = 5;
static const unsigned PR_SIZE = 4;
static const unsigned PB_SIZE = 4;
static const unsigned MAX_WORD_SIZE = 32;



 /**************************** computePR_avg() ****************************
 * 
 *  Purpose: Computes PR_avg--the average value of the pR component: the
 *           chroma that transmits the difference between red and luma--on 
 *           a given block of pixels 
 * 
 *  Parameters: 
 *      1. A Pnm_componentvid_flt_pixels structure named "block", which contains 
 *         four pixels representing a 2x2 block of the image in the YCbCr 
 *         color space.
 * 
 *  Returns: PR_avg, the average value of the pR component: the
 *           chroma that transmits the difference between red and luma 
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: The input pixel block must be valid and contain four pixel values 
 *           with valid PR component values.
 * 
 ****************************************************************************/
float computePR_avg(Pnm_componentvid_flt_pixels block) 
{
        float sum = block.pix1.pr + block.pix2.pr +  block.pix3.pr + 
                                                     block.pix4.pr;
        return (sum/4);
}

 /**************************** computePB_avg() ****************************
 * 
 *  Purpose: Computes PB_avg--the average value of the pR component: the
 *           chroma that transmits the difference between blue and luma--on 
 *           a given block of pixels  
 * 
 *  Parameters: 
 *     1. A Pnm_componentvid_flt_pixels structure named "block", which contains 
 *        four pixels representing a 2x2 block of the image in the YCbCr 
 *        color space.
 * 
 *  Returns:  PB_avg, the average value of the pB component: the
 *           chroma that transmits the difference between blue and luma 
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: The input pixel block must be valid and contain four pixel values 
 *           with valid PB component values. 
 * 
 ****************************************************************************/
float computePB_avg(Pnm_componentvid_flt_pixels block) 
{
        float sum = block.pix1.pb + block.pix2.pb + block.pix3.pb + 
                                                    block.pix4.pb;
        return (sum/4);
}

 /**************************** computeDCT() ***********************************
 * 
 *  Purpose: Computes the Discrete Cosine Transform (DCT) cosine coefficients 
 *           a, b, c, d of in a given image block of pixels
 * 
 *  Parameters: 
 *      1. A Pnm_componentvid_flt_pixels structure named "block", which contains 
 *         four pixels representing a 2x2 block of the image in the YCbCr 
 *         color space.
 * 
 *  Returns: The function returns a DCT_space struct, which contains the 
 *           computed cosine coefficients.
 * 
 *  Effects: This function does not have any side effects. It does not modify 
 *           any variables or data outside of its scope.
 * 
 *  Expects: The function expects a valid Pnm_componentvid_flt_pixels structure 
 *           as the input parameter. It assumes that the values of the four 
 *           pixels in the structure are valid and represent a 2x2 block of the 
 *           image in the YCbCr color space. It assumes that the computation of 
 *           the DCT coefficients is correctly implemented using the standard 
 *           formula for computing the DCT coefficients.
 * 
 ****************************************************************************/
DCT_space computeDCT(Pnm_componentvid_flt_pixels block)
{
        float a = (block.pix4.y + block.pix3.y + block.pix2.y + block.pix1.y) 
                                                                        / 4.0;
        float b = (block.pix4.y + block.pix3.y - block.pix2.y - block.pix1.y) 
                                                                        / 4.0;
        float c = (block.pix4.y - block.pix3.y + block.pix2.y - block.pix1.y) 
                                                                        / 4.0;
        float d = (block.pix4.y - block.pix3.y - block.pix2.y + block.pix1.y)
                                                                        / 4.0;

        DCT_space dct = {a, b, c, d};
        return dct;

}
 /**************************** quantizeDCT() *********************************
 * 
 *  Purpose: Quantizes the DCT coefficients of a 2x2 image block 
 * 
 *  Parameters: 
 *        1. a DCT_space struct named "block" which contains four floats:
 *          "a", "b", "c", and "d", which represent the four coefficients of a 
 *          discrete cosine transformation
 * 
 *  Returns: A DCT_space_int struct containing the quantized DCT 
 *           coefficients
 * 
 *  Effects: 
 * 
 *  Expects:  The function assumes that the input DCT coefficients are 
 *            represented using floating-point values and that the input 
 *            DCT_space struct contains valid values for the "a", "b", "c", and
 *            "d" coefficients. 
 * 
 ****************************************************************************/
DCT_space_int quantizeDCT(DCT_space block)
{
        uint64_t scaled_a = (block.a * 511 < 0) ? (0) : (block.a * 511 > 511) ? 511 : block.a * 511;
        int64_t scaled_b = (fabs(block.b) < 0.3) ? (block.b * 50) : 0.3 * 50 *
                                                                sign(block.b);
        int64_t scaled_c = (fabs(block.c) < 0.3) ? (block.c * 50) : 0.3 * 50 *
                                                                sign(block.c);
        int64_t scaled_d = (fabs(block.d) < 0.3) ? (block.d * 50) : 0.3 * 50 * 
                                                                sign(block.d);

        DCT_space_int quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

}

 /**************************** packCodeword() *********************************
 * 
 *  Purpose: Packs a, b, c, d, PB, PR into a 32-bit codeword
 *  Parameters: 
 *     1. word -- a Codeword struct containing the data to be packed
 *     2. An unsigned integer named "bits_left", which represents the number 
 *        of bits available to pack the Codeword structure into a 32-bit 
 *        codeword.
 *  Returns: A 32-bit unsigned integer representing the packed codeword
 *  Effects: This function does not have any side effects. It does not modify 
 *           any variables or data outside of its scope.
 *  Expects:  
 * 
 ****************************************************************************/
uint32_t packCodeword(Codeword word)
{
        uint32_t raw_word = 0;
        unsigned bit_count = MAX_WORD_SIZE;

        raw_word = Bitpack_newu(raw_word, A_SIZE, bit_count -= A_SIZE, 
                                                                word.dct.a);
        // fprintf(stderr, "raw word a: %" PRIu64 "\n", Bitpack_getu(raw_word, A_SIZE, 23));
        raw_word = Bitpack_news(raw_word, B_SIZE, bit_count -= B_SIZE, 
                                                                word.dct.b);
        // fprintf(stderr, "raw word b: %" PRId64 "\n", Bitpack_getu(raw_word, B_SIZE, 18));
        raw_word = Bitpack_news(raw_word, C_SIZE, bit_count -= C_SIZE, 
                                                                word.dct.c);
        // fprintf(stderr, "raw word c: %" PRId64 "\n", Bitpack_getu(raw_word, C_SIZE, 13));
        raw_word = Bitpack_news(raw_word, D_SIZE, bit_count -= D_SIZE, 
                                                                word.dct.d);
        // fprintf(stderr, "raw word d: %" PRId64 "\n", Bitpack_getu(raw_word, D_SIZE, 8));                                                   
        raw_word = Bitpack_newu(raw_word, PB_SIZE, bit_count -= PB_SIZE, 
                                                                word.avg_pb);
        // fprintf(stderr, "raw word pb: %" PRIu64 "\n", Bitpack_getu(raw_word, PB_SIZE, 4));
        raw_word = Bitpack_newu(raw_word, PR_SIZE, bit_count -= PR_SIZE, 
                                                                word.avg_pr);
        // fprintf(stderr, "raw word pr: %" PRIu64 "\n", Bitpack_getu(raw_word, PR_SIZE, 0));
        assert(bit_count == 0);
        assert(Bitpack_fitsu(raw_word, MAX_WORD_SIZE));
        
        return raw_word;
}

 /**************************** printCodeword() *********************************
 * 
 *  Purpose: Prints a packed codeword represented as a 64-bit unsigned integer
 *  Parameters:
 *      1. A 32-bit unsigned integer named "word", which represents the packed 
 *         codeword that is to be printed.
 *     2.  An unsigned integer named "bits_left", which represents the number of 
 *         bits in the codeword that are to be printed.
 *  Returns: None
 *  Effects: This function outputs the packed codeword to standard output
 *  Expects:  
 * 
 ****************************************************************************/
void printCodeword(uint32_t word) 
{
        for (unsigned w = MAX_WORD_SIZE; w > 0; w = w - BYTE_SIZE) {
                putc(Bitpack_getu(word, BYTE_SIZE, w), stdout);
        }
}

//-------------------------------------------------------
        /* DECOMPRESSION STARTS BELOW */
//--------------------------------------------------------

Brightness_values computeBrightnessValues(DCT_space block)
{
        float y_1 = block.a - block.b - block.c + block.d;
        float y_2 = block.a - block.b + block.c - block.d; 
        float y_3 = block.a + block.b - block.c - block.d; 
        float y_4 = block.a + block.b + block.c + block.d;
        Brightness_values pixels = {y_1, y_2, y_3, y_4};
        // fprintf(stderr, "--226--\na: %0d\nb: %d\nc: %d\nd: %d\n", block.a, block.b, block.c, block.d);
        // fprintf(stderr, "--226--\ny_1: %f\ny_2: %f\ny_3: %f\ny_4: %f\n", y_1, y_2, y_3, y_4);

        return pixels;
}

DCT_space quantizeRGB1(DCT_space_int block)
{
        float scaled_a = (float) block.a / 511.0; 
        float scaled_b = (float) block.b / 50.0;
        float scaled_c = (float) block.c / 50.0;
        float scaled_d = (float) block.d / 50.0;
        // fprintf(stderr, "--238--\na: %0d\nb: %d\nc: %d\nd: %d\n", block.a, block.b, block.c, block.d);
        DCT_space quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;
        
}


Codeword unpackCodeword1(uint64_t raw_word) 
{       
        unsigned bit_count = MAX_WORD_SIZE;

        /* for a, b, c, d, pb, and pr, we obtain the LSB by subtracting their 
         * respective LSB indices from bit_count */

        uint32_t a = Bitpack_getu(raw_word, A_SIZE, bit_count -= A_SIZE);
  
        int32_t b = Bitpack_gets(raw_word, B_SIZE, bit_count -= B_SIZE);

        int32_t c = Bitpack_gets(raw_word, C_SIZE, bit_count -= C_SIZE);
        
        int32_t d = Bitpack_gets(raw_word, D_SIZE, bit_count -= D_SIZE);

        uint32_t avg_pb = Bitpack_getu(raw_word, PB_SIZE, bit_count -= PB_SIZE);

        uint32_t avg_pr = Bitpack_getu(raw_word, PR_SIZE, bit_count -= PR_SIZE);
        
        
        DCT_space_int word_dct = {a, b, c, d};
        Codeword word = {word_dct, avg_pr, avg_pb};
        
        assert(bit_count == 0);
        return word;
}

/* TODO: ASK TA IF THIS IS MODULAR */
void compressImage(A2Methods_UArray2 original_image,
                        A2Methods_T methods, unsigned denominator)
{
        assert(original_image);
        int width = methods->width(original_image);
        int height = methods->height(original_image);

        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
        }

        // printf("COMP40 Compressed image format 2\n%u %u", width, height);
        for (int row = 0; row < height; row += 2) { 
                for (int col = 0; col < width; col += 2) {
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
                        Pnm_componentvid_flt_pixels compvid_block = create_compvid_pixels_comp(rgb_block);
                        if (row == 0 && col == 0) {

                                fprintf(stderr, "296: y_1: %f\ny_2: %f\ny_3: %f\ny_4: %f\n ----------------- \n",
                                compvid_block.pix1.y, compvid_block.pix2.y, compvid_block.pix3.y, compvid_block.pix4.y);
                        }
                

                        /* Compute average PR and PB values */
                        float avgpr = computePR_avg(compvid_block);
                        float avgpb = computePB_avg(compvid_block);

                        // /* Computing the chroma */
                        unsigned chroma_pr = Arith40_index_of_chroma(avgpr);
                        unsigned chroma_pb = Arith40_index_of_chroma(avgpb);

                        /* DCT */
                        DCT_space values = computeDCT(compvid_block); 
                        DCT_space_int DCT_int_values = quantizeDCT(values);

                        /* Bit packing */
                        Codeword encoded = {DCT_int_values, chroma_pr, chroma_pb};
                        //fprintf(stderr, "\nthe dcts:\na...%ld\nb...%ld\nc...%ld\nd...%ld\n", word.dct.a, word.dct.b, word.dct.c, word.dct.d);
                        uint32_t packed_word = packCodeword(encoded);
                        // printCodeword(packed_word, MAX_WORD_SIZE);
                        

//----------------------------------------------------------------------------------------------------
                                /* DECOMPRESSION STARTS BELOW */

                        /* Unpack word*/
                        //uint64_t extracted_values = readInCodeword(input, lsb);
                        Codeword unpacked_word = unpackCodeword1(packed_word);
                        
                         /* Convert 4-bit chroma codes to pB and pR */
                        float pb = Arith40_chroma_of_index(unpacked_word.avg_pb);
                        float pr = Arith40_chroma_of_index(unpacked_word.avg_pr);

                         /* Quantization to get floating point integers */
                        DCT_space dct = quantizeRGB1(unpacked_word.dct);
                         // if (row == 0 && col == 0) {
                        //         fprintf(stderr, "y_1: %f\ny_2: %f\ny_3: %f\ny_4: %f\n",
                        //         brightnesses.y_1, brightnesses.y_2, brightnesses.y_3, brightnesses.y_4);
                        // }
                

                         /* Convert 4-bit chroma codes to pB and pR */
                        // unsigned avgpr = Arith40_chroma_of_index(chroma_pr);
                        // unsigned avgpb = Arith40_chroma_of_index(chroma_pb)

                         /* Inverse DCT */
                        Brightness_values brightnesses = 
                                                computeBrightnessValues(dct);
                        // if (row == 0 && col == 0) {
                        //         fprintf(stderr, "y_1: %f\ny_2: %f\ny_3: %f\ny_4: %f\n",
                        //         brightnesses.y_1, brightnesses.y_2, brightnesses.y_3, brightnesses.y_4);
                        // }

                        /* create CV pixels */
                        Pnm_componentvid_flt_pixels comp_vid_block = create_compvid_pixels_decomp(pr, pb, brightnesses);

                        /* Transform from CV to RGB color space */
                        Pnm_rgb_int_pixels new_rgb_block = 
                                create_rgbint_pixels(comp_vid_block,
                                                        255);

                        /* Getting each pixel in the 2x2 block */

                        // Might not work, we may need to malloc the pixels
                        /* Putting each pixel into PPM*/
                        *pixel0 = *(new_rgb_block.pix1);
                        *pixel1 = *(new_rgb_block.pix2);
                        *pixel2 = *(new_rgb_block.pix3);
                        *pixel3 = *(new_rgb_block.pix4);

                        free(new_rgb_block.pix1);
                        free(new_rgb_block.pix2);
                        free(new_rgb_block.pix3);
                        free(new_rgb_block.pix4);

                }
        }
}

