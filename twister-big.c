/* twister-big.c */

#include <stdint.h>
#include <string.h>
#include "memxor.h"
#include "twister.h"
#include "twister-big.h"

#undef DEBUG
#define DEBUG

/*********************************************************************/
/*********************************************************************/

#ifdef DEBUG
#include <avr/pgmspace.h>
#include "uart.h"
void print_checksum(twister_big_ctx_t* ctx, PGM_P s){
	uint8_t i;
	uart_putstr_P(PSTR("\r\n"));
	uart_putstr_P(s);
	uart_putstr_P(PSTR("\r\n checksum:\r\n"));
	for(i=0; i<8; ++i){
		uart_putstr_P(PSTR("  [ "));
		uart_hexdump(&(ctx->checksum[i][0]), 8);
		uart_putstr_P(PSTR("]\r\n"));
	}
}

/*********************************************************************/

void print_matrix(void* m, PGM_P s){
	uint8_t i;
	uart_putstr_P(PSTR("\r\n"));
	uart_putstr_P(s);
	uart_putstr_P(PSTR("\r\n matrix:\r\n"));
	for(i=0; i<8; ++i){
		uart_putstr_P(PSTR("  [ "));
		uart_hexdump(((uint8_t*)m)+i*8, 8);
		uart_putstr_P(PSTR("]\r\n"));
	}
}

/*********************************************************************/

#define DEBUG_CHKSUM(a,s) print_checksum((a),PSTR(s)) 
#else
#define DEBUG_CHKSUM(a,s) 
#endif


#ifdef DEBUG
# define DEBUG_PRINT(ctx, msg) debug_print((ctx), PSTR(msg)) 
#else
# define DEBUG_PRINT(ctx, msg) 
#endif 

#ifdef DEBUG

/*********************************************************************/

void print_twister_state(twister_state_t* ctx){
	uint8_t i;
	uart_putstr_P(PSTR("\r\nState:\r\n matrix:\r\n"));
	for(i=0; i<8; ++i){
		uart_putstr_P(PSTR("\t[ "));
		uart_hexdump(&(ctx->s[i][0]), 8);
		uart_putstr_P(PSTR("]\r\n"));
	}
	uart_putstr_P(PSTR("counter: "));
	uart_hexdump(&(ctx->counter), 8);

	uart_putstr_P(PSTR("\r\nlength_counter_b: "));
	uart_hexdump(&(ctx->length_counter_b), 8);
	uart_putstr_P(PSTR("\r\n"));
} 

/*********************************************************************/

void debug_print(twister_state_t* ctx, PGM_P msg){
	uart_putstr_P(PSTR("\r\n"));
	uart_putstr_P(msg);
	print_twister_state(ctx);
}

#endif

/*********************************************************************/

static
void checksum_update(twister_big_ctx_t* ctx, uint8_t col){
	uint8_t i, col2;
	uint8_t carry = 0;
	int sum = 0;
	
	col2 = (col+1)%8; 
	
	for( i=0; i<8; i++ ) 
	{
		sum =  (int) ctx->checksum[col2][7-i] 
		     + (int) ctx->state.s[7-i][0] /* col or 0 ???*/ 
		     + carry;
		ctx->checksum[col][7-i] ^= (uint8_t)sum;
		carry = sum>>8;

 	}
//	DEBUG_CHKSUM(ctx, "post run");
}

/*********************************************************************/

void twister_big_init(twister_big_ctx_t* ctx, uint16_t hashsize_b){
	memset(ctx->state.s,  0, 64);
	memset(ctx->checksum, 0, 64);
	ctx->state.counter=0xffffffffffffffffLL;
	ctx->state.s[0][7] = hashsize_b>>8;
	ctx->state.s[1][7] = hashsize_b&0xff;
	ctx->state.length_counter_b = 0;
}

/*********************************************************************/

void twister_big_nextBlock(twister_big_ctx_t* ctx, void* msg){
	uint8_t tmp[8][8];

	/* 1st maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 0);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 1);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	
	checksum_update(ctx, 2);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	memxor(ctx->state.s, tmp, 64);

	/* 2nd maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 3);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	twister_blank_round(&(ctx->state));
	
	checksum_update(ctx, 4);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;
	memxor(ctx->state.s, tmp, 64);

	/* 3rd maxi round */
	memcpy(tmp, ctx->state.s, 64);	
	checksum_update(ctx, 5);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 6);
	twister_mini_round(&(ctx->state), msg);
	msg = (uint8_t*)msg + 8;

	checksum_update(ctx, 7);
	twister_mini_round(&(ctx->state), msg);

	twister_blank_round(&(ctx->state));	
	memxor(ctx->state.s, tmp, 64);
	ctx->state.length_counter_b += 512;
}

/*********************************************************************/

void twister_inject_chksum(twister_big_ctx_t* ctx, uint8_t col){
	*((uint64_t*)(&ctx->state.s[7][0])) ^= *((uint64_t*)(&ctx->checksum[col][0]));
	twister_blank_round(&ctx->state);
}

/*********************************************************************/

void twister_big_lastBlock(twister_big_ctx_t* ctx, void* msg, uint16_t length_b){
	uint8_t tmp[64];	
	while(length_b>512){
		twister_big_nextBlock(ctx, msg);
		msg = ((uint8_t*)msg)+64;
		length_b -= 512;
	}
	memset(tmp, 0, 64);
	memcpy(tmp, msg, (length_b+7)/8);
	tmp[length_b/8] |= 0x80 >> (length_b&0x07);
	twister_big_nextBlock(ctx, tmp);
	ctx->state.length_counter_b -= 512 - length_b;
	twister_mini_round(&(ctx->state), &(ctx->state.length_counter_b));

//	DEBUG_PRINT(&(ctx->state), "pre check-round");

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 0);
	twister_inject_chksum(ctx, 1);
	twister_inject_chksum(ctx, 2);
	memxor(ctx->state.s, tmp, 64);

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 3);
	twister_inject_chksum(ctx, 4);
	twister_inject_chksum(ctx, 5);
	memxor(ctx->state.s, tmp, 64);

	memcpy(tmp, ctx->state.s, 64);
	twister_inject_chksum(ctx, 6);
	twister_inject_chksum(ctx, 7);
	twister_blank_round(&(ctx->state));
	memxor(ctx->state.s, tmp, 64);


//	DEBUG_PRINT(&(ctx->state), "post check-round");
}

/*********************************************************************/

void twister_big_ctx2hash(void* dest, twister_big_ctx_t* ctx, uint16_t hashsize_b){
	twister_ctx2hash(dest, &(ctx->state), hashsize_b);
}

/*********************************************************************/
/*********************************************************************/

void twister384_init(twister384_ctx_t* ctx){
	twister_big_init(ctx, 384);
}

/*********************************************************************/

void twister384_nextBlock(twister384_ctx_t* ctx, void* msg){
	twister_big_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister384_lastBlock(twister384_ctx_t* ctx, void* msg, uint16_t length_b){
	twister_big_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister384_ctx2hash(void* dest, twister384_ctx_t* ctx){
	twister_big_ctx2hash(dest, ctx, 384);
}

/*********************************************************************/

void twister384(void* dest, void* msg, uint32_t msg_length_b){
	twister_big_ctx_t ctx;
	twister_big_init(&ctx, 384);
	while(msg_length_b >=512){
		twister_big_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_big_lastBlock(&ctx, msg, msg_length_b);
	twister_big_ctx2hash(dest, &ctx, 384);
}

/*********************************************************************/
/*********************************************************************/


void twister512_init(twister512_ctx_t* ctx){
	twister_big_init(ctx, 512);
}

/*********************************************************************/

void twister512_nextBlock(twister512_ctx_t* ctx, void* msg){
	twister_big_nextBlock(ctx, msg);
}

/*********************************************************************/

void twister512_lastBlock(twister512_ctx_t* ctx, void* msg, uint16_t length_b){
	twister_big_lastBlock(ctx, msg, length_b);
}

/*********************************************************************/

void twister512_ctx2hash(void* dest, twister512_ctx_t* ctx){
	twister_big_ctx2hash(dest, ctx, 512);
}

/*********************************************************************/

void twister512(void* dest, void* msg, uint32_t msg_length_b){
	twister_big_ctx_t ctx;
	twister_big_init(&ctx, 512);
	while(msg_length_b >=512){
		twister_big_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 512/8;
		msg_length_b -= 512;
	}
	twister_big_lastBlock(&ctx, msg, msg_length_b);
	twister_big_ctx2hash(dest, &ctx, 512);
}



