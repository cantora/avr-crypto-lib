/*
 * rc6 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "cast6.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

char* algo_name = "CAST-256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/


void testrun_nessie_cast6(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(cast6_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)cast6_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)cast6_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)cast6_init;
	
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 256;
	nessie_bc_run();
}

void testrun_rfc_cast6(void){
	cli_putstr_P(PSTR("\r\n testvalues from rfc-2612\r\n"));
	uint8_t key[32], data[16];
	cast6_ctx_t ctx;
	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\x0a\xf7\x56\x47\xf2\x9f\x61\x5d"), 16);
	memset(data, 0, 16);
	
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 128, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));

	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\xbe\xd0\xac\x83\x94\x0a\xc2\x98"
				       "\xba\xc7\x7a\x77\x17\x94\x28\x63"), 24);
	
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 24);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 192, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));
	
	memcpy_P(key, PSTR("\x23\x42\xbb\x9e\xfa\x38\x54\x2c"
	                   "\xbe\xd0\xac\x83\x94\x0a\xc2\x98"
				       "\x8d\x7c\x47\xce\x26\x49\x08\x46"
				       "\x1c\xc1\xb5\x13\x7a\xe6\xb6\x04"), 32);
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 32);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	cast6_init(key, 256, &ctx);
	cast6_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n CT:  "));
	cli_hexdump(data, 16);
	cast6_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n PT:  "));
	cli_hexdump(data, 16);
	
	cli_putstr_P(PSTR("\r\n\r\n"));
}

void testrun_performance_cast6(void){
	uint64_t t;
	char str[16];
	uint8_t key[16], data[16];
	cast6_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	cast6_init(key, 128, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	cast6_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	startTimer(1);
	cast6_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	cli_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_cast6 },
	{ test_str,        NULL, testrun_rfc_cast6},
	{ performance_str, NULL, testrun_performance_cast6},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
