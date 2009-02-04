/* hashfunction_descriptor.h */

#ifndef HASHFUNCTION_DESCRIPTOR_H_
#define HASHFUNCTION_DESCRIPTOR_H_
#include <stdint.h>
#include <avr/pgmspace.h>

#ifndef VOID_FPT
#define VOID_FPT
typedef void(*void_fpt)(void);
#endif

typedef void(*hf_init_fpt)(void*);
typedef void(*hf_nextBlock_fpt)(void*, const void*);
typedef void(*hf_lastBlock_fpt)(void*, const void*, uint16_t);
typedef void(*hf_ctx2hash_fpt)(void*, void*);
typedef void(*hf_free_fpt)(void*);
typedef void(*hf_mem_fpt)(void*, void*, uint32_t);


#define HFDESC_TYPE_HASHFUNCTION 0x02

typedef struct {
	uint8_t  type; /* 2 == hashfunction */
	uint8_t  flags;
	PGM_P    name;
	uint16_t ctxsize_B;
	uint16_t blocksize_b;
	uint16_t hashsize_b;
	
	hf_init_fpt init;
	hf_nextBlock_fpt  nextBlock;
	hf_lastBlock_fpt  lastBlock;
	hf_ctx2hash_fpt   ctx2hash;
	hf_free_fpt free;
	hf_mem_fpt mem;
} hfdesc_t; /* blockcipher descriptor type */

typedef struct{
	hfdesc_t* desc_ptr;
	void*     ctx;
} hfgen_ctx_t;

#endif /* HASHFUNCTION_DESCRIPTOR_H_ */

