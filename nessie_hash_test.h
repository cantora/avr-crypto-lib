#ifndef NESSIE_HASH_TEST_H_
#define NESSIE_HASH_TEST_H_

#include <stdint.h>

typedef void (*nessie_hash_init_fpt)(void* ctx);
typedef void (*nessie_hash_next_fpt)(void* buffer, void* ctx);
typedef void (*nessie_hash_last_fpt)(void* buffer, uint16_t size_b, void* ctx);
typedef void (*nessie_hash_conv_fpt)(void* buffer, void* ctx);


typedef struct nessie_hash_ctx_st{
	uint16_t hashsize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_hash_init_fpt hash_init;
	nessie_hash_next_fpt hash_next;
	nessie_hash_last_fpt hash_last;
	nessie_hash_conv_fpt hash_conv;
} nessie_hash_ctx_t; 


extern nessie_hash_ctx_t nessie_hash_ctx;

void nessie_hash_run(void);

#endif /*NESSIE_HASH_TEST_H_*/
