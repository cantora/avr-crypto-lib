/* nessie_mac_test.h */
/*
    This file is part of the This file is part of the AVR-Crypto-Lib.
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
#ifndef NESSIE_MAC_TEST_H_
#define NESSIE_MAC_TEST_H_

#include <stdint.h>

typedef void (*nessie_mac_init_fpt)(void* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_mac_next_fpt)(void* buffer, void* ctx);
typedef void (*nessie_mac_last_fpt)(void* buffer, uint16_t size_b, void* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_mac_conv_fpt)(void* buffer, void* ctx);


typedef struct nessie_mac_ctx_st{
	uint16_t macsize_b;
	uint16_t keysize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_mac_init_fpt mac_init;
	nessie_mac_next_fpt mac_next;
	nessie_mac_last_fpt mac_last;
	nessie_mac_conv_fpt mac_conv;
} nessie_mac_ctx_t; 


extern nessie_mac_ctx_t nessie_mac_ctx;

void nessie_mac_run(void);

#endif /*NESSIE_MAC_TEST_H_*/
