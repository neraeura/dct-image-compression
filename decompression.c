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
         /* TODO: Does this effectively handle case if c != \n ?? */
        assert(c = '\n');
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
 *  Purpose: Perform inverse discrete cosine transform (DCT) 
 *      
 *  Parameters: 
 *      
 *  Returns:  
 *  Effects: 
 *  Expects: 
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

 /**************************** quantizeRGB() ************************
 * 
 *  Purpose:
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
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
 *  Purpose:
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
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
 *  Purpose:
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
 * 
 ****************************************************************************/
uint64_t readInCodeword(FILE *input, unsigned bits_left) 
{       
        /* Piping each lsb amount into a variable and checking if the EOF 
        character is present in the characters  */
        uint64_t raw_word = 0;
        uint64_t single_byte;

        for (unsigned w = bits_left; w > 0; w = w - BYTE_SIZE) {
                single_byte = getc(input);
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

