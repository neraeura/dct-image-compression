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

static const unsigned DENOMINATOR = 255;
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
         /* Does this effectively handle case if c != \n ?? */
        assert(c == '\n');
        
        UArray2_T pixels = UArray2_new(width, height, sizeof(struct Pnm_rgb));
        Pnm_ppm pixmap; /* ptr to a struct */
        pixmap->height = height;
        pixmap->width = width;
        pixmap->denominator = DENOMINATOR;
        pixmap->pixels = pixels;
        
        return pixmap;
}
 
void readPixmap(Pnm_ppm pixmap)
{
        int height = pixmap->height;
        int width = pixmap->width;

        /* %4x is a conversion specifier that indicates that the input should be read as a hexadecimal integer with a width of at most 4 characters*/
        int expected_codewords = height * width;
        for (int i = 0; i < expected_codewords; i++) {
                uint32_t codeword;
                int read = fscanf(in, "%4x", &codeword);
                assert(read == 1);
}


DCT_space quantize_dct(DCT_space_int block)
{
        int scaled_a = (fabs(block.a * 50) < 0.3) ? (block.a * 50) : 0.3 * sign(block.a);
        int scaled_b = (fabs(block.b * 50) < 0.3) ? (block.b * 50) : 0.3 * sign(block.b);
        int scaled_c = (fabs(block.c * 50) < 0.3) ? (block.c * 50) : 0.3 * sign(block.c);
        int scaled_d = (fabs(block.d * 50) < 0.3) ? (block.d * 50) : 0.3 * sign(block.d);

        DCT_space_int quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

}


Pixel_space compute_pixel_values(DCT_space block)
{
        int y_1 = block.a - block.b - block.c + block.d;
        int y_2 = block.a - block.b + block.c - block.d; 
        int y_3 = block.a + block.b - block.c - block.d; 
        float y_4 = block.a + block.b + block.c + block.d;
        Pixel_space pixels = {y_1, y_2, y_3, y_4};

        return pixels;
}


/* TODO: ASK TA IF THIS IS MODULAR */
void decompress_image(A2Methods_UArray2 original_image, A2Methods_T methods)
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

                        /* Unpacking */
                        


                        /* Convert 4-bit chroma codes to pB and pR */
                        unsigned avgpr = Arith40_chroma_of_index(chroma_pr);
                        unsigned avgpb = Arith40_chroma_of_index(chroma_pb);

        
                        /* Quantization to get floating point integers */
                        DCT_space flt_values = quantize_dct()

                         /* Inverse DCT */
                        Pixel_space pixel_values = compute_pixel_values(flt_values); 

                        /* Transform from CV to RGB color space */
                        Pnm_rgb_int_pixels rgb_block = create_rgbint_pixels(pixel_values);
                        
                }
        }
}

