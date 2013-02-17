/* main-hmac-sha1-test.c */
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
 * HMAC-SHA1 test-suit
 * 
*/

#include "main-test-common.h"

#include "sha1.h"
#include "hmac-sha1.h"

#include "nessie_mac_test.h"

char *algo_name = "HMAC-SHA1";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_hmacsha1(void){
	nessie_mac_ctx.macsize_b   = HMAC_SHA1_BITS;
	nessie_mac_ctx.keysize_b   = HMAC_SHA1_BLOCK_BITS;
	nessie_mac_ctx.blocksize_B = HMAC_SHA1_BLOCK_BYTES;
	nessie_mac_ctx.ctx_size_B  = sizeof(hmac_sha1_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = (nessie_mac_init_fpt)hmac_sha1_init;
	nessie_mac_ctx.mac_next = (nessie_mac_next_fpt)hmac_sha1_nextBlock;
	nessie_mac_ctx.mac_last = (nessie_mac_last_fpt)hmac_sha1_lastBlock;
	nessie_mac_ctx.mac_conv = (nessie_mac_conv_fpt)hmac_sha1_final;
	
	nessie_mac_run();
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
/* const char performance_str[] PROGMEM = "performance"; */
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_hmacsha1},
	{ test_str,        NULL, testrun_nessie_hmacsha1},
/*	{ performance_str, NULL, testrun_performance_hmacsha1}, */
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
    main_setup();

    for(;;){
        welcome_msg(algo_name);
        cmd_interface(cmdlist);
	}
}
