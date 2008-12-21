/* twister.c */
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

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "twister.h"
#include "twister_tables.h"
#include "memxor.h"

#include "gf256mul.h"

//#define DEBUG

#ifdef DEBUG
# include "uart.h"
#endif

#ifdef DEBUG
# define DEBUG_PRINT(ctx, msg) debug_print((ctx), PSTR(msg)) 
#else
# define DEBUG_PRINT(ctx, msg) 
#endif 

#ifdef DEBUG

void print_twister_state(twister_state_t* ctx){
	uint8_t i,j;
	uart_putstr_P(PSTR("\r\nState:\r\n matrix:\r\n"));
	for(i=0; i<8; ++i){
		uart_putstr_P(PSTR("\t[ "));
		uart_hexdump(&(ctx->s[i][0]), 8);
		uart_putstr_P(PSTR("]\r\n"));
	}
	uart_putstr_P(PSTR("counter: "));
	uart_hexdump(&(ctx->counter), 8);

	uart_putstr_P(PSTR("\r\nlength_counter_b: "));
	uart_hexdump(&(ctx->length_counter_b), 8);
	uart_putstr_P(PSTR("\r\n"));
} 

void debug_print(twister_state_t* ctx, PGM_P msg){
	uart_putstr_P(PSTR("\r\n"));
	uart_putstr_P(msg);
	print_twister_state(ctx);
}

#endif

static
void shiftrow(void* row, uint8_t shift){
	*((uint64_t*)row) = *((uint64_t*)row)>>(8*shift) | *((uint64_t*)row)<<(64-8*shift);
}

#define MDS(a,b)  pgm_read_byte(&(twister_mds[a][b]))
//#define MULT(a,b) pgm_read_byte(&(twister_multab[a-1][b]))
#define MULT(a,b) gf256mul(a,b, 0x4D)

void blank_round(twister_state_t* ctx){
	uint8_t i,j;
	uint8_t tmp[8][8];
	DEBUG_PRINT(ctx, "blank init");
	/* add twist counter */
	for(i=0; i<8; ++i)
		ctx->s[i][1] ^= ((uint8_t*)&(ctx->counter))[7-i];
	ctx->counter--;
//	DEBUG_PRINT(ctx, "counter added");
	/* sub bytes */
	for(i=0; i<8; ++i)
		for(j=0;j<8;++j)
			tmp[i][j] = pgm_read_byte(twister_sbox+ctx->s[i][j]);
	/* shift rows */
	for(i=1;i<8; ++i){
		shiftrow(&(tmp[i][0]), i);
	}
	/* mix columns */
	for( i=0; i<8; i++ ){
		// multiply with mds matrix
		for( j=0; j<8; j++ ){
			ctx->s[j][i] =
				MULT( MDS(j,0), tmp[0][i] ) ^
				MULT( MDS(j,1), tmp[1][i] ) ^
				MULT( MDS(j,2), tmp[2][i] ) ^
				MULT( MDS(j,3), tmp[3][i] ) ^
				MULT( MDS(j,4), tmp[4][i] ) ^
				MULT( MDS(j,5), tmp[5][i] ) ^
				MULT( MDS(j,6), tmp[6][i] ) ^
				MULT( MDS(j,7), tmp[7][i] ) ;
				
		}	
	}
	DEBUG_PRINT(ctx, "post MDS");
}

void mini_round(twister_state_t* ctx, void* msg){
	/* inject message */
	uint8_t i;
	for(i=0; i<8; ++i){
		ctx->s[7][7-i] ^= *((uint8_t*)msg);
		msg = (uint8_t*)msg +1;	
	}
	blank_round(ctx);
}





