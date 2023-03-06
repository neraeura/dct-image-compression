#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "pnm.h"
#include "pixels.h"
#include <math.h>


// Pnm_rgb_int create_rgbint_pixel(unsigned red, unsigned green, unsigned blue)
// {
//         Pnm_rgb_int pixel = malloc(sizeof(pixel));
//         assert(pixel != NULL);
//         pixel->red = red;
//         pixel->green = green;
//         pixel->blue = blue;
//         return pixel;
// }

Pnm_rgb_flt create_rgbflt_pixel(Pnm_rgb pixel, float denominator) {
        float r = pixel->red / denominator;
        float g = pixel->green / denominator;
        float b = pixel->blue / denominator;
        Pnm_rgb_flt newpix = {r, g, b};
        return newpix;
}

Pnm_componentvid_flt create_componentvid_pixel(Pnm_rgb_flt pixel) {
        float y = 0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue;
        float pb = -0.168736 * pixel.red - 0.331264 * pixel.green  + 0.114 * pixel.blue;
        float pr = 0.5 * pixel.red - 0.418688 * pixel.green  - 0.081312 * pixel.blue;
        Pnm_componentvid_flt newpix = {y, pr, pb};
        return newpix;
}

Pnm_rgb_flt_pixels create_rgbflt_pixels(Pnm_rgb p1, Pnm_rgb p2, Pnm_rgb p3, Pnm_rgb p4, unsigned denominator)
{
        float denom = (float) denominator;
        Pnm_rgb_flt fltp1 = create_rgbflt_pixel(p1, denom);
        Pnm_rgb_flt fltp2 = create_rgbflt_pixel(p2, denom);
        Pnm_rgb_flt fltp3 = create_rgbflt_pixel(p3, denom);
        Pnm_rgb_flt fltp4 = create_rgbflt_pixel(p4, denom);

        Pnm_rgb_flt_pixels fltPEES = {fltp1, fltp2, fltp3, fltp4};
        return fltPEES;
}

Pnm_componentvid_flt_pixels create_compvid_pixels(Pnm_rgb_flt_pixels block)
{
        Pnm_componentvid_flt compvid_pix1 = create_componentvid_pixel(block.pix1);
        Pnm_componentvid_flt compvid_pix2 = create_componentvid_pixel(block.pix2);
        Pnm_componentvid_flt compvid_pix3 = create_componentvid_pixel(block.pix3);
        Pnm_componentvid_flt compvid_pix4 = create_componentvid_pixel(block.pix4);

        Pnm_componentvid_flt_pixels compvid_PEES = {compvid_pix1, compvid_pix2, compvid_pix3, compvid_pix4};
        return compvid_PEES;
}

