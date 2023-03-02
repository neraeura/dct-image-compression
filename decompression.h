#ifndef DECOMPRESSION_INCLUDED
#define DECOMPRESSION_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"


#define A2 A2Methods_UArray2
#define Mapfun A2Methods_mapfun

A2Methods_UArray2 ComponentVideotoRGB(A2Methods_UArray2 component_image, Mapfun *map, A2Methods_T methods);

A2Methods_UArray2 RGBtoInt(A2Methods_UArray2 RGB_image, Mapfun *map, A2Methods_T methods);



#undef A2
#undef Mapfun
#endif
