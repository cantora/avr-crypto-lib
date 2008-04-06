/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for streamciphers
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_stream_test.h"
#include "uart.h"

nessie_stream_ctx_t nessie_stream_ctx;

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

static
void memxor(void* dest, void* src, uint8_t length){
	while(length--){
		*((uint8_t*)dest) ^= *((uint8_t*)src);
		dest = (uint8_t*)dest +1;
		src  = (uint8_t*)src +1;
	}
} 

#define BLOCKSIZE_B 64

static 
void nessie_gen_block(void* ctx, uint8_t* block){
	uint16_t i;
	for(i=0; i<BLOCKSIZE_B; ++i){
		block[i] = nessie_stream_ctx.cipher_enc(ctx);
	}
}

static
void nessie_stream_enc(uint8_t* key){
	uint8_t ctx[nessie_stream_ctx.ctx_size_B];
	uint8_t buffer[BLOCKSIZE_B];
	uint8_t xorbuffer[BLOCKSIZE_B];
	uint8_t i;
	
	memset(xorbuffer, 0, BLOCKSIZE_B);
	
	printitem("key", key, (nessie_stream_ctx.keysize_b+7)/8);
	
	nessie_stream_ctx.cipher_genctx(key, nessie_stream_ctx.keysize_b, ctx);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[0..63]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((192-0)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[192..255]", buffer, BLOCKSIZE_B);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[256..319]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((448-256)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[448..511]", buffer, BLOCKSIZE_B);
	
	printitem("stream[0..511]xored", xorbuffer, BLOCKSIZE_B);
	
}


static
void nessie_stream_enc_large(uint8_t* key){
	uint8_t ctx[nessie_stream_ctx.ctx_size_B];
	uint8_t buffer[BLOCKSIZE_B];
	uint8_t xorbuffer[BLOCKSIZE_B];
	uint32_t i;
	
	memset(xorbuffer, 0, BLOCKSIZE_B);
	
	printitem("key", key, (nessie_stream_ctx.keysize_b+7)/8);
	
	nessie_stream_ctx.cipher_genctx(key, nessie_stream_ctx.keysize_b, ctx);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[0..63]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((65472-0)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[65472..65535]", buffer, BLOCKSIZE_B);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[65536..65599]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((131008-65536)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	printitem("stream[131008..131071]", buffer, BLOCKSIZE_B);
	
	printitem("stream[0..131071]xored", xorbuffer, BLOCKSIZE_B);
	
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
	uart_putstr(nessie_stream_ctx.name);
	uart_putstr_P(PSTR("\r\n"));
	for(i=0; i<16+strlen(nessie_stream_ctx.name); ++i){
		uart_putc('=');
	}
	uart_putstr_P(PSTR("\r\nKey size: "));
	if(nessie_stream_ctx.keysize_b>100){
		uart_putc('0'+nessie_stream_ctx.keysize_b/100);
	}
	if(nessie_stream_ctx.keysize_b>10){
		uart_putc('0'+(nessie_stream_ctx.keysize_b/10)%10);
	}
	uart_putc('0'+nessie_stream_ctx.keysize_b%10);
	uart_putstr_P(PSTR(" bits\r\n"));
}

static void print_footer(void){
	uart_putstr_P(PSTR("\r\n\r\n\r\n\r\nEnd of test vectors\r\n\r\n"));
}

void nessie_stream_run(void){
	uint16_t i;
	uint8_t set;
	uint8_t key[(nessie_stream_ctx.keysize_b+7)/8];
	
	print_header();
	/* test set 1 */
	set=1;
	print_setheader(set);
	for(i=0; i<nessie_stream_ctx.keysize_b; ++i){
		print_set_vector(set, i);
		memset(key, 0, (nessie_stream_ctx.keysize_b+7)/8);
		key[i/8] |= 0x80>>(i%8);
		nessie_stream_enc(key);
	}
	/* test set 2 */
	set=2;
	print_setheader(set);
	for(i=0; i<256; ++i){
		print_set_vector(set, i);
		memset(key, i, (nessie_stream_ctx.keysize_b+7)/8);
		nessie_stream_enc(key);
	}
	/* test set 3 */
	set=3;
	print_setheader(set);
	for(i=0; i<256; ++i){
		uint8_t j;
		print_set_vector(set, i);
		for(j=0; j<(nessie_stream_ctx.keysize_b+7)/8; ++j){
			key[j]=(i+j)&0xff;
		}
		nessie_stream_enc(key);
	}
	/* test set 4 */
	set=4;
	print_setheader(set);
	for(i=0; i<4; ++i){
		uint8_t j;
		print_set_vector(set, i);
		for(j=0; j<(nessie_stream_ctx.keysize_b+7)/8; ++j){
			key[j]=(i*5+j*0x53)&0xff;
		}
		nessie_stream_enc_large(key);
	}

	print_footer();
}
