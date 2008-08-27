/* nessie_mac_test.c */
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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for MACs
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_mac_test.h"
#include "nessie_common.h"
#include "uart.h"

nessie_mac_ctx_t nessie_mac_ctx;

#define KEYSIZE_B ((nessie_mac_ctx.keysize_b+7)/8)
#define MACSIZE_B ((nessie_mac_ctx.macsize_b+7)/8)

#define PRINTKEY nessie_print_item("key", key, KEYSIZE_B)
#define PRINTMAC nessie_print_item("MAC", mac, MACSIZE_B)

static
void ascii_mac(char* data, char* desc, uint8_t* key){
	uint8_t ctx[nessie_mac_ctx.ctx_size_B];
	uint8_t mac[MACSIZE_B];
	uint16_t sl;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr(desc);
	PRINTKEY;
	nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b, ctx);
	sl = strlen(data);
	while(sl>nessie_mac_ctx.blocksize_B){
		nessie_mac_ctx.mac_next(data, ctx);
		data += nessie_mac_ctx.blocksize_B;
		sl   -= nessie_mac_ctx.blocksize_B;
	}
	nessie_mac_ctx.mac_last(data, sl*8, key, nessie_mac_ctx.keysize_b, ctx);
	nessie_mac_ctx.mac_conv(mac, ctx);
	PRINTMAC;
}

// message=1 million times "a"

static
void amillion_mac(uint8_t* key){
	uint8_t ctx[nessie_mac_ctx.ctx_size_B];
	uint8_t mac[MACSIZE_B];
	uint8_t block[nessie_mac_ctx.blocksize_B];
	uint32_t n=1000000LL;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr_P(PSTR("1 million times \"a\""));
	PRINTKEY;
	
	memset(block, 'a', nessie_mac_ctx.blocksize_B);
	nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b, ctx);
	while(n>nessie_mac_ctx.blocksize_B){
		nessie_mac_ctx.mac_next(block, ctx);
		n    -= nessie_mac_ctx.blocksize_B;
	}
	nessie_mac_ctx.mac_last(block, n*8, key, nessie_mac_ctx.keysize_b, ctx);
	nessie_mac_ctx.mac_conv(mac, ctx);
	PRINTMAC;
}


static
void zero_mac(uint16_t n, uint8_t* key){
	uint8_t ctx[nessie_mac_ctx.ctx_size_B];
	uint8_t mac[MACSIZE_B];
	uint8_t block[nessie_mac_ctx.blocksize_B];
	
	uart_putstr_P(PSTR("\r\n                       message="));
	if(n>=10000)
		uart_putc('0'+n/10000);
	if(n>=1000)
		uart_putc('0'+(n/1000)%10);
	if(n>=100)
		uart_putc('0'+(n/100)%10);
	if(n>=10)
		uart_putc('0'+(n/10)%10);
	uart_putc('0'+n%10);
	uart_putstr_P(PSTR(" zero bits"));
	PRINTKEY;
	
	memset(block, 0, nessie_mac_ctx.blocksize_B); 
	nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b,ctx);;
	while(n>nessie_mac_ctx.blocksize_B*8){
		nessie_mac_ctx.mac_next(block, ctx);
		n   -= nessie_mac_ctx.blocksize_B*8;
	}
	nessie_mac_ctx.mac_last(block, n, key, nessie_mac_ctx.keysize_b, ctx);
	nessie_mac_ctx.mac_conv(mac, ctx);
	PRINTMAC;
}

static
void one_in512_mac(uint16_t pos, uint8_t* key){
	uint8_t ctx[nessie_mac_ctx.ctx_size_B];
	uint8_t mac[MACSIZE_B];
	uint8_t block[nessie_mac_ctx.blocksize_B];
	uint16_t n=512;
	char* tab[8]={"80", "40", "20", "10", 
	              "08", "04", "02", "01" };

	pos&=511;
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr_P(PSTR("512-bit string: "));
	if((pos/8) >=10){
		uart_putc('0'+(pos/8/10)%10);
	} else {
		uart_putc(' ');
	}
	uart_putc('0'+(pos/8)%10);
	uart_putstr_P(PSTR("*00,"));
	uart_putstr(tab[pos&7]);
	uart_putc(',');
	if(63-(pos/8) >=10){
		uart_putc('0'+((63-pos/8)/10)%10);
	} else {
		uart_putc(' ');
	}
	uart_putc('0'+(63-pos/8)%10);
	uart_putstr_P(PSTR("*00"));
	PRINTKEY;
	
	/* now the real stuff */
	memset(block, 0, 512/8);
	block[pos>>3] = 0x80>>(pos&0x7);
	uint8_t* bp;
	bp = block;
	nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b, ctx);
	while(n>nessie_mac_ctx.blocksize_B*8){
		nessie_mac_ctx.mac_next(bp, ctx);
		n   -= nessie_mac_ctx.blocksize_B*8;
		bp  += nessie_mac_ctx.blocksize_B;
	}
	nessie_mac_ctx.mac_last(bp, n, key, nessie_mac_ctx.keysize_b, ctx);
	nessie_mac_ctx.mac_conv(mac, ctx);
	PRINTMAC;
}

static
void tv4_mac(uint8_t* key){
	uint8_t ctx[nessie_mac_ctx.ctx_size_B];
	uint8_t mac[MACSIZE_B];
	uint8_t block[256/8];
	uint16_t n=256;
	uint32_t i;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr(PSTR("256 zero bits"));
	memset(block, 0, 256/8);
	
	nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b, ctx);;
	while(n>nessie_mac_ctx.blocksize_B*8){
		nessie_mac_ctx.mac_next(block, ctx);
		n    -= nessie_mac_ctx.blocksize_B*8;
	}
	nessie_mac_ctx.mac_last(block, n, key, nessie_mac_ctx.keysize_b, ctx);
	nessie_mac_ctx.mac_conv(mac, ctx);
	PRINTMAC;
	for(i=1; i<100000L; ++i){ /* this assumes BLOCKSIZE >= HASHSIZE */
		nessie_mac_ctx.mac_init(key, nessie_mac_ctx.keysize_b, ctx);;
		nessie_mac_ctx.mac_last(mac, nessie_mac_ctx.macsize_b, key, nessie_mac_ctx.keysize_b, ctx);
		nessie_mac_ctx.mac_conv(mac, ctx);
	}
	nessie_print_item("iterated 100000 times", mac, MACSIZE_B);
}


void nessie_mac_run(void){
	uint16_t i;
	uint8_t set;
	uint8_t keyproto[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                          0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
                          0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
	uint8_t key[KEYSIZE_B];
	
	nessie_print_header(nessie_mac_ctx.name, nessie_mac_ctx.keysize_b, 0, 0,
	                    nessie_mac_ctx.macsize_b, 0);
	/* test set 1 */
	char* challange[10][2]= {
		{"", "\"\" (empty string)"},
		{"a", "\"a\""},
		{"abc", "\"abc\""},
		{"message digest", "\"message digest\""},
		{"abcdefghijklmnopqrstuvwxyz","\"abcdefghijklmnopqrstuvwxyz\""},
		{"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
			"\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\""},
		{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		 "abcdefghijklmnopqrstuvwxyz"
		 "0123456789"	, "\"A...Za...z0...9\""},
		{"1234567890" "1234567890" "1234567890" "1234567890" 
		 "1234567890" "1234567890" "1234567890" "1234567890",
		 "8 times \"1234567890\""},
		{"Now is the time for all ", "\"Now is the time for all \""},
		{"Now is the time for it", "\"Now is the time for it\""}
	};

	set=1;
	nessie_print_setheader(set);
	for(i=0; i<KEYSIZE_B; ++i){
		key[i] = keyproto[i%sizeof(keyproto)];
	}
	for(i=0; i<10; ++i){
		nessie_print_set_vector(set, i);
		ascii_mac(challange[i][0], challange[i][1], key);
	}
	nessie_print_set_vector(set, i);
	amillion_mac(key);
	for(i=0; i<KEYSIZE_B; ++i){
		key[i] = keyproto[16+i%8];
	}
	for(i=0; i<10; ++i){
		nessie_print_set_vector(set, 11+i);
		ascii_mac(challange[i][0], challange[i][1], key);
	}
	nessie_print_set_vector(set, 11+i);
	amillion_mac(key);
	/* test set 2 */
	set=2;
	for(i=0; i<KEYSIZE_B; ++i){
		key[i] = keyproto[i%sizeof(keyproto)];
	}
	nessie_print_setheader(set);
	for(i=0; i<1024; ++i){
		nessie_print_set_vector(set, i);
		zero_mac(i, key);
	}
	/* test set 3 */
	set=3;
	nessie_print_setheader(set);
	/* we use the same key as above */
	for(i=0; i<512; ++i){
		nessie_print_set_vector(set, i);
		one_in512_mac(i, key);
	}
	/* test set 4 */
	set=4;
	nessie_print_setheader(set);
	/* we use the same key as above */
	nessie_print_set_vector(set, 0);
	tv4_mac(key);
	/* test set 5 */
	for(i=0; i<nessie_mac_ctx.keysize_b; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, KEYSIZE_B);
		key[i>>3]=0x80>>(i&0x7);
		ascii_mac("ABC", "\"ABC\"", key);
	}
	nessie_print_footer();
}
