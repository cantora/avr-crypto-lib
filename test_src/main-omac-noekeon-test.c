/* main-omac-noekeon-test.c */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
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
 * OMAC-Noekeon test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "noekeon.h"
#include "omac_noekeon.h"

#include "cli.h"
#include "nessie_mac_test.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "OMAC-Noekeon";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void test_mac(void* key, void* data, uint16_t datalength_b){
	uint8_t mac[16];
	uart_putstr_P(PSTR("\r\n-----------\r\n msg length (bit): 0x"));
	uart_hexdump(((uint8_t*)&datalength_b)+1, 1);
	uart_hexdump(((uint8_t*)&datalength_b)+0, 1);	
	uart_putstr_P(PSTR("\r\n msg: "));
	uart_hexdump(data, (datalength_b+7)/8);
	uart_putstr_P(PSTR("\r\n key: "));
	uart_hexdump(key, 16);
	omac_noekeon(mac, data, datalength_b, key, (uint8_t)-1);
	uart_putstr_P(PSTR("\r\n mac: "));
	uart_hexdump(mac, 16);
	
}

void testrun_test_noekeonomac(void){
	uint8_t key[16], data[64];
	uint16_t i;
	memset(key,  0xAA, 16);
	memset(data, 0x00, 64);
	for(i=1;i<64*8; ++i){
		test_mac(key, data, i);
	}
}

/******************************************************************************/

uint8_t stat_key[16];

void noekeonomac_next_dummy(void* buffer, void* ctx){
	omac_noekeon_next(buffer, stat_key, ctx);
}

void noekeonomac_init_dummy(void* key, uint16_t keysize_b, void* ctx){
	omac_noekeon_init(ctx);
	memcpy(stat_key, key, 16);
}

void noekeonomac_last_dummy(void* buffer, uint16_t size_b, void* key, uint16_t keysize_b, void* ctx){
	while(size_b>128){
		omac_noekeon_next(buffer, key, ctx);
		size_b -= 128;
		buffer = (uint8_t*)buffer +16;
	}
	omac_noekeon_last(buffer, size_b, key, ctx);
}

void noekeonomac_conv_dummy(void* buffer, void* ctx){
	memcpy(buffer, ctx, 16);
}

void testrun_nessie_noekeonomac(void){
	nessie_mac_ctx.macsize_b   = 128;
	nessie_mac_ctx.keysize_b   = 128;
	nessie_mac_ctx.blocksize_B = 16;
	nessie_mac_ctx.ctx_size_B  = sizeof(noekeon_omac_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = noekeonomac_init_dummy;
	nessie_mac_ctx.mac_next = noekeonomac_next_dummy;
	nessie_mac_ctx.mac_last = noekeonomac_last_dummy;
	nessie_mac_ctx.mac_conv = noekeonomac_conv_dummy;
	
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

	PGM_P    u   = PSTR("nessie\0test\0");
	void_fpt v[] = {testrun_nessie_noekeonomac, testrun_test_noekeonomac};

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

