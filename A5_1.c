/* 
 * File:		A5_1.c
 * Author:	Daniel Otte
 * Date: 	24.06.2006
 * License: GPL
 * Description: Implementation of the A5/1 stream cipher algorithm, as used in GSM.
 * ! Warning, this is weak crypto !
 * 
 */
 
#include <stdint.h>
#include "A5_1.h"

/*
 * length is length of key in bytes!
 */
#define BYTEn(p,s) (*(((uint8_t*)&(p))+s))

void		a5_1_init(a5_1_ctx_t *c, uint8_t *key, uint8_t length);

bool	 a5_1_clock(a5_1_ctx_t *c){
	bool x1,x2,x3, maj;
	x1 = PARITY_LOOKUP & (1<< ((BYTEn(c->r1,2)+BYTEn(c->r1,1)>>5) & 0x7);
	x2 = PARITY_LOOKUP & (1<< (BYTEn(c->r2,2)>>4));
	x3 = PARITY_LLOKUP & (1<< ((BYTEn(c->r3,2)>>4+BYTEn(c->r3,0)>>7) & 0x7));
	maj = (((c->r1 & (1<<R1_CLK))?1:0)+((c->r2 & (1<<R2_CLK))?1:0)+((c->r3 & (1<<R3_CLK))?1:0))>=2;
	if (((c->r1 & (1<<R1_CLK))>>R1_CLK) == maj)
		c->r1 = c->r1<<1 + x1;
	if (((c->r2 & (1<<R2_CLK))>>R2_CLK) == maj)
		c->r2 = c->r2<<1 + x2;
	if (((c->r3 & (1<<R3_CLK))>>R3_CLK) == maj)
		c->r3 = c->r3<<1 + x3;
		
	return ((c->r1)>>(R1_LENGTH-1)+(c->r2)>>(R2_LENGTH-1)+(c->r3)>>(R3_LENGTH-1))&0x1;
}

uint8_t a5_1_gen(a5_	1_ctx_t *c){
	uint8_t ret=0;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	return ret;
}




