/* sha256-asm.h */
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
 * \file		sha256-asm.h
 * \author	Daniel Otte 
 * \date		16.05.2006
 * \par License	
 * GPL
 * 
 */

#ifndef SHA256ASM_H_
#define SHA256ASM_H_


#define SHA256_HASH_BITS  256
#define SHA256_BLOCK_BITS 512

typedef struct {
	uint32_t h[8];
	uint64_t length;
} sha256_ctx_t;

typedef uint8_t sha256_hash_t[SHA256_HASH_BITS/8];

void sha256_ctx2hash(sha256_hash_t *dest, sha256_ctx_t *state);
void sha256(sha256_hash_t *dest, void* msg, uint32_t length);
void sha256_init(sha256_ctx_t *state);
void sha256_nextBlock(sha256_ctx_t *state, void* block);
void sha256_lastBlock(sha256_ctx_t *state, void* block, uint16_t length);
uint32_t rotr32(uint32_t, uint8_t);
uint32_t change_endian32(uint32_t x);

#endif /*SHA256ASM_H_*/
