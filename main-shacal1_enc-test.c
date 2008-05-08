/*
 * Shacal1 encryption only test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "shacal1_enc.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* cipher_name = "Shacal1 encryption only";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void shacal1_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, (keysize_b+7)/8);
}

void shacal1_enc_dummy(void* buffer, void* ctx){
	shacal1_enc(buffer, ctx, 512);
}

void testrun_nessie_shacal1enc(void){
	nessie_bc_ctx.blocksize_B = SHACAL1_BLOCKSIZE_B;
	nessie_bc_ctx.keysize_b   = SHACAL1_KEYSIZE;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = SHACAL1_KEYSIZE_B;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)shacal1_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)NULL;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)shacal1_genctx_dummy;
	
	nessie_bc_run();	
}

void testrun_performance_shacal1enc(void){
	uint64_t t;
	uint8_t key[SHACAL1_KEYSIZE_B], data[SHACAL1_BLOCKSIZE_B];
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, SHACAL1_KEYSIZE_B);
	memset(data, 0, SHACAL1_BLOCKSIZE_B);
	
	
	startTimer(1);
	shacal1_enc(data, key, SHACAL1_KEYSIZE);
	t = stopTimer();
	print_time_P(PSTR("\tencrypt time: "), t);
	
	uart_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_shacal1enc, 
		            testrun_nessie_shacal1enc,
		            testrun_performance_shacal1enc};

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
