#ifndef SHARED_DATA_H
#define SHARED_DATA_H

typedef struct Pnm_rgb_flt {
        float red, green, blue;
} Pnm_rgb_flt;

typedef struct Pnm_componentvid_flt {
        float y, pr, pb;
} Pnm_componentvid_flt;

typedef struct Brightness_values { 
        float y_1, y_2, y_3, y_4;
} Brightness_values;

typedef struct Pnm_componentvid_flt_pixels {
        Pnm_componentvid_flt pix1, pix2, pix3, pix4;
} Pnm_componentvid_flt_pixels;


 /**************************** sign() ***************************************
 * 
 *  Purpose: Determine the sign of an integer 
 *  Parameters: An single integer  
 *  Returns: -1 if the integer is negative, 1 if the integer is positive 
 *  Effects: This function does not have any side effects. It does not modify 
 *           any variables or data outside of its scope.
 *  Expects: 
 * 
 ****************************************************************************/
int sign(int num) 
{
        
        if (num > 0) {
                return 1;
        }
        return -1;
}

#endif