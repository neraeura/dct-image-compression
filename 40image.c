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


 /***************************** compress40() *********************************
 * 
 *  Purpose: Delineates the overall control flow of the image compressor 
 *  Parameters: 
 *      1. An file pointer that holds the address of the input file containing
 *         the PPM image to compress
 *  Returns: None
 *  Effects: Writes a compressed PPM to stdout 
 *  Expects: A valid PPM image for reading
 * 
 ****************************************************************************/
void compress40(FILE *input)
{
        /* Get methods object in order to access a2 functions */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        
        /* Create a ppm representing the original image */
        Pnm_ppm ppm;
        ppm = Pnm_ppmread(input, methods);

        /* Create an array 'original_image' that stores the pixels in the ppm */
        A2Methods_UArray2 original_image = ppm->pixels; 

        // A2Methods_UArray2 new_image = remake_image(original_image, map, methods);
        compressImage(original_image, methods, ppm->denominator);
        Pnm_ppmwrite(stdout, ppm);
        Pnm_ppmfree(&ppm); 

}

 /**************************** decompress40() ****************************
 * 
 *  Purpose: Delineates the overall control flow of the image deompressor 
 *  Parameters: 
 *      1. An file pointer that holds the address of the compressed file 
 *         containing the PPM image to decompress
 *  Returns: None
 *  Effects: Writes a decompressed PPM to stdout 
 *  Expects: A valid compressed PPM image for reading
 * 
 ****************************************************************************/
void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        Pnm_ppm new_image = readHeader(input);
        decompressImage(new_image->pixels, methods, input);
        /* will this follow row-major and big-endian ?*/
        Pnm_ppmwrite(stdout, new_image);
}

