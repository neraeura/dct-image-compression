#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <inttypes.h>

 /**************************** Pnm_rgb_flt() ***********************************
  * This struct is defining a type called Pnm_rgb_flt, which represents a single 
  * pixel in an image with red, green, and blue color channels stored as 
  * floating-point values.
  * 
  * Each Pnm_rgb_flt element represents a single pixel in an image, with the 
  * red, green, and blue color channels stored as separate floating-point 
  * values, enabling transformation between scaled and float representations of 
  * pixels.
  * 
  *****************************************************************************/ 
typedef struct Pnm_rgb_flt {
        float red, green, blue;
} Pnm_rgb_flt;


 /********************* Pnm_componentvid_flt() *****************************
  * This struct is defining a type called Pnm_componentvid_flt, which represents 
  * single pixel in a video frame with luminance and color difference channels 
  * stored as floating-point values.
  * 
  * Each Pnm_componentvid_flt element consists of three components: y, pr, and 
  * pb. y represents the luminance (brightness) of the pixel, while pr and pb 
  * represent the color difference components.
  * 
  * The purpose of this struct is to facilitate computations 
  * involving blocks of pixels in an image, where it is more efficient to 
  * operate on blocks of pixels at a time rather than individual pixels.
  * 
  *****************************************************************************/
typedef struct Pnm_componentvid_flt {
        float y, pr, pb;
} Pnm_componentvid_flt;


 /************************** Brightness_values() *****************************
  * This struct is defining a type called Brightness_values, which represents a 
  * container that holds four adjacent brightness values.
  * 
  * Each Brightness_values element consists of four components: y_1, y_2, y_3, 
  * and y_4, which represent the brightness values of four adjacent pixels in an 
  * image. The purpose of this struct is to facilitate computations 
  * involving blocks of pixels in an image, where it is more efficient to 
  * operate on blocks of pixels at a time rather than individual pixels.
  * 
  ****************************************************************************/
typedef struct Brightness_values { 
        float y_1, y_2, y_3, y_4;
} Brightness_values;

 /********************** Pnm_componentvid_flt_pixels() *********************
  * This struct is defining a type called Pnm_componentvid_flt_pixels, which 
  * represents a container that holds four adjacent pixels in a video frame, 
  * where each pixel is represented by the Pnm_componentvid_flt struct.
  * 
  * Each Pnm_componentvid_flt_pixels element consists of four components: 
  * pix1, pix2, pix3, and pix4, where each component represents a single pixel 
  * in the video frame, represented by the Pnm_componentvid_flt struct. 
  * This allows for convenient access and manipulation of a block of four 
  * adjacent pixels in a video frame.
  * 
  **************************************************************************/
typedef struct Pnm_componentvid_flt_pixels {
        Pnm_componentvid_flt pix1, pix2, pix3, pix4;
} Pnm_componentvid_flt_pixels;


/************************** DCT_space_int ******************************
 * The code declares a struct called "DCT_space_int" that contains four members:
 * an unsigned 32-bit integer "a" and three signed 32-bit integers "b", "c", 
 * and "d". This struct is used to represent cosine coefficients in the 
 * context of a Discrete Cosine Transform (DCT). 
 * 
 * Provides a convenient way to organize and access the coefficients 
 * needed to perform the DCT computation. The struct also allows for the 
 * coefficients to be stored and passed around as a single entity, making it 
 * easier to work with and pass the coefficients between functions or modules 
 * in a program. 
 *
 ******************************************************************/
typedef struct DCT_space_int {
        uint32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
} DCT_space_int;

/************************** Codeword ******************************
  * The code declares a struct called "Codeword" that contains two members. 
  * The first member, "dct", is a struct of type "DCT_space_int" that contains 
  * four members representing cosine coefficients. The second member is composed 
  * of two unsigned integers, "avg_pr" and "avg_pb", which represent the 
  * average values of color components for the pixels that correspond to the 
  * cosine coefficients in "dct".
  * 
  * Provides a convenient way to organize and access the coefficients needed to  
  * perform packing and unpacking.
  * 
  ******************************************************************/
typedef struct Codeword {
        DCT_space_int dct;
        unsigned avg_pr, avg_pb;
} Codeword;


int sign(int num);

#endif