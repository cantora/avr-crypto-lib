/* bigint.c */
/*
    This file is part of the ARM-Crypto-Lib.
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
 * \file		bigint.c
 * \author		Daniel Otte
 * \date		2010-02-22
 * 
 * \license	    GPLv3 or later
 * 
 */
 

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define STR_LINE STRING(__LINE__)

#include "bigint.h"
#include <string.h>

#define DEBUG 1

#if DEBUG || 1
#include "cli.h"
#include "uart_i.h"
#include "bigint_io.h"
#include <stdio.h>
#endif

#ifndef MAX
 #define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
 #define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define SET_FBS(a, v) do{(a)->info &= ~BIGINT_FBS_MASK; (a)->info |= (v);} while(0)
#define GET_FBS(a)   ((a)->info&BIGINT_FBS_MASK)
#define SET_NEG(a)   (a)->info |= BIGINT_NEG_MASK
#define SET_POS(a)   (a)->info &= ~BIGINT_NEG_MASK
#define XCHG(a,b)    do{(a) ^= (b); (b) ^= (a); (a) ^= (b);} while(0)
#define XCHG_PTR(a,b)    do{ a = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b))); \
	                         b = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b))); \
	                         a = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b)));} while(0)

#define GET_SIGN(a) ((a)->info & BIGINT_NEG_MASK)

/******************************************************************************/
void bigint_adjust(bigint_t *a){
	while(a->length_W!=0 && a->wordv[a->length_W-1]==0){
		a->length_W--;
	}
	if(a->length_W==0){
		a->info=0;
		return;
	}
	bigint_word_t t;
	uint8_t i = BIGINT_WORD_SIZE-1;
	t = a->wordv[a->length_W-1];
	while((t&(1L<<(BIGINT_WORD_SIZE-1)))==0 && i){
		t<<=1;
		i--;
	}
	SET_FBS(a, i);
}

/******************************************************************************/

bigint_length_t bigint_length_b(const bigint_t *a){
	if(!a->length_W || a->length_W==0){
		return 0;
	}
	return (a->length_W-1) * BIGINT_WORD_SIZE + GET_FBS(a);
}

/******************************************************************************/

bigint_length_t bigint_length_B(const bigint_t *a){
	return a->length_W * sizeof(bigint_word_t);
}

/******************************************************************************/

uint32_t bigint_get_first_set_bit(const bigint_t *a){
	if(a->length_W==0){
		return (uint32_t)(-1);
	}
	return (a->length_W-1)*sizeof(bigint_word_t)*8+GET_FBS(a);
}


/******************************************************************************/

uint32_t bigint_get_last_set_bit(const bigint_t *a){
	uint32_t r=0;
	uint8_t b=0;
	bigint_word_t x=1;
	if(a->length_W==0){
		return (uint32_t)(-1);
	}
	while(a->wordv[r]==0 && r<a->length_W){
		++r;
	}
	if(a->wordv[r] == 0){
		return (uint32_t)(-1);
	}
	while((x&a->wordv[r])==0){
		++b;
		x <<= 1;
	}
	return r*BIGINT_WORD_SIZE+b;
}

/******************************************************************************/

void bigint_copy(bigint_t *dest, const bigint_t *src){
    if(dest->wordv != src->wordv){
	    memcpy(dest->wordv, src->wordv, src->length_W * sizeof(bigint_word_t));
    }
    dest->length_W = src->length_W;
	dest->info = src->info;
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_add_u(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	bigint_length_t i;
	bigint_wordplus_t t = 0LL;
	if (a->length_W < b->length_W) {
		XCHG_PTR(a, b);
	}
	for(i = 0; i < b->length_W; ++i) {
		t += a->wordv[i];
		t += b->wordv[i];
		dest->wordv[i] = (bigint_word_t)t;
		t >>= BIGINT_WORD_SIZE;
	}
	for(; i < a->length_W; ++i){
		t += a->wordv[i];
		dest->wordv[i] = (bigint_word_t)t;
		t >>= BIGINT_WORD_SIZE;
	}
	if(t){
		dest->wordv[i++] = (bigint_word_t)t;
	}
	dest->length_W = i;
	SET_POS(dest);
	bigint_adjust(dest);
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_add_scale_u(bigint_t *dest, const bigint_t *a, bigint_length_t scale){
	if(a->length_W == 0){
		return;
	}
	if(scale == 0){
		bigint_add_u(dest, dest, a);
		return;
	}
	bigint_t x;
#if BIGINT_WORD_SIZE == 8
	memset(dest->wordv + dest->length_W, 0, MAX(dest->length_W, a->length_W + scale) - dest->length_W);
	x.wordv = dest->wordv + scale;
	x.length_W = dest->length_W - scale;
	if((int16_t)x.length_W < 0){
		x.length_W = 0;
		x.info = 0;
	} else {
		x.info = dest->info;
	}
	bigint_add_u(&x, &x, a);
	dest->length_W = x.length_W + scale;
	dest->info = 0;
	bigint_adjust(dest);
#else
	bigint_t s;
	bigint_length_t word_shift = scale / sizeof(bigint_word_t), byte_shift = scale % sizeof(bigint_word_t);
	bigint_word_t bv[a->length_W + 1];
	s.wordv = bv;
	bv[0] = bv[a->length_W] = 0;
	memcpy((uint8_t*)bv + byte_shift, a->wordv, a->length_W * sizeof(bigint_word_t));
	s.length_W = a->length_W + 1;
	bigint_adjust(&s);
	memset(dest->wordv + dest->length_W, 0, (MAX(dest->length_W, s.length_W + word_shift) - dest->length_W) * sizeof(bigint_word_t));
	x.wordv = dest->wordv + word_shift;
	x.length_W = dest->length_W - word_shift;
	if((int16_t)x.length_W < 0){
		x.length_W = 0;
		x.info = 0;
	}else{
		x.info = dest->info;
	}
	bigint_add_u(&x, &x, &s);
	dest->length_W = x.length_W + word_shift;
	dest->info = 0;
	bigint_adjust(dest);
#endif
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_sub_u(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	int8_t borrow = 0;
	int8_t  r;
	bigint_wordplus_signed_t t = 0LL;
	bigint_length_t i;
	if(b->length_W == 0){
		bigint_copy(dest, a);
		SET_POS(dest);
		return;
	}
	if(a->length_W == 0){
		bigint_copy(dest, b);
		SET_NEG(dest);
		return;
	}
    r = bigint_cmp_u(a,b);
    if(r == 0){
        bigint_set_zero(dest);
        return;
    }
	if(r < 0){
		bigint_sub_u(dest, b, a);
		SET_NEG(dest);
		return;
	}
	for(i = 0; i < a->length_W; ++i){
		t = a->wordv[i];
		if(i < b->length_W){
			t -= b->wordv[i];
		}
		t -= borrow;
		dest->wordv[i] = (bigint_word_t)t;
		if(t<0){
			borrow = 1;
		}else{
			borrow = 0;
		}
	}
	SET_POS(dest);
	dest->length_W = i;
	bigint_adjust(dest);
}

/******************************************************************************/

int8_t bigint_cmp_u(const bigint_t *a, const bigint_t *b){
	if(a->length_W > b->length_W){
		return 1;
	}
	if(a->length_W < b->length_W){
		return -1;
	}
	if(a->length_W == 0){
		return 0;
	}
	bigint_length_t i;
	i = a->length_W - 1;
	do{
		if(a->wordv[i] != b->wordv[i]){
			if(a->wordv[i] > b->wordv[i]){
				return 1;
			}else{
				return -1;
			}
		}
	}while(i--);
	return 0;
}

/******************************************************************************/

void bigint_add_s(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_add_u(dest, a,b);
			SET_POS(dest);
			break;
		case 1: /* a positive, b negative */
			bigint_sub_u(dest, a, b);
			break;
		case 2: /* a negative, b positive */
			bigint_sub_u(dest, b, a);
			break;
		case 3: /* both negative */
			bigint_add_u(dest, a, b);
			SET_NEG(dest);
			break;
		default: /* how can this happen?*/
			break;
	}
}

/******************************************************************************/

void bigint_sub_s(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_sub_u(dest, a,b);
			break;
		case 1: /* a positive, b negative */
			bigint_add_u(dest, a, b);
			SET_POS(dest);
			break;
		case 2: /* a negative, b positive */
			bigint_add_u(dest, a, b);
			SET_NEG(dest);
			break;
		case 3: /* both negative */
			bigint_sub_u(dest, b, a);
			break;
		default: /* how can this happen?*/
					break;
	}

}

/******************************************************************************/

int8_t bigint_cmp_s(const bigint_t *a, const bigint_t *b){
	uint8_t s;
	if(a->length_W==0 && b->length_W==0){
		return 0;
	}
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			return bigint_cmp_u(a, b);
			break;
		case 1: /* a positive, b negative */
			return 1;
			break;
		case 2: /* a negative, b positive */
			return -1;
			break;
		case 3: /* both negative */
			return bigint_cmp_u(b, a);
			break;
		default: /* how can this happen?*/
					break;
	}
	return 0; /* just to satisfy the compiler */
}

/******************************************************************************/

void bigint_shiftleft(bigint_t *a, bigint_length_t shift){
	bigint_length_t byteshift, words_to_shift;
	int16_t i;
	uint8_t bitshift;
	bigint_word_t *p;
	bigint_wordplus_t t = 0;
	if(shift == 0){
		return;
	}
	byteshift = shift / 8;
	bitshift = shift & 7;

	if(byteshift){
		memmove(((uint8_t*)a->wordv) + byteshift, a->wordv, a->length_W * sizeof(bigint_word_t));
		memset(a->wordv, 0, byteshift);
	}
	if(bitshift == 0){
	    a->length_W += (byteshift + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	    bigint_adjust(a);
	    return;
	}
	p = a->wordv + byteshift / sizeof(bigint_word_t);
	words_to_shift = a->length_W + (byteshift % sizeof(bigint_word_t)?1:0);
    for(i=0; i < words_to_shift; ++i){
        t |= ((bigint_wordplus_t)p[i]) << bitshift;
        p[i] = (bigint_word_t)t;
        t >>= BIGINT_WORD_SIZE;
    }
    if(t){
        p[i] = (bigint_word_t)t;
        a->length_W += 1;
    }
    a->length_W += (byteshift + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_shiftright(bigint_t *a, bigint_length_t shift){
	bigint_length_t byteshift;
	bigint_length_t i;
	uint8_t bitshift;
	bigint_wordplus_t t = 0;
	byteshift = shift / 8;
	bitshift = shift & 7;

	if(byteshift >= a->length_W * sizeof(bigint_word_t)){ /* we would shift out more than we have */
		bigint_set_zero(a);
		return;
	}
	if(byteshift == a->length_W * sizeof(bigint_word_t) - 1 && bitshift > GET_FBS(a)){
		bigint_set_zero(a);
		return;
	}

	if(byteshift){
		memmove(a->wordv, (uint8_t*)a->wordv + byteshift, a->length_W * sizeof(bigint_word_t) - byteshift);
	}

    byteshift /= sizeof(bigint_word_t); /* byteshift is now wordshift */
    a->length_W -= byteshift;
	if(bitshift != 0 && a->length_W){
	 /* shift to the right */
		i = a->length_W - 1;
		do{
			t |= ((bigint_wordplus_t)(a->wordv[i])) << (BIGINT_WORD_SIZE - bitshift);
			a->wordv[i] = (bigint_word_t)(t >> BIGINT_WORD_SIZE);
			t <<= BIGINT_WORD_SIZE;
		}while(i--);
	}
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_xor(bigint_t *dest, const bigint_t *a){
	bigint_length_t i;
	for(i=0; i<a->length_W; ++i){
		dest->wordv[i] ^= a->wordv[i];
	}
	bigint_adjust(dest);
}

/******************************************************************************/

void bigint_set_zero(bigint_t *a){
	a->length_W=0;
}

/******************************************************************************/

/* using the Karatsuba-Algorithm */
/* x*y = (xh*yh)*b**2n + ((xh+xl)*(yh+yl) - xh*yh - xl*yl)*b**n + yh*yl */
void bigint_mul_u(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	if(a->length_W == 0 || b->length_W == 0){
		bigint_set_zero(dest);
		return;
	}
	if(dest == a || dest == b){
		bigint_t d;
		bigint_word_t d_b[a->length_W + b->length_W];
		d.wordv = d_b;
		bigint_mul_u(&d, a, b);
		bigint_copy(dest, &d);
		return;
	}
	if(a->length_W == 1 || b->length_W == 1){
		if(a->length_W != 1){
			XCHG_PTR(a,b);
		}
		bigint_wordplus_t t = 0;
		bigint_length_t i;
		bigint_word_t x = a->wordv[0];
		for(i=0; i < b->length_W; ++i){
			t += ((bigint_wordplus_t)b->wordv[i]) * ((bigint_wordplus_t)x);
			dest->wordv[i] = (bigint_word_t)t;
			t >>= BIGINT_WORD_SIZE;
		}
		dest->length_W = i;
		if(t){
		    dest->wordv[i] = (bigint_word_t)t;
		    dest->length_W += 1;
		}
		dest->info = 0;
		bigint_adjust(dest);
		return;
	}
	if(a->length_W * sizeof(bigint_word_t) <= 4 && b->length_W * sizeof(bigint_word_t) <= 4){
		uint32_t p=0, q=0;
		uint64_t r;
		memcpy(&p, a->wordv, a->length_W*sizeof(bigint_word_t));
		memcpy(&q, b->wordv, b->length_W*sizeof(bigint_word_t));
		r = (uint64_t)p * (uint64_t)q;
		memcpy(dest->wordv, &r, (dest->length_W = a->length_W + b->length_W)*sizeof(bigint_word_t));
		bigint_adjust(dest);
		return;
	}
	/* split a in xh & xl; split b in yh & yl */
	const bigint_length_t n = (MAX(a->length_W, b->length_W)+1)/2;
	bigint_t xl, xh, yl, yh;
	xl.wordv = a->wordv;
	yl.wordv = b->wordv;
	if(a->length_W<=n){
		bigint_set_zero(&xh);
		xl.length_W = a->length_W;
		xl.info = a->info;
	}else{
		xl.length_W=n;
		xl.info = 0;
		bigint_adjust(&xl);
		xh.wordv = &(a->wordv[n]);
		xh.length_W = a->length_W-n;
		xh.info = a->info;
	}
	if(b->length_W<=n){
		bigint_set_zero(&yh);
		yl.length_W = b->length_W;
		yl.info = b->info;
	}else{
		yl.length_W=n;
		yl.info = 0;
		bigint_adjust(&yl);
		yh.wordv = &(b->wordv[n]);
		yh.length_W = b->length_W-n;
		yh.info = b->info;
	}
	/* now we have split up a and b */
	/* remember we want to do:
	 * x*y = (xh * b**n + xl) * (yh * b**n + yl)
	 *     = (xh * yh) * b**2n + xh * b**n * yl + yh * b**n * xl + xl * yl
	 *     = (xh * yh) * b**2n + (xh * yl + yh * xl) * b**n + xl *yl
	 *     // xh * yl + yh * xl = (xh + yh) * (xl + yl) - xh * yh - xl * yl
	 * x*y = (xh * yh) * b**2n + ((xh+xl)*(yh+yl) - xh*yh - xl*yl)*b**n + xl*yl
	 *          5              9     2   4   3    7   5   6   1         8   1
	 */
	bigint_word_t  tmp_b[2 * n + 2], m_b[2 * (n + 1)];
	bigint_t tmp, tmp2, m;
	tmp.wordv = tmp_b;
	tmp2.wordv = &(tmp_b[n + 1]);
	m.wordv = m_b;

	bigint_mul_u(dest, &xl, &yl);  /* 1: dest <= xl*yl     */
	bigint_add_u(&tmp2, &xh, &xl); /* 2: tmp2 <= xh+xl     */
	bigint_add_u(&tmp, &yh, &yl);  /* 3: tmp  <= yh+yl     */
	bigint_mul_u(&m, &tmp2, &tmp); /* 4: m    <= tmp2*tmp  */
	bigint_mul_u(&tmp, &xh, &yh);  /* 5: tmp  <= xh*yh     */
	bigint_sub_u(&m, &m, dest);    /* 6: m    <= m-dest    */
    bigint_sub_u(&m, &m, &tmp);    /* 7: m    <= m-tmp     */
	bigint_add_scale_u(dest, &m, n * sizeof(bigint_word_t));       /* 8: dest <= dest+m**n*/
	bigint_add_scale_u(dest, &tmp, 2 * n * sizeof(bigint_word_t)); /* 9: dest <= dest+tmp**(2*n) */
}

/******************************************************************************/

void bigint_mul_s(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_mul_u(dest, a,b);
			SET_POS(dest);
			break;
		case 1: /* a positive, b negative */
			bigint_mul_u(dest, a,b);
			SET_NEG(dest);
			break;
		case 2: /* a negative, b positive */
			bigint_mul_u(dest, a,b);
			SET_NEG(dest);
			break;
		case 3: /* both negative */
			bigint_mul_u(dest, a,b);
			SET_POS(dest);
			break;
		default: /* how can this happen?*/
			break;
	}
}

/******************************************************************************/
#if OLD_SQUARE

#if DEBUG_SQUARE
unsigned square_depth = 0;
#endif

/* square */
/* (xh*b^n+xl)^2 = xh^2*b^2n + 2*xh*xl*b^n + xl^2 */
void bigint_square(bigint_t *dest, const bigint_t *a){
    if(a->length_W * sizeof(bigint_word_t) <= 4){
		uint64_t r = 0;
		memcpy(&r, a->wordv, a->length_W * sizeof(bigint_word_t));
		r = r * r;
		memcpy(dest->wordv, &r, 2 * a->length_W * sizeof(bigint_word_t));
		SET_POS(dest);
		dest->length_W = 2 * a->length_W;
		bigint_adjust(dest);
		return;
	}

	if(dest->wordv == a->wordv){
		bigint_t d;
		bigint_word_t d_b[a->length_W*2];
		d.wordv = d_b;
		bigint_square(&d, a);
		bigint_copy(dest, &d);
		return;
	}
	bigint_fast_square(dest, a);
	return;
#if DEBUG_SQUARE
	square_depth += 1;
#endif

	bigint_length_t n;
	n=(a->length_W+1)/2;
	bigint_t xh, xl, tmp; /* x-high, x-low, temp */
	bigint_word_t buffer[2*n+1];
	xl.wordv = a->wordv;
	xl.length_W = n;
	xl.info = 0;
	xh.wordv = &(a->wordv[n]);
	xh.length_W = a->length_W-n;
	xh.info = a->info;
	bigint_adjust(&xl);
	tmp.wordv = buffer;
/* (xh * b**n + xl)**2 = xh**2 * b**2n + 2 * xh * xl * b**n + xl**2 */
#if DEBUG_SQUARE
	if(square_depth == 1){
        cli_putstr("\r\nDBG (a): xl: "); bigint_print_hex(&xl);
        cli_putstr("\r\nDBG (b): xh: "); bigint_print_hex(&xh);
	}
#endif
	bigint_square(dest, &xl);
#if DEBUG_SQUARE
	if(square_depth == 1){
	    cli_putstr("\r\nDBG (1): xl**2: "); bigint_print_hex(dest);
	}
#endif
    bigint_square(&tmp, &xh);
#if DEBUG_SQUARE
    if(square_depth == 1){
        cli_putstr("\r\nDBG (2): xh**2: "); bigint_print_hex(&tmp);
    }
#endif
	bigint_add_scale_u(dest, &tmp, 2 * n * sizeof(bigint_word_t));
#if DEBUG_SQUARE
	if(square_depth == 1){
	    cli_putstr("\r\nDBG (3): xl**2 + xh**2*n**2: "); bigint_print_hex(dest);
	}
#endif
	bigint_mul_u(&tmp, &xl, &xh);
#if DEBUG_SQUARE
	if(square_depth == 1){
	    cli_putstr("\r\nDBG (4): xl*xh: "); bigint_print_hex(&tmp);
	}
#endif
	bigint_shiftleft(&tmp, 1);
#if DEBUG_SQUARE
	if(square_depth == 1){
	    cli_putstr("\r\nDBG (5): xl*xh*2: "); bigint_print_hex(&tmp);
	}
#endif
	bigint_add_scale_u(dest, &tmp, n * sizeof(bigint_word_t));
#if DEBUG_SQUARE
	if(square_depth == 1){
	    cli_putstr("\r\nDBG (6): x**2: "); bigint_print_hex(dest);
	    cli_putstr("\r\n");
	}
	square_depth -= 1;
#endif
}

#endif

/******************************************************************************/

void bigint_square(bigint_t *dest, const bigint_t *a) {
    union {
        bigint_word_t u[2];
        bigint_wordplus_t uv;
    } acc;
    bigint_word_t q, c1, c2;
    bigint_length_t i, j;

    if(a->length_W * sizeof(bigint_word_t) <= 4){
        uint64_t r = 0;
        memcpy(&r, a->wordv, a->length_W * sizeof(bigint_word_t));
        r = r * r;
        memcpy(dest->wordv, &r, 2 * a->length_W * sizeof(bigint_word_t));
        SET_POS(dest);
        dest->length_W = 2 * a->length_W;
        bigint_adjust(dest);
        return;
    }

    if(dest->wordv == a->wordv){
        bigint_t d;
        bigint_word_t d_b[a->length_W*2];
        d.wordv = d_b;
        bigint_square(&d, a);
        bigint_copy(dest, &d);
        return;
    }

    memset(dest->wordv, 0, a->length_W * 2 * sizeof(bigint_word_t));

    for (i = 0; i < a->length_W; ++i) {
        acc.uv = a->wordv[i] * a->wordv[i] + dest->wordv[2 * i];
        dest->wordv[2 * i] = acc.u[0];
        c1 = acc.u[1];
        c2 = 0;
        for (j = i + 1; j < a->length_W; ++j) {
            acc.uv = a->wordv[i] * a->wordv[j];
            q = acc.u[1] >> (BIGINT_WORD_SIZE - 1);
            acc.uv <<= 1;
            acc.uv += dest->wordv[i + j];
            q += (acc.uv < dest->wordv[i + j]);
            acc.uv += c1;
            q += (acc.uv < c1);
            dest->wordv[i + j] = acc.u[0];
            c1 = acc.u[1] + c2;
            c2 = q + (c1 < c2);
        }
        dest->wordv[i + a->length_W] += c1;
        dest->wordv[i + a->length_W + 1] = c2 + (dest->wordv[i + a->length_W] < c1);
    }
    SET_POS(dest);
    dest->length_W = 2 * a->length_W;
    bigint_adjust(dest);
}

/******************************************************************************/

void bigint_sub_u_bitscale(bigint_t *a, const bigint_t *b, bigint_length_t bitscale){
	bigint_t tmp, x;
	bigint_word_t tmp_b[b->length_W + 1];
	const bigint_length_t word_shift = bitscale / BIGINT_WORD_SIZE;

	if(a->length_W < b->length_W + word_shift){
#if DEBUG
		cli_putstr("\r\nDBG: *bang*\r\n");
#endif
		bigint_set_zero(a);
		return;
	}
	tmp.wordv = tmp_b;
	bigint_copy(&tmp, b);
	bigint_shiftleft(&tmp, bitscale % BIGINT_WORD_SIZE);

	x.info = a->info;
	x.wordv = &(a->wordv[word_shift]);
	x.length_W = a->length_W - word_shift;

	bigint_sub_u(&x, &x, &tmp);
	bigint_adjust(a);
	return;
}

/******************************************************************************/

void bigint_reduce(bigint_t *a, const bigint_t *r){
	uint8_t rfbs = GET_FBS(r);
	if(r->length_W==0 || a->length_W==0){
		return;
	}

	if(bigint_length_b(a) + 3 > bigint_length_b(r)){
        if((r->length_W*sizeof(bigint_word_t)<=4) && (a->length_W*sizeof(bigint_word_t)<=4)){
            uint32_t p=0, q=0;
            memcpy(&p, a->wordv, a->length_W*sizeof(bigint_word_t));
            memcpy(&q, r->wordv, r->length_W*sizeof(bigint_word_t));
            p %= q;
            memcpy(a->wordv, &p, a->length_W*sizeof(bigint_word_t));
            bigint_adjust(a);
            return;
        }
        bigint_length_t shift;
        while(a->length_W > r->length_W){
            shift = (a->length_W - r->length_W) * 8 * sizeof(bigint_word_t) + GET_FBS(a) - rfbs - 1;
            bigint_sub_u_bitscale(a, r, shift);
        }
        while((GET_FBS(a) > rfbs) && (a->length_W == r->length_W)){
            shift = GET_FBS(a)-rfbs-1;
            bigint_sub_u_bitscale(a, r, shift);
        }
	}
	while(bigint_cmp_u(a,r)>=0){
		bigint_sub_u(a,a,r);
	}
	bigint_adjust(a);
}

/******************************************************************************/

/* calculate dest = a**exp % r */
/* using square&multiply */
void bigint_expmod_u_sam(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r){
	if(a->length_W == 0){
	    bigint_set_zero(dest);
		return;
	}

	bigint_t res, base;
	bigint_word_t t, base_w[MAX(a->length_W,r->length_W)], res_w[r->length_W*2];
	bigint_length_t i;
	uint8_t j;
//	bigint_length_t *xaddr = &i;
//	cli_putstr("\r\npre-alloc (");
//	cli_hexdump_rev(&xaddr, 4);
//	cli_putstr(") ...");
	res.wordv = res_w;
	base.wordv = base_w;
	bigint_copy(&base, a);
//	cli_putstr("\r\npost-copy");
	bigint_reduce(&base, r);
	res.wordv[0] = 1;
	res.length_W = 1;
	res.info = 0;
	bigint_adjust(&res);
	if(exp->length_W == 0){
		bigint_copy(dest, &res);
		return;
	}
	uint8_t flag = 0;
	t=exp->wordv[exp->length_W - 1];
	for(i = exp->length_W; i > 0; --i){
		t = exp->wordv[i - 1];
		for(j = BIGINT_WORD_SIZE; j > 0; --j){
			if(!flag){
				if(t & (1 << (BIGINT_WORD_SIZE - 1))){
					flag = 1;
				}
			}
			if(flag){
				bigint_square(&res, &res);
				bigint_reduce(&res, r);
				if(t & (1 << (BIGINT_WORD_SIZE - 1))){
					bigint_mul_u(&res, &res, &base);
					bigint_reduce(&res, r);
				}
			}
			t <<= 1;
		}
	}

//	cli_putc('+');
	SET_POS(&res);
	bigint_copy(dest, &res);
}

/******************************************************************************/
/* gcd <-- gcd(x,y) a*x+b*y=gcd */
void bigint_gcdext(bigint_t *gcd, bigint_t *a, bigint_t *b, const bigint_t *x, const bigint_t *y){
	 bigint_length_t i = 0;
	 if(x->length_W == 0 || y->length_W == 0){
	     if(gcd){
	         bigint_set_zero(gcd);
	     }
	     if(a){
	         bigint_set_zero(a);
	     }
         if(b){
             bigint_set_zero(b);
         }
		 return;
	 }
	 if(x->length_W == 1 && x->wordv[0] == 1){
	     if(gcd){
             gcd->length_W = 1;
             gcd->wordv[0] = 1;
             gcd->info = 0;
	     }
		 if(a){
			 a->length_W = 1;
			 a->wordv[0] = 1;
			 SET_POS(a);
			 bigint_adjust(a);
		 }
		 if(b){
			 bigint_set_zero(b);
		 }
		 return;
	 }
	 if(y->length_W == 1 && y->wordv[0] == 1){
		 if(gcd){
             gcd->length_W = 1;
             gcd->wordv[0] = 1;
             gcd->info = 0;
		 }
		 if(b){
			 b->length_W = 1;
			 b->wordv[0] = 1;
			 SET_POS(b);
			 bigint_adjust(b);
		 }
		 if(a){
			 bigint_set_zero(a);
		 }
		 return;
	 }

	 while(x->wordv[i] == 0 && y->wordv[i] == 0){
		 ++i;
	 }
	 bigint_word_t g_b[i + 2], x_b[x->length_W - i], y_b[y->length_W - i];
	 bigint_word_t u_b[x->length_W - i], v_b[y->length_W - i];
	 bigint_word_t a_b[y->length_W + 2], c_b[y->length_W + 2];
	 bigint_word_t b_b[x->length_W + 2], d_b[x->length_W + 2];
     bigint_t g, x_, y_, u, v, a_, b_, c_, d_;

	 g.wordv = g_b;
	 x_.wordv = x_b;
	 y_.wordv = y_b;
	 memset(g_b, 0, i * sizeof(bigint_word_t));
	 g_b[i] = 1;
	 g.length_W = i + 1;
	 g.info = 0;
	 x_.info = y_.info = 0;
	 x_.length_W = x->length_W - i;
	 y_.length_W = y->length_W - i;
	 memcpy(x_.wordv, x->wordv + i, x_.length_W * sizeof(bigint_word_t));
	 memcpy(y_.wordv, y->wordv + i, y_.length_W * sizeof(bigint_word_t));
	 for(i = 0; (x_.wordv[0] & (1 << i)) == 0 && (y_.wordv[0] & (1 << i)) == 0; ++i){
	 }

	 bigint_adjust(&x_);
	 bigint_adjust(&y_);

	 if(i){
		 bigint_shiftleft(&g, i);
		 bigint_shiftright(&x_, i);
		 bigint_shiftright(&y_, i);
	 }

	 u.wordv = u_b;
	 v.wordv = v_b;
	 a_.wordv = a_b;
	 b_.wordv = b_b;
	 c_.wordv = c_b;
	 d_.wordv = d_b;

	 bigint_copy(&u, &x_);
	 bigint_copy(&v, &y_);
	 a_.wordv[0] = 1;
	 a_.length_W = 1;
	 a_.info = 0;
	 d_.wordv[0] = 1;
	 d_.length_W = 1;
	 d_.info = 0;
	 bigint_set_zero(&b_);
	 bigint_set_zero(&c_);
	 do{
		 while((u.wordv[0] & 1) == 0){
			 bigint_shiftright(&u, 1);
			 if((a_.wordv[0] & 1) || (b_.wordv[0] & 1)){
				 bigint_add_s(&a_, &a_, &y_);
				 bigint_sub_s(&b_, &b_, &x_);
			 }
			 bigint_shiftright(&a_, 1);
			 bigint_shiftright(&b_, 1);
		 }
		 while((v.wordv[0] & 1) == 0){
			 bigint_shiftright(&v, 1);
			 if((c_.wordv[0] & 1) || (d_.wordv[0] & 1)){
				 bigint_add_s(&c_, &c_, &y_);
				 bigint_sub_s(&d_, &d_, &x_);
			 }
			 bigint_shiftright(&c_, 1);
			 bigint_shiftright(&d_, 1);
		 }
		 if(bigint_cmp_u(&u, &v) >= 0){
			bigint_sub_u(&u, &u, &v);
			bigint_sub_s(&a_, &a_, &c_);
			bigint_sub_s(&b_, &b_, &d_);
		 }else{
			bigint_sub_u(&v, &v, &u);
			bigint_sub_s(&c_, &c_, &a_);
			bigint_sub_s(&d_, &d_, &b_);
		 }
	 }while(u.length_W);
	 if(gcd){
		 bigint_mul_s(gcd, &v, &g);
	 }
	 if(a){
		bigint_copy(a, &c_);
	 }
	 if(b){
		 bigint_copy(b, &d_);
	 }
}

/******************************************************************************/

void bigint_inverse(bigint_t *dest, const bigint_t *a, const bigint_t *m){
	bigint_gcdext(NULL, dest, NULL, a, m);
	while(dest->info&BIGINT_NEG_MASK){
		bigint_add_s(dest, dest, m);
	}
}

/******************************************************************************/

void bigint_changeendianess(bigint_t *a){
	uint8_t t, *p, *q;
	p = (uint8_t*)(a->wordv);
	q = p + a->length_W * sizeof(bigint_word_t) - 1;
	while(p<q){
		t = *p;
		*p = *q;
		*q = t;
		++p; --q;
	}
}

/******************************************************************************/

void bigint_mul_word_u(bigint_t *a, bigint_word_t b){
    bigint_wordplus_t c0 = 0, c1 = 0;
    bigint_length_t i;

    if(b == 0){
        bigint_set_zero(a);
        return;
    }

    for(i = 0; i < a->length_W; ++i){
        c1 = ((bigint_wordplus_t)(a->wordv[i])) * ((bigint_wordplus_t)b);
        c1 += c0;
        a->wordv[i] = (bigint_word_t)c1;
        c0 = c1 >> BIGINT_WORD_SIZE;
    }
    if(c0){
        a->wordv[a->length_W] = (bigint_word_t)c0;
        a->length_W += 1;
    }
    bigint_adjust(a);
}

/******************************************************************************/
#if 1

void bigint_clip(bigint_t *dest, bigint_length_t length_W){
    if(dest->length_W > length_W){
        dest->length_W = length_W;
    }
    bigint_adjust(dest);
}
/******************************************************************************/

/*
 * m_ = m * m'[0]
 */

void bigint_mont_mul(bigint_t *dest, const bigint_t *a, const bigint_t *b, const bigint_t *m, const bigint_t *m_){
    const bigint_length_t s = MAX(MAX(a->length_W, b->length_W), m->length_W);
    bigint_t u, t;
    bigint_word_t u_w[s + 2], t_w[s + 2];
    bigint_length_t i;

    if (a->length_W == 0 || b->length_W == 0) {
        bigint_set_zero(dest);
        return;
    }
    u.wordv = u_w;
    u.info = 0;
    u.length_W = 0;
    t.wordv = t_w;
    for (i = 0; i < a->length_W; ++i) {
        bigint_copy(&t, b);
        bigint_mul_word_u(&t, a->wordv[i]);
        bigint_add_u(&u, &u, &t);
        bigint_copy(&t, m_);
        if (u.length_W != 0) {
            bigint_mul_word_u(&t, u.wordv[0]);
            bigint_add_u(&u, &u, &t);
        }
        bigint_shiftright(&u, BIGINT_WORD_SIZE);
    }
    for (; i < s; ++i) {
        bigint_copy(&t, m_);
        if (u.length_W != 0) {
            bigint_mul_word_u(&t, u.wordv[0]);
            bigint_add_u(&u, &u, &t);
        }
        bigint_shiftright(&u, BIGINT_WORD_SIZE);
    }
    bigint_reduce(&u, m);
    bigint_copy(dest, &u);
}

/******************************************************************************/

void bigint_mont_red(bigint_t *dest, const bigint_t *a, const bigint_t *m, const bigint_t *m_){
    bigint_t u, t;
    bigint_length_t i, s = MAX(a->length_W, m->length_W);
    bigint_word_t u_w[s + 2], t_w[s + 2];

    t.wordv = t_w;
    u.wordv = u_w;
    if (a->length_W == 0) {
        bigint_set_zero(dest);
        return;
    }
    bigint_copy(&u, a);
    for (i = 0; i < m->length_W; ++i) {
        bigint_copy(&t, m_);
        if (u.length_W != 0) {
            bigint_mul_word_u(&t, u.wordv[0]);
            bigint_add_u(&u, &u, &t);
        }
        bigint_shiftright(&u, BIGINT_WORD_SIZE);
    }
    bigint_reduce(&u, m);
    bigint_copy(dest, &u);
}

/******************************************************************************/
/*
 * m_ = m * (- m0^-1 (mod 2^W))
 */
void bigint_mont_gen_m_(bigint_t* dest, const bigint_t* m){
    bigint_word_t x_w[2], m_w_0[1];
    bigint_t x, m_0;
    if (m->length_W == 0) {
        bigint_set_zero(dest);
        return;
    }
    if ((m->wordv[0] & 1) == 0) {
        printf_P(PSTR("ERROR: m must not be even, m = "));
        bigint_print_hex(m);
        putchar('\n');
        uart0_flush();
        return;
    }
    x.wordv = x_w;
    x.info = 0;
    x.length_W = 2;
    x_w[0] = 0;
    x_w[1] = 1;
    m_0.wordv = m_w_0;
    m_0.info = 0;
    m_0.length_W = 1;
    m_0.wordv[0] = m->wordv[0];
    bigint_adjust(&x);
    bigint_adjust(&m_0);
    bigint_inverse(dest, &m_0, &x);
    bigint_sub_s(&x, &x, dest);
    bigint_copy(dest, m);
    bigint_mul_word_u(dest, x.wordv[0]);
}

/******************************************************************************/

/*
 * dest = a * R mod m
 */
void bigint_mont_trans(bigint_t *dest, const bigint_t *a, const bigint_t *m){
    bigint_t t;
    bigint_word_t t_w[a->length_W + m->length_W];

    t.wordv = t_w;
    memset(t_w, 0, m->length_W * sizeof(bigint_word_t));
    memcpy(t_w + m->length_W * sizeof(bigint_word_t), a->wordv, a->length_W * sizeof(bigint_word_t));
    t.info = a->info;
    t.length_W = a->length_W + m->length_W;
    bigint_reduce(&t, m);
    bigint_copy(dest, &t);
}

/******************************************************************************/

/* calculate dest = a**exp % r */
/* using square&multiply */
void bigint_expmod_u_mont_sam(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r){
    if(r->length_W == 0) {
        return;
    }
    if(a->length_W == 0) {
        bigint_set_zero(dest);
        return;
    }
    bigint_length_t s = r->length_W;
    bigint_t res, m_, ax;
    bigint_word_t t, res_w[r->length_W*2], ax_w[MAX(s, a->length_W)];
    bigint_word_t m_w_[s + 1];
    bigint_length_t i;
    uint8_t j;

    res.wordv = res_w;
    m_.wordv = m_w_;
    ax.wordv = ax_w;

    res.wordv[0] = 1;
    res.length_W = 1;
    res.info = 0;
    bigint_adjust(&res);
    if (exp->length_W == 0) {
        bigint_copy(dest, &res);
        return;
    }
    bigint_copy(&ax, a);
    bigint_reduce(&ax, r);
    bigint_mont_trans(&ax, &ax, r);
    bigint_mont_trans(&res, &res, r);
    bigint_mont_gen_m_(&m_, r);
    uint8_t flag = 0;
    t = exp->wordv[exp->length_W - 1];
    for (i = exp->length_W; i > 0; --i) {
        t = exp->wordv[i - 1];
        for(j = BIGINT_WORD_SIZE; j > 0; --j){
            if (!flag) {
                if(t & (1 << (BIGINT_WORD_SIZE - 1))){
                    flag = 1;
                }
            }
            if (flag) {
                bigint_square(&res, &res);
                bigint_mont_red(&res, &res, r, &m_);
                if (t & (1 << (BIGINT_WORD_SIZE - 1))) {
                    bigint_mont_mul(&res, &res, &ax, r, &m_);
                }
            }
            t <<= 1;
        }
    }
    SET_POS(&res);
    bigint_mont_red(dest, &res, r, &m_);
}

/******************************************************************************/

#endif

void bigint_expmod_u(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r){
    if (r->wordv[0] & 1) {
        bigint_expmod_u_mont_sam(dest, a, exp, r);
    } else {
        bigint_expmod_u_sam(dest, a, exp, r);
    }
}
















