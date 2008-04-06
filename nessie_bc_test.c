/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for blockciphers
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_bc_test.h"
#include "uart.h"



nessie_ctx_t nessie_ctx;

static void printblock(uint8_t* block, uint16_t blocksize_bit){
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

static void printitem(char* name, uint8_t* buffer, uint16_t size_B){
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
		printblock(buffer, size_B*8);
	} else {
		/* we need more lines */
		printblock(buffer, BYTESPERLINE*8); /* first line */
		int16_t toprint = size_B - BYTESPERLINE;
		buffer += BYTESPERLINE;
		while(toprint > 0){
			uart_putstr_P(PSTR("\r\n"));
			for(i=0; i<SPACES; ++i){
				uart_putc(' ');
			}
			printblock(buffer, ((toprint>BYTESPERLINE)?BYTESPERLINE:toprint)*8);
			buffer  += BYTESPERLINE;
			toprint -= BYTESPERLINE;
		}
	}
} 

void nessie_enc(uint8_t* key, uint8_t* pt){
	uint8_t ctx[nessie_ctx.ctx_size_B];
	uint8_t buffer[nessie_ctx.blocksize_B];
	uint16_t i;
	
	/* single test */
	printitem("key", key, (nessie_ctx.keysize+7)/8);
	nessie_ctx.cipher_genctx(key, nessie_ctx.keysize, ctx);
	memcpy(buffer, pt, nessie_ctx.blocksize_B);
	printitem("plain", buffer, nessie_ctx.blocksize_B);
	nessie_ctx.cipher_enc(buffer, ctx);
	printitem("cipher", buffer, nessie_ctx.blocksize_B);
	nessie_ctx.cipher_dec(buffer, ctx);
	printitem("decrypted", buffer, nessie_ctx.blocksize_B);
	
	/* 100 times test */
	memcpy(buffer, pt, nessie_ctx.blocksize_B);
	for(i=0; i<100; ++i){
		nessie_ctx.cipher_enc(buffer, ctx);
	}
	printitem("Iterated 100 times", buffer, nessie_ctx.blocksize_B);
#ifndef NESSIE_NO1KTEST	
	/* 1000 times test, we use the 100 precedig steps to fasten things a bit */
	for(; i<1000; ++i){
		nessie_ctx.cipher_enc(buffer, ctx);
	}
	printitem("Iterated 1000 times", buffer, nessie_ctx.blocksize_B);
#endif
}

void nessie_dec(uint8_t* key, uint8_t* ct){
	uint8_t ctx[nessie_ctx.ctx_size_B];
	uint8_t buffer[nessie_ctx.blocksize_B];
	
	/* single test */
	printitem("key", key, (nessie_ctx.keysize+7)/8);
	nessie_ctx.cipher_genctx(key, nessie_ctx.keysize, ctx);
	memcpy(buffer, ct, nessie_ctx.blocksize_B);
	printitem("cipher", buffer, nessie_ctx.blocksize_B);
	nessie_ctx.cipher_dec(buffer, ctx);
	printitem("plain", buffer, nessie_ctx.blocksize_B);
	nessie_ctx.cipher_enc(buffer, ctx);
	printitem("encrypted", buffer, nessie_ctx.blocksize_B);
	
}

static void print_set_vector(uint8_t set, uint16_t vector){
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
static void print_setheader(uint8_t set){
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

static void print_header(void){
	uint16_t i;
	uart_putstr_P(PSTR("\r\n\r\n"
	"********************************************************************************\r\n"
	"* micro-cryt - crypto primitives for microcontrolles by Daniel Otte            *\r\n"
	"********************************************************************************\r\n"
	"\r\n"));
	uart_putstr_P(PSTR("Primitive Name: "));
	uart_putstr(nessie_ctx.name);
	uart_putstr_P(PSTR("\r\n"));
	for(i=0; i<16+strlen(nessie_ctx.name); ++i){
		uart_putc('=');
	}
	uart_putstr_P(PSTR("\r\nKey size: "));
	if(nessie_ctx.keysize>100){
		uart_putc('0'+nessie_ctx.keysize/100);
	}
	if(nessie_ctx.keysize>10){
		uart_putc('0'+(nessie_ctx.keysize/10)%10);
	}
	uart_putc('0'+nessie_ctx.keysize%10);
	uart_putstr_P(PSTR(" bits\r\nBlock size: "));
	if(nessie_ctx.blocksize_B*8>100){
		uart_putc('0'+(nessie_ctx.blocksize_B*8)/100);
	}
	if(nessie_ctx.blocksize_B*8>10){
		uart_putc('0'+((nessie_ctx.blocksize_B*8)/10)%10);
	}
	uart_putc('0'+(nessie_ctx.blocksize_B*8)%10);
	uart_putstr_P(PSTR(" bits"));
}

static void print_footer(void){
	uart_putstr_P(PSTR("\r\n\r\n\r\n\r\nEnd of test vectors\r\n\r\n"));
}

void nessie_run(void){
	uint16_t i;
	uint8_t set;
	uint8_t key[(nessie_ctx.keysize+7)/8];
	uint8_t buffer[nessie_ctx.blocksize_B];
	
	print_header();
	/* test set 1 */
	set=1;
	print_setheader(set);
	for(i=0; i<nessie_ctx.keysize; ++i){
		print_set_vector(set, i);
		memset(key, 0, (nessie_ctx.keysize+7)/8);
		key[i/8] |= 0x80>>(i%8);
		memset(buffer, 0, nessie_ctx.blocksize_B);
		nessie_enc(key, buffer);
	}
	/* test set 2 */
	set=2;
	print_setheader(set);
	for(i=0; i<nessie_ctx.blocksize_B*8; ++i){
		print_set_vector(set, i);
		memset(key, 0, (nessie_ctx.keysize+7)/8);
		memset(buffer, 0, nessie_ctx.blocksize_B);
		buffer[i/8] |= 0x80>>(i%8);
		nessie_enc(key, buffer);
	}
	/* test set 3 */
	set=3;
	print_setheader(set);
	for(i=0; i<256; ++i){
		print_set_vector(set, i);
		memset(key, i, (nessie_ctx.keysize+7)/8);
		memset(buffer, i, nessie_ctx.blocksize_B);
		nessie_enc(key, buffer);
	}
	/* test set 4 */
	set=4;
	print_setheader(set);
	/* 4 - 0*/
	print_set_vector(set, 0);
	for(i=0; i<(nessie_ctx.keysize+7)/8; ++i){
		key[i]=i;
	}
	for(i=0; i<nessie_ctx.blocksize_B; ++i){
		buffer[i]=i*0x11;
	}
	nessie_enc(key, buffer);
	/* 4 - 1 */
	print_set_vector(set, 1);	
    /* This is the test vectors in Kasumi */
    static uint8_t kasumi_key[] = {
           0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00,
           0x95, 0x2C, 0x49, 0x10, 0x48, 0x81, 0xFF, 0x48 };
    static uint8_t kasumi_plain[]={
           0xEA, 0x02, 0x47, 0x14, 0xAD, 0x5C, 0x4D, 0x84 };
	for(i=0; i<(nessie_ctx.keysize+7)/8; ++i){
		key[i]=kasumi_key[i%sizeof(kasumi_key)];
	}
	for(i=0; i<nessie_ctx.blocksize_B; ++i){
		buffer[i]=kasumi_plain[i%sizeof(kasumi_plain)];
	}
	/* half done ;-) */
	/* test set 5 */
	set=1;
	print_setheader(set);
	for(i=0; i<nessie_ctx.keysize; ++i){
		print_set_vector(set, i);
		memset(key, 0, (nessie_ctx.keysize+7)/8);
		key[i/8] |= 0x80>>(i%8);
		memset(buffer, 0, nessie_ctx.blocksize_B);
		nessie_dec(key, buffer);
	}
	/* test set 6 */
	set=6;
	print_setheader(set);
	for(i=0; i<nessie_ctx.blocksize_B*8; ++i){
		print_set_vector(set, i);
		memset(key, 0, (nessie_ctx.keysize+7)/8);
		memset(buffer, 0, nessie_ctx.blocksize_B);
		buffer[i/8] |= 0x80>>(i%8);
		nessie_dec(key, buffer);
	}
	/* test set 7 */
	set=7;
	print_setheader(set);
	for(i=0; i<256; ++i){
		print_set_vector(set, i);
		memset(key, i, (nessie_ctx.keysize+7)/8);
		memset(buffer, i, nessie_ctx.blocksize_B);
		nessie_dec(key, buffer);
	}
	/* test set 8 */
	set=8;
	print_setheader(set);
	/* 8 - 0*/
	print_set_vector(set, 0);
	for(i=0; i<(nessie_ctx.keysize+7)/8; ++i){
		key[i]=i;
	}
	for(i=0; i<nessie_ctx.blocksize_B; ++i){
		buffer[i]=i*0x11;
	}
	nessie_dec(key, buffer);
	/* 8 - 1 */
	print_set_vector(set, 1);	
 	for(i=0; i<(nessie_ctx.keysize+7)/8; ++i){
		key[i]=kasumi_key[i%sizeof(kasumi_key)];
	}
	for(i=0; i<nessie_ctx.blocksize_B; ++i){
		buffer[i]=kasumi_plain[i%sizeof(kasumi_plain)];
	}
	print_footer();
}
