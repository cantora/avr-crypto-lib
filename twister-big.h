/* twister-big.h */

#ifndef TWISTER_BIG_H_
#define TWISTER_BIG_H_

#include <stdint.h>
#include "twister.h"

typedef uint8_t twister384_hash_t[384/8];
typedef uint8_t twister512_hash_t[512/8];

typedef struct {
	twister_state_t    state;
	twister_checksum_t checksum;
} twister_big_ctx_t;

typedef twister_big_ctx_t twister384_ctx_t;
typedef twister_big_ctx_t twister512_ctx_t;

/*********************************************************************/

void twister_big_nextBlock(twister_big_ctx_t* ctx, const void* msg);
void twister_big_init(twister_big_ctx_t* ctx, uint16_t hashsize_b);
void twister_big_lastBlock(twister_big_ctx_t* ctx, const void* msg, uint16_t length_b);
void twister_big_ctx2hash(void* dest, twister_big_ctx_t* ctx, uint16_t hashsize_b);

/*********************************************************************/

void twister384_init(twister384_ctx_t* ctx);
void twister384_nextBlock(twister384_ctx_t* ctx, const void* msg);
void twister384_lastBlock(twister384_ctx_t* ctx, const void* msg, uint16_t length_b);
void twister384_ctx2hash(void* dest, twister384_ctx_t* ctx);
void twister384(void* dest, const void* msg, uint32_t msg_length_b);

/*********************************************************************/

void twister512_init(twister512_ctx_t* ctx);
void twister512_nextBlock(twister512_ctx_t* ctx, const void* msg);
void twister512_lastBlock(twister512_ctx_t* ctx, const void* msg, uint16_t length_b);
void twister512_ctx2hash(void* dest, twister512_ctx_t* ctx);
void twister512(void* dest, const void* msg, uint32_t msg_length_b);


#endif /* TWISTER_BIG_H_ */

