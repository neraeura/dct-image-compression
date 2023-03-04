#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "pixels.h"

typedef A2Methods_UArray2 A2;
typedef A2Methods_mapfun Mapfun;

typedef struct RGBtoInt_cl {
        A2Methods_UArray2 array2;
        A2Methods_T methods;
} RGBtoInt_cl;

typedef struct ComponentToRGB_cl {
        A2Methods_UArray2 array2;
        A2Methods_T methods;
} ComponentToRGB_cl;



// static void apply_freepixel(int col, int row, A2Methods_UArray2 image, void *elem, void *cl) 
// {
//         (void) col;
//         (void) row;
//         (void) image;
//         (void) cl;
//         free(elem);
// }


/* Convert RGB floating point numbers to scaled integers */
static void apply_RGBtoInt(int col, int row, A2Methods_UArray2 RGB_image, void *elem, void *cl) 
{
        (void) RGB_image;
        
        RGBtoInt_cl *RGBtoInt = cl;
        
        Pnm_rgb_flt pixel = (Pnm_rgb_flt) elem;
        int denominator = 255;
        /* convert from scaled to float */
        int r = (int) pixel->red * denominator;
        int g = (int) pixel->green * denominator;
        int b = (int) pixel->blue * denominator;

        /* Get the index of an element in the new array */
        Pnm_rgb_int new_index = (Pnm_rgb_int) RGBtoInt->methods->at(RGBtoInt->array2, col, row);
        /* Create a new_pixel of float representation */
        Pnm_rgb_int new_pixel = create_int_pixel(r, g, b, 255);
        /* Put that pixel in converted_image @ new_index */
        *new_index = *new_pixel;
}

A2Methods_UArray2 RGBtoInt(A2Methods_UArray2 RGB_image, Mapfun map,
                                                A2Methods_T methods) 
{
        
        int width = methods->width(RGB_image);
        int height = methods->height(RGB_image);

      
        A2Methods_UArray2 converted_image = methods->new(width, height, sizeof(struct Pnm_rgb_int));
        assert(converted_image != NULL);

        RGBtoInt_cl cl = {converted_image, methods};
        map(RGB_image, apply_RGBtoInt, &cl);
        //map(RGB_image, apply_freepixel, NULL);

        /* Frees processed_image */
        methods->free(RGB_image);

        return converted_image;
}



static void apply_ComponentToRGB(int col, int row, A2Methods_UArray2 component_image, 
                                  void *elem, void *cl)
{
        (void) component_image; 

        ComponentToRGB_cl *ComponentToRGB = cl;
        Pnm_componentvid_flt pixel = (Pnm_componentvid_flt) elem;

        float pr = (float) pixel->pr;
        float y = (float) pixel->y;
        float pb = (float) pixel->pb;

        float r = 1.0 * y + 0.0 + pb + 1.402 * pr; 
        float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr; 
        float b = 1.0 * y + 1.772 * pb + 0.0 * pr;

         /* Get the index of an element in the new array */
        Pnm_rgb_flt new_index = (Pnm_rgb_flt) ComponentToRGB->methods->at(ComponentToRGB->array2, col, row);
        /* Create a new_pixel of float representation */
        Pnm_rgb_flt new_pixel = create_flt_pixel(r, g, b);
        /* Put that pixel in converted_image @ new_index */
        *new_index = *new_pixel;
}

A2Methods_UArray2 ComponentVideotoRGB(A2Methods_UArray2 component_image, Mapfun map,
                                                A2Methods_T methods)
{
        int width = methods->width(component_image);
        int height = methods->height(component_image);

        A2Methods_UArray2 RGB_image = methods->new(width, height,
                                                sizeof(struct Pnm_rgb_flt));

        struct ComponentToRGB_cl cl = {RGB_image, methods};
        map(component_image, apply_ComponentToRGB, &cl);
        // map(component_image, apply_freepixel, NULL);

        /* Frees component_image */
        methods->free(component_image);
        return RGB_image;
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

