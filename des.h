/* des.h */
/*
    This file is part of the Crypto-avr-lib/microcrypt-lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
/* the FIPS 46-3 (1999-10-25) name for triple DES is triple data encryption algorithm so TDEA.
 * Also we only implement the three key mode  */
#define tdea_encrypt tdes_encrypt
#define tdea_decrypt tdes_decrypt

void des_encrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void des_decrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void tdes_encrypt(uint8_t* out, uint8_t* in, uint8_t* key);
void tdes_decrypt(uint8_t* out, uint8_t* in, uint8_t* key);

#endif /*DES_H_*/
