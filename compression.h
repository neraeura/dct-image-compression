
#ifndef COMPRESSION_INCLUDED
#define COMPRESSION_INCLUDED

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


#define A2 A2Methods_UArray2

void compressImage(A2Methods_UArray2 original_image,
                        A2Methods_T methods, unsigned denominator);


#undef A2 

#endif