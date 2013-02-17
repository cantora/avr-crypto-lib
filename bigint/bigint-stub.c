/* bigint.c */
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
/*
#include "cli.h"
#include "bigint_io.h"
*/
#ifndef MAX
 #define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
 #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define SET_FBS(a, v) do{(a)->info &=0xF8; (a)->info |= (v);}while(0)
#define GET_FBS(a)   ((a)->info&BIGINT_FBS_MASK)
#define SET_NEG(a)   (a)->info |= BIGINT_NEG_MASK
#define SET_POS(a)   (a)->info &= ~BIGINT_NEG_MASK
#define XCHG(a,b)    do{(a)^=(b); (b)^=(a); (a)^=(b);}while(0)
#define XCHG_PTR(a,b)    do{ a = (void*)(((uint16_t)(a)) ^ ((uint16_t)(b))); \
	                         b = (void*)(((uint16_t)(a)) ^ ((uint16_t)(b))); \
	                         a = (void*)(((uint16_t)(a)) ^ ((uint16_t)(b)));}while(0)

#define GET_SIGN(a) ((a)->info&BIGINT_NEG_MASK)


/******************************************************************************/
/*
void bigint_copy(bigint_t *dest, const bigint_t *src){
	memcpy(dest->wordv, src->wordv, src->length_W);
	dest->length_W = src->length_W;
	dest->info = src->info;
}
*/
/******************************************************************************/

/* this should be implemented in assembly */
/*
void bigint_add_u(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	uint16_t t=0, i;
	if(a->length_W < b->length_W){
		XCHG_PTR(a,b);
	}
	for(i=0; i<b->length_W; ++i){
		t = a->wordv[i] + b->wordv[i] + t;
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
	}
	for(; i<a->length_W; ++i){
		t = a->wordv[i] + t;
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
	}
	dest->wordv[i++] = t;
	dest->length_W = i;
	bigint_adjust(dest);
}
*/
/******************************************************************************/

/* this should be implemented in assembly */
/*
void bigint_add_scale_u(bigint_t *dest, const bigint_t *a, uint16_t scale){
	uint16_t i,j=0;
	uint16_t t=0;
	if(scale>dest->length_W)
		memset(dest->wordv+dest->length_W, 0, scale-dest->length_W);
	for(i=scale; i<a->length_W+scale; ++i,++j){
		t = a->wordv[j] + t;
		if(dest->length_W>i){
			t += dest->wordv[i];
		}
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
	}
	while(t){
		if(dest->length_W>i){
			t = dest->wordv[i] + t;
		}
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
		++i;
	}
	if(dest->length_W < i){
		dest->length_W = i;
	}
	bigint_adjust(dest);
}
*/
/******************************************************************************/

/* this should be implemented in assembly */
void bigint_sub_u(bigint_t *dest, const bigint_t *a, const bigint_t *b){
	int8_t borrow=0;
	int8_t  r;
	int16_t t;
	uint16_t i, min, max;
	min = MIN(a->length_W, b->length_W);
	max = MAX(a->length_W, b->length_W);
	r = bigint_cmp_u(a,b);
	if(r==0){
		dest->length_W = 0;
		dest->wordv[0] = 0;
		bigint_adjust(dest);
		return;
	}
	if(b->length_W==0){
		dest->length_W = a->length_W;
		memcpy(dest->wordv, a->wordv, a->length_W);
		dest->info = a->info;
		SET_POS(dest);
		return;
	}
	if(a->length_W==0){
			dest->length_W = b->length_W;
			memcpy(dest->wordv, b->wordv, b->length_W);
			dest->info = b->info;
			SET_NEG(dest);
			return;
	}
	if(r<0){
		bigint_sub_u(dest, b, a);
		SET_NEG(dest);
	}else{
		for(i=0; i<min; ++i){
			t = a->wordv[i] - b->wordv[i] - borrow;
			if(t<0){
				borrow = 1;
				dest->wordv[i]=(uint8_t)t;
			}else{
				borrow = 0;
				dest->wordv[i]=(uint8_t)t;
			}
		}
		for(;i<max; ++i){
			t = a->wordv[i] - borrow;
			if(t<0){
				borrow = 1;
				dest->wordv[i]=(uint8_t)t;
			}else{
				borrow = 0;
				dest->wordv[i]=(uint8_t)t;
			}

		}
		SET_POS(dest);
		dest->length_W = i;
		bigint_adjust(dest);
	}
}

/******************************************************************************/

int8_t bigint_cmp_u(const bigint_t *a, const bigint_t *b){
	if(a->length_W > b->length_W){
		return 1;
	}
	if(a->length_W < b->length_W){
		return -1;
	}
	if(a->length_W==0){
		return 0;
	}
	uint16_t i;
	i = a->length_W-1;
	do{
		if(a->wordv[i]!=b->wordv[i]){
			if(a->wordv[i]>b->wordv[i]){
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

void bigint_shiftleft(bigint_t *a, uint16_t shift){
	uint16_t byteshift;
	uint16_t i;
	uint8_t bitshift;
	uint16_t t=0;
	byteshift = (shift+3)/8;
	bitshift = shift&7;
	memmove(a->wordv+byteshift, a->wordv, a->length_W);
	memset(a->wordv, 0, byteshift);
	if(bitshift!=0){
		if(bitshift<=4){ /* shift to the left */
			for(i=byteshift; i<a->length_W+byteshift; ++i){
				t |= (a->wordv[i])<<bitshift;
				a->wordv[i] = (uint8_t)t;
				t >>= 8;
			}
			a->wordv[i] = (uint8_t)t;
			byteshift++;
		}else{ /* shift to the right */
			for(i=a->length_W+byteshift-1; i>byteshift-1; --i){
				t |= (a->wordv[i])<<(bitshift);
				a->wordv[i] = (uint8_t)(t>>8);
				t <<= 8;
			}
			t |= (a->wordv[i])<<(bitshift);
			a->wordv[i] = (uint8_t)(t>>8);
		}
	}
	a->length_W += byteshift;
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_shiftright(bigint_t *a, uint16_t shift){
	uint16_t byteshift;
	uint16_t i;
	uint8_t bitshift;
	uint16_t t=0;
	byteshift = shift/8;
	bitshift = shift&7;
	if(byteshift >= a->length_W){ /* we would shift out more than we have */
		bigint_set_zero(a);
		return;
	}
	if(byteshift == a->length_W-1 && bitshift>GET_FBS(a)){
		bigint_set_zero(a);
		return;
	}
	if(byteshift){
		memmove(a->wordv, a->wordv+byteshift, a->length_W-byteshift);
		memset(a->wordv+a->length_W-byteshift, 0,  byteshift);
	}
	if(bitshift!=0){
	 /* shift to the right */
		for(i=a->length_W-byteshift-1; i>0; --i){
			t |= (a->wordv[i])<<(8-bitshift);
			a->wordv[i] = (uint8_t)(t>>8);
			t <<= 8;
		}
		t |= (a->wordv[0])<<(8-bitshift);
		a->wordv[0] = (uint8_t)(t>>8);
	}
	a->length_W -= byteshift;
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_xor(bigint_t *dest, const bigint_t *a){
	uint16_t i;
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
	if(a->length_W==0 || b->length_W==0){
		bigint_set_zero(dest);
		return;
	}
	if(dest==a || dest==b){
		bigint_t d;
		uint8_t d_b[a->length_W+b->length_W];
		d.wordv = d_b;
		bigint_mul_u(&d, a, b);
		bigint_copy(dest, &d);
		return;
	}
	if(a->length_W==1 || b->length_W==1){
		if(a->length_W!=1){
			XCHG_PTR(a,b);
		}
		uint16_t i, t=0;
		uint8_t x = a->wordv[0];
		for(i=0; i<b->length_W; ++i){
			t += b->wordv[i]*x;
			dest->wordv[i] = (uint8_t)t;
			t>>=8;
		}
		dest->wordv[i] = (uint8_t)t;
		dest->length_W=i+1;
		bigint_adjust(dest);
		return;
	}
	if(a->length_W<=4 && b->length_W<=4){
		uint32_t p=0, q=0;
		uint64_t r;
		memcpy(&p, a->wordv, a->length_W);
		memcpy(&q, b->wordv, b->length_W);
		r = (uint64_t)p*(uint64_t)q;
		memcpy(dest->wordv, &r, a->length_W+b->length_W);
		dest->length_W =  a->length_W+b->length_W;
		bigint_adjust(dest);
		return;
	}
	bigint_set_zero(dest);
	/* split a in xh & xl; split b in yh & yl */
	uint16_t n;
	n=(MAX(a->length_W, b->length_W)+1)/2;
	bigint_t xl, xh, yl, yh;
	xl.wordv = a->wordv;
	yl.wordv = b->wordv;
	if(a->length_W<=n){
		xh.info=0;
		xh.length_W = 0;
		xl.length_W = a->length_W;
		xl.info = 0;
	}else{
		xl.length_W=n;
		xl.info = 0;
		bigint_adjust(&xl);
		xh.wordv = a->wordv+n;
		xh.length_W = a->length_W-n;
		xh.info = 0;
	}
	if(b->length_W<=n){
		yh.info=0;
		yh.length_W = 0;
		yl.length_W = b->length_W;
		yl.info = b->info;
	}else{
		yl.length_W=n;
		yl.info = 0;
		bigint_adjust(&yl);
		yh.wordv = b->wordv+n;
		yh.length_W = b->length_W-n;
		yh.info = 0;
	}
	/* now we have split up a and b */
	uint8_t  tmp_b[2*n+2], m_b[2*(n+1)];
	bigint_t tmp, tmp2, m;
	tmp.wordv = tmp_b;
	tmp2.wordv = tmp_b+n+1;
	m.wordv = m_b;

	bigint_mul_u(dest, &xl, &yl);  /* dest <= xl*yl     */
	bigint_add_u(&tmp2, &xh, &xl); /* tmp2 <= xh+xl     */
	bigint_add_u(&tmp, &yh, &yl);  /* tmp  <= yh+yl     */
	bigint_mul_u(&m, &tmp2, &tmp); /* m    <= tmp2*tmp  */
	bigint_mul_u(&tmp, &xh, &yh);  /* h    <= xh*yh     */
	bigint_sub_u(&m, &m, dest);    /* m    <= m-dest    */
    bigint_sub_u(&m, &m, &tmp);    /* m    <= m-h       */
	bigint_add_scale_u(dest, &m, n);
	bigint_add_scale_u(dest, &tmp, 2*n);
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

/* square */
/* (xh*b^n+xl)^2 = xh^2*b^2n + 2*xh*xl*b^n + xl^2 */
void bigint_square(bigint_t *dest, const bigint_t *a){
	if(a->length_W<=4){
		uint64_t r=0;
		memcpy(&r, a->wordv, a->length_W);
		r = r*r;
		memcpy(dest->wordv, &r, 2*a->length_W);
		SET_POS(dest);
		dest->length_W=2*a->length_W;
		bigint_adjust(dest);
		return;
	}
	if(dest==a){
		bigint_t d;
		uint8_t d_b[a->length_W*2];
		d.wordv = d_b;
		bigint_square(&d, a);
		bigint_copy(dest, &d);
		return;
	}
	uint16_t n;
	n=(a->length_W+1)/2;
	bigint_t xh, xl, tmp; /* x-high, x-low, temp */
	uint8_t buffer[2*n+1];
	xl.wordv = a->wordv;
	xl.length_W = n;
	xh.wordv = a->wordv+n;
	xh.length_W = a->length_W-n;
	tmp.wordv = buffer;
	bigint_square(dest, &xl);
	bigint_square(&tmp, &xh);
	bigint_add_scale_u(dest, &tmp, 2*n);
	bigint_mul_u(&tmp, &xl, &xh);
	bigint_shiftleft(&tmp, 1);
	bigint_add_scale_u(dest, &tmp, n);
}

/******************************************************************************/

void bigint_sub_u_bitscale(bigint_t *a, const bigint_t *b, uint16_t bitscale){
	bigint_t tmp;
	uint8_t tmp_b[b->length_W+1];
	uint16_t i,j,byteshift=bitscale/8;
	uint8_t borrow=0;
	int16_t t;

	if(a->length_W < b->length_W+byteshift){
		bigint_set_zero(a);
		return;
	}

	tmp.wordv = tmp_b;
	bigint_copy(&tmp, b);
	bigint_shiftleft(&tmp, bitscale&7);

	for(j=0,i=byteshift; i<tmp.length_W+byteshift; ++i, ++j){
		t = a->wordv[i] - tmp.wordv[j] - borrow;
		a->wordv[i] = (uint8_t)t;
		if(t<0){
			borrow = 1;
		}else{
			borrow = 0;
		}
	}
	while(borrow){
		if(i+1 > a->length_W){
			bigint_set_zero(a);
			return;
		}
		a->wordv[i] -= borrow;
		if(a->wordv[i]!=0xff){
			borrow=0;
		}
		++i;
	}
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_reduce(bigint_t *a, const bigint_t *r){
//	bigint_adjust(r);
	uint8_t rfbs = GET_FBS(r);

	if(r->length_W==0 || a->length_W==0){
		return;
	}
	while(a->length_W > r->length_W){
		bigint_sub_u_bitscale(a, r, (a->length_W-r->length_W)*8+GET_FBS(a)-rfbs-1);
	}
	while((GET_FBS(a) > rfbs+1) && (a->length_W == r->length_W)){
		bigint_sub_u_bitscale(a, r, GET_FBS(a)-rfbs-1);
	}
	while(bigint_cmp_u(a,r)>=0){
		bigint_sub_u(a,a,r);
	}
	bigint_adjust(a);
}

/******************************************************************************/

/* calculate dest = a**exp % r */
/* using square&multiply */
void bigint_expmod_u(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r){
	if(a->length_W==0 || r->length_W==0){
		return;
	}

	bigint_t res, base;
	uint8_t base_b[MAX(a->length_W,r->length_W*2)], res_b[r->length_W*2];
	uint16_t i;
	uint8_t j, t;
	res.wordv = res_b;
	base.wordv = base_b;
	bigint_copy(&base, a);
	bigint_reduce(&base, r);
	res.wordv[0]=1;
	res.length_W=1;
	res.info = 0;
	bigint_adjust(&res);
	for(i=0; i+1<exp->length_W; ++i){
		t=exp->wordv[i];
		for(j=0; j<8; ++j){
			if(t&1){
				bigint_mul_u(&res, &res, &base);
				bigint_reduce(&res, r);
			}
			bigint_square(&base, &base);
			bigint_reduce(&base, r);
			t>>=1;
		}
	}
	t=exp->wordv[i];
	while(t){
		if(t&1){
			bigint_mul_u(&res, &res, &base);
			bigint_reduce(&res, r);
		}
		bigint_square(&base, &base);
		bigint_reduce(&base, r);
		t>>=1;
	}
	SET_POS(&res);
	bigint_copy(dest, &res);
}

/******************************************************************************/
/* gcd <-- gcd(x,y) a*x+b*y=gcd */
void bigint_gcdext(bigint_t *gcd, bigint_t *a, bigint_t *b, const bigint_t *x, const bigint_t *y){
	 bigint_t g, x_, y_, u, v, a_, b_, c_, d_;
	 volatile uint16_t i=0;
	 if(x->length_W==0 || y->length_W==0){
		 return;
	 }
	 while(x->wordv[i]==0 && y->wordv[i]==0){
		 ++i;
	 }
	 uint8_t g_b[i+2], x_b[x->length_W-i], y_b[y->length_W-i];
	 uint8_t u_b[x->length_W-i], v_b[y->length_W-i];
	 uint8_t a_b[y->length_W+2], c_b[y->length_W+2];
	 uint8_t b_b[x->length_W+2], d_b[x->length_W+2];

	 g.wordv = g_b;
	 x_.wordv = x_b;
	 y_.wordv = y_b;
	 memset(g_b, 0, i);
	 g_b[i]=1;
	 g.length_W = i+1;
	 g.info=0;
	 x_.info = y_.info = 0;
	 x_.length_W = x->length_W-i;
	 y_.length_W = y->length_W-i;
	 memcpy(x_.wordv, x->wordv+i, x_.length_W);
	 memcpy(y_.wordv, y->wordv+i, y_.length_W);
	 for(i=0; (x_.wordv[0]&(1<<i))==0 && (y_.wordv[0]&(1<<i))==0; ++i){
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
		 while((u.wordv[0]&1)==0){
			 bigint_shiftright(&u, 1);
			 if((a_.wordv[0]&1) || (b_.wordv[0]&1)){
				 bigint_add_s(&a_, &a_, &y_);
				 bigint_sub_s(&b_, &b_, &x_);
			 }
			 bigint_shiftright(&a_, 1);
			 bigint_shiftright(&b_, 1);
		 }
		 while((v.wordv[0]&1)==0){
			 bigint_shiftright(&v, 1);
			 if((c_.wordv[0]&1) || (d_.wordv[0]&1)){
				 bigint_add_s(&c_, &c_, &y_);
				 bigint_sub_s(&d_, &d_, &x_);
			 }
			 bigint_shiftright(&c_, 1);
			 bigint_shiftright(&d_, 1);

		 }
		 if(bigint_cmp_u(&u, &v)>=0){
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
	p = a->wordv;
	q = p+a->length_W-1;
	while(p<q){
		t = *p;
		*p = *q;
		*q = t;
		++p; --q;
	}
}

/******************************************************************************/























