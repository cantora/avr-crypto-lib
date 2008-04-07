/*
 * HMAC-SHA256 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "sha256.h"
#include "hmac-sha256.h"

#include "nessie_mac_test.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "HMAC-SHA256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void hmacsha256_next_dummy(void* buffer, void* ctx){
	sha256_nextBlock(ctx, buffer);
}

void hmacsha256_init_dummy(void* key, uint16_t keysize_b, void* ctx){
	hmac_sha256_init(ctx, key, keysize_b);
}

void hmacsha256_last_dummy(void* buffer, uint16_t size_b, void* key, uint16_t keysize_b, void* ctx){
	sha256_lastBlock(ctx, buffer, size_b);
	hmac_sha256_final(ctx, key, keysize_b);
}

void testrun_nessie_hmacsha256(void){
	nessie_mac_ctx.macsize_b   = 256;
	nessie_mac_ctx.keysize_b   = 512;
	nessie_mac_ctx.blocksize_B = 512/8;
	nessie_mac_ctx.ctx_size_B  = sizeof(hmac_sha256_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = (nessie_mac_init_fpt)hmacsha256_init_dummy;
	nessie_mac_ctx.mac_next = (nessie_mac_next_fpt)hmacsha256_next_dummy;
	nessie_mac_ctx.mac_last = (nessie_mac_last_fpt)hmacsha256_last_dummy;
	nessie_mac_ctx.mac_conv = (nessie_mac_conv_fpt)sha256_ctx2hash;
	
	nessie_mac_run();
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
			testrun_nessie_hmacsha256();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}

