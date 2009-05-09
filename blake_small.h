/* blake_small.h */
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
 * \file    blake_small.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */
#ifndef BLAKE_SMALL_H_
#define BLAKE_SMALL_H_

#include <stdint.h>

#define BLAKE_SMALL_BLOCKSIZE   512
#define BLAKE_SMALL_BLOCKSIZE_B ((BLAKE_SMALL_BLOCKSIZE+7)/8)
#define BLAKE28_BLOCKSIZE      BLAKE_SMALL_BLOCKSIZE
#define BLAKE28_BLOCKSIZE_B    BLAKE_SMALL_BLOCKSIZE_B
#define BLAKE32_BLOCKSIZE      BLAKE_SMALL_BLOCKSIZE
#define BLAKE32_BLOCKSIZE_B    BLAKE_SMALL_BLOCKSIZE_B

typedef struct {
	uint32_t h[8];
	uint32_t s[4];
	uint32_t counter;
	uint8_t  appendone;
} blake_small_ctx_t;

typedef blake_small_ctx_t blake28_ctx_t;
typedef blake_small_ctx_t blake32_ctx_t;

void blake28_init(blake28_ctx_t* ctx);
void blake32_init(blake32_ctx_t* ctx);

void blake_small_nextBlock(blake_small_ctx_t* ctx, const void* block);
void blake_small_lastBlock(blake_small_ctx_t* ctx, const void* block, uint16_t length_b);

void blake28_nextBlock(blake28_ctx_t* ctx, const void* block);
void blake28_lastBlock(blake28_ctx_t* ctx, const void* block, uint16_t length_b);

void blake32_nextBlock(blake32_ctx_t* ctx, const void* block);
void blake32_lastBlock(blake32_ctx_t* ctx, const void* block, uint16_t length_b);

void blake28_ctx2hash(void* dest, const blake28_ctx_t* ctx);
void blake32_ctx2hash(void* dest, const blake32_ctx_t* ctx);

void blake28(void* dest, const void* msg, uint32_t length_b);
void blake32(void* dest, const void* msg, uint32_t length_b);

#endif /* BLAKE_SMALL_H_ */
