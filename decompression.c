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


Brightness_values compute_brightness_values(DCT_space block)
{
        float y_1 = block.a - block.b - block.c + block.d;
        float y_2 = block.a - block.b + block.c - block.d; 
        float y_3 = block.a + block.b - block.c - block.d; 
        float y_4 = block.a + block.b + block.c + block.d;
        Brightness_values pixels = {y_1, y_2, y_3, y_4};

        return pixels;
}
DCT_space quantize_dct_decompress(DCT_space_int block)
{
        int scaled_a = (float) block.a;
        int scaled_b = (float) block.b / 50.0;
        int scaled_c = (float) block.c / 50.0;
        int scaled_d = (float) block.d / 50.0;

        DCT_space quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

}

unsigned char check_EOF(input)
{
        int c = getc(input);
        assert(c != EOF);
        // questionable
        return (unsigned char)(c & 0xff);
}


Codeword fill_codeword(unsigned bits_left, FILE *input) 
{       
        /* Piping each lsb amount into a variable and checking if 
                the EOF character is present the characters              */
        uint32_t a |= check_EOF(input) << (bits_left -= A_SIZE);
        uint32_t b |= check_EOF(input) << (bits_left -= B_SIZE);
        uint32_t c |= check_EOF(input) << (bits_left -= C_SIZE);
        uint32_t d |= check_EOF(input) << (bits_left -= D_SIZE);
        uint32_t pr |= check_EOF(input) << (bits_left -= PR_SIZE);
        uint32_t pb |= check_EOF(input) << (bits_left -= PB_SIZE);
        assert(bits_left == 0);

        Codeword word;
        word.dct.a = (int) a * 1;
        word.dct.b = (int) b;
        word.dct.c = (int) c;
        word.dct.d = (int) d;

        /* Convert 4-bit chroma codes to pB and pR */
        word.avg_pb = Arith40_chroma_of_index(pb);
        word.avg_pr = Arith40_chroma_of_index(pr);
        return word;
}

/* TODO: ASK TA IF THIS IS MODULAR */
void decompress_image(A2Methods_UArray2 original_image, A2Methods_T methods,
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
                        Codeword extracted_values = fill_codeword(lsb, input);

                        /* Quantization to get floating point integers */
                        DCT_space dct = quantize_dct_decompress(extracted_values.dct);
                

                        // /* Convert 4-bit chroma codes to pB and pR */
                        // unsigned avgpr = Arith40_chroma_of_index(chroma_pr);
                        // unsigned avgpb = Arith40_chroma_of_index(chroma_pb)

                         /* Inverse DCT */
                        Brightness_values brightnesses = 
                                                compute_brightness_values(dct);

                        /* create CV pixels */
                        Pnm_componentvid_flt_pixels comp_vid_block = 
                                create_compvid_pixels_decomp(
                                        extracted_values.avg_pr,
                                        extracted_values.avg_pb, 
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
                        pixel0 = rgb_block.pix1;
                        pixel1 = rgb_block.pix2;
                        pixel2 = rgb_block.pix3;
                        pixel3 = rgb_block.pix4;
                }
        }
}

