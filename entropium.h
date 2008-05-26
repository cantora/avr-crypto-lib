/* entropium.h */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
