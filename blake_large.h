/* blake_large.h */
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
 * \file    blake_large.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-08
 * \license GPLv3 or later
 * 
 */
#ifndef BLAKE_LARGE_H_
#define BLAKE_LARGE_H_

#include <stdint.h>

#define BLAKE_LARGE_BLOCKSIZE   1024
#define BLAKE_LARGE_BLOCKSIZE_B ((BLAKE_LARGE_BLOCKSIZE+7)/8)
#define BLAKE48_BLOCKSIZE      BLAKE_LARGE_BLOCKSIZE
#define BLAKE48_BLOCKSIZE_B    BLAKE_LARGE_BLOCKSIZE_B
#define BLAKE64_BLOCKSIZE      BLAKE_LARGE_BLOCKSIZE
#define BLAKE64_BLOCKSIZE_B    BLAKE_LARGE_BLOCKSIZE_B

typedef struct {
	uint64_t h[8];
	uint64_t s[4];
	uint32_t counter;
	uint8_t  appendone;
} blake_large_ctx_t;

typedef blake_large_ctx_t blake48_ctx_t;
typedef blake_large_ctx_t blake64_ctx_t;

void blake48_init(blake48_ctx_t* ctx);
void blake64_init(blake64_ctx_t* ctx);

void blake_large_nextBlock(blake_large_ctx_t* ctx, const void* block);
void blake_large_lastBlock(blake_large_ctx_t* ctx, const void* block, uint16_t length_b);

void blake48_nextBlock(blake48_ctx_t* ctx, const void* block);
void blake48_lastBlock(blake48_ctx_t* ctx, const void* block, uint16_t length_b);

void blake64_nextBlock(blake64_ctx_t* ctx, const void* block);
void blake64_lastBlock(blake64_ctx_t* ctx, const void* block, uint16_t length_b);

void blake48_ctx2hash(void* dest, const blake48_ctx_t* ctx);
void blake64_ctx2hash(void* dest, const blake64_ctx_t* ctx);

void blake48(void* dest, const void* msg, uint32_t length_b);
void blake64(void* dest, const void* msg, uint32_t length_b);

#endif /* BLAKE_LARGE_H_ */
