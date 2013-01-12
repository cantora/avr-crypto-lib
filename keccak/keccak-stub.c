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

void keccak_f1600(uint8_t a[200]);

void keccak_lastBlock(keccak_ctx_t* ctx, const void* block, uint16_t length_b){
    uint8_t length_B;
    while(length_b >= ctx->r){
        keccak_nextBlock(ctx, block);
        block = (uint8_t*)block + ctx->bs;
        length_b -=  ctx->r;
    }
    length_B = length_b / 8;
    memxor(ctx->a, block, length_B);
    /* append 1 */
    if(length_b & 7){
        /* we have some single bits */
        uint8_t t;
        t = ((uint8_t*)block)[length_B] >> (8 - (length_b & 7));
        t |= 0x01 << (length_b & 7);
        ctx->a[length_B] ^= t;
    }else{
        ctx->a[length_B] ^= 0x01;
    }
    if(length_b == ctx->r - 1){
        keccak_f1600(ctx->a);
    }
    ctx->a[ctx->bs - 1] ^= 0x80;
    keccak_f1600(ctx->a);
}


