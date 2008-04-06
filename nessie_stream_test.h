#ifndef NESSIE_STREAM_TEST_H_
#define NESSIE_STREAM_TEST_H_

#include <stdint.h>

typedef void (*nessie_stream_genctx_fpt)(uint8_t* key, uint16_t keylength_b, void* ctx);
typedef uint8_t (*nessie_stream_genenc_fpt)(void* ctx);

typedef struct nessie_stream_ctx_st{
	uint16_t keysize_b;
	uint16_t outsize_b;
	uint16_t ctx_size_B;
	char* name; 
	nessie_stream_genctx_fpt cipher_genctx;
	nessie_stream_genenc_fpt cipher_enc;
} nessie_stream_ctx_t; 


extern nessie_stream_ctx_t nessie_stream_ctx;

void nessie_stream_run(void);

#endif /*NESSIE_STREAM_TEST_H_*/
