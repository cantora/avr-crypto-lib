/* main-sha256-test.c */
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
 * SHA-256 test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "sha256.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-performance.h"
#include "hfal-nessie.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "shavs.h"
#include "hfal_sha256.h"
#include "dump.h"

char* algo_name = "SHA-256";

const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&sha256_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_sha256(void){
	hfal_nessie_multiple(algolist);
}

void testrun_performance_sha256(void){
	hfal_performance_multiple(algolist);
}

void test_monte(void){
	uint8_t data1[] = {
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7 };

   uint8_t data2[] = {
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xFD, 0xDF, 0x1B, 0x37, 0xDD, 0x34, 0xB3, 0xB2,
     0x01, 0xD4, 0x3C, 0x57, 0xBC, 0xDE, 0x11, 0x58,
     0x38, 0xF0, 0xDF, 0x70, 0x1D, 0xA9, 0x3C, 0x3B,
     0xF2, 0xC9, 0xC8, 0x68, 0x96, 0xE7, 0xE6, 0xC7 };
   uint8_t hash[SHA256_HASH_BYTES];
   sha256(hash, data1, 3*32*8);
   cli_putstr_P(PSTR("\r\n hash(data1) = "));
   cli_hexdump(hash, 32);
   sha256(hash, data2, 3*32*8);
   cli_putstr_P(PSTR("\r\n hash(data2) = "));
   cli_hexdump(hash, 32);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char monte_str[]       PROGMEM = "monte";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char dump_str[]        PROGMEM = "dump";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_sha256},
	{ test_str,            NULL, testrun_nessie_sha256},
	{ monte_str,           NULL, test_monte},
	{ performance_str,     NULL, testrun_performance_sha256},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ shavs_test2_str,     NULL, shavs_test2},
	{ dump_str,        (void*)1, (void_fpt)dump},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&sha256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
