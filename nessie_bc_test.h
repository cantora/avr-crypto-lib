#ifndef NESSIE_BC_TEST_H_
#define NESSIE_BC_TEST_H_

#include <stdint.h>

typedef void (*nessie_gen_fpt)(uint8_t*, uint16_t, void*);
typedef void (*nessie_enc_fpt)(void*, void*);
typedef void (*nessie_dec_fpt)(void*, void*);

typedef struct nessie_ctx_st{
	uint16_t keysize;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	void (*cipher_genctx)(uint8_t* key, uint16_t keysize, void* ctx);
	void (*cipher_enc)(void* buffer, void* ctx);
	void (*cipher_dec)(void* buffer, void* ctx);
} nessie_ctx_t; 


extern nessie_ctx_t nessie_ctx;

void nessie_enc(uint8_t* key, uint8_t* pt);
void nessie_dec(uint8_t* key, uint8_t* ct);
void nessie_run(void);

#endif /*NESSIE_BC_TEST_H_*/
