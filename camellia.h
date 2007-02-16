#ifndef CAMELLIA_H_
#define CAMELLIA_H_

#include <stdint.h>

typedef struct camellia128_ctx_s{
	uint64_t klr; 
	uint64_t kll;
	uint64_t kar;
	uint64_t kal;
}camellia128_ctx_t;


void camellia128_init(camellia128_ctx_t* s, uint8_t* key);
void camellia128_enc(camellia128_ctx_t* s, void* block);
void camellia128_dec(camellia128_ctx_t* s, void* block);


#endif /*CAMELLIA_H_*/
