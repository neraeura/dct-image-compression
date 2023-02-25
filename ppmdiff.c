


/******************************** ppmdiff ********************************
 * 
 *  Purpose: Compare two images to get a quantitative estimate of how similar 
 *           they are.
 *  Parameters: Two arguments on the command line
 *              1) the name of a PPM file 
 *              2) the name of a PPM file or C-string "-"
 *         
 *  Returns: A single number which is a measure of the difference between two   
 *           input images.
 *  Effects: 
 *  Expects: 
 * 
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

typedef A2Methods_UArray2 A2;
typedef A2Methods_T methods;

struct pixelData {
        A2 image1;
        A2 image2;
        unsigned diff_total;
        A2Methods_Object *(*at)(A2 array2, int i, int j); 
};


double calculateMSE(A2 image1, A2 image2, unsigned width, unsigned height, A2Methods_T methods);

int main(int argc, char *argv[]) 
{
        FILE *fp1; 
        FILE *fp2; 
        Pnm_ppm ppm1; 
        Pnm_ppm ppm2;

        assert(argc == 3);
        
        A2Methods_T methods = uarray2_methods_plain; 

        /* Open the files and read into a PPM */
        if (strcmp(argv[1], "-") == 0) {
                fp1 = fopen(argv[1], "r"); 
                assert(fp1);
                ppm1 = Pnm_ppmread(fp1, methods);
                ppm2 = Pnm_ppmread(stdin, methods);
        } else if (strcmp("-", argv[2]) == 0) {
                fp2 = fopen(argv[2], "r");
                assert(fp2);
                ppm1 = Pnm_ppmread(stdin, methods);
                ppm2 = Pnm_ppmread(fp2, methods);
        } else {
                fp1 = fopen(argv[1], "r"); 
                assert(fp1);
                
                fp2 = fopen(argv[2], "r");
                assert(fp2);
               
                ppm1 = Pnm_ppmread(fp1, methods);
                ppm2 = Pnm_ppmread(fp2, methods);

        }

        fclose(fp1);
        fclose(fp2);

        /* Checking width and height of for difference greater than 1*/ 
        unsigned height_diff = ppm1->height - ppm2->height;
        unsigned width_diff = ppm1->width - ppm2->width;
        
        if (height_diff > 1 || width_diff > 1) {
                Pnm_ppmfree(&ppm1);
                Pnm_ppmfree(&ppm2);
                fprintf(stderr, "Difference between image width and height greater than 1\n");
                fprintf(stdout, "1.0");
                exit(1);
        }


        A2Methods_UArray2 image1 = ppm1->pixels; 
        A2Methods_UArray2 image2 = ppm2->pixels;

        unsigned height = ppm1->height;
        unsigned width = ppm1->width; 

        return calculateMSE(image1, image2, width, height, uarray2_methods_plain);

}


void extractRGB(int col, int row, A2 image, void *method, void *cl)
{                                                  
        /* extract the pixel at col, row */
        (void) method;
        (void) image;

        struct pixelData *images = cl;
        A2 img1 = images->image1;
        A2 img2 = images->image2;

        A2Methods_Object *pixel1 = images->at(img1, col, row);
        A2Methods_Object *pixel2 = images->at(img2, col, row);

        int red_diff = ((struct Pnm_rgb*) pixel1)->red - ((struct Pnm_rgb*) pixel2)->red;
        int sq_red_diff = red_diff * red_diff;
        
        int green_diff = ((struct Pnm_rgb*) pixel1)->green - ((struct Pnm_rgb*) pixel2)->green;
        int sq_green_diff = green_diff * green_diff;
        
        int blue_diff = ((struct Pnm_rgb*) pixel1)->blue - ((struct Pnm_rgb*) pixel2)->blue; 
        int sq_blue_diff = blue_diff * blue_diff; 

        struct pixelData *pd = (struct pixelData *)cl;
        pd->diff_total += sq_red_diff + sq_green_diff + sq_blue_diff;
        
        //images->diff_total += sq_red_diff + sq_green_diff +sq_blue_diff;
                                                             
}

double calculateMSE(A2 image1, A2 image2, unsigned width, unsigned height, A2Methods_T methods) 
{
        
        double mse = 0.0;

        A2Methods_mapfun *map = methods->map_default;
        
        struct pixelData temp = {image1, image2, 0, methods->at};
        map(image1, extractRGB, &temp);

        mse = temp.diff_total / (3 * width * height);
        
        return mse;
}


