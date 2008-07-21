#include <stdint.h>

void memxor(void * dest, const void * src, uint16_t n){
  while(n--){
    ((uint8_t*)dest)[n] ^= ((uint8_t*)src)[n];
  }
}

