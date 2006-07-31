/**
 * File:		prng.h
 * Author:		Daniel Otte
 * Date:		23.07.2006
 * License:		GPL
 * Description:	This file contains the declarations for the pseudo-random-number generator.
 **/

#ifndef PRNG_H_
#define PRNG_H_

#include <stdint.h>
/*
 * length in bits 
 */
void addEntropy(unsigned length, void* data); 
void getRandomBlock(uint32_t* b);
/* this does some simple buffering */
uint8_t getRandomByte(void);
 

#endif /*PRNG_H_*/
