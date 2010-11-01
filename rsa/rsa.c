/* rsa_enc.c */
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
#include "cli.h"
#include "bigint.h"
#include "rsa.h"

uint8_t rsa_enc_bigint(bigint_t* c, const bigint_t* m,
		                const rsa_ctx_t* ctx){
	bigint_expmod_u(c, m, &(ctx->pubexp), &(ctx->modulus));
	return 0;
}

uint8_t rsa_dec_bigint(bigint_t* m, const bigint_t* c,
		                const rsa_ctx_t* ctx){
	bigint_expmod_u(m, c, &(ctx->privexp), &(ctx->modulus));
	return 0;
}

