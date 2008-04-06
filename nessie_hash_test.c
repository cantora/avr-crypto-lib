/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for hashes
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_hash_test.h"
#include "uart.h"

nessie_hash_ctx_t nessie_hash_ctx;

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
	uart_putstr(nessie_hash_ctx.name);
	uart_putstr_P(PSTR("\r\n"));
	for(i=0; i<16+strlen(nessie_hash_ctx.name); ++i){
		uart_putc('=');
	}
	uart_putstr_P(PSTR("\r\nHash size: "));
	if(nessie_hash_ctx.hashsize_b >100){
		uart_putc('0'+nessie_hash_ctx.hashsize_b/100);
	}
	if(nessie_hash_ctx.hashsize_b>10){
		uart_putc('0'+(nessie_hash_ctx.hashsize_b/10)%10);
	}
	uart_putc('0'+nessie_hash_ctx.hashsize_b%10);
	uart_putstr_P(PSTR(" bits\r\n"));
}

static void print_footer(void){
	uart_putstr_P(PSTR("\r\n\r\n\r\n\r\nEnd of test vectors"));
}

static
void ascii_hash(char* data, char* desc){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint16_t sl;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr(desc);
	nessie_hash_ctx.hash_init(ctx);
	sl = strlen(data);
	while(sl>=nessie_hash_ctx.blocksize_B){
		nessie_hash_ctx.hash_next(data, ctx);
		data += nessie_hash_ctx.blocksize_B;
		sl   -= nessie_hash_ctx.blocksize_B;
	}
	nessie_hash_ctx.hash_last(data, sl*8, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	printitem("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

// message=1 million times "a"

static
void amillion_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	uint32_t n=1000000LL;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr_P(PSTR("1 million times \"a\""));
	memset(block, 'a', nessie_hash_ctx.blocksize_B);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B){
		nessie_hash_ctx.hash_next(block, ctx);
		n    -= nessie_hash_ctx.blocksize_B;
	}
	nessie_hash_ctx.hash_last(block, n*8, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	printitem("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}


static
void zero_hash(uint16_t n){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	
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
	
	memset(block, 0, nessie_hash_ctx.blocksize_B); 
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	printitem("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

static
void one_in512_hash(uint16_t pos){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
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
	
	/* now the real stuff */
	block[pos>>3] = 0x80>>(pos&0x7);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	printitem("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
	
	
}

static
void tv4_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[256/8];
	uint16_t n=256;
	uint32_t i;
	
	uart_putstr_P(PSTR("\r\n                       message="));
	uart_putstr(PSTR("256 zero bits"));
	memset(block, 0, 256/8);
	
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(block, ctx);
		n    -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(block, n*8, ctx);
	nessie_hash_ctx.hash_conv(hash, ctx);
	printitem("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
	for(i=1; i<100000L; ++i){ /* this assumes BLOCKSIZE >= HASHSIZE */
		nessie_hash_ctx.hash_init(ctx);
		nessie_hash_ctx.hash_last(block, nessie_hash_ctx.hashsize_b, ctx);
		nessie_hash_ctx.hash_conv(hash, ctx);
	}
	printitem("iterated 100000 times", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

/*
   "" (empty string)
   message="a"
   message="abc"
   message="message digest"
   message="abcdefghijklmnopqrstuvwxyz"
   message="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
   message="A...Za...z0...9"
   message=8 times "1234567890"
*/


void nessie_hash_run(void){
	uint16_t i;
	uint8_t set;
	
	print_header();
	/* test set 1 */
	char* challange[8][2]= {
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
		 "8 times \"1234567890\""} 	
	};
	set=1;
	print_setheader(set);
	for(i=0; i<8; ++i){
		print_set_vector(set, i);
		ascii_hash(challange[i][0], challange[i][1]);
	}
	print_set_vector(set, i);
	amillion_hash();
	/* test set 2 */
	set=2;
	print_setheader(set);
	for(i=0; i<1024; ++i){
		print_set_vector(set, i);
		zero_hash(i);
	}
	/* test set 3 */
	set=3;
	print_setheader(set);
	for(i=0; i<512; ++i){
		print_set_vector(set, i);
		one_in512_hash(i);
	}
	/* test set 4 */
	set=4;
	print_setheader(set);
	print_set_vector(set, 0);
	tv4_hash();

	print_footer();
}
