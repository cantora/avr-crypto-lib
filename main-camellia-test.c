/* main-camellia-test.c */
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
 * camellia test-suit
 * 
 */

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "camellia.h"

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>


#ifndef BOOL
#define BOOL
 #ifndef __BOOL
 #define __BOOL
  #ifndef __BOOL__
  #define __BOOL__
	typedef enum{false=0,true=1} bool;
  #endif
 #endif
#endif



/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/
void camellia128_ctx_dump(camellia128_ctx_t *s);

void test_encrypt(uint8_t *block, uint8_t *key, uint16_t keylength, bool print){
	camellia128_ctx_t s;
	if (print){
		uart_putstr("\r\nCamellia (enc):\r\n key:\t\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 16);
	}
	
	camellia128_init(&s, key);;
	camellia128_enc(&s, block);
	if (print){
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 16);
	}
} 

void test_decrypt(uint8_t *block, uint8_t *key, uint16_t keylength, bool print){
	camellia128_ctx_t s;
	if (print){
		uart_putstr("\r\nCamellia (dec):\r\n key:\t\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 16);
	}
	camellia128_init(&s, key);
	camellia128_dec(&s, block);
	if (print){
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 16);
	}
} 

void nessie_test_iterate(uint8_t *block, uint8_t *key){
	uint16_t i;
	test_encrypt(block, key, 128, true);
	test_decrypt(block, key, 128, true);
	uart_putstr("\r\n100 times:");
	for(i=0; i<99; ++i){
		test_encrypt(block, key, 128, false);
	}
	test_encrypt(block, key, 128, true);
	uart_putstr("\r\n1000 times:");
	for(i=0; i<(999-100); ++i){
		test_encrypt(block, key, 128, false);
	}
	test_encrypt(block, key, 128, true);
}

void nessie_test_iterate_inv(uint8_t *block, uint8_t *key){
	uint16_t i;
	test_decrypt(block, key, 128, true);
	test_encrypt(block, key, 128, true);
	uart_putstr("\r\n100 times:");
	for(i=0; i<99; ++i){
		test_decrypt(block, key, 128, false);
	}
	test_encrypt(block, key, 128, true);
	uart_putstr("\r\n1000 times:");
	for(i=0; i<(999-100); ++i){
		test_decrypt(block, key, 128, false);
	}
	test_decrypt(block, key, 128, true);
}

prog_uint8_t ntt_test_values_in[16] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
	0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

prog_uint8_t ntt_test_values_out[16] = {
	0x67, 0x67, 0x31, 0x38, 0x54, 0x96, 0x69, 0x73, 
	0x08, 0x57, 0x06, 0x56, 0x48, 0xea, 0xbe, 0x43
};
/* memcmp_P() is now implemented in avr-libc
int memcmp_P(const void *s1, PGM_P s2, size_t n){
	uint8_t b;
	while(n--){
		b = pgm_read_byte_near(s2);
		if( *((uint8_t*)s1) != b)
			return(*((uint8_t*)s1)-b);
		++s1; ++s2;
	}
	return 0;
}
*/
void testrun_camellia(void){
	/* we run the NESSIE test for Camellia here see 
	 * https://www.cosic.esat.kuleuven.be/nessie/testvectors/bc/camellia/Camellia-128-128.verified.test-vectors
	 * for the vectors 
	 */
	unsigned j, setn;
	uint8_t block[16];
	uint8_t key[16];
	memcpy_P(block, ntt_test_values_in, 16);	
	memcpy_P(key,   ntt_test_values_in, 16);
	test_encrypt(block, key, 128, true);
	if(memcmp_P(block, ntt_test_values_out, 16)){
		uart_putstr("\t[FAILED]\r\n");
		return;
	}
	uart_putstr("\t[OK]");
	test_decrypt(block, key, 128, true);
	if(memcmp_P(block, ntt_test_values_in, 16)){
		uart_putstr("\t[FAILED]\r\n");
		return;
	}
	uart_putstr("\t[OK]");
	
/* test set #1 & #2 */
	setn=1;
	for(setn=1; setn<=2; ++setn){
		for(j=0; j<128; ++j){
			uart_putstr("\r\n\r\n### SET: ");
			uart_hexdump(&setn,1);
			uart_putstr(" Vector: ");
			uart_hexdump(&j,1);
			
			memset(block, 0, 16);
			memset(key, 0, 16);
			((setn&0x1)?key:block)[j>>3] = 1<<(((~j)&0x7));
			nessie_test_iterate(block, key);
		}
	}
/* test set #3 */	
	for(j=0; j<256; ++j){
		uart_putstr("\r\n### SET: ");
		uart_hexdump(&setn,1);
		uart_putstr(" Vector: ");
		uart_hexdump(&j,1);
		
		memset(block, j, 16);
		memset(key, 0, 16);
		nessie_test_iterate(block, key);
	}
	setn++;
/* test set #4 (some strange values*/
	setn++;
/* test ser #5 & #6 (same as 1&2 but enc and dec exchanged)*/	
	for(setn=5; setn<=6; ++setn){
		for(j=0; j<128; ++j){
			uart_putstr("\r\n\r\n### SET: ");
			uart_hexdump(&setn,1);
			uart_putstr(" Vector: ");
			uart_hexdump(&j,1);
			
			memset(block, 0, 16);
			memset(key, 0, 16);
			((setn&0x1)?key:block)[j>>3] = 1<<(((~j)&0x7));
			nessie_test_iterate_inv(block, key);
		}
	}
/* test set #7 */	
	for(j=0; j<256; ++j){
		uart_putstr("\r\n### SET: ");
		uart_hexdump(&setn,1);
		uart_putstr(" Vector: ");
		uart_hexdump(&j,1);
		
		memset(block, j, 16);
		memset(key, 0, 16);
		nessie_test_iterate_inv(block, key);
	}
	setn++;
/* test set #4 (some strange values*/
	setn++;
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (Camellia)\r\nloaded and running\r\n");
restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "test")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_camellia();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	} /* while (1) */
}

