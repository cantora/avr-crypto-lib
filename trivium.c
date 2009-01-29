/* trivium.c */
/*
    This file is part of the This file is part of the AVR-Crypto-Lib.
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
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 */

 
#include <stdint.h>
#include <string.h>
#include "trivium.h"

#define S(i) ((((*ctx)[(i)/8])>>((i)%8))&1)
uint8_t trivium_enc(trivium_ctx_t* ctx){
	uint8_t t1,t2,t3,z;
	
	t1 = S(65)  ^ S(92);
	t2 = S(161) ^ S(176);
	t3 = S(242) ^ S(287);
	z  = t1^t2^t3;
	t1 ^= (S(90)  & S(91))  ^ S(170);
	t2 ^= (S(174) & S(175)) ^ S(263);
	t3 ^= (S(285) & S(286)) ^ S(68);
	
	/* shift whole state and insert ts later */
	uint8_t i,c1=0,c2;
	for(i=0; i<36; ++i){
		c2=(((*ctx)[i])>>7);
		(*ctx)[i] = (((*ctx)[i])<<1)|c1;
		c1=c2;
	}
	/* insert ts */
	(*ctx)[0] = (((*ctx)[0])&0xFE)| t3; /* s0*/
	(*ctx)[93/8] = (((*ctx)[93/8])& (~(1<<(93%8)))) | (t1<<(93%8)); /* s93 */
	(*ctx)[177/8] = (((*ctx)[177/8])& (~(1<<(177%8)))) | (t2<<(177%8));/* s177 */
	
	return z;
}

#define KEYSIZE_B ((keysize_b+7)/8)
#define IVSIZE_B  ((ivsize_b +7)/8)

void trivium_init(const void* key, uint8_t keysize_b, 
                  const void* iv,  uint8_t ivsize_b,
                  trivium_ctx_t* ctx){
	uint16_t i;
	uint8_t c1=0,c2;

	memset((*ctx)+KEYSIZE_B, 0, 35-KEYSIZE_B);
	memcpy((*ctx), key, KEYSIZE_B);
	memcpy((*ctx)+12, iv, IVSIZE_B); /* iv0 is at s96, must shift to s93 */
	
	for(i=12+IVSIZE_B; i>10; --i){
		c2=(((*ctx)[i])<<5);
		(*ctx)[i] = (((*ctx)[i])>>3)|c1;
		c1=c2;
	}
	(*ctx)[35]=0xE0;
	
	for(i=0; i<4*288; ++i){
		trivium_enc(ctx);
	}
}


