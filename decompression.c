#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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


void *create_int_pixel(unsigned red, unsigned green, unsigned blue)
{
        Pnm_rgb pixel = malloc(sizeof(*pixel));
        assert(pixel != NULL);
        pixel->red = red;
        pixel->green = green;
        pixel->blue = blue;
        return pixel;
}


// void readHeader(FILE *in)
// {
//         unsigned height, width;
//         int read = fscanf(in, "COMP40 Compressed image format 2\n%u %u", 
//                                 &width, &height); assert(read == 2);
//         int c = getc(in);
//         assert(c == '\n');
//         /* handle case if c != \n ?? */

//         UArray2_T pixels = UArray2_new(width, height, sizeof(struct Pnm_rgb));
//         Pnm_ppm pixmap; /* ptr to a struct */
//         pixmap->height = height;
//         pixmap->width = width;
//         pixmap->denominator = denominator;
//         pixmap->pixels = pixels;
        
//         // struct Pnm_ppm compressed_ppm = { .width = width, .height = height, 
//         //                         .denominator = demoninator, .pixels = array, 
//         //                         .methods = methods };                              
// }




/* Convert RGB floating point numbers to scaled integers */
A2Methods_UArray2
RGBtoInt(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        int denominator = 255; 

        
        A2Methods_UArray2 intRGB = methods->new(width, height, 
                                                sizeof(struct Pnm_color_flt));

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        A2Methods_Object *pixel = methods->at(image, col, row);

                        /* convert from scaled to float */
                        int red = (int) ((Pnm_color_flt) pixel)->red * denominator;
                        int green = (int) ((Pnm_color_flt) pixel)->green * denominator;
                        int blue = (int) ((Pnm_color_flt) pixel)->blue * denominator;

                        Pnm_rgb newpixel = methods->at(intRGB, col, row);
                        *newpixel = *((Pnm_rgb) create_int_pixel(red, green, blue));
                        // newpixel = (Pnm_rgb) newpixel;
                        // printf("DECOMPRESSED PIXEL VALUE....%d\n", blue);
                }
        }

        return intRGB;
}

/* Convert from Component Video Color Space to RGB */
void
ComponentVideotoRGB(A2Methods_UArray2 image, A2Methods_T methods)
{
        int width = methods->width(image);
        int height = methods->height(image);

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        Pnm_color_flt pixel = (Pnm_color_flt) methods->at(image, col, row);
                        float pr = (float) ((struct Pnm_color_flt*) pixel)->red;
                        float y = (float) ((struct Pnm_color_flt*) pixel)->green;
                        float pb = (float) ((struct Pnm_color_flt*) pixel)->blue;

                        float r = 1.0 * y + 0.0 + pb + 1.402 * pr; 
                        float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr; 
                        float b = 1.0 * y + 1.772 * pb + 0.0 * pr;

                        pixel->red = r;
                        pixel->blue = b; 
                        pixel->green = g; 
                }
        }
}