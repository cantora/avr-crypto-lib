/*
 * rc5 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "rc5.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define RC5_ROUNDS 12
char* cipher_name = "RC5-32/12/16";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void rc5_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	rc5_init(key, keysize_b, RC5_ROUNDS, ctx);
}

void testrun_nessie_rc5(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(rc5_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)rc5_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)rc5_dec;
	nessie_bc_ctx.cipher_free = (nessie_bc_free_fpt)rc5_free;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)rc5_genctx_dummy;
	
	nessie_bc_run();
}


void testrun_performance_rc5(void){
	uint16_t i,c;
	uint64_t t;
	char str[16];
	uint8_t key[16], data[16];
	rc5_ctx_t ctx;
	
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
	rc5_init(key, 128, RC5_ROUNDS, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	startTimer(1);
	rc5_enc(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	startTimer(1);
	rc5_dec(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);

	startTimer(1);
	rc5_free(&ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tfree time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	uart_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

typedef void(*void_fpt)(void);

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_rc5, testrun_nessie_rc5, testrun_performance_rc5};

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

