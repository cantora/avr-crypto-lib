#ifndef NESSIE_MAC_TEST_H_
#define NESSIE_MAC_TEST_H_

#include <stdint.h>

typedef void (*nessie_mac_init_fpt)(void* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_mac_next_fpt)(void* buffer, void* ctx);
typedef void (*nessie_mac_last_fpt)(void* buffer, uint16_t size_b, void* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_mac_conv_fpt)(void* buffer, void* ctx);


typedef struct nessie_mac_ctx_st{
	uint16_t macsize_b;
	uint16_t keysize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_mac_init_fpt mac_init;
	nessie_mac_next_fpt mac_next;
	nessie_mac_last_fpt mac_last;
	nessie_mac_conv_fpt mac_conv;
} nessie_mac_ctx_t; 


extern nessie_mac_ctx_t nessie_mac_ctx;

void nessie_mac_run(void);

#endif /*NESSIE_MAC_TEST_H_*/
