/** 
 * \file	xtea.c
 * \brief	XTEA implemantation 
 *   copy'n'pasted from http://en.wikipedia.org/wiki/XTEA
 *   and slightly modified
 */
 
#include <stdint.h> 
 

void xtea_enc(void* dest, const void* v, const void* k) {
    uint8_t i;
    uint32_t v0=((uint32_t*)v)[0], v1=((uint32_t*)v)[1];
    uint32_t sum=0, delta=0x9E3779B9;
    for(i=0; i<32; i++) {
        v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + ((uint32_t*)k)[sum & 3]);
        sum += delta;
        v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + ((uint32_t*)k)[sum>>11 & 3]);
    }
    ((uint32_t*)dest)[0]=v0; ((uint32_t*)dest)[1]=v1;
}

void xtea_dec(void* dest, const void* v, const void* k) {
    uint8_t i;
    uint32_t v0=((uint32_t*)v)[0], v1=((uint32_t*)v)[1];
    uint32_t sum=0xC6EF3720, delta=0x9E3779B9;
    for(i=0; i<32; i++) {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + ((uint32_t*)k)[sum>>11 & 3]);
        sum -= delta;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + ((uint32_t*)k)[sum & 3]);
    }
    ((uint32_t*)dest)[0]=v0; ((uint32_t*)dest)[1]=v1;
}


