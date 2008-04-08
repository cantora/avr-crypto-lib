/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * common function for nessie-tests
 * 
 * */

#ifndef NESSIE_COMMON_H_
#define NESSIE_COMMON_H_


#include <stdint.h>

void nessie_print_block(uint8_t* block, uint16_t blocksize_bit);
void nessie_print_item(char* name, uint8_t* buffer, uint16_t size_B);
void nessie_print_set_vector(uint8_t set, uint16_t vector);
void nessie_print_setheader(uint8_t set);
void nessie_print_header(char* name,
                         uint16_t keysize_b, 
                         uint16_t blocksize_b,
                         uint16_t hashsize_b, 
                         uint16_t macsize_b,
                         uint16_t ivsize_b );
void nessie_print_footer(void);

#endif /*NESSIE_COMMON_H_*/
