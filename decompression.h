#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"


void writeDecompressedImage(A2Methods_UArray2 decompressed, A2Methods_T methods);

A2Methods_UArray2 RGBtoInt(A2Methods_UArray2 image, A2Methods_T methods);