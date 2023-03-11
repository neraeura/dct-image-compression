#ifndef COLORSPACE_H
#define COLORSPACE_H

#include "sharedData.h"
#include "pnm.h"


 /************************** Pnm_rgb_flt_pixels *******************************
 * This struct is defining a type called Pnm_rgb_flt_pixels, which consists of 
 * four elements of type Pnm_rgb_flt: pix1, pix2, pix3, and pix4.
 * 
 * Each Pnm_rgb_flt element represents a single pixel in an image, with red, 
 * green, and blue color channels stored as floating-point values. 
 * Therefore, Pnm_rgb_flt_pixels can be thought of as a container that holds 
 * four adjacent pixels in an image, arranged in a 2x2 block 
 * 
 * This struct allows the image compresion algorithm to operate on blocks of 
 * pixels in an image, rather than individual pixels. 
 * 
 ****************************************************************************/
typedef struct Pnm_rgb_flt_pixels {
        Pnm_rgb_flt pix1, pix2, pix3, pix4;
} Pnm_rgb_flt_pixels;


/**************************** Pnm_rgb_int_pixels ******************************
 * This struct is defining a type called Pnm_rgb_int_pixels, which consists of 
 * four elements of type Pnm_rgb: pix1, pix2, pix3, and pix4.
 * 
 * Each Pnm_rgb element represents a single pixel in an image, with red, green, 
 * and blue color channels stored as integer values.
 *  
 * Therefore, Pnm_rgb_int_pixels can be thought of as a container that holds 
 * four adjacent pixels in an image, arranged in a 2x2 block
 * 
 * This struct allows the image compresion algorithm to operate on blocks of 
 * pixels in an image, rather than individual pixels. 
 * 
 * ****************************************************************************/
typedef struct Pnm_rgb_int_pixels {
        Pnm_rgb pix1, pix2, pix3, pix4;
} Pnm_rgb_int_pixels;




float computePR_avg(Pnm_componentvid_flt_pixels block);
float computePB_avg(Pnm_componentvid_flt_pixels block);
Pnm_rgb_flt_pixels create_rgbflt_pixels(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, 
                                        Pnm_rgb p4, unsigned denominator);
Pnm_componentvid_flt_pixels create_compvid_pixels_comp(Pnm_rgb_flt_pixels block);
Pnm_componentvid_flt_pixels create_compvid_pixels_decomp(float pr, float pb,
                                                Brightness_values brightness);
Pnm_rgb_int_pixels create_rgbint_pixels(Pnm_componentvid_flt_pixels block,
                                                        unsigned denominator);
                                                        

#endif