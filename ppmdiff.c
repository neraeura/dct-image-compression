#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

typedef A2Methods_UArray2 A2;
typedef A2Methods_T methods;

int main(int argc, char *argv[]) {
        FILE *fp1; 
        FILE *fp2; 
        Pnm_ppm image1; 
        Pnm_ppm image2;

        assert(argc == 3);
        
        A2Methods_T methods = uarray2_methods_plain; 

        /* Open the files and read into a PPM */
        if (strcmp(argv[1], "-") == 0) {
                fp1 = fopen(argv[1], "r"); 
                assert(fp1);
                image1 = Pnm_ppmread(fp1, methods);
                image2 = Pnm_ppmread(stdin, methods);
                fclose(fp1);
        } else if (strcmp("-", argv[2]) == 0) {
                fp2 = fopen(argv[2], "r");
                assert(fp2);
                image1 = Pnm_ppmread(stdin, methods);
                image2 = Pnm_ppmread(fp2, methods);
                fclose(fp2);
        } else {
                fp1 = fopen(argv[1], "r"); 
                assert(fp1);
                
                fp2 = fopen(argv[2], "r");
                assert(fp2);
               
                image1 = Pnm_ppmread(fp1, methods);
                image2 = Pnm_ppmread(fp2, methods);

                fclose(fp1);
                fclose(fp2);
        }


        if (abs(uarray2_methods_plain->width(image1) - uarray2_methods_plain->width(image2)) > 1 ||
                abs(uarray2_methods_plain->height(image1) - uarray2_methods_plain->height(image2)) > 1) {
                        fprintf(stderr, "Error: images have incompatible sizes\n");
                        Pnm_ppmfree(&image1);
                        Pnm_ppmfree(&image2);
                        return 1;
        }

        int w = uarray2_methods_plain->width(image1) < uarray2_methods_plain->width(image2) ? uarray2_methods_plain->width(image1) : uarray2_methods_plain->width(image2);
        int h = uarray2_methods_plain->height(image1) < uarray2_methods_plain->height(image2) ? uarray2_methods_plain->height(image1) : uarray2_methods_plain->height(image2);

        double sum_sq_diff = 0.0;
        int n_pixels = 3 * w * h;
        
        A2Methods_UArray2 ppm1 = image1->pixels; 
        A2Methods_UArray2 ppm2 = image2->pixels;


        for (int i = 0; i < w; i++) {
                        for (int j = 0; j < h; j++) {
                                A2Methods_Object *pixel1 = uarray2_methods_plain->at(ppm1, i, j);
                                A2Methods_Object *pixel2 = uarray2_methods_plain->at(ppm2, i, j);

                                /* convert from scaled to float */
                                double r1 = (double) ((struct Pnm_rgb*) pixel1)->red / (double) image1->denominator;
                                double g1 = (double) ((struct Pnm_rgb*) pixel1)->green / (double) image1->denominator; 
                                double b1 = (double) ((struct Pnm_rgb*) pixel1)->blue / (double) image1->denominator;

                                double r2 = (double) ((struct Pnm_rgb*) pixel2)->red / (double) image2->denominator;
                                double g2 = (double) ((struct Pnm_rgb*) pixel2)->green / (double) image2->denominator;
                                double b2 = (double) ((struct Pnm_rgb*) pixel2)->blue / (double) image2->denominator;

                                double sq_diff = pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2);
                                sum_sq_diff += sq_diff;
                }
        }

        Pnm_ppmfree(&image1);
        Pnm_ppmfree(&image2);

        double rms_diff = sqrt(sum_sq_diff / n_pixels);

        printf("%.4f\n", rms_diff);

        return 0;

}