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

typedef struct Pnm_rgb_flt {
        float red, green, blue;
} Pnm_rgb_flt;



void *create_flt_pixel(float red, float green, float blue)
{
        Pnm_rgb_flt *temp = malloc(sizeof(Pnm_rgb_flt));
        assert(temp);
        temp->red = red;
        temp->green = green;
        temp->blue = blue;
        return temp;
}

A2Methods_UArray2
RGBtoFloat(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        
        A2Methods_UArray2 fltRGB = methods->new(width, height, 
                                                sizeof(struct Pnm_rgb_flt));

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        A2Methods_Object *pixel = methods->at(image, col, row);

                        /* convert from scaled to float */
                        float red = (float) ((struct Pnm_rgb*) pixel)->red;
                        float green = (float) ((struct Pnm_rgb*) pixel)->green;
                        float blue = (float) ((struct Pnm_rgb*) pixel)->blue;
                        
                        float flt_red = red / (float) ((Pnm_ppm) image)->denominator;
                        float flt_green = green / (float) ((Pnm_ppm) image)->denominator; 
                        float flt_blue = blue / (float) ((Pnm_ppm) image)->denominator;

                        void *newpixel = methods->at(fltRGB, col, row);
                        newpixel = create_flt_pixel(flt_red, flt_green, flt_blue);
                        printf("row:%d...%lf\n", row, ((struct Pnm_rgb_flt*) newpixel)->red);
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


                printf("Im here");
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
