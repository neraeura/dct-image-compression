#ifndef FILEIO_H
#define FILEIO_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "sharedData.h"

#include "bitpack.h"
#include "pnm.h"


void 
printCodeword(uint32_t word);

uint32_t 
readInCodeword(FILE *input);

Pnm_ppm 
readHeader(FILE *in);


#endif

