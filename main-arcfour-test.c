/*
 * arcfour (RC4 compatible) test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "arcfour.h"
#include "nessie_stream_test.h"

#include <stdint.h>
#include <string.h>

char* cipher_name = "Arcfour";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void arcfour_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	arcfour_init(ctx, key, (keysize+7)/8);
}



void testrun_nessie_arcfour(void){
	nessie_stream_ctx.outsize_b = 8; /* actually unused */
	nessie_stream_ctx.keysize_b = 128; /* this is theone we have refrence vectors for */
	nessie_stream_ctx.ivsize_b = (uint16_t)-1;
	nessie_stream_ctx.name = cipher_name;
	nessie_stream_ctx.ctx_size_B = sizeof(arcfour_ctx_t);
	nessie_stream_ctx.cipher_genctx = (nessie_stream_genctx_fpt)arcfour_genctx_dummy;
	nessie_stream_ctx.cipher_enc = (nessie_stream_genenc_fpt)arcfour_gen;
	
	nessie_stream_run();	
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
			testrun_nessie_arcfour();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
	
}

