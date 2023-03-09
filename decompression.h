
#ifndef DECOMPRESSION_INCLUDED
#define DECOMPRESSION_INCLUDED

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

void decompress_image(A2Methods_UArray2 original_image, A2Methods_T methods);

#endif