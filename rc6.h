/* 
 * File:	rc6.h
 * Author:	Daniel Otte
 * Date: 	06.08.2006
 * License: GPL
 * Description: Implementation of the RC6 cipher algorithm.
 * 	This implementation is restricted to 32-bit words, but free in the choice of number of rounds (0 to 255).
 * 	so it is RC6-32/r/b
 */

#ifndef RC6_H_
#define RC6_H_

 
#include <stdint.h>
 
typedef struct rc6_ctx_st{
	uint8_t		rounds;		/* specifys the number of rounds; default: 20 */
 	uint32_t*	S;			/* the round-keys */
} rc6_ctx_t;
 
 
uint8_t rc6_init(void* key, uint16_t keylength_b, rc6_ctx_t *s);
 
uint8_t rc6_initl(void* key, uint16_t keylength_b, uint8_t rounds, rc6_ctx_t *s);
 
void rc6_enc(void* block, rc6_ctx_t *s);

void rc6_dec(void* block, rc6_ctx_t *s);
 
void rc6_free(rc6_ctx_t *s); 

#endif /* RC6_H_ */

