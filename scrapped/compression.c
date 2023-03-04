#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pixels.h"
#include "pnm.h"
#include "compression.h"


typedef A2Methods_UArray2 A2;
typedef A2Methods_mapfun Mapfun;

typedef struct RGBtoFloat_cl {
        A2Methods_UArray2 array2;
        A2Methods_T methods;
        unsigned denominator;
} RGBtoFloat_cl;


typedef struct RGBToComponent_cl {
        A2Methods_UArray2 array2;
        A2Methods_T methods;
} RGBToComponent_cl;


// static void apply_freepixel(int col, int row, A2Methods_UArray2 image, void *elem, void *cl) 
// {
//         (void) col;
//         (void) row;
//         (void) image;
//         (void) cl;
//         free(elem);
// }


A2Methods_UArray2 imageProcessing(A2Methods_UArray2 original_image, Mapfun map,
                                         A2Methods_T methods)
{
        (void) map;
        /* if width and height are not even numbers, trim them */   
        int width = methods->width(original_image);
        int height = methods->height(original_image);
        
        if (width % 2 != 0 || height % 2 != 0) {
                width -= width % 2;
                height -= height % 2;
                A2Methods_UArray2 trimmed_image = methods->new(width, height,
                                                sizeof(struct Pnm_rgb_int));

                for (int col = 0; col < width; col++) { 
                        for (int row = 0; row < height; row++) {
                                A2Methods_Object *pixel =
                                        methods->at(original_image, col, row);

                                int r = ((Pnm_rgb)pixel)->red;
                                int g = ((Pnm_rgb)pixel)->green;
                                int b = ((Pnm_rgb)pixel)->blue;

                
                                Pnm_rgb_int new_index = (Pnm_rgb_int) methods->at(trimmed_image, col, row);
                                Pnm_rgb_int new_pixel = create_int_pixel(r, g, b, 255);
                                *new_index = *new_pixel;
                        }
                }
                /* Frees original_image */
                methods->free(&original_image);
                return trimmed_image;
        }

        /* else, just return the A2 object */
        return original_image;
}


static void apply_RGBtoFloat(int col, int row, A2Methods_UArray2 processed_image, void *elem, void *cl) 
{
        (void) processed_image;
        RGBtoFloat_cl *RGBtoFloat = cl;
        Pnm_rgb_int pixel = (Pnm_rgb_int) elem;

        /* convert from scaled to float */
        float r = (float) pixel->red;
        float g = (float) pixel->green;
        float b = (float) pixel->blue;

        float flt_r = r / (float) RGBtoFloat->denominator;
        float flt_g = g / (float) RGBtoFloat->denominator;
        float flt_b = b / (float) RGBtoFloat->denominator;

        /* Get the index of an element in the new array */
        Pnm_rgb_flt new_index = (Pnm_rgb_flt) RGBtoFloat->methods->at(RGBtoFloat->array2, col, row);
        /* Create a new_pixel of float representation */
        Pnm_rgb_flt new_pixel = create_flt_pixel(flt_r, flt_g, flt_b);
        /* Put that pixel in converted_image @ new_index */
        *new_index = *new_pixel;
        free(pixel);

}

A2Methods_UArray2 RGBtoFloat(A2Methods_UArray2 processed_image, Mapfun map, A2Methods_T methods, unsigned denominator) 
{
        
        int width = methods->width(processed_image);
        int height = methods->height(processed_image);

        /* create a new UArray2 */
        A2Methods_UArray2 fltRGB_image = methods->new(width, height, sizeof(struct Pnm_rgb_flt));
        assert(fltRGB_image != NULL);

        RGBtoFloat_cl RGBtoFloat = {fltRGB_image, methods, denominator};
        map(processed_image, apply_RGBtoFloat, &RGBtoFloat);
        // map(processed_image, apply_freepixel, NULL);

        /* Frees processed_image */
        methods->free(processed_image);

        return fltRGB_image;
}


static void apply_RGBToComponent(int col, int row, A2Methods_UArray2 component_image, 
                                  void *elem, void *cl)
{
        (void) component_image; 

        RGBToComponent_cl *RGBtoComponent = cl;
        Pnm_rgb_flt pixel = (Pnm_rgb_flt) elem;


        float r = (float) pixel->red;
        float g = (float) pixel->green;
        float b = (float) pixel->blue;

        float y = 0.299 * r + 0.587 * g + 0.114 * b;
        float pb = -0.168736 * r - 0.331264 * g + 0.114 * b;
        float pr = 0.5 * r - 0.418688 * g - 0.081312 * b;


        Pnm_componentvid_flt new_index = (Pnm_componentvid_flt) RGBtoComponent->methods->at(RGBtoComponent->array2, col, row);
        Pnm_componentvid_flt new_pixel = create_componentvid_pixel(y, pr, pb);
        *new_index = *new_pixel;
}

A2Methods_UArray2 RGBtoComponentVideo(A2Methods_UArray2 fltRGB_image, Mapfun map,
                                                A2Methods_T methods)
{
        assert(fltRGB_image);
        int width = methods->width(fltRGB_image);
        int height = methods->height(fltRGB_image);

        A2Methods_UArray2 component_image = methods->new(width, height,
                                                        sizeof(struct Pnm_componentvid_flt));

        RGBToComponent_cl cl = {component_image, methods};
        map(fltRGB_image, apply_RGBToComponent, &cl);
        // map(fltRGB_image, apply_freepixel, NULL);

        /* Frees component_image */
        methods->free(fltRGB_image);
        return component_image;
}
