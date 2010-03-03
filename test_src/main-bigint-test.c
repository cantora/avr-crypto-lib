/* main-base64-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * base64 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "bigint.h"
#include "bigint_io.h"

#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "BigInt";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void test_echo_bigint(void){
	bigint_t a;
	cli_putstr_P(PSTR("\r\necho test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter hex number:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end echo test"));
			return;
		}
		cli_putstr_P(PSTR("\r\necho: "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
	}
}

void test_add_bigint(void){
	bigint_t a, b, c;
	cli_putstr_P(PSTR("\r\nadd test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end add test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end add test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" + "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" = "));
		uint8_t *c_b;
		c_b = malloc(((a.length_B>b.length_B)?a.length_B:b.length_B)+2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_add_s(&c, &a, &b);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c_b);
	}
}

void test_mul_bigint(void){
	bigint_t a, b, c;
	cli_putstr_P(PSTR("\r\nmul test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end mul test"));
			return;
		}
		cli_putstr_P(PSTR("\r\nenter b:"));
		if(bigint_read_hex_echo(&b)){
			free(a.wordv);
			cli_putstr_P(PSTR("\r\n end mul test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR(" * "));
		bigint_print_hex(&b);
		cli_putstr_P(PSTR(" = "));
		uint8_t *c_b;
		c_b = malloc((((a.length_B>b.length_B)?a.length_B:b.length_B)+1)*2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			free(b.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_mul_s(&c, &a, &b);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(b.wordv);
		free(c_b);
	}
}

void test_square_bigint(void){
	bigint_t a, c;
	cli_putstr_P(PSTR("\r\nsquare test\r\n"));
	for(;;){
		cli_putstr_P(PSTR("\r\nenter a:"));
		if(bigint_read_hex_echo(&a)){
			cli_putstr_P(PSTR("\r\n end square test"));
			return;
		}
		cli_putstr_P(PSTR("\r\n "));
		bigint_print_hex(&a);
		cli_putstr_P(PSTR("**2 = "));
		uint8_t *c_b;
		c_b = malloc(a.length_B*2);
		if(c_b==NULL){
			cli_putstr_P(PSTR("\n\rERROR: Out of memory!"));
			free(a.wordv);
			continue;
		}
		c.wordv = c_b;
		bigint_square(&c, &a);
		bigint_print_hex(&c);
		cli_putstr_P(PSTR("\r\n"));
		free(a.wordv);
		free(c_b);
	}
}

void test_simple(void){
	bigint_t a, b, c;
	uint8_t a_b[1], b_b[1], c_b[2];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 1;
	b.length_B = 1;
	a_b[0] = 1;
	b_b[0] = 2;
	bigint_add_u(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n 1+2="));
	bigint_print_hex(&c);
}
/*
void test_mul_simple(void){
	bigint_t a, b, c;
	uint8_t a_b[5] = {0x79, 0x36, 0x9e, 0x72, 0xec};
	uint8_t b_b[5] = {0x4a, 0x47, 0x0d, 0xec, 0xfd};
	uint8_t c_b[12];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 5;
	b.length_B = 5;
	bigint_adjust(&a);
	bigint_adjust(&b);
	bigint_mul_s(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&c);
}
*/

// -3d1d 6db7 8251 f371 * -7a18 3791 d18b b7c5 = 1d25ce4fdf93390f8d6c709f4d711cf5
// -20538248dece6d29068d * 400b1411b874f81394c6 = -81646b193d95136a6fedb73cee6d30c39fb950e
// -BC8B 7D53 4921 853D * 0DDA 6044 00CE DDE6   =  -a33eb0c5847db8837589c22db395dce
void test_mul_simple(void){
	bigint_t a, b, c;

//	uint8_t a_b[10] = {0x8d, 0x06, 0x29, 0x6d, 0xce, 0xde, 0x48, 0x82, 0x53, 0x20};
//	uint8_t b_b[10] = {0xc6, 0x94, 0x13, 0xf8, 0x74, 0xb8, 0x11, 0x14, 0x0b, 0x40};
	uint8_t a_b[8] = {0x3d, 0x85, 0x21, 0x49, 0x53, 0x7d, 0x8b, 0xbc};
	uint8_t b_b[8] = {0xe6, 0xdd, 0xce, 0x00, 0x44, 0x60, 0xda, 0x0d};

	uint8_t c_b[16];
	a.wordv=a_b;
	b.wordv=b_b;
	c.wordv=c_b;
	a.length_B = 8;
	b.length_B = 8;
	a.info=0x80;
	bigint_adjust(&a);
	bigint_adjust(&b);
	bigint_mul_s(&c, &a, &b);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR(" * "));
	bigint_print_hex(&b);
	cli_putstr_P(PSTR(" = "));
	bigint_print_hex(&c);
}

// f4 b86a 2220 0774 437d 70e6 **2 = e9f00f29ca1c876a7a682bd1e04f6925caffd6660ea4
void test_square_simple(void){
	bigint_t a, c;

	uint8_t a_b[11] = {0xe6, 0x70, 0x7d, 0x43, 0x74, 0x07, 0x20, 0x22, 0x6a, 0xb8, 0xf4};
	uint8_t c_b[22];
	a.wordv=a_b;
	c.wordv=c_b;
	a.length_B = 11;
	a.info=0x00;
	bigint_adjust(&a);
	bigint_square(&c, &a);
	cli_putstr_P(PSTR("\r\n test: "));
	bigint_print_hex(&a);
	cli_putstr_P(PSTR("**2 = "));
	bigint_print_hex(&c);
}


void testrun_performance_bigint(void){

}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char echo_test_str[]        PROGMEM = "echo-test";
const char add_test_str[]         PROGMEM = "add-test";
const char mul_test_str[]         PROGMEM = "mul-test";
const char square_test_str[]      PROGMEM = "square-test";
const char quick_test_str[]       PROGMEM = "quick-test";
const char performance_str[]      PROGMEM = "performance";
const char echo_str[]             PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ add_test_str,         NULL, test_add_bigint               },
	{ mul_test_str,         NULL, test_mul_bigint               },
	{ square_test_str,      NULL, test_square_bigint            },
	{ quick_test_str,       NULL, test_mul_simple               },
	{ echo_test_str,        NULL, test_echo_bigint              },
	{ performance_str,      NULL, testrun_performance_bigint    },
	{ echo_str,         (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                 NULL, NULL                          }
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
