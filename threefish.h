/* threefish.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 */

#ifndef THREEFISH_H_
#define THREEFISH_H_

#include <stdint.h>

typedef struct{
	uint64_t k[5];
	uint64_t t[3];
} threefish256_ctx_t;


typedef struct{
	uint64_t k[9];
	uint64_t t[3];
} threefish512_ctx_t;


typedef struct{
	uint64_t k[17];
	uint64_t t[3];
} threefish1024_ctx_t;



void threefish256_init(void* key, void* tweak, threefish256_ctx_t* ctx);
void threefish512_init(void* key, void* tweak, threefish512_ctx_t* ctx);
void threefish1024_init(void* key, void* tweak, threefish1024_ctx_t* ctx);

void threefish256_enc(void* data, threefish256_ctx_t* ctx);
void threefish512_enc(void* data, threefish512_ctx_t* ctx);
void threefish1024_enc(void* data, threefish1024_ctx_t* ctx);

#endif /* THREEFISH_H_ */
