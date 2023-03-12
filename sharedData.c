 /**************************************************************************
 *                      
 *                              sharedData.c
 * 
 *      Contains data structures and functions needed by multiple modules
 *      responsible for carrying out vital phases of compression and 
 *      decompression.
 * 
 ****************************************************************************/

#include "sharedData.h"

 /**************************** sign() ***************************************
 * 
 *  Purpose: Determine the sign of an integer 
 *  Parameters: An single integer  
 *  Returns: -1 if the integer is negative, 1 if the integer is positive 
 *  Effects: This function does not have any side effects. It does not modify 
 *           any variables or data outside of its scope
 * 
 ****************************************************************************/
int sign(int num) 
{
        if (num > 0) {
                return 1;
        }
        return -1;
}