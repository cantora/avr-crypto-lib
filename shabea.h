/* shabea.h */
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
 * \file	shabea.h
 * \author	Daniel Otte 
 * \date	2007-06-07
 * \brief	SHABEA - a SHA Based Encryption Algorithm declarations
 * \license	GPLv3 or later
 * 
 * SHABEAn-r where n is the blocksize and r the number of round used
 * 
 */
 
#ifndef SHABEA_H_
#define SHABEA_H_

/** \fn void shabea256(void * block, const void * key, uint16_t keysize_b, uint8_t enc, uint8_t rounds);
 * \brief shabea256 encryption/decryption
 * 
 * \param block pointer to a 256 bit (32 byte block) to en/decrypt
 * \param key   pointer to the key material
 * \param keysize_b length of the key in bits
 * \param enc  controls if encryption (1) or decryption (0) is done
 * \param rounds rounds to be done by the cipher (it is not recommended to use less then four rounds)
 */
void shabea256(void * block, const void * key, uint16_t keysize_b, 
               uint8_t enc, uint8_t rounds);

#endif /*SHABEA_H_*/
