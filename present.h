#ifndef PRESENT_H_
#define PRESENT_H_

#include <stdint.h>

typedef struct present_ctx_st{
	uint64_t k[32];
} present_ctx_t;


void present_init(const uint8_t* key, uint8_t keysize_b, present_ctx_t* ctx);
void present_enc(void* buffer, present_ctx_t* ctx);
void present_dec(void* buffer, present_ctx_t* ctx);


#endif /*PRESENT_H_*/
