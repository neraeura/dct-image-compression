
#ifndef DECOMPRESSION_INCLUDED
#define DECOMPRESSION_INCLUDED


#include <stdlib.h>
#include <stdio.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

void decompressImage(A2Methods_UArray2 original_image, A2Methods_T methods,
                                                                FILE *input);

Pnm_ppm readHeader(FILE *in);


#endif