#ifndef PIXELS_H
#define PIXELS_H
#include <inttypes.h>

#include "pnm.h"



typedef struct Pnm_rgb_flt {
        float red, green, blue;
} Pnm_rgb_flt;


typedef struct Pnm_componentvid_flt {
        float y, pr, pb;
} Pnm_componentvid_flt;

typedef struct Pnm_componentvid_flt_pixels {
        Pnm_componentvid_flt pix1, pix2, pix3, pix4;
} Pnm_componentvid_flt_pixels;

typedef struct Pnm_rgb_flt_pixels {
        Pnm_rgb_flt pix1, pix2, pix3, pix4;
} Pnm_rgb_flt_pixels;

typedef struct Pnm_rgb_int_pixels {
        Pnm_rgb pix1, pix2, pix3, pix4;
} Pnm_rgb_int_pixels;

typedef struct DCT_space {
        float a, b, c, d;
} DCT_space;

typedef struct DCT_space_int {
        uint64_t a;
        int64_t b, c, d;
} DCT_space_int;

typedef struct Brightness_values { 
        float y_1, y_2, y_3, y_4;
} Brightness_values;

typedef struct Codeword {
        DCT_space_int dct;
        unsigned avg_pr, avg_pb;
} Codeword;





int sign(int num);
Pnm_rgb_flt_pixels create_rgbflt_pixels(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, Pnm_rgb p4, unsigned denominator);
Pnm_componentvid_flt_pixels create_compvid_pixels_comp(Pnm_rgb_flt_pixels block);
Pnm_componentvid_flt_pixels create_compvid_pixels_decomp(float pr, float pb,
                                                Brightness_values brightness);
Pnm_rgb_int_pixels create_rgbint_pixels(Pnm_componentvid_flt_pixels block, unsigned denominator);
DCT_space compute_dct_values(Pnm_componentvid_flt_pixels block);
DCT_space_int quantize_dct(DCT_space block);

#endif