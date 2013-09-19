/* bigint.h */
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
 * \file		bigint.h
 * \author		Daniel Otte
 * \date		2010-02-22
 * 
 * \license	    GPLv3 or later
 * 
 */

#ifndef BIGINT_H_
#define BIGINT_H_

#include <stddef.h>
#include <stdint.h>

typedef uint8_t  bigint_word_t;
typedef uint16_t bigint_wordplus_t;
typedef int16_t  bigint_wordplus_signed_t;
#define BIGINT_WORD_SIZE 8

#define BIGINT_FBS_MASK (BIGINT_WORD_SIZE-1) /* the last five bits indicate which is the first bit set */
#define BIGINT_NEG_MASK 0x80 /* this bit indicates a negative value */

typedef size_t bigint_length_t;
typedef uint_fast8_t bigint_info_t;

typedef struct{
	bigint_length_t length_W;
	bigint_info_t   info;
	bigint_word_t   *wordv; /* word vector, pointing to the LSB */
}bigint_t;



/******************************************************************************/

void   bigint_adjust(bigint_t *a);
uint32_t bigint_get_first_set_bit(const bigint_t *a);
uint32_t bigint_get_last_set_bit(const bigint_t *a);
bigint_length_t bigint_length_b(const bigint_t *a);
bigint_length_t bigint_length_B(const bigint_t *a);
void   bigint_copy(bigint_t *dest, const bigint_t *src);
void   bigint_add_u(bigint_t *dest, const bigint_t *a, const bigint_t *b);
void   bigint_add_scale_u(bigint_t *dest, const bigint_t *a, bigint_length_t scale);
void   bigint_sub_u(bigint_t *dest, const bigint_t *a, const bigint_t *b);
int8_t bigint_cmp_u(const bigint_t * a, const bigint_t * b);
void   bigint_add_s(bigint_t *dest, const bigint_t *a, const bigint_t *b);
void   bigint_sub_s(bigint_t *dest, const bigint_t *a, const bigint_t *b);
int8_t bigint_cmp_s(const bigint_t *a, const bigint_t *b);
void   bigint_shiftleft(bigint_t *a, bigint_length_t shift);
void   bigint_shiftright(bigint_t *a, bigint_length_t shift);
void   bigint_xor(bigint_t *dest, const bigint_t *a);
void   bigint_set_zero(bigint_t *a);
void   bigint_mul_u(bigint_t *dest, const bigint_t *a, const bigint_t *b);
void   bigint_mul_s(bigint_t *dest, const bigint_t *a, const bigint_t *b);
void   bigint_square(bigint_t *dest, const bigint_t *a);
void   bigint_sub_u_bitscale(bigint_t *a, const bigint_t *b, bigint_length_t bitscale);
void   bigint_reduce(bigint_t *a, const bigint_t *r);
void   bigint_mul_word_u(bigint_t *a, bigint_word_t b);
void   bigint_gcdext(bigint_t *gcd, bigint_t *a, bigint_t *b, const bigint_t *x, const bigint_t *y);
void   bigint_inverse(bigint_t *dest, const bigint_t *a, const bigint_t *m);
void   bigint_changeendianess(bigint_t *a);
void   bigint_clip(bigint_t *dest, bigint_length_t s);
void   bigint_mont_mul(bigint_t *dest, const bigint_t *a, const bigint_t *b, const bigint_t *m, const bigint_t *m_);
void   bigint_mont_red(bigint_t *dest, const bigint_t *a, const bigint_t *m, const bigint_t *m_);
void   bigint_mont_gen_m_(bigint_t* dest, const bigint_t* m);
void   bigint_mont_trans(bigint_t *dest, const bigint_t *a, const bigint_t *m);

void   bigint_expmod_u(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r);
void   bigint_expmod_u_sam(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r);
void   bigint_expmod_u_mont_sam(bigint_t *dest, const bigint_t *a, const bigint_t *exp, const bigint_t *r);


#endif /*BIGINT_H_*/
