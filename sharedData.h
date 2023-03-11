#ifndef SHARED_DATA_H
#define SHARED_DATA_H

 /**************************** Pnm_rgb_flt() ***********************************
  * This struct is defining a type called Pnm_rgb_flt, which represents a single 
  * pixel in an image with red, green, and blue color channels stored as 
  * floating-point values.
  * 
  * Each Pnm_rgb_flt element represents a single pixel in an image, with the 
  * red, green, and blue color channels stored as separate floating-point 
  * values. This allows for more precise color representation compared to using 
  * integer values.
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
  * represent the color difference components, where pr is the difference 
  * between the red component and the luminance, and pb is the difference 
  * between the blue component and the luminance.
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

 /************************ Pnm_componentvid_flt_pixels() *******************
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


int sign(int num);

#endif