/* skipjack.h */
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
#ifndef SKIPJACK_H_
#define SKIPJACK_H_

#include <stdint.h>

/*****************************************************************************/
/**
 * block is 64 bits (=8 bytes) in size, key is 80 bits (=10 bytes) in size.
 * 
 */
void skipjack_enc(void* block, void* key);
/*****************************************************************************/
/**
 * block is 64 bits (=8 bytes) in size, key is 80 bits (=10 bytes) in size.
 * 
 */
void skipjack_dec(void* block, void* key);

#endif /*SKIPJACK_H_*/
