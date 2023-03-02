#include "pnm.h"
#include "pixels.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"


Pnm_component_flt create_component_pixel(float y, float pr, float pb)
{
        Pnm_component_flt pixel = malloc(sizeof(*pixel));
        assert(pixel != NULL);
        pixel->y = y;
        pixel->pr = pr;
        pixel->pb = pb;
        return pixel;
}

Pnm_rgb create_int_pixel(unsigned red, unsigned green, unsigned blue)
{
        Pnm_rgb pixel = malloc(sizeof(*pixel));
        assert(pixel != NULL);
        pixel->red = red;
        pixel->green = green;
        pixel->blue = blue;
        return pixel;
}

Pnm_rgb_flt create_flt_pixel(float red, float green, float blue)
{
        Pnm_rgb_flt pixel = malloc(sizeof(*pixel));
        assert(pixel != NULL);
        pixel->red = red;
        pixel->green = green;
        pixel->blue = blue;
        return pixel;
}