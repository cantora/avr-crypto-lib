/* blockcipher_descriptor.h */

#ifndef BLOCKCIPHER_DESCRIPTOR_H_
#define BLOCKCIPHER_DESCRIPTOR_H_
#include <stdint.h>
#include <avr/pgmspace.h>

typedef void(*void_fpt)(void);
typedef void(*bc_init1_fpt)(void*, void*);
typedef void(*bc_init2_fpt)(void*, uint16_t,void*);
typedef void(*bc_enc1_fpt)(void*, void*);
typedef void(*bc_enc2_fpt)(void*, void*, void*);
typedef void(*bc_dec1_fpt)(void*, void*);
typedef void(*bc_dec2_fpt)(void*, void*, void*);
typedef void(*bc_free_fpt)(void*);

typedef union{
	void_fpt  initvoid;
	bc_init1_fpt init1;
	bc_init2_fpt init2;
} bc_init_fpt;

typedef union{
	void_fpt  encvoid;
	bc_enc1_fpt enc1;
	bc_enc2_fpt enc2;
} bc_enc_fpt;

typedef union{
	void_fpt  decvoid;
	bc_dec1_fpt dec1;
	bc_dec2_fpt dec2;
} bc_dec_fpt;

#define BC_INIT_TYPE   0x01
#define BC_INIT_TYPE_1 0x00
#define BC_INIT_TYPE_2 0x01

#define BC_ENC_TYPE    0x02
#define BC_ENC_TYPE_1  0x00
#define BC_ENC_TYPE_2  0x02
#
#define BC_DEC_TYPE    0x04
#define BC_DEC_TYPE_1  0x00
#define BC_DEC_TYPE_2  0x04

#define BCDESC_TYPE_BLOCKCIPHER 0x01

typedef struct {
	uint8_t  type; /* 1==blockcipher */
	uint8_t  flags;
	PGM_P    name;
	uint16_t ctxsize_B;
	uint16_t blocksize_b;
	bc_init_fpt init;
	bc_enc_fpt  enc;
	bc_dec_fpt  dec;
	bc_free_fpt free;
	PGM_VOID_P valid_keysize_desc;
} bcdesc_t; /* blockcipher descriptor type */

typedef struct{
	bcdesc_t* desc_ptr;
	uint16_t  keysize;
	void*     ctx;
} bcgen_ctx_t;

#endif /* BLOCKCIPHER_DESCRIPTOR_H_ */

