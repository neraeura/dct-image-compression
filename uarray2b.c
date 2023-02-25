/*****************************************************************
 *                      
 *                      uarray2b.c
 * 
 *      Assignment: CS40 Project 3 - locality
 *      Authors: Nora A-Rahim and Daniel Opara 
 *      CS Logins: narahi01, dopara01
 *      Date:   2/16/2023
 *  
 *      This file implements an interface for blocked two dimensional
 *      arrays, referred to as UArray2b.
 *     
*****************************************************************/


#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h>
#include <math.h>
#include "uarray2b.h"
#include "uarray2.h" 
#include <uarray.h>
#define T UArray2b_T

const int MAX_BYTES = 65536;
struct T {
        int height, width;
        int size;
        int blocksize;
        UArray2_T blocks;
};


 /***************************** UArray2b_new() *******************************
 * 
 *  Purpose: Creates a new 2D unboxed array, UArray2b
 *  Parameters:  
 *              int width = Represents the number of columns in the UArray2b
 *              int height = Represents the number of rows in the UArray2b
 *              int size = Represents the number of bytes contained by one 
 *                         element of the UArray2b
 *              int blocksize = Represents the number of cells on one side of 
 *                              the block 
 *  Returns:  A pointer of type UArray2b_T to the recently declared UArray2b
 *  Effects: Throws a checked runtime error if the blocksize is less than 1
 *  Expects: UArray2b_free to deallocate the heap space allocated by this 
 *           function using the pointer returned here 
 * 
 ****************************************************************************/
T UArray2b_new(int width, int height, int size, int blocksize)
{
        T array2b = malloc(sizeof(struct T));
        assert(array2b != NULL);
        assert(blocksize > 0);
        assert(size != 0);
        
        int block_cols = (width  + blocksize - 1) / blocksize;
        int block_rows = (height + blocksize - 1) / blocksize;

        array2b->width = width;
        array2b->height = height;
        array2b->size = size;
        array2b->blocksize = blocksize;
        array2b->blocks = UArray2_new(block_cols, block_rows, sizeof(UArray_T));

        assert(array2b->blocks != NULL);

        for (int col = 0; col < block_cols; col++) {
                for (int row = 0; row < block_rows; row++) {
                        UArray_T *block = UArray2_at(array2b->blocks, col, row);
                        assert(block != NULL);
                        *block = UArray_new(blocksize * blocksize, size);
                }
        }

        return array2b;
}

/************************ UArray2b_new_64K_block() ***************************
 * 
 *  Purpose: Creates a new 2D unboxed array, UArray2b using less than or exactly
                64 kilobytes of memory
 *  Parameters: 
 *              int width = Represents the number of columns in the UArray2b
 *              int height = Represents the number of rows in the UArray2b
 *              int size = Represents the number of bytes contained by one 
 *                         element of the UArray2b
 *  Returns: A pointer of type UArray2b_T to the recently declared UArray2b
 *  Effects: Throws a checked runtime error if the blocksize is less than 1
 *  Expects: UArray2b_free to deallocate the heap space allocated by this 
 *           function using the pointer returned here
 * 
 ****************************************************************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(width > 0 && height > 0);
        assert(size > 0);

        /* create a block size that can fit into 64 * 1024 */
        int blocksize = (int) sqrt((double) MAX_BYTES/(double) size);

        /* if blocksize > MAX_BYTES, make blocksize == 1 */
        if (blocksize == 0)
                blocksize = 1;

        if (size <= MAX_BYTES)
              assert(blocksize * blocksize * size <= MAX_BYTES);  
        
        /* call UArray_2b to make a UArray2b with the block size we chose */
        return UArray2b_new(width, height, size, blocksize);
}

 /**************************** UArray2b_free  ********************************
 * 
 *  Purpose: Deallocates and clears memory associated with a Uarray2b 
 *           allocated on the heap 
 *  Parameters: A pointer to an instance of a UArray2b
 *  Returns: None
 *  Effects: Throw checked runtime error if array2b or *array2b is NULL 
 * 
 ****************************************************************************/
void UArray2b_free(T *array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        int block_cols = 
        ((*array2b)->width + (*array2b)->blocksize - 1)/(*array2b)->blocksize;
        int block_rows = 
        ((*array2b)->height + (*array2b)->blocksize - 1)/(*array2b)->blocksize;

        /* free the UArray in each block */
        for (int col = 0; col < block_cols; col++) {
                for (int row = 0; row < block_rows; row++) {
                        UArray_T *free_me = UArray2_at((*array2b)->blocks, col, 
                                                                        row);
                        assert(*free_me != NULL && free_me != NULL);
                        UArray_free(free_me);
                }
        }
        
        /* free the UArray2 and the struct itself, respectively */
        UArray2_free(&((*array2b)->blocks));
        free(*array2b);
}

 /************************* UArray2b_at() ***********************************
 * 
 *  Purpose: Returns a pointer to the element number 
 *  Parameters: An instance of a UArray2, an int representing the column or 
                ith index, and an int representing the row or jth index of the 
                element which the user is attempting to access
 *  Returns: A void pointer to the UArray element at the specified col
             and row indices 
 *  Effects: Throws a checked runtime if uarr2 is equal to NULL, if column or  
             row is not greater than or equal to 0 and less than the 
             width and height of array2b, respectively
 * 
 ****************************************************************************/
void *UArray2b_at(T array2b, int column, int row)
{
        assert(column >= 0 && row >= 0);
        assert(column < array2b->width && row < array2b->height);

        /* Finds the block at index block_col, block_row */
        int block_col = (column / array2b->blocksize);
        int block_row = (row / array2b->blocksize);


        /* Finds the UArray at that block */
        UArray_T *block_cells =  UArray2_at(array2b->blocks, block_col,
                                                                   block_row);
        assert(block_cells != NULL);


        int cell_idx = (array2b->blocksize * (column % array2b->blocksize) +
                                              (row % array2b->blocksize));

        /* Finds the element in that UArray */
        void *data = UArray_at(*block_cells, cell_idx);
   
        return data; 
}

 /******************************* UArray2b_width() ****************************
 * 
 *  Purpose: Obtains the number of columns in the UArray2b
 *  Parameters: An instance of a UArray2b
 *  Returns: An int representing the number of columns in the UArray2b
 *  Effects: Throws a checked runtime if array2b is equal to NULL
 * 
 ****************************************************************************/
int UArray2b_width(T array2b)
{
        assert(array2b);
        return array2b->width;
}

 /***************************** UArray2b_height()*****************************
 * 
 *  Purpose: Obtains the number of rows in the UArray2b
 *  Parameters: An instance of a UArray2b
 *  Returns: An int representing the number of rows in the UArray2b
 *  Effects: Throws a checked runtime if array2b is equal to NULL
 * 
 ****************************************************************************/
int UArray2b_height(T array2b)
{
        assert(array2b);
        return array2b->height;
}

 /************************* UArray2b_size ************************************
 * 
 *  Purpose: Obtains the number of bytes allocated by an element in 
             the UArray2b
 *  Parameters: An instance of a array2b
 *  Returns: Returns the number of bytes contained by an element of the 
 *           UArray2b
 *  Effects: Throws a checked runtime if array2b is equal to NULL
 * 
 *****************************************************************************/
int UArray2b_size(T array2b)
{
        assert(array2b);
        return array2b->size;
}

 /****************************** UArray2b_blocksize ************************
 * 
 *   Purpose: Obtains the number of cells one on side of a single 
 *            block in a UArray2b
 *   Parameters: An instance of an array2b
 *   Returns: Returns the number of cells one on side of a single 
 *            block in a UArray2b
 *   Effects: Throws a checked runtime if array2b is equal to NULL
 * 
 **************************************************************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b);
        return array2b->blocksize;
}

 /***************************** UArray2b_map ********************************
 * 
 *  Purpose: Iterates through each index in the 2D array
 *  Parameters: 
 *              T array2b: an instance of a array2b
 *
 *              void apply(int col, int row,T array2b, void *elem, 
 *                                                         void *cl):
 *                       a pointer to a function to apply on each element
 *                              looped through in the 2D array, takes in 
 *                              the element itself, an instance of a array2b
 *                               and a closure as parameter
 *              void *cl: a pointer to a closure, closure gives access to the  
 *                              outer function's scope from an inner function 
 *                              (map)
 *Calls the apply function to each element in the 2D array  Returns: Nothing
 *  Effects: 
 * 
 **************************************************************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b, void *elem, 
                                                           void *cl), void *cl)
{
        int block_cols = 
        (array2b->width  + array2b->blocksize - 1) / array2b->blocksize;
        int block_rows = 
        (array2b->height + array2b->blocksize - 1) / array2b->blocksize;

        /* Access each block, i.e. each element of the UArray2 */
        for (int col = 0; col < block_cols; col++) {
                for (int row = 0; row < block_rows; row++) {

                        UArray_T *block_cells = UArray2_at(array2b->blocks, 
                                                                    col, row);
                       
                        int num_cells = UArray_length(*block_cells);
                
                        int curr_col = array2b->blocksize * col;
                        int curr_row = array2b->blocksize * row;

                        for(int cell_idx = 0; cell_idx < num_cells; cell_idx++){
                                int valid_cell_col = curr_col + 
                                                (cell_idx / array2b->blocksize); 
                                int valid_cell_row = curr_row + 
                                                (cell_idx % array2b->blocksize);
        
                                if(valid_cell_row < array2b->height && 
                                        valid_cell_col < array2b->width) {
                                        void *elem = UArray_at(*block_cells, 
                                                                cell_idx);
                                        apply(valid_cell_col, valid_cell_row, 
                                                        array2b, elem, cl);
                                }
                        }
                
                }
        }
}