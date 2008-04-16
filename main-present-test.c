/*
 * present test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "present.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* cipher_name = "Present";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void present_genctx_dummy(uint8_t* key, uint16_t keysize_b, present_ctx_t* ctx){
	present_init(key, keysize_b, ctx);
}

void testrun_nessie_present(void){
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   =  80;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(present_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)present_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)present_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)present_genctx_dummy;
	
	nessie_bc_run();	
}

void testrun_selfenc(uint8_t* key, uint8_t* buffer){
	present_ctx_t ctx;
	uart_putstr_P(PSTR("\r\nkey   : "));
	uart_hexdump(key, 10);
	uart_putstr_P(PSTR("\r\nplain : "));
	uart_hexdump(buffer, 8);
	present_init(key, 80, &ctx);
	present_enc(buffer, &ctx);
	uart_putstr_P(PSTR("\r\ncipher: "));
	uart_hexdump(buffer, 8);
	present_dec(buffer, &ctx);
	uart_putstr_P(PSTR("\r\nplain : "));
	uart_hexdump(buffer, 8);
	uart_putstr_P(PSTR("\r\n"));
}

void testrun_self_present(void){
	uint8_t buffer[8], key[10];
	uart_putstr_P(PSTR("\r\n\r\n=== Testvectors from the paper ===\r\n"));
	
	memset(buffer, 0, 8);
	memset(key, 0, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0, 8);
	memset(key, 0xFF, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0xFF, 8);
	memset(key, 0, 10);
	testrun_selfenc(key, buffer);
	
	memset(buffer, 0xFF, 8);
	memset(key, 0xFF, 10);
	testrun_selfenc(key, buffer);
	
}

void testrun_performance_present(void){
	uint16_t i,c;
	uint64_t t;
	char str[16];
	uint8_t key[10], data[8];
	present_ctx_t ctx;
	
	calibrateTimer();
	getOverhead(&c, &i);
	uart_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(c, str, 10);
	uart_putstr_P(PSTR("\r\n\tconst overhead:     "));
	uart_putstr(str);
	utoa(i, str, 10);
	uart_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	uart_putstr(str);
	
	memset(key,  0, 10);
	memset(data, 0,  8);
	
	startTimer(1);
	present_init(key, 80, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	startTimer(1);
	present_enc(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	startTimer(1);
	present_dec(data, &ctx);
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
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_present, testrun_self_present, testrun_performance_present};

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
