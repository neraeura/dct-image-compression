#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"


typedef A2Methods_UArray2 A2;

typedef struct Pnm_rgb_flt {
        float red, green, blue;
} Pnm_rgb_flt;


void *create_int_pixel(unsigned red, unsigned green, unsigned blue)
{
        Pnm_rgb *temp = malloc(sizeof(Pnm_rgb));
        assert(temp);
        (*temp)->red = red;
        (*temp)->green = green;
        (*temp)->blue = blue;
        return temp;
}

/* Write the contents of the A2 object containing decompressed image to stdout */
void writeDecompressedImage(A2Methods_UArray2 decompressed, A2Methods_T methods)
{
       Pnm_ppm decompressed_ppm; 
       decompressed_ppm->width = methods->width(decompressed);
       decompressed_ppm->height = methods->height(decompressed);
       decompressed_ppm->denominator = 255;
       decompressed_ppm->pixels = decompressed;
       Pnm_ppmwrite(stdout, decompressed_ppm);
}


/* Convert RGB floating point numbers to scaled integers */
A2Methods_UArray2
RGBtoInt(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        int denominator = 255; 

        
        A2Methods_UArray2 intRGB = methods->new(width, height, 
                                                sizeof(struct Pnm_rgb_flt));

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        A2Methods_Object *pixel = methods->at(image, col, row);

                        /* convert from scaled to float */
                        int red = (int) ((struct Pnm_rgb_flt*) pixel)->red * denominator;
                        int green = (int) ((struct Pnm_rgb_flt*) pixel)->green * denominator;
                        int blue = (int) ((struct Pnm_rgb_flt*) pixel)->blue * denominator;

                        void *newpixel = methods->at(intRGB, col, row);
                        newpixel = create_int_pixel(red, green, blue);
                        printf("row:%d...%d\n", row, ((struct Pnm_rgb*) newpixel)->red);
                }
        }
        return intRGB;
}
