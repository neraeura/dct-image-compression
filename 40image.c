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

typedef A2Methods_UArray2 A2;

typedef struct Pnm_color_flt {
        float red, green, blue;
} Pnm_color_flt;

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


void compress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        Pnm_ppm ppm;
        ppm = Pnm_ppmread(input, methods);
        (void) input;

        A2Methods_UArray2 original_image = ppm->pixels; 
        A2Methods_UArray2 processed_image = imageProcessing(original_image, methods);
        A2Methods_UArray2 fltRGB = RGBtoFloat(processed_image, methods);
        RGBtoComponentVideo(fltRGB, methods);

        /// Start decompressing here

        // printf("\nNow decompressing\n");
        
        ComponentVideotoRGB(fltRGB, methods);
        A2Methods_UArray2 decompressed_image = RGBtoInt(fltRGB, methods);
        assert(decompressed_image != NULL);
       

        Pnm_ppm decompressed_ppm = malloc(sizeof(*decompressed_ppm));

        decompressed_ppm->methods = methods;
        decompressed_ppm->width = methods->width(decompressed_image);
        decompressed_ppm->height = methods->height(decompressed_image);
        decompressed_ppm->pixels = decompressed_image;
        decompressed_ppm->denominator = 255; 
        
        Pnm_ppmwrite(stdout, decompressed_ppm);
        // methods->free(&decompressed_image);
        // methods->free(&original_image);
        // methods->free(&processed_image);
        // Pnm_ppmfree(&decompressed_ppm); 
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

