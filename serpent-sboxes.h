#ifndef SERPENTSBOXES_H_
#define SERPENTSBOXES_H_

#include <stdint.h>

void sbox128(void * w, uint8_t box);
void inv_sbox128(void * w, uint8_t box);


#endif /*SERPENTSBOXES_H_*/
