/*
 * crypto-test
 * 
*/


#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "sha256.h"
#include "xtea.h"
#include "prng.h"
#include "cast5.h"

#include <stdint.h>
#include <string.h>


/*****************************************************************************
 *  additional validation-functions											 *
*****************************************************************************/

void shavs_rnd(sha256_hash_t seed){
	uint8_t md[4][SHA256_HASH_BITS/8], buffer[3*SHA256_HASH_BITS/8];
	uint8_t j;
	uint16_t i;
	
	for(j=0; j< 100; ++j){
		memcpy(md[0], seed, SHA256_HASH_BITS/8);
		memcpy(md[1], seed, SHA256_HASH_BITS/8);
		memcpy(md[2], seed, SHA256_HASH_BITS/8);
		for(i=3; i<1003; ++i){
			memcpy(buffer+0*(SHA256_HASH_BITS/8), md[(i-3)%4], SHA256_HASH_BITS/8);
			memcpy(buffer+1*(SHA256_HASH_BITS/8), md[(i-2)%4], SHA256_HASH_BITS/8);
			memcpy(buffer+2*(SHA256_HASH_BITS/8), md[(i-1)%4], SHA256_HASH_BITS/8);
			sha256(((void*)md[i%4]), buffer, 3*SHA256_HASH_BITS);
			uart_putc('.');
		}
		/* OUTPUT */
		--i;
		uart_putstr("\r\nMD = ");
		uart_hexdump(md[i%4], SHA256_HASH_BITS/8);
		uart_putstr("\r\n");
		memcpy(seed, (md[i%4]), SHA256_HASH_BITS/8);
	}
}

/*****************************************************************************
 *  self tests																 *
*****************************************************************************/
void testrun_sha256(void){
	uint8_t block[SHA256_BLOCK_BITS/8];
	
	uart_putstr("\r\nsha256(\"\", 0)= ");
	sha256((void*)block, (void*)"\x00", 0);
	uart_hexdump(block, SHA256_HASH_BITS/8);
	

        uart_putstr("\r\nsha256(0x80, 8)= ");
	sha256((void*)block, (void*)"\x80", 8);
	uart_hexdump(block, SHA256_HASH_BITS/8);

	uart_putstr("\r\nsha256(0x02, 8)= ");
        sha256((void*)block, (void*)"\x02", 8);
	uart_hexdump(block, SHA256_HASH_BITS/8);


	uart_putstr("\r\nsha256(\"abc\", 24)= ");
	sha256((void*)block, (void*)"abc", 24);
	uart_hexdump(block, SHA256_HASH_BITS/8);
	
	uart_putstr("\r\nsha256(\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\", 24)= ");
	sha256((void*)block, (void*) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 448);
	uart_hexdump(block, SHA256_HASH_BITS/8);
	
	uart_putstr("\r\nsha256(1,000,000 x 'a')= ");
	{
		uint16_t i;
		sha256_ctx_t s;
		sha256_init(&s);
		memset(block,'a',SHA256_BLOCK_BITS/8);
		for(i=0; i<(1000000/(SHA256_BLOCK_BITS/8)); ++i){ /* 15625 times*/
			sha256_nextBlock(&s, block);
		} 
		sha256_lastBlock(&s, block, 0);
		sha256_ctx2hash((void*)block, &s);
	}
	uart_hexdump(block, SHA256_HASH_BITS/8);
}

void testrun_xtea(void){
	uint8_t block[8], block2[8];
	uint8_t key [16];
	
	memcpy (block, "abcdefgh", 8);
	memset (key, 0, 16);
	memset (block2, 0, 8);
	uart_putstr("\r\nxtea_enc(\"abcdefgh\", 0)= ");
	xtea_enc((void*)block2, (void*)block, (void*)key);
	uart_hexdump(block2, 8);
	uart_putstr("\r\nxtea_dec(form above)= ");
	xtea_dec((void*)block, (void*)block2, (void*)key);
	uart_hexdump(block, 8);

	memcpy (key, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", 16);
	uart_putstr("\r\nxtea_enc(\"abcdefgh\", 000102030405060708090a0b0c0d0e0f)= ");
	xtea_enc((void*)block, (void*)block, (void*)key);
	uart_hexdump(block, 8);
	uart_putstr("\r\nxtea_dec(form above)= ");
	xtea_dec((void*)block, (void*)block, (void*)key);
	uart_hexdump(block, 8);	
}

#if 0

void testrun_arcfour(void){
	arcfour_ctx_t s;
	char *b;
	/* using wikipedia test-vectors:
	 *	RC4( "Key", "Plaintext" ) == "bbf316e8 d940af0a d3"
	 *	RC4( "Wiki", "pedia" ) == "1021bf0420"
	 *	RC4( "Secret", "Attack at dawn" ) == "45a01f64 5fc35b38 3552544b 9bf5"
	 **/
	uart_putstr("\r\narcfour(\"Plaintext\", \"Key\")=");
	arcfour_init(&s, (uint8_t*)"Key", 3);
	b="Plaintext";
	while (*b)
		*b++ ^= arcfour_gen(&s);
	uart_hexdump(b-9, 9);
	
	uart_putstr("\r\narcfour(\"pedia\", \"Wiki\")=");
	arcfour_init(&s, (uint8_t*)"Wiki", 4);
	b="pedia";
	while (*b)
		*b++ ^= arcfour_gen(&s);
	uart_hexdump(b-5, 5);
	
	uart_putstr("\r\narcfour(\"Attack at dawn\", \"Secret\")=");
	arcfour_init(&s, (uint8_t*)"Secret", 6);
	b="Attack at dawn";
	while (*b)
		*b++ ^= arcfour_gen(&s);
	uart_hexdump(b-14, 14);
	
	uart_putstr("\r\narcfour(00.00.00.00.00.00.00.00, 01.23.45.67.89.AB.CD.EF)=");
	arcfour_init(&s, (uint8_t*)"\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8);
	int i=0;
	uint8_t a[8];
	memset(a, 0 , 8);
	while (i < 8)
		a[i++] ^= arcfour_gen(&s);
	uart_hexdump(a, 8);	
}

#endif

void testrun_prng(void){
	uint8_t i,block[32];
	uart_putstr("\r\naddEntropy(32, 0x00000000)");
	addEntropy(32,"\x00\x00\x00\x00");
	for(i=0;i<12;++i){
		getRandomBlock((void*)block);
		uart_putstr("\r\n");
		uart_hexdump(block, 32);
	}
}

void testrun_cast5(void){
	cast5_ctx_t s;
	uint8_t i;
	uart_putstr("\r\nCAST5:\r\nkey: 01 23 45 67 34 56 78 23 45 67 89 34 56 78 9A");
	cast5_init(&s, (uint8_t*)"\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9A", 128);
	uint8_t block[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
	uart_putstr("\r\nplaintext: ");
	uart_hexdump(block, 8);
	cast5_enc(&s, block);
	uart_putstr("\r\nciphertext: ");
	uart_hexdump(block, 8);
	for(i=0; i<16; ++i){
		uart_putstr("\r\nK"); uart_putc('0'+(i+1)/10); uart_putc('0'+(i+1)%10); uart_putstr(": ");
		uart_hexdump(&(s.mask[i]),4);
	}
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	uint64_t length=0;
	sha256_ctx_t s;
	char str[20];
	int i;
	uint8_t block[SHA256_BLOCK_BITS/8];
	
	DEBUG_INIT();
	
	sha256_init(&s);
	uart_putstr("\r\n");

	uart_putstr("\r\n\r\nCrypto-VS\r\nloaded and running\r\n");
restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "REQ")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W2\r\n"); goto error;}
		if (strcmp(str, "SHA256")) {
			if (strcmp(str, "test")){DEBUG_S("DBG: 1d\r\n"); goto error;};
			/* use some fixed test-vectors and all Algos */
					uart_putstr("\r\n intergrated selftests:\r\n");
				testrun_xtea();
					uart_putstr("\r\n");
				testrun_prng();
					uart_putstr("\r\n");
				testrun_cast5();
					uart_putstr("\r\n");
		//		testrun_arcfour();
		//			uart_putstr("\r\n");
				testrun_sha256();
			goto restart;
		}
		if (!getnextwordn(str,20)) {DEBUG_S("DBG: W4\r\n"); goto error;}
		if (strcmp(str, "Len=")) {
	/* 1d9370cdccba99b23670e2e0d6514001006f50d3c7a453201d2776f03c5e58fd */
	/* 	f41ece26 13e45739 15696b5a dcd51ca3
		28be3bf5 66a9ca99 c9ceb027 9c1cb0a7
	 */
			if(strcmp(str, "rnd")){DEBUG_S("DBG: 2b\r\n"); goto error;}
			sha256_hash_t seed = {0x1d, 0x93, 0x70, 0xcd, 0xcc, 0xba, 0x99, 0xb2, 0x36, 0x70,
					   			  0xe2, 0xe0, 0xd6, 0x51, 0x40, 0x01, 0x00, 0x6f, 0x50, 0xd3,
					     		  0xc7, 0xa4, 0x53, 0x20, 0x1d, 0x27, 0x76, 0xf0, 0x3c, 0x5e,
					  			  0x58, 0xfd  }; /*
				{ 0xf4, 0x1e, 0xce, 0x26,  0x13, 0xe4, 0x57, 0x39,  0x15, 0x69, 0x6b, 0x5a,  0xdc, 0xd5, 0x1c, 0xa3, 
				0x28, 0xbe, 0x3b, 0xf5,  0x66, 0xa9, 0xca, 0x99,  0xc9, 0xce, 0xb0, 0x27,  0x9c, 0x1c, 0xb0, 0xa7 };
			//	*/
			shavs_rnd(seed);
			goto restart;
			
			}
		if (!getnextwordn(str,20)) {DEBUG_S("DBG: W5\r\n"); goto error;}
		{	
			length=0;
			i=0;
			while (str[i]){	/* we should check for error here */
				length *= 10;
				length += str[i++] - '0';
			}
		};
//		DEBUG_S("\r\nDBG: Length="); DEBUG_B(length&0xff); DEBUG_S("\r\n");
//		DEBUG_S("A");
		sha256_init(&s);
//		DEBUG_S("B");
		if (!getnextwordn(str,20)) {DEBUG_S("DBG: W6\r\n"); goto error;}
//		DEBUG_S("b2");
		if (strcmp(str, "Msg=")) {DEBUG_S("DBG: 4b\r\n"); goto error;}	
//		DEBUG_S("b3");
		{
			memset(block, 0, SHA256_BLOCK_BITS/8);
//			DEBUG_S("b3.0");
			while (length>=SHA256_BLOCK_BITS){
				readhex2buffer(block, SHA256_BLOCK_BITS/8);
//			DEBUG_S("b3.1");
				sha256_nextBlock(&s, block); 
//			DEBUG_S("b3.2");
				length -= SHA256_BLOCK_BITS;
			}
//		DEBUG_S("C");	
			readhex2buffer(block, (length/8) + ((length&0x7)?1:0) + ((length)?0:1));
//		DEBUG_S("D");	
			sha256_lastBlock(&s, block, length);
//		DEBUG_S("E");	
			sha256_ctx2hash((void*)block, &s);
			uart_putstr("\n\rMD= ");
			uart_hexdump(block, SHA256_HASH_BITS/8);
			uart_putstr("\n\r\n\r");
		}		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	} /* while (1) */
}

