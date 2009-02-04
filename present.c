/* present.c */
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
 * present.c
 * a implementation of the PRESENT block-cipher
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * */
 
#include <string.h>
#include <stdint.h> 
#include "present.h"

static uint8_t sbox(uint8_t b){
	uint8_t sb[]={0xC, 0x5, 0x6, 0xB, 
		 		  0x9, 0x0, 0xA, 0xD, 
				  0x3, 0xE, 0xF, 0x8, 
				  0x4, 0x7, 0x1, 0x2 };
	return (((sb[b>>4])<<4)|(sb[b&0xf]));
}

static uint8_t sbox_inv(uint8_t b){
	uint8_t sb[]={0x5, 0xE, 0xF, 0x8, 
				  0xC, 0x1, 0x2, 0xD, 
				  0xB, 0x4, 0x6, 0x3, 
				  0x0, 0x7, 0x9, 0xA };
	return (((sb[b>>4])<<4)|(sb[b&0xf]));
}

#define SHR_O(a) c=(a)&1; (a)>>=1;
#define SHR_I(a) (a)=(c?0x8000:0x0000) | ((a)>>1);

static void p(uint16_t* o, uint8_t* i){
	uint8_t c;
	uint8_t m,n;
	for(m=0; m<8; ++m){
		for(n=0; n<2; ++n){
			SHR_O(i[m]);
			SHR_I(o[0]);
			SHR_O(i[m]);
			SHR_I(o[1]);
			SHR_O(i[m]);
			SHR_I(o[2]);
			SHR_O(i[m]);
			SHR_I(o[3]);
		}
	}
}

static void p_inv(uint8_t* o, uint8_t* i){
	uint8_t tmp[8];
	p((uint16_t*)tmp, i);
	p((uint16_t*)o, tmp);
}

void present_init(const uint8_t* key, uint8_t keysize_b, present_ctx_t* ctx){
	uint8_t buffer[10], tmp[2];
	uint8_t i;
	memcpy(buffer, key, 10);
	memcpy(&(ctx->k[0]), buffer+2, 8);
	for(i=1; i<32; ++i){
		/* rotate buffer 19 right */
		memcpy(tmp, buffer, 2);
		memmove(buffer, buffer+2, 8);
		memcpy(buffer+8, tmp, 2);
		 /* three shifts to do*/
		tmp[1]=buffer[0];
		*((uint64_t*)buffer)>>=3;
		*((uint16_t*)(buffer+8))>>=3;
		buffer[9] |= tmp[1]<<5;
		buffer[7] |= tmp[0]<<5;
		/* rotating done now substitution */
		buffer[9] = (sbox(buffer[9])&0xF0) | ((buffer[9])&0x0F);
		/* xor with round counter */
		*((uint16_t*)(buffer+1)) ^= (uint16_t)i<<7;
		memcpy(&(ctx->k[i]), buffer+2, 8);
	}
}

void present_enc(void* buffer, present_ctx_t* ctx){
	uint8_t i,j,tmp[8];
	for(i=0; i<31; ++i){
		*((uint64_t*)buffer) ^= ctx->k[i];
		 for(j=0; j<8; ++j){
		 	tmp[j] = sbox(((uint8_t*)buffer)[j]);
		 }
		 p((uint16_t*)buffer, tmp);
	}
	*((uint64_t*)buffer) ^= ctx->k[31];
}


void present_dec(void* buffer, present_ctx_t* ctx){
	uint8_t j,tmp[8];
	int8_t i;
	*((uint64_t*)buffer) ^= ctx->k[31];

	for(i=30; i>=0; --i){ 
		p_inv(tmp, (uint8_t*)buffer);
		for(j=0; j<8; ++j){
		 	((uint8_t*)buffer)[j] = sbox_inv(tmp[j]);
		}
		*((uint64_t*)buffer) ^= ctx->k[i];
	}
}
