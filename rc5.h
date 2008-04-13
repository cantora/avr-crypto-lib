/* rc5.h a C implementation of RC5 for AVR microcontrollers
 * 
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * this implementation is limited to 64bit blocks and a maximum of 255 rounds
 * 
 */
#ifndef RC5_H_
#define RC5_H_


#include <stdint.h>
#include <stdlib.h> /* malloc() & free() */
#include <string.h> /* memset() & memcpy() */
 
typedef struct rc5_ctx_st {
	uint8_t rounds;
	uint32_t *s;
}rc5_ctx_t; 

void rc5_enc(void* buffer, const rc5_ctx_t* ctx);
void rc5_dec(void* buffer, const rc5_ctx_t* ctx);
void rc5_init(void* key, uint16_t keysize_b, uint8_t rounds, rc5_ctx_t* ctx);
void rc5_free(rc5_ctx_t* ctx);

#endif /*RC5_H_*/
