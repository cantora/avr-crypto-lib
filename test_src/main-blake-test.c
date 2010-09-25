/* main-blake-test.c */
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
 * blake test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "blake_small.h"
#include "blake_large.h"
#include "hfal/hfal_blake_small.h"
#include "hfal/hfal_blake_large.h"
#include "hfal/hfal-nessie.h"
#include "hfal/hfal-test.h"
#include "hfal/hfal-performance.h"
#include "shavs.h"
#include "cli.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Blake";


const hfdesc_t* algolist[] PROGMEM = {
	(hfdesc_t*)&blake28_desc,
	(hfdesc_t*)&blake32_desc,
	(hfdesc_t*)&blake48_desc,
	(hfdesc_t*)&blake64_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_blake(void){
	hfal_nessie_multiple(algolist);
}
void blake28_test(void* msg, uint32_t length_b){
	hfal_test(&blake28_desc, msg, length_b);
}

void blake32_test(void* msg, uint32_t length_b){
	hfal_test(&blake32_desc, msg, length_b);
}

void blake48_test(void* msg, uint32_t length_b){
	hfal_test(&blake48_desc, msg, length_b);
}

void blake64_test(void* msg, uint32_t length_b){
	hfal_test(&blake64_desc, msg, length_b);
}
void testrun_stdtest_blake(void){
	uint8_t msg1[144];
	memset(msg1, 0, 144);
	blake28_test("", 8);
	blake28_test(msg1, 576);
	blake32_test("", 8);
	blake32_test(msg1, 576);
	blake48_test("", 8);
	blake48_test(msg1, 1152);
	blake64_test("", 8);
	blake64_test(msg1, 1152);
}

void testshort(void){
	blake32_test("", 8);
}

void testlshort(void){
	blake64_test("", 8);
}

void test512_32(void){
  uint8_t d[] = { 0xE9, 0x26, 0xAE, 0x8B, 0x0A, 0xF6, 0xE5, 0x31,
				 0x76, 0xDB, 0xFF, 0xCC, 0x2A, 0x6B, 0x88, 0xC6,
				 0xBD, 0x76, 0x5F, 0x93, 0x9D, 0x3D, 0x17, 0x8A,
				 0x9B, 0xDE, 0x9E, 0xF3, 0xAA, 0x13, 0x1C, 0x61,
				 0xE3, 0x1C, 0x1E, 0x42, 0xCD, 0xFA, 0xF4, 0xB4,
				 0xDC, 0xDE, 0x57, 0x9A, 0x37, 0xE1, 0x50, 0xEF,
				 0xBE, 0xF5, 0x55, 0x5B, 0x4C, 0x1C, 0xB4, 0x04,
				 0x39, 0xD8, 0x35, 0xA7, 0x24, 0xE2, 0xFA, 0xE7 };
	blake32_test(d, 512);
}

void performance_blake(void){
	hfal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/


const char nessie_str[]       PROGMEM = "nessie";
const char test_str[]         PROGMEM = "test";
const char testshort_str[]    PROGMEM = "short";
const char testlshort_str[]   PROGMEM = "lshort";
const char test512_str[]      PROGMEM = "test512";
const char performance_str[]  PROGMEM = "performance";
const char echo_str[]         PROGMEM = "echo";
const char shavs_list_str[]   PROGMEM = "shavs_list";
const char shavs_set_str[]    PROGMEM = "shavs_set";
const char shavs_test1_str[]  PROGMEM = "shavs_test1";
const char shavs_test3_str[]  PROGMEM = "shavs_test3";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_blake},
	{ test_str,            NULL, testrun_stdtest_blake},
	{ testshort_str,       NULL, testshort},
	{ testlshort_str,      NULL, testlshort},
	{ test512_str,         NULL, test512_32},
	{ performance_str,     NULL, performance_blake},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ shavs_test3_str,     NULL, shavs_test3},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&blake32_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));

		cmd_interface(cmdlist);
	}
}
