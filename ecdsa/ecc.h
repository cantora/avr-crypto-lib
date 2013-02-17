/* ecc.h */
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

#ifndef ECC_H_
#define ECC_H_

#include <stddef.h>

typedef struct {
    bigint_t x;
    bigint_t y;
    bigint_t z1;
    bigint_t z2;
    bigint_t z3;
} ecc_chudnovsky_point_t;

typedef struct {
    bigint_t x;
    bigint_t y;
} ecc_affine_point_t;

typedef union __attribute__((packed)){
    ecc_affine_point_t affine;
    ecc_chudnovsky_point_t chudnovsky;
} ecc_combi_point_t;

typedef struct {
    bigint_t *p;
    bigint_t *b;
    int (*reduce_p)(bigint_t*);
} ecc_curve_sp_t;


uint8_t ecc_chudnovsky_point_alloc(ecc_chudnovsky_point_t *p, size_t length_B);

void ecc_chudnovsky_point_free(ecc_chudnovsky_point_t *p);

void ecc_chudnovsky_point_print(const ecc_chudnovsky_point_t *p);


uint8_t ecc_affine_to_chudnovsky_point(ecc_chudnovsky_point_t *dest,
                                       const ecc_affine_point_t *src);
uint8_t ecc_chudnovsky_to_affine_point(ecc_affine_point_t *dest,
                                       const ecc_chudnovsky_point_t *src,
                                       const ecc_curve_sp_t *curve);
uint8_t ecc_chudnovsky_point_double_sp(ecc_chudnovsky_point_t *dest,
                                       const ecc_chudnovsky_point_t *a,
                                       const ecc_curve_sp_t *curve);
void ecc_chudnovsky_point_copy(ecc_chudnovsky_point_t *dest,
                               const ecc_chudnovsky_point_t *src);
uint8_t ecc_chudnovsky_point_add_sp(ecc_chudnovsky_point_t *dest,
                                    const ecc_chudnovsky_point_t *a,
                                    const ecc_chudnovsky_point_t *b,
                                    const ecc_curve_sp_t *curve);
uint8_t ecc_chudnovsky_double_and_add(ecc_chudnovsky_point_t *dest,
                                      const bigint_t *k,
                                      const ecc_chudnovsky_point_t *p,
                                      const ecc_curve_sp_t *curve);
uint8_t bigint_to_naf(uint8_t *dest, uint16_t *length, const bigint_t *src);
uint8_t ecc_chudnovsky_naf_multiplication(ecc_chudnovsky_point_t *dest,
                                          const bigint_t *k,
                                          const ecc_chudnovsky_point_t *p,
                                          const ecc_curve_sp_t *curve);
uint8_t ecc_chudnovsky_multiplication(ecc_chudnovsky_point_t *dest,
                                      const bigint_t *k,
                                      const ecc_chudnovsky_point_t *p,
                                      const ecc_curve_sp_t *curve);

#endif /* ECC_H_ */
