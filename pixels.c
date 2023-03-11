#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "pnm.h"
#include "pixels.h"
#include <limits.h>


 /**************************** sign() ***************************************
 * 
 *  Purpose: Determine the signdness of an integer 
 *  Parameters: An single integer  
 *  Returns: -1 if the integer is negative, 1 if the integer is positive 
 *  Effects: This function does not have any side effects. It does not modify 
 *           any variables or data outside of its scope.
 *  Expects: 
 * 
 ****************************************************************************/
int sign(int num) 
{
        
        if (num > 0) {
                return 1;
        }
        return -1;
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
 *  Effects: 
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
 *  Effects: 
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


 /************************** create_rgbint_pixels() ************************
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
        float red = (1.0 * pixel.y + 0.0 * pixel.pb + 1.402 * pixel.pr) * denominator; 
        float green = (1.0 * pixel.y - 0.344136 * pixel.pb - 0.714136 * pixel.pr) * denominator;    
        float blue = (1.0 * pixel.y + 1.772 * pixel.pb + 0.0 * pixel.pr) * denominator;
        
        newpix->red = (red < 0) ? (0) : (red > denominator) ? denominator : red;
        newpix->green = (green < 0) ? (0) : (green > denominator) ? denominator : green;
        newpix->blue = (blue < 0) ? (0) : (blue > denominator) ? denominator : blue;

        return newpix;
}

 /************************** create_rgb_flt_pixels() ************************
 * 
 *  Purpose: Scales a 2x2 block of PPM pixels to a floating point representation 
 *           with the given denominator, effectively transforming from a 
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
 *  Effects:
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

 /*********************** create_compvid_pixels_comp() ************************
 * 
 *  Purpose: Converts a block of four Pnm_rgb_flt pixels into a block of four 
 *           Pnm_componentvid_flt pixels, effecitvely transforming from 
 *           RGB to component video color space.
 *  Parameters: 
 *      1. A block of four Pnm_rgb_flt pixels represented by the structure 
 *         Pnm_rgb_flt_pixels, containing floating-point values for its red, 
 *         green, and blue components
 *  Returns: A block of four Pnm_componentvid_flt pixels represented by the 
 *          tructure Pnm_componentvid_flt_pixels, contianingthe Y, Pb, and Pr 
 *          values for a 2x2 group of pixels
 *  Effects: 
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
 *  Purpose:
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
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

 /************************* create_rgbint_pixels() **************************
 * 
 *  Purpose:
 *  Parameters: 
 *  Returns:  
 *  Effects: 
 *  Expects: 
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

