#ifndef DCTRANSFORMS_H
#define DCTRANSFORMS_H

#include "sharedData.h"



/************************** DCT_space ******************************
 * The code declares a struct called "DCT_space" that contains four members:
 * floats "a", "b", "c", "d". This struct is used to represent cosine 
 * coefficients in the context of a Discrete Cosine Transform (DCT). 
 *
 * Each member variable is expressed as a float to contain the raw 
 * values of the cosine coeffecients when the brightness values of 
 * pixels are used to transform to and from cosine space
 ******************************************************************/
typedef struct DCT_space {
        float a, b, c, d;
} DCT_space;


Brightness_values 
computeInverseDCT(DCT_space block);

DCT_space_int 
quantizeDCT(DCT_space block);

DCT_space 
computeDCT(Pnm_componentvid_flt_pixels block);

DCT_space 
quantizeInverseDCT(DCT_space_int block);

#endif

