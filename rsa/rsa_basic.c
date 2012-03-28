/* rsa_basic.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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
#include <stdlib.h>
#include <string.h>
#include "bigint.h"
#include "bigint_io.h"
#include "rsa_basic.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

void rsa_enc(bigint_t* data, rsa_publickey_t* key){
/*
	cli_putstr_P(PSTR("\r\n -->rsa_enc()\r\n m = "));
	bigint_print_hex(data);
	cli_putstr_P(PSTR("\r\n e = "));
	bigint_print_hex(key->exponent);
	cli_putstr_P(PSTR("\r\n n = "));
	bigint_print_hex(key->modulus);
*/
	bigint_expmod_u(data, data, key->exponent, key->modulus);
}

/*
(p,q,dp,dq,qinv)
m1 = c**dp % p
m2 = c**dq % q
h = (m1 - m2) * qinv % p
m = m2 + q * h
*/

uint8_t rsa_dec_crt_mono(bigint_t* data, rsa_privatekey_t* key){
	bigint_t m1, m2;
	m1.wordv = malloc((key->modulus->length_B + 1) * sizeof(bigint_word_t));
	m2.wordv = malloc(key->components[1]->length_B * sizeof(bigint_word_t));
	if(!m1.wordv || !m2.wordv){
#if DEBUG
		cli_putstr_P(PSTR("\r\nERROR: OOM! (" __FILE__ ")"));
#endif
		free(m1.wordv);
		free(m2.wordv);
		return 1;
	}
#if DEBUG
		cli_putstr_P(PSTR("\r\nexp_mod a + b "));
#endif
	bigint_expmod_u(&m1, data, key->components[2], key->components[0]);
	bigint_expmod_u(&m2, data, key->components[3], key->components[1]);
#if DEBUG
	cli_putstr_P(PSTR("[done] "));
#endif
	bigint_sub_s(&m1, &m1, &m2);
#if DEBUG
	cli_putstr_P(PSTR("[done2] "));
#endif
	while(BIGINT_NEG_MASK & m1.info){
#if DEBUG
		cli_putc(',');
#endif
		bigint_add_s(&m1, &m1, key->components[0]);
	}
#if DEBUG
		cli_putstr_P(PSTR("\r\nreduce_mul "));
#endif
	bigint_reduce(&m1, key->components[0]);
	bigint_mul_u(&m1, &m1, key->components[4]);
#if DEBUG
		cli_putstr_P(PSTR("[done]"));
#endif
	bigint_reduce(&m1, key->components[0]);
	bigint_mul_u(&m1, &m1, key->components[1]);
#if DEBUG
		cli_putstr_P(PSTR(" [done]"));
#endif
	bigint_add_u(data, &m1, &m2);
	free(m1.wordv);
	free(m2.wordv);
	return 0;
}

uint8_t rsa_dec(bigint_t* data, rsa_privatekey_t* key){
	if(key->n == 1){
		bigint_expmod_u(data, data, key->components[0], key->modulus);
		return 0;
	}
	if(key->n == 5){
		if (rsa_dec_crt_mono(data, key)){
			return 3;
		}
		return 0;
	}
	if(key->n<8 || (key->n-5)%3 != 0){
		return 1;
	}
	//rsa_dec_crt_multi(data, key, (key->n-5)/3);
	return 2;
}

void rsa_os2ip(bigint_t* dest, const void* data, uint32_t length_B){
	uint8_t off;
	off = length_B % sizeof(bigint_word_t);
	if(!data){
		if(off){
			dest->wordv = realloc(dest->wordv, length_B + sizeof(bigint_word_t) - off);
			memmove((uint8_t*)dest->wordv+off, dest->wordv, length_B);
			memset(dest->wordv, 0, off);
		}
	}else{
		if(off){
			memcpy((uint8_t*)dest->wordv + off, data, length_B);
			memset(dest, 0, off);
		}else{
			memcpy(dest->wordv, data, length_B);
		}
	}
	dest->length_B = (length_B + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	bigint_changeendianess(dest);
	bigint_adjust(dest);
}

void rsa_i2osp(void* dest, bigint_t* src, uint16_t* out_length_B){
	*out_length_B = bigint_get_first_set_bit(src) / 8 + 1;
	if(dest){
		uint16_t i;
		for(i=*out_length_B; i>0; --i){
			*((uint8_t*)dest) = ((uint8_t*)src->wordv)[i-1];
			dest = (uint8_t*)dest + 1;
		}
	}else{
		uint8_t off;
		bigint_changeendianess(src);
		bigint_adjust(src);

		off = bigint_get_last_set_bit(src)/8;
		if(off){
			memmove(src->wordv, (uint8_t*)src->wordv+off, *out_length_B);
		}
	}
}

