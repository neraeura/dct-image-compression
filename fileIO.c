
/**************************************************************************
 *
 *                               fileIO.c
 * 
 *     Assignment: arith
 *     Authors:  Nora A-Rahim and Daniel Opara
 *     CS Logins: narahi01 and dopara01
 *     Date:  March 11, 2023
 * 
 *     This module contains functions for reading and writing compressed 
 *     PPM image files.
 *
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>


#include "fileIO.h"

#define header "COMP40 Compressed image format 2\n%u %u"

static const unsigned DENOMINATOR = 255; 
static const unsigned MAX_WORD_SIZE = 32;
static const unsigned BYTES = 8;

 /**************************** printCodeword() *********************************
 * 
 *  Purpose: Prints a packed codeword represented as a 64-bit unsigned integer
 *  Parameters:
 *      1. A 32-bit unsigned integer named "word", which represents the packed 
 *         codeword that is to be printed.
 *  Returns: None
 *  Effects: This function outputs the packed codeword to standard output
 *  Expects:  
 * 
 ****************************************************************************/
void printCodeword(uint32_t word) 
{
        for (int index = MAX_WORD_SIZE - BYTES; index >= 0; index -= BYTES) {
                putc(Bitpack_getu(word, BYTES, index), stdout);
        }
}


 /**************************** readInCodeword() ************************
 * 
 *  Purpose: Reads in a single codeword from the input file stream in sequence, 
 *           remembering that each word is stored in big-endian order, with 
 *           the MSB first.
 * 
 *  Parameters: 
 *      1. input - a pointer to a FILE object representing the input file stream 
 *               from which the compressed image blocks are read.
 * 
 *  Returns: An unsigned 64-bit integer that represents the codeword read from 
 *           the input file.
 * 
 *  Effects: If the supplied file is too short, i.e. the number of codewords
 *           is too low for the stated width and height, or the last one is 
 *           incomplete, this function fails with a Checked Runtime Error 
 * 
 *  Expects: Assumes that the input file stream is open and points to a valid 
 *           input file containing compressed image blocks. It also assumes that
 *           the bits_left parameter accurately specifies how many bits of the 
 *           current codeword are still available for reading from the input 
 *           file, and that the input file stream is properly formatted with 
 *           respect to the compressed image block encoding scheme.
 * 
 ****************************************************************************/

uint32_t readInCodeword(FILE *input) 
{

        uint32_t raw_word = 0;

        for (int w = MAX_WORD_SIZE - BYTES; w >= 0; w -= BYTES) {
                int single_byte = getc(input);
                assert(single_byte != EOF);
                /* This is done to ensure that a value is represented as an 
                unsigned byte or ASCII character code with a value between 
                                        0 and 255                       */
                single_byte = (single_byte < 0) ? 0 : 
                                ((single_byte > 255) ? 255 : single_byte);
                raw_word = raw_word | single_byte << w;
        }

        return raw_word;

}


 /**************************** readInHeader() *********************************
 * 
 *  Purpose: The function reads the header information of a compressed PPM image 
 *           file 
 * 
 *  Parameters: compressed_file -- a pointer to a file stream for the input 
 *                                 compressed PPM image file.
 * 
 *  Returns: A pointer to a newly created Pnm_ppm struct with the header 
 *           information read from the input file.
 *
 *  Effects: The function allocates memory for the new Pnm_ppm struct and its 
 *           pixel array using malloc(). 
 * 
 * 
 *  Expects: Assumes that the input file stream contains the correct header 
 *          information for a compressed PPM image file in the expected format. 
 *          The input file stream must be opened and passed in as a valid file 
 *          stream pointer. Additionally, the compressed PPM image file must 
 *          have the correct format and values for the width, height, and 
 *          denominator. 
 * 
 ****************************************************************************/
Pnm_ppm readHeader(FILE *compressed_file)
{
        unsigned height, width;
        int read = fscanf(compressed_file, header, &width, &height);
        assert(read == 2);
        int c = getc(compressed_file);
        if (c != '\n') { 
                fprintf(stderr, "Error: Missing a newline from header\n");
                ungetc(c, compressed_file);
        }

        A2Methods_T methods = uarray2_methods_plain;
        A2Methods_UArray2 pixels = methods->new(width, height,
                                        sizeof(struct Pnm_rgb));
        Pnm_ppm pixmap = malloc(sizeof(*pixmap));
        assert(pixmap);
        pixmap->height = height;
        pixmap->width = width;
        pixmap->denominator = DENOMINATOR;
        pixmap->methods = methods;
        pixmap->pixels = pixels;

        return pixmap;
}