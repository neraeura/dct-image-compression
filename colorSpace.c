/**************************************************************************
 *
 *                               colorSpace.c
 * 
 *     Assignment: arith
 *     Authors:  Nora A-Rahim and Daniel Opara
 *     CS Logins: narahi01 and dopara01
 *     Date:  March 11, 2023
 *
 *     This file contains the implementation of the colorSpace interface
 *     
 *     colorSpace.c implements conversions between Y'Pb'Pr and RGB color spaces, 
 *     scaled and floating point representations, as well as utilities for 
 *     manipulating color values in PPM images.
 *     
 *
 **************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "colorSpace.h"


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



 /**************************** create_rgbflt_pixel() ************************
 *
 *  Purpose: Creates a new pixel in a floating point representation, 
 *           effectively converting from a scaled representation of pixels to 
 *           a floating point representation
 * 
 *  Parameters: 
 *      1. pixel -- a pointer to a Pnm_rgb struct representing the original 
 *                  pixel with integer color channel values.
 *      2. denominator -- a float value used to divide the color channel values.
 * 
 *  Returns: A Pnm_rgb_flt struct representing the new pixel with floating-point 
 *           color channel values.
 *      
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: Assumes that pixel is a valid pointer to a Pnm_rgb struct. Assumes
 *           denominator is a non-zero float value. If the denominator is zero, 
 *           the function will produce a runtime error.
 * 
 ****************************************************************************/
Pnm_rgb_flt create_rgbflt_pixel(Pnm_rgb pixel, float denominator) 
{
        float r = pixel->red / denominator;
        float g = pixel->green / denominator;
        float b = pixel->blue / denominator;
        Pnm_rgb_flt newpix = {r, g, b};
        return newpix;
}

 /************************** create_rgb_flt_pixels() ************************
 * 
 *  Purpose: Scales a 2x2 block of PPM pixels to a floating point representation 
 *           with the given denominator, effectively converting from a 
 *           scaled to floating point representation 
 *  Parameters: 
 *      1. p1 -- a Pnm_rgb struct representing a pixel
 *      2. p2 -- a Pnm_rgb struct representing a pixel
 *      3. p3 -- a Pnm_rgb struct representing a pixel
 *      4. p4 -- a Pnm_rgb struct representing a pixel
 *      5. denominator - an unsigned int representing the denominator used to 
 *                      scale the pixel values to the floating point 
 *                      representation
 *  Returns: A Pnm_rgb_flt_pixels struct containing four Pnm_rgb_flt structs 
 *           representing the scaled pixels.
 *
 *  Effects: The function has no side effects on external variables or state.
 *
 *  Expects: Assumes that the four Pnm_rgb pixels passed in as arguments are not 
 *           NULL and that the denominator passed in as an argument is greater  
 *           than 0
 * 
 ****************************************************************************/
Pnm_rgb_flt_pixels create_rgbflt_pixels(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, 
                                        Pnm_rgb p4, unsigned denominator)
{
        float denom = (float) denominator;
        Pnm_rgb_flt fltp1 = create_rgbflt_pixel(p1, denom);
        Pnm_rgb_flt fltp2 = create_rgbflt_pixel(p2, denom);
        Pnm_rgb_flt fltp3 = create_rgbflt_pixel(p3, denom);
        Pnm_rgb_flt fltp4 = create_rgbflt_pixel(p4, denom);
        Pnm_rgb_flt_pixels fltPEES = {fltp1, fltp2, fltp3, fltp4};
        return fltPEES;
}

 /************************** create_rgbint_pixel() ************************
 * 
 *  Purpose: Creates a new pixel with integer color channel values from the 
 *           input pixel with Y'PbPr component video color model values, 
 *           effectively transforming from component video to RGB color space.
 * 
 *  Parameters: 
 *      1. pixel --  a Pnm_componentvid_flt struct representing the original 
 *                   pixel with Y'PbPr component video color model values.
 *      2. denominator --  a float value used to scale the color channel values 
 *                         back to the integer range.
 *  Returns: A Pnm_rgb struct representing the new pixel with integer color 
 *           channel values.
 * 
 *  Effects: The function clips the color channel values to the range 
 *           [0, denominator].
 * 
 *  Expects: Assumes that pixel is a valid Pnm_componentvid_flt struct, and 
 *           that denominator is a non-zero float value.
 * 
 ****************************************************************************/
Pnm_rgb create_rgbint_pixel(Pnm_componentvid_flt pixel, float denominator)
{

        Pnm_rgb newpix = malloc(sizeof(*newpix));
        assert(newpix);
        float red = (1.0 * pixel.y + 0.0 * pixel.pb + 1.402 
                                                * pixel.pr) * denominator; 
        float green = (1.0 * pixel.y - 0.344136 * pixel.pb - 0.714136 
                                                * pixel.pr) * denominator;    
        float blue = (1.0 * pixel.y + 1.772 * pixel.pb + 0.0 
                                                * pixel.pr) * denominator;
        
        newpix->red = (red < 0) ? (0) : (red > denominator) 
                                ? denominator : red;

        newpix->green = (green < 0) ? (0) : (green > denominator)
                                    ? denominator : green;

        newpix->blue = (blue < 0) ? (0) : (blue > denominator) 
                                  ? denominator : blue;

        return newpix;
}

 /************************* create_rgbint_pixels() **************************
 * 
 *  Purpose: Creates an array of Pnm_rgb_int_pixels from an array of 
 *           Pnm_componentvid_flt_pixels, where each pixel in the 
 *           Pnm_componentvid_flt_pixels array is converted to Pnm_rgb 
 *           format with an integer range of 0-255.
 * 
 *  Parameters: 
 *      1. block -- an array of Pnm_componentvid_flt_pixels, which represents a 
 *                  2x2 block of pixels in component-video format.
 *      2. denominator -- an unsigned integer representing the maximum value of 
 *                        the input values, which is used to scale the RGB 
 *                        values.
 *  Returns: A Pnm_rgb_int_pixels struct that contains the converted Pnm_rgb 
 *           pixels.
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: The input Pnm_componentvid_flt_pixels array must have four 
 *           elements. The denominator value should be a positive integer value. 
 *           The input Pnm_componentvid_flt_pixels values should be in the 
 *           range [0, 1].
 * 
 ****************************************************************************/
Pnm_rgb_int_pixels create_rgbint_pixels(Pnm_componentvid_flt_pixels block, 
unsigned denominator)
{
        float denom = (float) denominator;
        Pnm_rgb pix1 = create_rgbint_pixel(block.pix1, denom);
        Pnm_rgb pix2 = create_rgbint_pixel(block.pix2, denom);
        Pnm_rgb pix3 = create_rgbint_pixel(block.pix3, denom);
        Pnm_rgb pix4 = create_rgbint_pixel(block.pix4, denom);
        Pnm_rgb_int_pixels intPEES = {pix1, pix2, pix3, pix4};
        return intPEES;
}


 /************************ create_componentvid_pixel() ************************
 * 
 *  Purpose: Creates a new pixel with Y'PbPr component video color model values 
 *           from the input pixel with floating-point RGB color model values.
 * 
 *  Parameters: 
 *      1. pixel -- a Pnm_rgb_flt struct representing the original pixel with 
 *                  floating-point RGB color model values
 * 
 *  Returns: A Pnm_componentvid_flt struct representing the new pixel with 
 *           Y'PbPr component video color model values.
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: Assumes that pixel is a valid Pnm_rgb_flt struct.
 * 
 ****************************************************************************/
Pnm_componentvid_flt create_componentvid_pixel(Pnm_rgb_flt pixel) 
{
        float y = 0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue;
        float pb = -0.168736 * pixel.red - 0.331264 * pixel.green  + 0.5 
                                                                * pixel.blue;
        float pr = 0.5 * pixel.red - 0.418688 * pixel.green  - 0.081312 
                                                                * pixel.blue;
        
        y = (y < 0) ? (0) : (y > 1) ? 1 : y;
        pb = (fabs(pb) < 0.5) ? (pb) : 0.5 * sign((int)pb);
        pr = (fabs(pr) < 0.5) ? (pr) : 0.5 * sign((int)pr);

        Pnm_componentvid_flt newpix = {y, pr, pb};
        return newpix;
}

/*********************** create_compvid_pixels_comp() ************************
 * 
 *  Purpose: Converts a block of four Pnm_rgb_flt pixels into a block of four 
 *           Pnm_componentvid_flt pixels, effecitvely transforming from 
 *           RGB to component video color space.
 * 
 *  Parameters: 
 *      1. A block of four Pnm_rgb_flt pixels represented by the structure 
 *         Pnm_rgb_flt_pixels, containing floating-point values for its red, 
 *         green, and blue components
 * 
 *  Returns: A block of four Pnm_componentvid_flt pixels represented by the 
 *          structure Pnm_componentvid_flt_pixels, contianing the Y, Pb, and Pr 
 *          values for a 2x2 group of pixels
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects: Assumes the input block contains valid Pnm_rgb_flt pixels with 
 *           values between 0 and the specified denominator. The 
 *           Pnm_componentvid_flt_pixels structure must be defined with four 
 *           Pnm_componentvid_flt variables, named pix1, pix2, pix3, and pix4, 
 *           in that order.
 * 
 ****************************************************************************/
Pnm_componentvid_flt_pixels create_compvid_pixels_comp(Pnm_rgb_flt_pixels block)
{
        Pnm_componentvid_flt compvid_pix1 = 
                                        create_componentvid_pixel(block.pix1);
        Pnm_componentvid_flt compvid_pix2 = 
                                        create_componentvid_pixel(block.pix2);
        Pnm_componentvid_flt compvid_pix3 = 
                                        create_componentvid_pixel(block.pix3);
        Pnm_componentvid_flt compvid_pix4 = 
                                        create_componentvid_pixel(block.pix4);
        Pnm_componentvid_flt_pixels compvid_PEES = {compvid_pix1, compvid_pix2, 
                                                compvid_pix3, compvid_pix4};
        return compvid_PEES;
}

 /*********************** create_compvid_pixels_decomp() *******************
 * 
 *  Purpose: Converts a
 *  Parameters: 
 *      1. pr --  a float value representing the red-green chrominance component 
 *                value of the pixel. 
 *      2. pb -- a float value representing the blue-green chrominance component 
 *               value of the pixel.
 *      3. brightness: A Brightness_values struct containing the luminance     
 *                     values for the four pixels that make up a 2x2 image 
 *                     block
 * 
 *  Returns: A block of four Pnm_componentvid_flt_pixels struct that 
 *           represents the decompressed version of the image 
 * 
 *  Effects: The function has no side effects on external variables or state.
 * 
 *  Expects:  The function expects valid float values for pr and pb, and a 
 *            valid Brightness_values struct containing the brightness component 
 *            values for the four pixels that make up the component video. 
 *          
 * 
 ****************************************************************************/
Pnm_componentvid_flt_pixels create_compvid_pixels_decomp(float pr, float pb,
                                                Brightness_values brightness)
{
        Pnm_componentvid_flt compvid_pix1 = {brightness.y_1, pr, pb};
        Pnm_componentvid_flt compvid_pix2 = {brightness.y_2, pr, pb};
        Pnm_componentvid_flt compvid_pix3 = {brightness.y_3, pr, pb};
        Pnm_componentvid_flt compvid_pix4 = {brightness.y_4, pr, pb};
        Pnm_componentvid_flt_pixels compvid_PEES = {compvid_pix1, compvid_pix2, 
                                                compvid_pix3, compvid_pix4};
        return compvid_PEES;
}