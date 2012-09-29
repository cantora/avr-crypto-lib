/* nist_p192.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2012 Daniel Otte (daniel.otte@rub.de)

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

#ifndef NIST_P192_H_
#define NIST_P192_H_

#include "bigint.h"
#include "ecc.h"

extern bigint_t nist_curve_p192_p;

extern bigint_t nist_curve_p192_b;

extern ecc_combi_point_t nist_curve_p192_basepoint;

extern ecc_curve_sp_t nist_curve_p192;

int bigint_reduce_p192(bigint_t *a);

#endif /* NIST_P192_H_ */
