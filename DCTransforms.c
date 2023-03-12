
/**************************************************************************
 *
 *                               DCTransforms.c
 * 
 *     Assignment: arith
 *     Authors:  Nora A-Rahim and Daniel Opara
 *     CS Logins: narahi01 and dopara01
 *     Date:  March 11, 2023
 *
 *      This file contains the implementation of the DCTransforms interface.
 * 
 *      DCTransforms.c implements the Discrete Cosine Transform (DCT) and 
 *      lienar quantization in order to compress and decompress blocks of 
 *      PPM image data.
 * 
 **************************************************************************/

#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "DCTransforms.h"
#include "sharedData.h"

 
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
 *  Purpose: Quantizes the cosine coefficients of a 2x2 image block using the 
 *           Discrete Cosine Transformation
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
        uint64_t scaled_a = (block.a * 511 < 0) ? (0) : (block.a * 511 > 511) 
                                                ? 511 : block.a * 511;
        int64_t scaled_b = (fabs(block.b) < 0.3) ? (block.b * 50) : 0.3 * 50 *
                                                                sign(block.b);
        int64_t scaled_c = (fabs(block.c) < 0.3) ? (block.c * 50) : 0.3 * 50 *
                                                                sign(block.c);
        int64_t scaled_d = (fabs(block.d) < 0.3) ? (block.d * 50) : 0.3 * 50 * 
                                                                sign(block.d);

        DCT_space_int quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;

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


 /*********************** quantizeInverseDCT() *****************************
 * 
 *  Purpose: Quantizes the cosine coefficients of a 2x2 image block using the 
 *           inverse of a Discrete Cosine Transformation
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
DCT_space quantizeInverseDCT(DCT_space_int block)
{
        float scaled_a = (float) block.a / 511.0; 
        float scaled_b = (float) block.b / 50.0;
        float scaled_c = (float) block.c / 50.0;
        float scaled_d = (float) block.d / 50.0;
        DCT_space quantized_dct = {scaled_a, scaled_b, scaled_c, scaled_d};
        return quantized_dct;
}