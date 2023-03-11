/**************************************************************************
 *
 *                             decompression.c
 * 
 *     Assignment: arith
 *     Authors:  Nora A-Rahim and Daniel Opara
 *     CS Logins: narahi01 and dopara01
 *     Date:  March 11, 2023
 *
 *     
 *     
 *
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include "decompression.h"
#include "colorSpace.h"
#include "DCTransforms.h"
#include "bitpack.h"
#include "fileIO.h"
#include "arith40.h"

static const unsigned DENOMINATOR = 255;
static const unsigned A_SIZE = 9;
static const unsigned B_SIZE = 5;
static const unsigned C_SIZE = 5;
static const unsigned D_SIZE = 5;
static const unsigned PR_SIZE = 4;
static const unsigned PB_SIZE = 4;
static const unsigned MAX_WORD_SIZE = A_SIZE + B_SIZE + C_SIZE + D_SIZE
                                                        + PR_SIZE + PB_SIZE;



typedef struct Codeword {
        DCT_space_int dct;
        unsigned avg_pr, avg_pb;
} Codeword;

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
Codeword unpackCodeword(uint64_t raw_word) 
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


 /**************************** decompressImage() ************************
 * 
 *  Purpose: Delineates the overall control flow of the image decompressor 
 * 
 *  Parameters:
 *      1. compressed_image --the original image array in compressed format 
 *                                   
 *      2. methods: an A2Methods_T object to give access to functions that 
 *                  operate on the image
 *      3. compressed_file: a file pointer to the compressed image file
 * 
 *  Returns: None
 * 
 *  Effects: The function modifies the dimensions of the compressed_image array 
 *           to write the decompressed image data. It also frees up memory 
 *           allocated by pixel creation functions. It is a Checked Runtime 
 *           Error for  compressed_image, compressed_file, or methods to be 
 *           NULL.
 * 
 *  Expects: The function expects the compressed_image array to be in compressed 
 *           format, the methods object to be initialized, and the 
 *           compressed_file pointer to point to a valid compressed image file
 * 
 ****************************************************************************/
void decompressImage(A2Methods_UArray2 compressed_image, A2Methods_T methods,
                                                        FILE *compressed_file)
{
        assert(compressed_image != NULL);
        assert(methods != NULL);
        assert(compressed_file != NULL);
        int width = methods->width(compressed_image);
        int height = methods->height(compressed_image);

        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
        }

        for (int col = 0; col < width; col+=2) { 
                for (int row = 0; row < height; row+=2) {

                        uint64_t extracted_values = 
                                readInCodeword(compressed_file, MAX_WORD_SIZE);
                        Codeword unpacked_word = 
                                                unpackCodeword(extracted_values);
                        
                
                        float pb = 
                                Arith40_chroma_of_index(unpacked_word.avg_pb);
                        float pr = 
                                Arith40_chroma_of_index(unpacked_word.avg_pr);

             
                        DCT_space dct = quantizeInverseDCT(unpacked_word.dct);

        
                        Brightness_values brightnesses =
                                                        computeInverseDCT(dct);

        
                        Pnm_componentvid_flt_pixels comp_vid_block = 
                        create_compvid_pixels_decomp(pr, pb, brightnesses);

        
                        Pnm_rgb_int_pixels new_rgb_block = 
                                create_rgbint_pixels(comp_vid_block, DENOMINATOR);

                        Pnm_rgb pixel0 =
                                methods->at(compressed_image, col, row);
                        Pnm_rgb pixel1 =
                                methods->at(compressed_image, col + 1, row);
                        Pnm_rgb pixel2 =
                                methods->at(compressed_image, col, row + 1);
                        Pnm_rgb pixel3 =
                                methods->at(compressed_image, col + 1, row + 1);

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

