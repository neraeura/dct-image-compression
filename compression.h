#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

A2Methods_UArray2 imageProcessing(A2Methods_UArray2 original_image, A2Methods_T methods);

A2Methods_UArray2 RGBtoFloat(A2Methods_UArray2 image, A2Methods_T methods);

void
RGBtoComponentVideo(A2Methods_UArray2 image, A2Methods_T methods);