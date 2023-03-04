#ifndef PIXELS_H
#define PIXELS_H

#include "pnm.h"


<<<<<<< Updated upstream
typedef struct Pnm_rgb_flt {
        float red, green, blue;
} *Pnm_rgb_flt;


typedef struct Pnm_component_flt {
        float y, pr, pb;
} *Pnm_component_flt;



Pnm_component_flt create_component_pixel(float y, float pr, float pb);
Pnm_rgb_flt create_flt_pixel(float red, float green, float blue);
Pnm_rgb create_int_pixel(unsigned red, unsigned green, unsigned blue);
=======

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

typedef struct DCT_space {
        float a, b, c, d;
} DCT_space;


Pnm_rgb_flt_pixels create_rgbflt_pixels(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, Pnm_rgb p4, unsigned denominator);
Pnm_componentvid_flt_pixels create_compvid_pixels(Pnm_rgb_flt_pixels block);
Pnm_componentvid_flt_pixels create_compvid_pixels(Pnm_rgb_flt_pixels block);
DCT_space compute_dct_values(Pnm_componentvid_flt_pixels block);
>>>>>>> Stashed changes


#endif