/* aes_keyschedule.c */
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
 * \file     aes_keyschedule.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-30
 * \license  GPLv3 or later
 * 
 */

#include <stdint.h>
#include "gf256mul.h"
#include "aes.h"
#include "aes_keyschedule.h"
#include "aes_sbox.h"
#include <string.h>
#include <avr/pgmspace.h>

static
void aes_rotword(void* a){
	uint8_t t;
	t=((uint8_t*)a)[0];
	((uint8_t*)a)[0] = ((uint8_t*)a)[1];
	((uint8_t*)a)[1] = ((uint8_t*)a)[2];
	((uint8_t*)a)[2] = ((uint8_t*)a)[3];
	((uint8_t*)a)[3] = t;
}

void aes_init(const void* key, uint16_t keysize_b, aes_genctx_t* ctx){
	uint8_t hi,i,nk, next_nk;
	uint8_t rc=1;
	uint8_t tmp[4];
	nk=keysize_b>>5; /* 4, 6, 8 */
	hi=4*(nk+6+1);
	memcpy(ctx, key, keysize_b/8);
	next_nk = nk;
	for(i=nk;i<hi;++i){
		*((uint32_t*)tmp) = ((uint32_t*)(ctx->key[0].ks))[i-1];
		if(i!=next_nk){
			if(nk==8 && i%8==4){
				tmp[0] = pgm_read_byte(aes_sbox+tmp[0]);
				tmp[1] = pgm_read_byte(aes_sbox+tmp[1]);
				tmp[2] = pgm_read_byte(aes_sbox+tmp[2]);
				tmp[3] = pgm_read_byte(aes_sbox+tmp[3]);
			}
		} else {
			next_nk += nk;
			aes_rotword(tmp);
			tmp[0] = pgm_read_byte(aes_sbox+tmp[0]);
			tmp[1] = pgm_read_byte(aes_sbox+tmp[1]);
			tmp[2] = pgm_read_byte(aes_sbox+tmp[2]);
			tmp[3] = pgm_read_byte(aes_sbox+tmp[3]);
			tmp[0] ^= rc;
			rc = gf256mul(2,rc,0x1b);
		}
		((uint32_t*)(ctx->key[0].ks))[i] = ((uint32_t*)(ctx->key[0].ks))[i-nk]
		                                   ^ *((uint32_t*)tmp);
	}
	
	uint8_t buffer[16];
	for(i=0; i<nk+7; ++i){
		memcpy(buffer, ctx->key[i].ks, 16);
		aes_buffer2state(ctx->key[i].ks, buffer);
	}
}

void aes128_init(const void* key, aes128_ctx_t* ctx){
	aes_init(key, 128, (aes_genctx_t*)ctx);
}

void aes192_init(const void* key, aes192_ctx_t* ctx){
	aes_init(key, 192, (aes_genctx_t*)ctx);
}

void aes256_init(const void* key, aes256_ctx_t* ctx){
	aes_init(key, 256, (aes_genctx_t*)ctx);
}
