/* md5.h */
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
/* 
 * File:	md5.h
 * Author:	Daniel Otte
 * Date: 	31.07.2006
 * License: GPL
 * Description: Implementation of the MD5 hash algorithm as described in RFC 1321
 * 
 */


#ifndef MD5_H_
#define MD5_H_

#include <stdint.h>

typedef struct md5_ctx_st {
	uint32_t a[4];
	uint32_t counter;
} md5_ctx_t;

 
void md5_init(md5_ctx_t *s);
void md5_nextBlock(md5_ctx_t *state, void* block);
void md5_lastBlock(md5_ctx_t *state, void* block, uint16_t length);

#endif /*MD5_H_*/
