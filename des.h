/**
 * \file	des.h
 * \author	Daniel Otte 
 * \date	2007-06-16
 * \brief 	des and tdes declarations
 * \par License	
 * GPL
 * 
 */
#ifndef DES_H_
#define DES_H_

#include <stdint.h>
/* the FIPS 46-3 (1999-10-25) name for triple DES is triple data encrytion algorithm so TDEA.
 * Also we only implement the three key mode  */
#define tdea_encrypt tdes_encrypt
#define tdea_decrypt tdes_decrypt

void des_encrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void des_decrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void tdes_encrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void tdes_decrypt(uint8_t* out, uint8_t* in, uint8_t* key);

#endif /*DES_H_*/
