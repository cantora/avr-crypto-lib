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

#define SERPENT_KEY128 128
#define SERPENT_KEY192 192
#define SERPENT_KEY256   0


/* key must be 256bit (32 byte) large! */
void serpent_genctx(void * key, uint8_t keysize, serpent_ctx_t * ctx);
void serpent_enc(void * buffer, serpent_ctx_t * ctx);
void serpent_dec(void * buffer, serpent_ctx_t * ctx);


#endif /*SERPENT_H_*/
