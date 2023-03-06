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

DCT_space_int quantize_dct(DCT_space block)
{
        int scaled_a = (fabs(block.a * 50) < 0.3) ? (block.a * 50) : 0.3 * sign(block.a);
        int scaled_b = (fabs(block.b * 50) < 0.3) ? (block.b * 50) : 0.3 * sign(block.b);
        int scaled_c = (fabs(block.c * 50) < 0.3) ? (block.c * 50) : 0.3 * sign(block.c);
        int scaled_d = (fabs(block.d * 50) < 0.3) ? (block.d * 50) : 0.3 * sign(block.d);

        DCT_space_int quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

}

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

                        /* Getting denominator */
                        float denom = (float) denominator;


                        /* Convert block of RGB scaled integers to float points */
                        Pnm_rgb_flt_pixels block1 = create_rgbflt_pixels(pixel0, pixel1, pixel2, pixel3, denom);
                       
                        /* Convert from RGB to CV color space */
                        Pnm_componentvid_flt_pixels block2 = create_compvid_pixels(block1);


                        /* Compute average PR and PB values */
                        float avgpr = compute_pr_avg(block2);
                        float avgpb = compute_pb_avg(block2);

                        /* Computing the chroma */
                        unsigned chroma_pr = Arith40_index_of_chroma(avgpr);
                        unsigned chroma_pb = Arith40_index_of_chroma(avgpb);

                
                        /* DCT */
                        DCT_space values = compute_dct_values(block2); 
                        DCT_space_int int_values = quantize_dct(values);
                        (void) int_values;
                        (void) chroma_pb; 
                        (void) chroma_pr;
                        (void) denom;
                }
        }
}

