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
#include "decompression.h"
#include "arith40.h"
#include "bitpack.h"

static const unsigned DENOMINATOR = 255;
static const unsigned BYTE_SIZE = 8;
static const unsigned A_SIZE = 9;
static const unsigned B_SIZE = 5;
static const unsigned C_SIZE = 5;
static const unsigned D_SIZE = 5;
static const unsigned PR_SIZE = 4;
static const unsigned PB_SIZE = 4;
static const unsigned MAX_WORD_SIZE = A_SIZE + B_SIZE + C_SIZE + D_SIZE
                                                        + PR_SIZE + PB_SIZE;


Pnm_ppm readHeader(FILE *in)
{
        unsigned height, width;
        int read = fscanf(in, "COMP40 Compressed image format 2\n%u %u", 
                                &width, &height); 
        assert(read == 2);
        int c = getc(in);
        if (c != '\n') { // cope with bad student format
                fprintf(stderr, "Newline missing from compressed-image header\n");
                ungetc(c, in);
        }

        A2Methods_T methods = uarray2_methods_plain;
        A2Methods_UArray2 pixels = methods->new(width, height,
                                        sizeof(struct Pnm_rgb));
        Pnm_ppm pixmap = malloc(sizeof(*pixmap));
        pixmap->height = height;
        pixmap->width = width;
        pixmap->denominator = DENOMINATOR;
        pixmap->pixels = pixels;
        
        return pixmap;
}

 /**************************** computeInverseDCT() ************************
 * 
 *  Purpose: Computes the inverse discrete cosine transform (DCT) coefficients 
 *            of the luminance values y_1, y_2, y_3, y_4 in a given 2x2 image 
 *            block of pixels 
 *         
 *  Parameters: 
 *      1. block - a DCT_space struct representing a block of DCT coefficients
 *
 *  Returns: A Brightness_values struct containing the computed coefficients 
 *           of the luminance values 
 *
 *  Effects: The function has no side effects on external variables or state.
 *
 *  Expects: Assumes that the input block contains valid DCT coefficients and 
 *           that the inverse DCT can be calculated using the given equations.
 *           The coefficients must be in the correct order and format for the 
 *            inverse DCT equations to be applied correctly. 
 * 
 ****************************************************************************/
Brightness_values computeInverseDCT(DCT_space block)
{
        float y_1 = block.a - block.b - block.c + block.d;
        float y_2 = block.a - block.b + block.c - block.d; 
        float y_3 = block.a + block.b - block.c - block.d; 
        float y_4 = block.a + block.b + block.c + block.d;
        Brightness_values pixels = {y_1, y_2, y_3, y_4};

        return pixels;
}

 /**************************** quantizeRGB() ********************************
 * 
 *  Purpose: Quantizes the DCT coefficients of a 2x2 image block 
 *
 *  Parameters: 
 *      1. block -- A DCT_space_int struct which contains four integer values, 
 *                  a, b, c, and d, which represent the DCT coefficients of the 
 *                  RGB channels.
 * 
 *  Returns:  A DCT_space struct called quantized_dct that also contains four 
 *            values, a, b, c, and d, which represent the quantized DCT 
 *            coefficients.
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: Assumes that the input values are stored as integers, and that the 
 *           output values are stored as floating-point numbers.
 * 
 ****************************************************************************/
DCT_space quantizeRGB(DCT_space_int block)
{
        int scaled_a = (float) block.a;
        int scaled_b = (float) block.b / 50.0;
        int scaled_c = (float) block.c / 50.0;
        int scaled_d = (float) block.d / 50.0;

        DCT_space quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;
}

 /**************************** unpackCodeword() ************************
 * 
 *  Purpose: Unpacks a compressed 2x2 image block represented by a single 
 *           codeword 
 * 
 *  Parameters: 
 *      1. raw_word - an unsigned 64-bit integer that represents a compressed 
 *                    image block as a single codeword.
 *      2. bits_left - an unsigned integer that specifies how many bits of the 
 *                     codeword are still available for reading.
 * 
 *  Returns: A Codeword struct that contains the DCT coefficients and average 
 *           color values for the corresponding block of the image.
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: The function assumes that the input raw_word contains a valid 
 *           compressed image block and that the bits_left parameter accurately 
 *           specifies how many bits are available for reading from the 
 *           codeword. The sizes of the various fields (A_SIZE, B_SIZE, C_SIZE, 
 *           D_SIZE, PB_SIZE, PR_SIZE) are also assumed to be defined and to 
 *           correspond correctly to the bitpacking used when compressing the
 *           image.
 * 
 ****************************************************************************/
Codeword unpackCodeword(uint64_t raw_word, unsigned bits_left) 
{       
        Codeword word;
        word.dct.a = Bitpack_getu(raw_word, A_SIZE, bits_left -= A_SIZE);
        word.dct.b = Bitpack_gets(raw_word, B_SIZE, bits_left -= B_SIZE);
        word.dct.c = Bitpack_gets(raw_word, C_SIZE, bits_left -= C_SIZE);
        word.dct.d = Bitpack_gets(raw_word, D_SIZE, bits_left -= D_SIZE);
        word.avg_pb = Bitpack_getu(raw_word, PB_SIZE, bits_left -= PB_SIZE);
        word.avg_pr = Bitpack_getu(raw_word, PR_SIZE, bits_left -= PR_SIZE);

        return word;
}

 /**************************** readInCodeword() ************************
 * 
 *  Purpose: Reads in a single codeword from the input file stream in sequence, 
 *           remembering that each word is stored in big-endian order, with 
 *           the MSB first.
 * 
 *  Parameters: 
 *      1. input - a pointer to a FILE object representing the input file stream 
 *               from which the compressed image blocks are read.
 *     2. bits_left - an unsigned integer that specifies how many bits of the 
 *                    current codeword are still available for reading.
 * 
 *  Returns: An unsigned 64-bit integer that represents the codeword read from 
 *           the input file.
 * 
 *  Effects: If the supplied file is too short, i.e. the number of codewords
 *           is too low for the stated width and height, or the last one is 
 *           incomplete, this function fails with a Checked Runtime Error 
 * 
 *  Expects: Assumes that the input file stream is open and points to a valid 
 *           input file containing compressed image blocks. It also assumes that 
 *           the bits_left parameter accurately specifies how many bits of the 
 *           current codeword are still available for reading from the input 
 *           file, and that the input file stream is properly formatted with 
 *           respect to the compressed image block encoding scheme.
 * 
 ****************************************************************************/
uint64_t readInCodeword(FILE *input, unsigned bits_left) 
{

        uint64_t raw_word = 0;
        uint64_t single_byte;

        for (unsigned w = bits_left; w > 0; w = w - BYTE_SIZE) {
                single_byte = getc(input);
                assert(bits_left % 8 == 0);
                assert((int) single_byte != EOF);
                /* within byte, align codeword */
                single_byte = single_byte << w;
                raw_word = raw_word | single_byte;
        }

        return raw_word;

}



/* TODO: ASK TA IF THIS IS MODULAR */

 /**************************** decompressImage() ************************
 * 
 *  Purpose: 
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
 * 
 ****************************************************************************/
void decompressImage(A2Methods_UArray2 original_image, A2Methods_T methods,
                                                                FILE *input)
{
        int width = methods->width(original_image);
        int height = methods->height(original_image);

        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
        }

        for (int col = 0; col < width; col+=2) { 
                for (int row = 0; row < height; row+=2) {

                        /* Unpacking */
                        unsigned lsb = MAX_WORD_SIZE;
                        uint64_t extracted_values = readInCodeword(input, lsb);
                        Codeword word = unpackCodeword(extracted_values, lsb);

                         /* Convert 4-bit chroma codes to pB and pR */
                        float pb = Arith40_chroma_of_index(word.avg_pb);
                        float pr = Arith40_chroma_of_index(word.avg_pr);

                        /* Quantization to get floating point integers */
                        DCT_space dct = quantizeRGB(word.dct);
                

                        // /* Convert 4-bit chroma codes to pB and pR */
                        // unsigned avgpr = Arith40_chroma_of_index(chroma_pr);
                        // unsigned avgpb = Arith40_chroma_of_index(chroma_pb)

                         /* Inverse DCT */
                        Brightness_values brightnesses = 
                                                computeInverseDCT(dct);

                        /* create CV pixels */
                        Pnm_componentvid_flt_pixels comp_vid_block = 
                                        create_compvid_pixels_decomp(pr, pb, 
                                                                brightnesses);

                        /* Transform from CV to RGB color space */
                        Pnm_rgb_int_pixels rgb_block = 
                                create_rgbint_pixels(comp_vid_block,
                                                        DENOMINATOR);

                        /* Getting each pixel in the 2x2 block */
                        Pnm_rgb pixel0 =
                                methods->at(original_image, col, row);
                        Pnm_rgb pixel1 =
                                methods->at(original_image, col + 1, row);
                        Pnm_rgb pixel2 =
                                methods->at(original_image, col, row + 1);
                        Pnm_rgb pixel3 =
                                methods->at(original_image, col + 1, row + 1);

                        // Might not work, we may need to malloc the pixels
                        /* Putting each pixel into PPM*/
                        *pixel0 = *(rgb_block.pix1);
                        *pixel1 = *(rgb_block.pix2);
                        *pixel2 = *(rgb_block.pix3);
                        *pixel3 = *(rgb_block.pix4);
                }
        }
}

