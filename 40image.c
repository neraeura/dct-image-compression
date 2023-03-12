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
        assert(argc - i <= 1); 
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
 *  Purpose: Compress a PPM image
 * 
 *  Parameters: 
 *      1. original_file -- a file pointer that holds the address of the input 
 *                         file containing the PPM image to compress
 *  Returns: None
 * 
 *  Effects: Writes a compressed PPM to stdout. It is a Checked Runtime Error
 *           if 'methods' or 'ppm' has been improperly initialized 
 * 
 *  Expects: Assumes the file pointer points to a valid input image file 
 *           that can be read using the Pnm_ppmread function.
 * 
 ****************************************************************************/
void compress40(FILE *original_file)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        Pnm_ppm ppm;
        assert(ppm != NULL);
        ppm = Pnm_ppmread(original_file, methods);
        A2Methods_UArray2 original_image = ppm->pixels; 

        compressImage(original_image, methods, ppm->denominator);
        Pnm_ppmwrite(stdout, ppm);
        Pnm_ppmfree(&ppm); 
}

 /**************************** decompress40() ****************************
 * 
 *  Purpose: Decompresses a PPM image
 * 
 *  Parameters: 
 *      1. compressed_file -- a file pointer that holds the address of the 
 *                            compressed file containing the PPM image to 
 *                            decompress
 *  Returns: None
 * 
 *  Effects: Writes a decompressed PPM to stdout. It is a Checked Runtime Error
 *           if 'methods' has been improperly initialized to allow access to A2 
 *           functions
 * 
 *  Expects: Assumes the file pointer points to a valid compressed PPM image 
 *           file that can be read 
 * 
 ****************************************************************************/
void decompress40(FILE *compressed_file)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        Pnm_ppm compressed_image = readHeader(compressed_file);
        decompressImage(compressed_image->pixels, methods, compressed_file);
        Pnm_ppmwrite(stdout, compressed_image);
        Pnm_ppmfree(&compressed_image);
}

