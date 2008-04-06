/*
 * SHA-256 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "sha256.h"
#include "nessie_hash_test.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "SHA-256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void sha256_next_dummy(void* buffer, void* ctx){
	sha256_nextBlock(ctx, buffer);
}

void sha256_last_dummy(void* buffer, uint16_t size_b, void* ctx){
	sha256_lastBlock(ctx, buffer, size_b);
}

void testrun_nessie_sha256(void){
	nessie_hash_ctx.hashsize_b  = 256;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(sha256_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)sha256_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)sha256_next_dummy;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)sha256_last_dummy;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)sha256_ctx2hash;
	
	nessie_hash_run();
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(algo_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "nessie")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_nessie_sha256();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}

