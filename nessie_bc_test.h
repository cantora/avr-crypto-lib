#ifndef NESSIE_BC_TEST_H_
#define NESSIE_BC_TEST_H_

#include <stdint.h>

typedef void (*nessie_bc_gen_fpt)(uint8_t* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_bc_free_fpt)(void* ctx);
typedef void (*nessie_bc_enc_fpt)(void* buffer, void* ctx);
typedef void (*nessie_bc_dec_fpt)(void* buffer, void* ctx);

typedef struct nessie_bc_ctx_st{
	uint16_t keysize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_bc_gen_fpt  cipher_genctx;
	nessie_bc_free_fpt cipher_free;
	nessie_bc_enc_fpt  cipher_enc;
	nessie_bc_dec_fpt  cipher_dec;
} nessie_bc_ctx_t; 


extern nessie_bc_ctx_t nessie_bc_ctx;

void nessie_bc_run(void);
void nessie_bc_init(void);


#endif /*NESSIE_BC_TEST_H_*/
