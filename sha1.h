/* sha1.h */
/*
    This file is part of the AVR-Crypto-Lib.
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
 * \file	sha1.c
 * \author	Daniel Otte
 * \date	2006-10-08
 * \par License:
 * GPL
 * \brief SHA-1 declaration.
 * 
 */
 
#ifndef SHA1_H_
#define SHA1_H_

#include <stdint.h>


#define SHA1_HASH_BITS  160
#define SHA1_HASH_BYTES (SHA1_HASH_BITS/8)
#define SHA1_BLOCK_BITS 512
#define SHA1_BLOCK_BYTES (SHA1_BLOCK_BITS/8)

/**
 * \brief SHA-1 context type
 * 
 */
typedef struct {
	uint32_t h[5];
	uint64_t length;
} sha1_ctx_t;

typedef uint8_t sha1_hash_t[SHA1_HASH_BITS/8];

void sha1_init(sha1_ctx_t *state);

void sha1_nextBlock (sha1_ctx_t *state, void* block);
void sha1_lastBlock (sha1_ctx_t *state, void* block, uint16_t length_b);

void sha1_ctx2hash (sha1_hash_t *dest, sha1_ctx_t *state);
void sha1 (sha1_hash_t *dest, void* msg, uint32_t length_b);



#endif /*SHA1_H_*/
