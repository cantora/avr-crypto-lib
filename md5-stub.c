/* md5-asm.c */
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

 #include "md5.h"
 #include "uart.h" 
 #include <stdint.h>
 #include <string.h>
 
 #undef DEBUG


void md5_ctx2hash(md5_hash_t* dest, const md5_ctx_t* state){
	memcpy(dest, state->a, MD5_HASH_BYTES);
}

void md5(md5_hash_t* dest, const void* msg, uint32_t length_b){
	md5_ctx_t ctx;
	md5_init(&ctx);
	while(length_b>=MD5_BLOCK_BITS){
		md5_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + MD5_BLOCK_BYTES;
		length_b -= MD5_BLOCK_BITS;
	}
	md5_lastBlock(&ctx, msg, length_b);
	md5_ctx2hash(dest, &ctx);
}

