/**
 * \file	shabea.h
 * \author	Daniel Otte 
 * \date	2007-06-07
 * \brief	SHABEA - a SHA Based Encrytion Algorithm declarations
 * \par License	
 * GPL
 * 
 * SHABEAn-r where n is the blocksize and r the number of round used
 * 
 */
 
#ifndef SHABEA_H_
#define SHABEA_H_

void shabea256(void * block, void * key, uint16_t keysize, uint8_t enc, uint8_t rounds);
#endif /*SHABEA_H_*/
