/* serpent.h
 * a bitsliced implementation of the serpent cipher for avr microcontrollers
 * author: Daniel Otte
 * license: GPLv3
 */

#ifndef SERPENT_H_
#define SERPENT_H_

#include <stdint.h>

typedef uint32_t serpent_subkey_t[4];

typedef struct serpent_ctx_st {
	serpent_subkey_t k[33];
}  serpent_ctx_t;

/* key must be 256bit (32 byte) large! */
void serpent_genctx(void * key, serpent_ctx_t * ctx);
void serpent_enc(void * buffer, serpent_ctx_t * ctx);
void serpent_dec(void * buffer, serpent_ctx_t * ctx);


#endif /*SERPENT_H_*/
