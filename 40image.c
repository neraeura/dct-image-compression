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
        Pnm_ppm ppm;
        ppm = Pnm_ppmread(input, methods);
        (void) input;

        A2Methods_UArray2 original_image = ppm->pixels; 
        A2Methods_UArray2 processed_image = imageProcessing(original_image, methods);
        A2Methods_UArray2 fltRGB = RGBtoFloat(processed_image, methods);
        (void) fltRGB;

        A2Methods_UArray2 decompressed_image = RGBtoInt(fltRGB, methods);
        writeDecompressedImage(decompressed_image, methods);
}

// void decompress40(FILE *input)
// {
//         A2Methods_T methods = uarray2_methods_plain;
//         A2Methods_UArray2 decompressed_image = RGBtoInt()

//         writeDecompressedImage(decompressed_image);
// }

