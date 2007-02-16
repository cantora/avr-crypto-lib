#ifndef HMACSHA256_H_
#define HMACSHA256_H_

#include "sha256.h"

#define HMAC_BITS SHA256_HASH_BITS
#define HMAC_BYTES (HMAC_BITS/8)

typedef sha256_ctx_t hmac_sha256_ctx_t;


void hmac_sha256_init(hmac_sha256_ctx_t *s, void* key, uint16_t kl);
void hmac_sha256_final(hmac_sha256_ctx_t *s, void* key, uint16_t kl);
void hmac_sha256(void* dest, void* key, uint16_t kl, void* msg, uint64_t ml);


#endif /*HMACSHA256_H_*/
