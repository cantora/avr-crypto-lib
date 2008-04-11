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
#include <avr/pgmspace.h>
#include <stdlib.h> /* utoa() */
#include "uart.h"

void nessie_print_block(uint8_t* block, uint16_t blocksize_bit){
	char tab [] = {'0', '1', '2', '3', 
				   '4', '5', '6', '7', 
				   '8', '9', 'A', 'B', 
				   'C', 'D', 'E', 'F'};
	uint16_t i;
	for(i=0; i<(blocksize_bit+7)/8; ++i){
		uart_putc(tab[(block[i])>>4]);
		uart_putc(tab[(block[i])&0xf]);
	}				   
}

#define SPACES 31
#define BYTESPERLINE 16

void nessie_print_item(char* name, uint8_t* buffer, uint16_t size_B){
	uint8_t name_len;
	uint8_t i;
	name_len=strlen(name);
	if(name_len>SPACES-1){
		uart_putstr_P(PSTR("\r\n!!! formatting error !!!\r\n"));
		return;
	}
	uart_putstr_P(PSTR("\r\n"));
	for(i=0; i<SPACES-name_len-1; ++i){
		uart_putc(' ');
	}
	uart_putstr(name);
	uart_putc('=');
	/* now the data printing begins */
	if(size_B<=BYTESPERLINE){
		/* one line seems sufficient */
		nessie_print_block(buffer, size_B*8);
	} else {
		/* we need more lines */
		nessie_print_block(buffer, BYTESPERLINE*8); /* first line */
		int16_t toprint = size_B - BYTESPERLINE;
		buffer += BYTESPERLINE;
		while(toprint > 0){
			uart_putstr_P(PSTR("\r\n"));
			for(i=0; i<SPACES; ++i){
				uart_putc(' ');
			}
			nessie_print_block(buffer, ((toprint>BYTESPERLINE)?BYTESPERLINE:toprint)*8);
			buffer  += BYTESPERLINE;
			toprint -= BYTESPERLINE;
		}
	}
} 


void nessie_print_set_vector(uint8_t set, uint16_t vector){
	uart_putstr_P(PSTR("\r\n\r\nSet "));
	uart_putc('0'+set%10);
	uart_putstr_P(PSTR(", vector#"));
	uart_putc((vector<100)?' ':'0'+vector/100);
	uart_putc((vector<10 )?' ':'0'+(vector/10)%10);
	uart_putc('0'+vector%10);
	uart_putc(':');
}

/* example:
Test vectors -- set 3
=====================
 */ 
void nessie_print_setheader(uint8_t set){
	uart_putstr_P(PSTR("\r\n\r\nTest vectors -- set "));
	uart_putc('0'+set%10);
	uart_putstr_P(PSTR("\r\n====================="));
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

void nessie_print_header(char* name,
                         uint16_t keysize_b, 
                         uint16_t blocksize_b,
                         uint16_t hashsize_b, 
                         uint16_t macsize_b,
                         uint16_t ivsize_b ){
	uint16_t i;
	uart_putstr_P(PSTR("\r\n\r\n"
	"********************************************************************************\r\n"
	"* micro-cryt - crypto primitives for microcontrolles by Daniel Otte            *\r\n"
	"********************************************************************************\r\n"
	"\r\n"));
	uart_putstr_P(PSTR("Primitive Name: "));
	uart_putstr(name);
	uart_putstr_P(PSTR("\r\n"));
	/* underline */	
	for(i=0; i<16+strlen(name); ++i){
		uart_putc('=');
	}
	char str[6]; /* must catch numbers up to 65535 + terminatin \0 */
	if(keysize_b){
		uart_putstr_P(PSTR("\r\nKey size: "));
		utoa(keysize_b, str, 10);
		uart_putstr(str);
		uart_putstr_P(PSTR(" bits"));
	}
	if(blocksize_b){
		uart_putstr_P(PSTR("\r\nBlock size: "));
		utoa(blocksize_b, str, 10);
		uart_putstr(str);
		uart_putstr_P(PSTR(" bits"));
	}
	if(hashsize_b){
		uart_putstr_P(PSTR("\r\nHash size: "));
		utoa(hashsize_b, str, 10);
		uart_putstr(str);
		uart_putstr_P(PSTR(" bits"));
	}
	if(macsize_b){
		uart_putstr_P(PSTR("\r\nMac size: "));
		utoa(macsize_b, str, 10);
		uart_putstr(str);
		uart_putstr_P(PSTR(" bits"));
	}
	if(ivsize_b){
		if(ivsize_b==(uint16_t)-1){
			uart_putstr_P(PSTR("\r\nNo initial value (IV) mode"));
		}
		{
			uart_putstr_P(PSTR("\r\nIV size: "));
			utoa(ivsize_b, str, 10);
			uart_putstr(str);
			uart_putstr_P(PSTR(" bits"));
		}
	}
	uart_putstr_P(PSTR("\r\n"));
}

void nessie_print_footer(void){
	uart_putstr_P(PSTR("\r\n\r\n\r\n\r\nEnd of test vectors\r\n\r\n"));
}

