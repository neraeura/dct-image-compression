
/**************************************************************************
 *
 *                               compression.c
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

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"


#include "compression.h"
#include "colorSpace.h"
#include "DCTransforms.h"
#include "bitpack.h"
#include "fileIO.h"
#include "arith40.h"

static const unsigned A_SIZE = 9;
static const unsigned B_SIZE = 5;
static const unsigned C_SIZE = 5;
static const unsigned D_SIZE = 5;
static const unsigned PR_SIZE = 4;
static const unsigned PB_SIZE = 4;
static const unsigned MAX_WORD_SIZE = 32;



typedef struct Codeword {
        DCT_space_int dct;
        unsigned avg_pr, avg_pb;
} Codeword;



 /**************************** packCodeword() *********************************
 * 
 *  Purpose: Packs a, b, c, d, PB, PR into a 32-bit codeword
 * 
 *  Parameters: 
 *     1. word -- a Codeword struct containing the data to be packed
 *     2. An unsigned integer named "bits_left", which represents the number 
 *        of bits available to pack the Codeword structure into a 32-bit 
 *        codeword.
 * 
 *  Returns: A 32-bit unsigned integer representing the packed codeword
 * 
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
        raw_word = Bitpack_news(raw_word, B_SIZE, bit_count -= B_SIZE, 
                                                                word.dct.b);
        raw_word = Bitpack_news(raw_word, C_SIZE, bit_count -= C_SIZE, 
                                                                word.dct.c);
        raw_word = Bitpack_news(raw_word, D_SIZE, bit_count -= D_SIZE, 
                                                                word.dct.d);                                              
        raw_word = Bitpack_newu(raw_word, PB_SIZE, bit_count -= PB_SIZE, 
                                                                word.avg_pb);
        raw_word = Bitpack_newu(raw_word, PR_SIZE, bit_count -= PR_SIZE, 
                                                                word.avg_pr);
        assert(bit_count == 0);
        assert(Bitpack_fitsu(raw_word, MAX_WORD_SIZE));
        
        return raw_word;
}

 /**************************** compressImage() *********************************
 * 
 *  Purpose: Delineates the overall control flow of the image compressor 
 * 
 *  Parameters:
 *      1. original_image -- a 2D UArray2 that represents the original image
 *      2. methods -- an A2Methods_T object that provides access to methods 
 *                   for operating on the UArray2.
 *      3. denominator -- an unsigned integer that represents the denominator 
 *                        used in the Pnm_ppm structure.
 *  
 *  Returns: None
 * 
 *  Effects:  Prints the compressed image format and the compressed codewords to 
 *            stdout. It is a Checked Runtime Error for original_image or 
 *            mehtods to be NULL.
 * 
 *  Expects: This function expects that the original_image parameter is not 
 *           NULL, and that the methods parameter is not NULL and provides 
 *           access to the necessary methods for operating on original_image. 
 *           It also expects that the denominator parameter is a positive 
 *           integer. Additionally, it assumes that the dimensions of the 
 *           original_image are even, and that there are enough pixels in the 
 *           image to form 2x2 blocks.
 * 
 ****************************************************************************/
void compressImage(A2Methods_UArray2 original_image,
                        A2Methods_T methods, unsigned denominator)
{
        assert(original_image != NULL);
        assert(methods != NULL);
        
        int width = methods->width(original_image);
        int height = methods->height(original_image);

        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
        }

        printf("COMP40 Compressed image format 2\n%u %u", width, height);

        for (int row = 0; row < height; row += 2) { 
                for (int col = 0; col < width; col += 2) {
      
                        Pnm_rgb pixel0 =
                                methods->at(original_image, col, row);
                        Pnm_rgb pixel1 =
                                methods->at(original_image, col + 1, row);
                        Pnm_rgb pixel2 =
                                methods->at(original_image, col, row + 1);
                        Pnm_rgb pixel3 =
                                methods->at(original_image, col + 1, row + 1);

                
                        Pnm_rgb_flt_pixels rgb_block =
                        create_rgbflt_pixels(pixel0, pixel1, 
                                             pixel2, pixel3, 
                                             denominator);


                        Pnm_componentvid_flt_pixels compvid_block = 
                                        create_compvid_pixels_comp(rgb_block);
 

                        float avgpr = computePR_avg(compvid_block);
                        float avgpb = computePB_avg(compvid_block);

  
                        unsigned chroma_pr = Arith40_index_of_chroma(avgpr);
                        unsigned chroma_pb = Arith40_index_of_chroma(avgpb);

       
                        DCT_space values = computeDCT(compvid_block); 
                        DCT_space_int DCT_int_values = quantizeDCT(values);

                        Codeword encoded = 
                                        {DCT_int_values, chroma_pr, chroma_pb};
                       
                        uint32_t packed_word = packCodeword(encoded);
                        printCodeword(packed_word);
                }
        }
}