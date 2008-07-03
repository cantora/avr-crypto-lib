/* md5.c */
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
/* 
 * \file	md5.c
 * \author	Daniel Otte
 * \date 	2006-07-31
 * \par License:
 * GPLv3 or later
 * \brief Implementation of the MD5 hash algorithm as described in RFC 1321
 * 
 */

 #include "md5.h"
 #include "md5_sbox.h"
 #include "uart.h" 
 #include <stdint.h>
 #include <string.h>
 
 #undef DEBUG
 
void md5_init(md5_ctx_t *s){
	s->counter = 0;
	s->a[0] = 0x67452301;
	s->a[1] = 0xefcdab89;
	s->a[2] = 0x98badcfe;
	s->a[3] = 0x10325476;
}
 
uint32_t md5_F(uint32_t x, uint32_t y, uint32_t z){
	return ((x&y)|((~x)&z));
}

uint32_t md5_G(uint32_t x, uint32_t y, uint32_t z){
	return ((x&z)|((~z)&y));
}

uint32_t md5_H(uint32_t x, uint32_t y, uint32_t z){
	return (x^y^z);
}

uint32_t md5_I(uint32_t x, uint32_t y, uint32_t z){
	return (y ^ (x | (~z)));
}

typedef uint32_t md5_func_t(uint32_t, uint32_t, uint32_t);

#define ROTL32(x,n) (((x)<<(n)) | ((x)>>(32-(n))))  

void md5_core(uint32_t* a, uint8_t as, void* block, uint8_t k, uint8_t s, uint8_t i, uint8_t fi){
	uint32_t t;
	md5_func_t* funcs[]={md5_F, md5_G, md5_H, md5_I};
	as &= 0x3;
	/* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#ifdef DEBUG
	char funcc[]={'*', '-', '+', '~'};
	uart_putstr("\r\n DBG: md5_core [");
	uart_putc(funcc[fi]);
	uart_hexdump(&as, 1); uart_putc(' ');
	uart_hexdump(&k, 1); uart_putc(' ');
	uart_hexdump(&s, 1); uart_putc(' ');
	uart_hexdump(&i, 1); uart_putc(']');
#endif	
	t = a[as] + funcs[fi](a[(as+1)&3], a[(as+2)&3], a[(as+3)&3]) + ((uint32_t*)block)[k] + md5_T[i] ;
	a[as]=a[(as+1)&3] + ROTL32(t, s);
}

void md5_nextBlock(md5_ctx_t *state, void* block){
	uint32_t	a[4];
	uint8_t		m,n,i=0;
	/* this requires other mixed sboxes */
#ifdef DEBUG
	uart_putstr("\r\n DBG: md5_nextBlock: block:\r\n");
	uart_hexdump(block, 16);	uart_putstr("\r\n");
	uart_hexdump(block+16, 16);	uart_putstr("\r\n");
	uart_hexdump(block+32, 16);	uart_putstr("\r\n");
	uart_hexdump(block+48, 16);	uart_putstr("\r\n");
#endif	
	
	a[0]=state->a[0];
	a[1]=state->a[1];
	a[2]=state->a[2];
	a[3]=state->a[3];
	
	/* round 1 */
	uint8_t s1t[]={7,12,17,22};
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, 4-n, block, m*4+n, s1t[n],i++,0);
		}
	}
	/* round 2 */
	uint8_t s2t[]={5,9,14,20};
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, 4-n, block, (1+m*4+n*5)&0xf, s2t[n],i++,1);
		}
	}
	/* round 3 */
	uint8_t s3t[]={4,11,16,23};
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, 4-n, block, (5-m*4+n*3)&0xf, s3t[n],i++,2);
		}
	}
	/* round 4 */
	uint8_t s4t[]={6,10,15,21};
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, 4-n, block, (0-m*4+n*7)&0xf, s4t[n],i++,3);
		}
	}
	state->a[0] += a[0];
	state->a[1] += a[1];
	state->a[2] += a[2];
	state->a[3] += a[3];
	state->counter++;
}

void md5_lastBlock(md5_ctx_t *state, void* block, uint16_t length){
	uint16_t l;
	uint8_t b[64];
	while (length >= 512){
		md5_nextBlock(state, block);
		length -= 512;
		block = ((uint8_t*)block) + 512/8;
	}
	memset(b, 0, 64);
	memcpy(b, block, length/8);
	/* insert padding one */
	l=length/8;
	if(length%8){
		uint8_t t;
		t = ((uint8_t*)block)[l];
		t |= (0x80>>(length%8));
		b[l]=t;
	}else{
		b[l]=0x80;
	}
	/* insert length value */
	if(l+sizeof(uint64_t) > 512/8){
		md5_nextBlock(state, b);
		state->counter--;
		memset(b, 0, 64);
	}
	*((uint64_t*)&b[64-sizeof(uint64_t)]) = (state->counter * 512) + length;
	md5_nextBlock(state, b);
}
