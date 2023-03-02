
#ifndef COMPRESSION_INCLUDED
#define COMPRESSION_INCLUDED



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"



#define A2 A2Methods_UArray2
#define Mapfun A2Methods_mapfun

A2Methods_UArray2 imageProcessing(A2Methods_UArray2 original_image, Mapfun map,
                                                         A2Methods_T methods);

A2Methods_UArray2 RGBtoFloat(A2Methods_UArray2 processed_image, Mapfun map, A2Methods_T methods, unsigned denominator);

A2Methods_UArray2 RGBtoComponentVideo(A2Methods_UArray2 fltRGB_image, Mapfun map,
                                                A2Methods_T methods);


#undef A2 
#undef Mapfun


#endif
