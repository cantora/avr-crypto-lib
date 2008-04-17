/**
 * File:		entropium.h
 * Author:		Daniel Otte
 * Date:		23.07.2006
 * License:		GPL
 * Description:	This file contains the declarations for the pseudo-random-number generator.
 **/

#ifndef ENTROPIUM_H_
#define ENTROPIUM_H_

#include <stdint.h>
/*
 * length in bits 
 */
#define ENTROPIUM_RANDOMBLOCK_SIZE 32 /* bytes */
 
void entropium_addEntropy(unsigned length_b, const void* data); 
void entropium_getRandomBlock(void* b);
/* this does some simple buffering */
uint8_t entropium_getRandomByte(void);

void entropium_fillBlockRandom(void* block, unsigned length_B);

#endif /*PRNG_H_*/
