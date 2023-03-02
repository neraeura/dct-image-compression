#ifndef PIXELS_H
#define PIXELS_H

#include "pnm.h"


typedef struct Pnm_rgb_flt {
        float red, green, blue;
} *Pnm_rgb_flt;


typedef struct Pnm_component_flt {
        float y, pr, pb;
} *Pnm_component_flt;



Pnm_component_flt create_component_pixel(float y, float pr, float pb);
Pnm_rgb_flt create_flt_pixel(float red, float green, float blue);
Pnm_rgb create_int_pixel(unsigned red, unsigned green, unsigned blue);


#endif