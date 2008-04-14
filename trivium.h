#ifndef TRIVIUM_H_
#define TRIVIUM_H_

typedef uint8_t trivium_ctx_t[36]; /* 288bit */

uint8_t trivium_enc(trivium_ctx_t* ctx);
void trivium_init(const void* key, uint8_t keysize_b, 
                  const void* iv,  uint8_t ivsize_b,
                  trivium_ctx_t* ctx);

#endif /*TRIVIUM_H_*/
