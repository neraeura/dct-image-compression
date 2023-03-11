#ifndef DCTRANSFORMS_H
#define DCTRANSFORMS_H

#include <inttypes.h>
#include "sharedData.h"


typedef struct DCT_space {
        float a, b, c, d;
} DCT_space;

typedef struct DCT_space_int {
        uint32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
} DCT_space_int;

Brightness_values computeInverseDCT(DCT_space block);
DCT_space_int quantizeDCT(DCT_space block);
DCT_space computeDCT(Pnm_componentvid_flt_pixels block);
DCT_space quantizeInverseDCT(DCT_space_int block);

#endif

