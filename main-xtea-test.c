/*
 * XTEA test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "xtea.h"
#include "nessie_bc_test.h"

#include <stdint.h>
#include <string.h>

char* cipher_name = "XTEA";

void xtea_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, (keysize+7)/8);
}

void xtea_enc_dummy(uint8_t* buffer, void* ctx){
	xtea_enc((uint32_t*)buffer, (uint32_t*)buffer, ctx);
}

void xtea_dec_dummy(uint8_t* buffer, void* ctx){
	xtea_dec((uint32_t*)buffer, (uint32_t*)buffer, ctx);
}

void testrun_xtea(void){
	nessie_ctx.blocksize_B =   8;
	nessie_ctx.keysize     = 128;
	nessie_ctx.name        = cipher_name;
	nessie_ctx.ctx_size_B  = 128/8;
	nessie_ctx.cipher_enc  = (nessie_enc_fpt)xtea_enc_dummy;
	nessie_ctx.cipher_dec  = (nessie_dec_fpt)xtea_dec_dummy;
	nessie_ctx.cipher_genctx  = (nessie_gen_fpt)xtea_genctx_dummy;
	
	nessie_run();	
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

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "nessie")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_xtea();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}	
}
