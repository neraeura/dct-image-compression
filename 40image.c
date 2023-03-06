#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "decompression.h"
#include "compression.h"


typedef A2Methods_UArray2 A2;

void compress40(FILE *input);
void decompress40(FILE *input);
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

// A2Methods_UArray2 remake_image(A2Methods_UArray2 original_image, Mapfun map,
//                                 A2Methods_T methods, unsigned denominator)
// {
//         (void) map;
//         int width = methods->width(original_image);
//         int height = methods->height(original_image);
//         A2Methods_UArray2 new_image = methods->new(width, height,
//                                                 sizeof(struct Pnm_rgb_flt));
//         for (int col = 0; col < width; col++) { 
//                 for (int row = 0; row < height; row++) {
//                         Pnm_rgb pixel =
//                                 methods->at(original_image, col, row);

//                         float denom = (float) denominator;
//                         float r = ((Pnm_rgb)pixel)->red / denom;
//                         float g = ((Pnm_rgb)pixel)->green / denom;
//                         float b = ((Pnm_rgb)pixel)->blue / denom;


//                         void *new_index = methods->at(new_image, col, row);
//                         Pnm_rgb_flt new_pixel = create_rgbflt_pixel(r, g, b);
//                         new_index = &new_pixel;
//                         fprintf(stderr, "pixel thing...%f", ((Pnm_rgb_flt) new_index)->red);
//                 }
//         }
//         /* Frees original_image */
//         return new_image;
// }


void compress40(FILE *input)
{
        /* Get methods object in order to access a2 functions */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        // A2Methods_mapfun *map = methods->map_default;
        
        
        /* Create a ppm representing the original image */
        Pnm_ppm ppm;
        ppm = Pnm_ppmread(input, methods);
        (void) input;

        /* Create an array 'original_image' that stores the pixels in the ppm */
        A2Methods_UArray2 original_image = ppm->pixels; 

        // A2Methods_UArray2 new_image = remake_image(original_image, map, methods);
        compress_image(original_image, methods, ppm->denominator);
        Pnm_ppmfree(&ppm); 

}



void decompress40(FILE *input)
{
        (void) input;
}

