#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

typedef A2Methods_UArray2 A2;

/* When converted to component video format, green 
   stores the lum of the image, red stores the red 
   component, and blue stored the blue component */
typedef struct Pnm_color_flt {
        float red, green, blue;
} *Pnm_color_flt;



void *create_flt_pixel(float red, float green, float blue)
{
        Pnm_color_flt pixel = malloc(sizeof(*pixel));
        assert(pixel != NULL);
        pixel->red = red;
        pixel->green = green;
        pixel->blue = blue;
        return pixel;
}

A2Methods_UArray2
RGBtoFloat(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        
        A2Methods_UArray2 fltRGB = methods->new(width, height, 
                                                sizeof(struct Pnm_color_flt));

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        A2Methods_Object *pixel = methods->at(image, col, row);

                        /* convert from scaled to float */
                        float red = (float) ((Pnm_rgb) pixel)->red;
                        float green = (float) ((Pnm_rgb) pixel)->green;
                        float blue = (float) ((Pnm_rgb) pixel)->blue;
                        
                        float flt_red = red / (float) ((Pnm_ppm) image)->denominator;
                        float flt_green = green / (float) ((Pnm_ppm) image)->denominator; 
                        float flt_blue = blue / (float) ((Pnm_ppm) image)->denominator;

                        Pnm_color_flt newpixel = methods->at(fltRGB, col, row);
                        *newpixel = *((Pnm_color_flt) create_flt_pixel(flt_red, flt_green, flt_blue));
                        // printf("pixel value....%lf\n", ((Pnm_color_flt)newpixel)->blue);
                        
                        
                }
        }
        return fltRGB;
}


A2Methods_UArray2 
imageProcessing(A2Methods_UArray2 original_image, A2Methods_T methods)
{
        /* if width and height are not even numbers, trim them */   
        int width = methods->width(original_image);
        int height = methods->height(original_image);
        
        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
                A2Methods_UArray2 trimmed_image = methods->new(width, height,
                                                        sizeof(struct Pnm_rgb));


                printf("Im here on line 81\n");
                for (int col = 0; col < width; col++) { 
                        for (int row = 0; row < height; row++) { 
                                *((struct Pnm_rgb*) methods->at(trimmed_image, 
                                col, row)) = *((struct Pnm_rgb*) 
                                methods->at(original_image, col, row));
                        }
                }
                return trimmed_image;
        }

        /* else, just return the A2 object */
        return original_image;
}


void
RGBtoComponentVideo(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        Pnm_color_flt pixel = (Pnm_color_flt) methods->at(image, col, row);
                        float red = (float) ((Pnm_color_flt) pixel)->red;
                        float green = (float) ((Pnm_color_flt) pixel)->green;
                        float blue = (float) ((Pnm_color_flt) pixel)->blue;

                        float y = 0.299 * red + 0.587 * green + 0.114 * blue;
                        float pb = -0.168736 * red - 0.331264 * green 
                                                                + 0.114 * blue;
                        float pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;


                        pixel->red = pr;
                        pixel->blue = pb;
                        pixel->green = y;
                }
        }
}
