/*
 * rc6 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "rc6.h"

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

void test_encrypt(uint8_t *block, uint8_t *key, uint16_t keylength, bool print){
	rc6_ctx_t s;
	if (print){
		uart_putstr("\r\nRC6 (enc):\r\n key:\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 16);
	}
	if (rc6_init(&s, key, keylength)){
		uart_putstr("RC6 init failed!");
		return;
	}	
	rc6_enc(&s, block);
	if (print){
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 16);
	}
	rc6_free(&s);
} 

void test_decrypt(uint8_t *block, uint8_t *key, uint16_t keylength, bool print){
	rc6_ctx_t s;
	if (print){
		uart_putstr("\r\nRC6 (dec):\r\n key:\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 16);
	}
	if (rc6_init(&s, key, keylength)){
		uart_putstr("RC6 init failed!");
		return;
	}
	rc6_dec(&s, block);
	if (print){
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 16);
	}
	rc6_free(&s);
} 

/*
 *               Test vectors for encryption with RC6
	plaintext  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	user key   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	ciphertext 8f c3 a5 36 56 b1 f7 78 c1 29 df 4e 98 48 a4 1e
	
	plaintext  02 13 24 35 46 57 68 79 8a 9b ac bd ce df e0 f1
	user key   01 23 45 67 89 ab cd ef 01 12 23 34 45 56 67 78
	ciphertext 52 4e 19 2f 47 15 c6 23 1f 51 f6 36 7e a4 3f 18
	
	plaintext  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	user key   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	           00 00 00 00 00 00 00 00
	ciphertext 6c d6 1b cb 19 0b 30 38 4e 8a 3f 16 86 90 ae 82
	
	plaintext  02 13 24 35 46 57 68 79 8a 9b ac bd ce df e0 f1
	user key   01 23 45 67 89 ab cd ef 01 12 23 34 45 56 67 78
	           89 9a ab bc cd de ef f0
	ciphertext 68 83 29 d0 19 e5 05 04 1e 52 e9 2a f9 52 91 d4
	
	plaintext  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	user key   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	           00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	ciphertext 8f 5f bd 05 10 d1 5f a8 93 fa 3f da 6e 85 7e c2
	
	plaintext  02 13 24 35 46 57 68 79 8a 9b ac bd ce df e0 f1
	user key   01 23 45 67 89 ab cd ef 01 12 23 34 45 56 67 78
	           89 9a ab bc cd de ef f0 10 32 54 76 98 ba dc fe
	ciphertext c8 24 18 16 f0 d7 e4 89 20 ad 16 a1 67 4e 5d 48
 * 
 */

	uint8_t PROGMEM testkey[6][256/8]={
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
			0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
			0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe}};
	uint8_t PROGMEM testplain[2][128/8]={
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79, 0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1}};
	uint8_t PROGMEM testcipher[6][128/8]={
		{0x8f, 0xc3, 0xa5, 0x36, 0x56, 0xb1, 0xf7, 0x78, 0xc1, 0x29, 0xdf, 0x4e, 0x98, 0x48, 0xa4, 0x1e},
		{0x52, 0x4e, 0x19, 0x2f, 0x47, 0x15, 0xc6, 0x23, 0x1f, 0x51, 0xf6, 0x36, 0x7e, 0xa4, 0x3f, 0x18},
		{0x6c, 0xd6, 0x1b, 0xcb, 0x19, 0x0b, 0x30, 0x38, 0x4e, 0x8a, 0x3f, 0x16, 0x86, 0x90, 0xae, 0x82},
		{0x68, 0x83, 0x29, 0xd0, 0x19, 0xe5, 0x05, 0x04, 0x1e, 0x52, 0xe9, 0x2a, 0xf9, 0x52, 0x91, 0xd4},
		{0x8f, 0x5f, 0xbd, 0x05, 0x10, 0xd1, 0x5f, 0xa8, 0x93, 0xfa, 0x3f, 0xda, 0x6e, 0x85, 0x7e, 0xc2},
		{0xc8, 0x24, 0x18, 0x16, 0xf0, 0xd7, 0xe4, 0x89, 0x20, 0xad, 0x16, 0xa1, 0x67, 0x4e, 0x5d, 0x48}};


void testrun_rc6(void){
	uint16_t keysize[]={128, 128, 192, 192, 256, 256};
	uint8_t i;
	uint8_t block[16];
	uint8_t key[32];
	memset(block, 0, 16);
	memset(key, 0, 16);
	
	test_encrypt(block, key, 128, true);
	for(i=0; i<6; ++i){
		memcpy_P(block, testplain[i&1], 128/8);
		memcpy_P(key, testkey[i], keysize[i]/8);
		test_encrypt(block, key, keysize[i], true);
		memcpy_P(key, testkey[i], keysize[i]/8);
		test_decrypt(block, key, keysize[i], true);
	}
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char str[20];

	
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS (RC6)\r\nloaded and running\r\n");
restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "test")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_rc6();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	} /* while (1) */
}

