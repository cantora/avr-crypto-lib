/* main-camellia-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * camellia test-suit
 * 
 */

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "camellia.h"
#include "nessie_bc_test.h"
#include "performance_test.h"
#include "cli.h"

char* algo_name = "Camellia";


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void camellia128_init_dummy(void* key, uint16_t keysize_b, void* ctx){
	camellia128_init(key, ctx);
}

void testrun_nessie_camellia(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(camellia128_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)camellia128_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)camellia128_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)camellia128_init_dummy;
	
	nessie_bc_run();
}


void test_performance_camellia(void){
	uint64_t t;
	char str[6];
	uint8_t key[16], data[16];
	camellia128_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	camellia128_init(key, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	camellia128_enc(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	camellia128_dec(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}



/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/
/*
128-bit key
key         01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
plaintext   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
ciphertext  67 67 31 38 54 96 69 73 08 57 06 56 48 ea be 43
*/
void testrun_camellia(void){

  uint8_t data[16] = { 0x01, 0x23, 0x45, 0x67, 
                       0x89, 0xab, 0xcd, 0xef, 
                       0xfe, 0xdc, 0xba, 0x98, 
                       0x76, 0x54, 0x32, 0x10 };

  uint8_t key[16] = { 0x01, 0x23, 0x45, 0x67, 
                       0x89, 0xab, 0xcd, 0xef, 
                       0xfe, 0xdc, 0xba, 0x98, 
                       0x76, 0x54, 0x32, 0x10 };


  camellia128_ctx_t ctx;
  camellia128_init(key, &ctx);
  uart_putstr_P(PSTR("\r\n key:        "));
  uart_hexdump(data, 16);
  uart_putstr_P(PSTR("\r\n plaintext:  "));
  uart_hexdump(data, 16);
  camellia128_enc(data, &ctx);
  uart_putstr_P(PSTR("\r\n ciphertext: "));
  uart_hexdump(data, 16);
  camellia128_dec(data, &ctx);
  uart_putstr_P(PSTR("\r\n decrypted:  "));
  uart_hexdump(data, 16);

}


/*****************************************************************************
 * main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_camellia },
	{ test_str,        NULL, testrun_camellia},
	{ performance_str, NULL, test_performance_camellia},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	uart_putstr("\r\n");
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	for(;;){
		uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		uart_putstr(algo_name);
		uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}

