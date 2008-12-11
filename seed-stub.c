/* seed.c */
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
 * \file	seed.c
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief	SEED parts in C for AVR
 * \par License	
 * GPL
 * 
 */
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "seed.h"
#include "uart.h"
#include "debug.h"



/******************************************************************************/
typedef struct {
	uint32_t k0, k1;
} keypair_t;

uint64_t  seed_f_function(const uint64_t* a, uint32_t k0, uint32_t k1);
keypair_t seed_getnextkeys(uint32_t *keystate, uint8_t curround);
keypair_t seed_getprevkeys(uint32_t *keystate, uint8_t curround);

/******************************************************************************/

#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_enc(void * buffer, seed_ctx_t * ctx){
	uint8_t r;
	keypair_t k;
	for(r=0; r<8; ++r){
			k = seed_getnextkeys(ctx->k, 2*r);
/*
	DEBUG_S("\r\n\tDBG ka,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L:   "); uart_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R:   "); uart_hexdump((uint8_t*)buffer+8, 8);
*/
			L ^= seed_f_function(&R,k.k0,k.k1);
			
			k = seed_getnextkeys(ctx->k, 2*r+1);
/*
	DEBUG_S("\r\n\tDBG kb,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L:   "); uart_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R:   "); uart_hexdump((uint8_t*)buffer+0, 8);
*/
			R ^= seed_f_function(&L,k.k0,k.k1);
	}
	/* just an exchange without temp. variable */
	L ^= R;
	R ^= L;
	L ^= R;
}

/******************************************************************************/

#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_dec(void * buffer, seed_ctx_t * ctx){
	int8_t r;
	keypair_t k;
	for(r=7; r>=0; --r){
			k = seed_getprevkeys(ctx->k, 2*r+1);
/*
	DEBUG_S("\r\n\tDBG ka,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); uart_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R: "); uart_hexdump((uint8_t*)buffer+8, 8);
*/
			L ^= seed_f_function(&R,k.k0,k.k1);
			
			k = seed_getprevkeys(ctx->k, 2*r+0);
/*
	DEBUG_S("\r\n\tDBG kb,0: "); uart_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); uart_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); uart_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R: "); uart_hexdump((uint8_t*)buffer+0, 8);
*/
			R ^= seed_f_function(&L,k.k0,k.k1);
	}
	/* just an exchange without temp. variable */
	L ^= R;
	R ^= L;
	L ^= R;
}











