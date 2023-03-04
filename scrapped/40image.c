#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "decompression.h"
#include "compression.h"
#include "pixels.h"


typedef A2Methods_UArray2 A2;
typedef A2Methods_mapfun Mapfun;

void compress40(FILE *input);
void decompress40(FILE *input);

/* takes a FP, returns void; creates variable called compress_or_decompress, 
a func pointer that points to compress40 or decompress40 */
static void (*compress_or_decompress)(FILE *input) = compress40;


int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        
        return EXIT_SUCCESS; 
}

A2Methods_UArray2 remake_image(A2Methods_UArray2 original_image, Mapfun map,
                                         A2Methods_T methods)
{
        (void) map;
        int width = methods->width(original_image);
        int height = methods->height(original_image);
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
        return trimmed_image;
}

void compress40(FILE *input)
{
        /* Get methods object in order to access a2 functions */
        A2Methods_T methods = uarray2_methods_plain;
        // A2Methods_T methods2b = uarray2_methods_blocked;

        A2Methods_mapfun *map = methods->map_default;
        // A2Methods_mapfun *map2b = methods2b->map_default;
        assert(methods != NULL);

        /* Create a ppm representing the original image */
        Pnm_ppm ppm;
        ppm = Pnm_ppmread(input, methods);
        (void) input;

       /***************COMPRESSION STARTS BELOW:*******************/

        /* Create an array 'original_image' that stores the pixels in the ppm */
        A2Methods_UArray2 original_image = ppm->pixels; 

        A2Methods_UArray2 new_image = remake_image(original_image, map, methods);
        Pnm_ppmfree(&ppm); 

        /* Pass this image to imageProcessing, so it can be trimmed */
        A2Methods_UArray2 processed_image = imageProcessing(new_image, map, methods);

        /* Change the pixel representation from scaled RGB to float */
        A2Methods_UArray2 fltRGB_image = RGBtoFloat(processed_image, map, methods,
                                                                ppm->denominator);

        /* Change color space from RGB to Component Video */
        A2Methods_UArray2 component_image = RGBtoComponentVideo(fltRGB_image,
                                                                map, methods);

       /***************DECOMPRESSION STARTS BELOW:*******************/

        /* Change color space from Component Video to RGB */
        A2Methods_UArray2 RGB_image = ComponentVideotoRGB(component_image,
                                                                map, methods);

        /* Change pixel representation from RGB floating point representation to scaled integer */
        A2Methods_UArray2 converted_image = RGBtoInt(RGB_image, map, methods);
        
        /* Create a fake decompressed ppm for testing purposes */
        Pnm_ppm decompressed_ppm = malloc(sizeof(*decompressed_ppm));
        decompressed_ppm->methods = methods;
        decompressed_ppm->width = methods->width(converted_image);
        decompressed_ppm->height = methods->height(converted_image);
        decompressed_ppm->pixels = converted_image;
        decompressed_ppm->denominator = 255; 
        
        /* Write the decompressed image to standard output */
        Pnm_ppmwrite(stdout, decompressed_ppm);
        // methods->free(&decompressed_image);


        /* Free the compressed ppm */
        //Pnm_ppmfree(&ppm);
        /* Free the decompressed ppm */
      //  methods->free(&converted_image);
        Pnm_ppmfree(&decompressed_ppm); 


}



void decompress40(FILE *input)
{
        (void) input;
}

// void decompress40(FILE *input)
// {
//         A2Methods_T methods = uarray2_methods_plain;
//         A2Methods_UArray2 decompressed_image = RGBtoInt()

//         writeDecompressedImage(decompressed_image);
// }

