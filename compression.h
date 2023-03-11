
#ifndef COMPRESSION_INCLUDED
#define COMPRESSION_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"



#define A2 A2Methods_UArray2

void compressImage(A2Methods_UArray2 original_image,
                        A2Methods_T methods, unsigned denominator);


#undef A2 

#endif