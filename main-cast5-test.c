/*
 * cast5 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "cast5.h"
#include "nessie_bc_test.h"
#include "performance_test.h"
#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* cipher_name = "cast-128 (cast5)";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
/*
	void cast5_init(cast5_ctx_t* s, uint8_t* key, uint8_t keylength);
	void cast5_enc(cast5_ctx_t *s, void* block);
	void cast5_dec(cast5_ctx_t *s, void* block);
*/ 

void cast5_init_dummy(uint8_t* key, uint8_t keylength_b, cast5_ctx_t* ctx){
	cast5_init(ctx, key, keylength_b);
}

void cast5_enc_dummy(void* buffer, cast5_ctx_t* ctx){
	cast5_enc(ctx, buffer);
}

void cast5_dec_dummy(void* buffer, cast5_ctx_t* ctx){
	cast5_dec(ctx, buffer);
}

void test_nessie_cast5(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(cast5_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)cast5_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)cast5_dec_dummy;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)cast5_init_dummy;
	
	nessie_bc_run();
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void cast5_ctx_dump(cast5_ctx_t *s){
	uint8_t i;
	uart_putstr("\r\n==== cast5_ctx_dump ====\r\n shortkey: ");
	uart_putstr(s->shortkey?"yes":"no");
	for(i=0;i<16;++i){
		uint8_t r;
		uart_putstr("\r\n Km"); uart_hexdump(&i, 1); uart_putc(':');
		uart_hexdump(&(s->mask[i]), 4);
		uart_putstr("\r\n Kr"); uart_hexdump(&i, 1); uart_putc(':');
		r = (s->rotl[i/2]);
		if (i&0x01) r >>= 4;
		r &= 0xf;
		r += (s->roth[i>>3]&(1<<(i&0x7)))?0x10:0x00;
		uart_hexdump(&r, 1);
	}
}


void test_encrypt(uint8_t *block, uint8_t *key, uint8_t keylength, bool print){
	cast5_ctx_t s;
	if (print){
		uart_putstr("\r\nCAST5:\r\n key:\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 8);
	}
	cast5_init(&s, key, keylength);
//	cast5_ctx_dump(&s);
	cast5_enc(&s, block);
	if (print){
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 8);
	}
} 

void test_decrypt(uint8_t *block, uint8_t *key, uint8_t keylength, bool print){
	cast5_ctx_t s;
	if (print){
		uart_putstr("\r\nCAST5:\r\n key:\t");
		uart_hexdump(key, keylength/8);
		uart_putstr("\r\n ciphertext:\t");
		uart_hexdump(block, 8);
	}
	cast5_init(&s, key, keylength);
//	cast5_ctx_dump(&s);
	cast5_dec(&s, block);
	if (print){
		uart_putstr("\r\n plaintext:\t");
		uart_hexdump(block, 8);
	}
} 

void testrun_cast5(void){
	uint8_t block[8];
	uint8_t key[16];
	uint8_t *tda = (uint8_t*)"\x01\x23\x45\x67\x89\xAB\xCD\xEF",
	        *tka = (uint8_t*)"\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9A";
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 128, true);
	test_decrypt(block, key, 128, true);
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 80, true);
	test_decrypt(block, key, 80, true);
	memcpy(block, tda, 8);
	memcpy(key, tka, 16);
	test_encrypt(block, key, 40, true);
	test_decrypt(block, key, 40, true);
	
/**** long test *****/
	uart_putstr("\r\nmaintance-test");
	uint8_t a[16]= {0x01, 0x23, 0x45, 0x67, 0x12,
				    0x34, 0x56, 0x78, 0x23, 0x45, 
				    0x67, 0x89, 0x34, 0x56, 0x78, 
				    0x9A}, 
			b[16]= {0x01, 0x23, 0x45, 0x67, 0x12,
				    0x34, 0x56, 0x78, 0x23, 0x45, 
				    0x67, 0x89, 0x34, 0x56, 0x78, 
				    0x9A};
	uint32_t i;
	for(i=0;i<1000000; ++i){
		test_encrypt(&(a[0]), &(b[0]), 128, false);
		test_encrypt(&(a[8]), &(b[0]), 128, false);
		test_encrypt(&(b[0]), &(a[0]), 128, false);
		test_encrypt(&(b[8]), &(a[0]), 128, false);
		if ((i&0x000000ff) == 0){
			uart_putstr("\r\n");
			uart_hexdump(&i, 4);
		}
	}
	uart_putstr("\r\na = "); uart_hexdump(a, 16);
	uart_putstr("\r\nb = "); uart_hexdump(b, 16);

}

void test_performance_cast5(void){
	uint16_t i,c;
	uint64_t t;
	char str[6];
	uint8_t key[16], data[16];
	cast5_ctx_t ctx;
	
	calibrateTimer();
	getOverhead(&c, &i);
	uart_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(c, str, 10);
	uart_putstr_P(PSTR("\r\n\tconst overhead:     "));
	uart_putstr(str);
	utoa(i, str, 10);
	uart_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	uart_putstr(str);
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	cast5_init(&ctx, key, 128);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	cast5_enc(&ctx, data);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	cast5_dec(&ctx, data);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

typedef void(*void_fpt)(void);

int main (void){
	char str[20];

	
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {test_nessie_cast5, test_nessie_cast5, test_performance_cast5};
	
	while(1){ 
		if (!getnextwordn(str,20)){DEBUG_S("DBG: W1\r\n"); goto error;}
		if(execcommand_d0_P(str, u, v)<0){
			uart_putstr_P(PSTR("\r\nunknown command\r\n"));
		}
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
}

