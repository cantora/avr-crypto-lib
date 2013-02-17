/* nessie_common.c */
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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * common function for nessie-tests
 * 
 * */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "nessie_common.h"
#include "hexdigit_tab.h"

#define nessie_out_file stdout

void nessie_set_output_stream(FILE *out_stream){
    nessie_out_file = out_stream;
}


#ifdef NESSIE_ALIVE
void nessie_send_alive(void){
	putc(NESSIE_ALIVE_CHAR, nessie_out_file);
}

void nessie_send_alive_a(uint16_t i){
	if((i&31)==0)
		putc(NESSIE_ALIVE_CHAR, nessie_out_file);
}
#endif

void nessie_print_block(uint8_t *block, uint16_t blocksize_bit){
	uint16_t i;
	for(i=0; i<(blocksize_bit+7)/8; ++i){
		putc(pgm_read_byte(hexdigit_tab_uc_P + ((block[i]) >>   4)), nessie_out_file);
		putc(pgm_read_byte(hexdigit_tab_uc_P + ((block[i]) &  0xf)), nessie_out_file);
	}				   
}

#define SPACES 31
#define BYTESPERLINE 16

void nessie_print_item(const char *name, uint8_t *buffer, uint16_t size_B){
	uint8_t name_len;
	uint8_t i;
	name_len = strlen(name);
	if(name_len > SPACES - 1){
		fputs_P(PSTR("\n!!! formatting error !!!\n"), nessie_out_file);
		return;
	}
	putc('\n', nessie_out_file);
	for(i = 0; i < SPACES-name_len - 1; ++i){
		putc(' ', nessie_out_file);
	}
	fputs(name, stdout);
	putc('=', nessie_out_file);
	/* now the data printing begins */
	if(size_B <= BYTESPERLINE){
		/* one line seems sufficient */
		nessie_print_block(buffer, size_B * 8);
	} else {
		/* we need more lines */
		nessie_print_block(buffer, BYTESPERLINE * 8); /* first line */
		int16_t toprint = size_B - BYTESPERLINE;
		buffer += BYTESPERLINE;
		while(toprint > 0){
			putc('\n', nessie_out_file);
			for(i = 0; i < SPACES; ++i){
				putc(' ', nessie_out_file);
			}
			nessie_print_block(buffer, ((toprint > BYTESPERLINE) ? BYTESPERLINE : toprint) * 8);
			buffer  += BYTESPERLINE;
			toprint -= BYTESPERLINE;
		}
	}
} 


void nessie_print_set_vector(uint8_t set, uint16_t vector){
	fprintf_P(nessie_out_file, PSTR("\n\nSet %"PRIu8", vector#%4"PRIu16":"), set, vector);
}

/* example:
Test vectors -- set 3
=====================
 */ 
void nessie_print_setheader(uint8_t set){
	fprintf_P(nessie_out_file, PSTR("\r\n\r\nTest vectors -- set %"PRIu8"\n====================="), set);
}

/* example:
********************************************************************************
*Project NESSIE - New European Schemes for Signature, Integrity, and Encryption*
********************************************************************************

Primitive Name: Serpent
=======================
Key size: 256 bits
Block size: 128 bits
*/

void nessie_print_header(const char *name,
                         uint16_t keysize_b, 
                         uint16_t blocksize_b,
                         uint16_t hashsize_b, 
                         uint16_t macsize_b,
                         uint16_t ivsize_b ){
	uint16_t i;
	fputs_P(PSTR("\n\n"
	"********************************************************************************\n"
	"* AVR-Crypto-Lib - crypto primitives for AVR microcontrollers by Daniel Otte   *\n"
	"********************************************************************************\n\n"),
	nessie_out_file);
	fprintf_P(nessie_out_file, PSTR("Primitive Name: %s\n"), name);
	/* underline */	
	for(i=0; i<16+strlen(name); ++i){
		putc('=', nessie_out_file);
	}
	if(keysize_b){
		fprintf_P(nessie_out_file, PSTR("\nKey size: %"PRIu16" bits"), keysize_b);
	}
	if(blocksize_b){
        fprintf_P(nessie_out_file, PSTR("\nBlock size: %"PRIu16" bits"), blocksize_b);
	}
	if(hashsize_b){
	    fprintf_P(nessie_out_file, PSTR("\nHash size: %"PRIu16" bits"), hashsize_b);

	}
	if(macsize_b){
		fprintf_P(nessie_out_file, PSTR("\nMac size: %"PRIu16" bits"), macsize_b);
	}
	if(ivsize_b){
		if(ivsize_b==(uint16_t)-1){
			fputs_P(PSTR("\nNo initial value (IV) mode"), stdout);
		}else{
		    fprintf_P(nessie_out_file, PSTR("\nIV size: %"PRIu16" bits"), ivsize_b);
		}
	}
	putc('\n', nessie_out_file);
}

void nessie_print_footer(void){
	puts_P(PSTR("\n\n\n\nEnd of test vectors\n\n"));
}

