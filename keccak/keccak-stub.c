/* keecak.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "memxor.h"
#include "keccak.h"

/*
//void keccak_f1600(uint8_t a[200]);
void keccak_lastBlock_f1(keccak_ctx_t* ctx, const void* block, uint16_t length_b);
void keccak_lastBlock(keccak_ctx_t* ctx, const void* block, uint16_t length_b){
    keccak_lastBlock_f1(ctx, block, length_b);
 //   ctx->a[ctx->bs - 1] ^= 0x80;
 //   keccak_f1600(ctx->a);
}

*/
