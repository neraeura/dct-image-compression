
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

static const unsigned DENOMINATOR = 255; 
static const unsigned MAX_WORD_SIZE = 64;
static const unsigned BYTE_SIZE = 8;

 /**************************** printCodeword() *********************************
 * 
 *  Purpose: Prints a packed codeword represented as a 64-bit unsigned integer
 *  Parameters:
 *      1. A 32-bit unsigned integer named "word", which represents the packed 
 *         codeword that is to be printed.
 *     2.  An unsigned integer named "bits_left", which represents the number of 
 *         bits in the codeword that are to be printed.
 *  Returns: None
 *  Effects: This function outputs the packed codeword to standard output
 *  Expects:  
 * 
 ****************************************************************************/
void printCodeword(uint32_t word) 
{
        for (unsigned w = MAX_WORD_SIZE; w > 0; w = w - BYTE_SIZE) {
                putc(Bitpack_getu(word, BYTE_SIZE, w), stdout);
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
 *     2. bits_left - an unsigned integer that specifies how many bits of the 
 *                    current codeword are still available for reading.
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
uint64_t readInCodeword(FILE *input, unsigned bits_left) 
{

        uint64_t raw_word = 0;
        uint64_t single_byte;

        for (unsigned w = bits_left; w > 0; w = w - BYTE_SIZE) {
                single_byte = getc(input);
                assert(bits_left % 8 == 0);
                assert((int) single_byte != EOF);
                /* within byte, align codeword */
                single_byte = single_byte << w;
                raw_word = raw_word | single_byte;
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
        int read = fscanf(compressed_file, "COMP40 Compressed image format 2\n%u %u", 
                                &width, &height); 
        assert(read == 2);
        int c = getc(compressed_file);
        if (c != '\n') { // cope with bad student format
                fprintf(stderr, "Newline missing from compressed-image header\n");
                ungetc(c, compressed_file);
        }

        A2Methods_T methods = uarray2_methods_plain;
        A2Methods_UArray2 pixels = methods->new(width, height,
                                        sizeof(struct Pnm_rgb));
        Pnm_ppm pixmap = malloc(sizeof(*pixmap));
        pixmap->height = height;
        pixmap->width = width;
        pixmap->denominator = DENOMINATOR;
        pixmap->pixels = pixels;
        
        return pixmap;
}