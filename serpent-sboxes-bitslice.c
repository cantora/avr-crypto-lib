/* serpent-sboxes.c
 * a non-bitsliced implementation of the serpent sboxes
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "serpent-sboxes.h"




/* S0:   3  8 15  1 10  6  5 11 14 13  4  2  7  0  9 12 */

/* depth = 5,7,4,2, Total gates=18 */
static
void sb0(uint8_t* out, const uint8_t* in, uint8_t i){
// (a,b,c,d,w,x,y,z) 
	uint8_t t01, t02, t03, t05, t06, t07, t08, t09, t11, t12, t13, t14, t15, t17;
	t01          = in[4*1+i]  ^ in[4*2+i]; 
	t02          = in[4*0+i]  | in[4*3+i]; 
	t03          = in[4*0+i]  ^ in[4*1+i]; 
	out[4*3+i]   = t02        ^ t01; 
	t05          = in[4*2+i]  | out[4*3+i]; 
	t06          = in[4*0+i]  ^ in[4*3+i]; 
	t07          = in[4*1+i]  | in[4*2+i]; 
	t08          = in[4*3+i]  & t05; 
	t09          = t03        & t07; 
	out[4*2+i]   = t09        ^ t08; 
	t11          = t09        & out[4*2+i]; 
	t12          = in[4*2+i]  ^ in[4*3+i]; 
	t13          = t07        ^ t11; 
	t14          = in[4*1+i]  & t06; 
	t15          = t06        ^ t13; 
	out[4*0+i]   =     ~ t15; 
	t17          = out[4*0+i] ^ t14; 
	out[4*1+i]   = t12        ^ t17; 
}

/* InvS0:  13  3 11  0 10  6  5 12  1 14  4  7 15  9  8  2 */

/* depth = 8,4,3,6, Total gates=19 */
static
void sb0_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t08, t09, t10, t12, t13, t14, t15, t17, t18, t01;
	t01          = in[4*2+i] ^ in[4*3+i];
	t02          = in[4*0+i] | in[4*1+i];
	t03          = in[4*1+i] | in[4*2+i];
	t04          = in[4*2+i] & t01;
	t05          = t02       ^ t01;
	t06          = in[4*0+i] | t04;
	out[4*2+i]   =     ~ t05;
	t08          = in[4*1+i]   ^ in[4*3+i];
	t09          = t03       & t08;
	t10          = in[4*3+i]   | out[4*2+i];
	out[4*1+i]   = t09       ^ t06;
	t12          = in[4*0+i]   | t05;
	t13          = out[4*1+i]   ^ t12;
	t14          = t03       ^ t10;
	t15          = in[4*0+i]   ^ in[4*2+i];
	out[4*3+i]   = t14       ^ t13;
	t17          = t05       & t13;
	t18          = t14       | t17;
	out[4*0+i]   = t15       ^ t18; 
}

/* S1:  15 12  2  7  9  0  5 10  1 11 14  8  6 13  3  4 */

/* depth = 10,7,3,5, Total gates=18 */
static 
void sb1(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t08, t10, t11, t12, t13, t16, t17, t01;
	t01          = in[4*0+i]   | in[4*3+i];
	t02          = in[4*2+i]   ^ in[4*3+i];
	t03          =     ~ in[4*1+i];
	t04          = in[4*0+i]   ^ in[4*2+i];
	t05          = in[4*0+i]   | t03;
	t06          = in[4*3+i]   & t04;
	t07          = t01       & t02;
	t08          = in[4*1+i]   | t06;
	out[4*2+i]   = t02       ^ t05;
	t10          = t07       ^ t08;
	t11          = t01       ^ t10;
	t12          = out[4*2+i]   ^ t11;
	t13          = in[4*1+i]   & in[4*3+i];
	out[4*3+i]   =     ~ t10;
	out[4*1+i]   = t13       ^ t12;
	t16          = t10       | out[4*1+i];
	t17          = t05       & t16;
	out[4*0+i]   = in[4*2+i]   ^ t17; 
}

/* InvS1:   5  8  2 14 15  6 12  3 11  4  7  9  1 13 10  0 */

/* depth = 7,4,5,3, Total gates=18 */
static void sb1_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t08, t09, t10, t11, t14, t15, t17, t01;
	t01          = in[4*0+i]   ^ in[4*1+i];
	t02          = in[4*1+i]   | in[4*3+i];
	t03          = in[4*0+i]   & in[4*2+i];
	t04          = in[4*2+i]   ^ t02;
	t05          = in[4*0+i]   | t04;
	t06          = t01       & t05;
	t07          = in[4*3+i]   | t03;
	t08          = in[4*1+i]   ^ t06;
	t09          = t07       ^ t06;
	t10          = t04       | t03;
	t11          = in[4*3+i]   & t08;
	out[4*2+i]   =     ~ t09;
	out[4*1+i]   = t10       ^ t11;
	t14          = in[4*0+i]   | out[4*2+i];
	t15          = t06       ^ out[4*1+i];
	out[4*3+i]   = t01       ^ t04;
	t17          = in[4*2+i]   ^ t15;
	out[4*0+i]   = t14       ^ t17; 
}

/* S2:   8  6  7  9  3 12 10 15 13  1 14  4  0 11  5  2 */

/* depth = 3,8,11,7, Total gates=16 */
static void sb2(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t05, t06, t07, t08, t09, t10, t12, t13, t14, t01;
	t01          = in[4*0+i]   | in[4*2+i];
	t02          = in[4*0+i]   ^ in[4*1+i];
	t03          = in[4*3+i]   ^ t01;
	out[4*0+i]   = t02       ^ t03;
	t05          = in[4*2+i]   ^ out[4*0+i];
	t06          = in[4*1+i]   ^ t05;
	t07          = in[4*1+i]   | t05;
	t08          = t01       & t06;
	t09          = t03       ^ t07;
	t10          = t02       | t09;
	out[4*1+i]   = t10       ^ t08;
	t12          = in[4*0+i]   | in[4*3+i];
	t13          = t09       ^ out[4*1+i];
	t14          = in[4*1+i]   ^ t13;
	out[4*3+i]   =     ~ t09;
	out[4*2+i]   = t12       ^ t14; 
}

/* InvS2:  12  9 15  4 11 14  1  2  0  3  6 13  5  8 10  7 */

/* depth = 3,6,8,3, Total gates=18 */
static void sb2_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t06, t07, t08, t09, t10, t11, t12, t15, t16, t17, t01;
	t01          = in[4*0+i]   ^ in[4*3+i];
	t02          = in[4*2+i]   ^ in[4*3+i];
	t03          = in[4*0+i]   & in[4*2+i];
	t04          = in[4*1+i]   | t02;
	out[4*0+i]   = t01       ^ t04;
	t06          = in[4*0+i]   | in[4*2+i];
	t07          = in[4*3+i]   | out[4*0+i];
	t08          =     ~ in[4*3+i];
	t09          = in[4*1+i]   & t06;
	t10          = t08       | t03;
	t11          = in[4*1+i]   & t07;
	t12          = t06       & t02;
	out[4*3+i]   = t09       ^ t10;
	out[4*1+i]   = t12       ^ t11;
	t15          = in[4*2+i]   & out[4*3+i];
	t16          = out[4*0+i]   ^ out[4*1+i];
	t17          = t10       ^ t15;
	out[4*2+i]   = t16       ^ t17; 
}

/* S3:   0 15 11  8 12  9  6  3 13  1  2  4 10  7  5 14 */

/* depth = 8,3,5,5, Total gates=18 */
static void sb3(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t08, t09, t10, t11, t13, t14, t15, t01;
	t01          = in[4*0+i]   ^ in[4*2+i];
	t02          = in[4*0+i]   | in[4*3+i];
	t03          = in[4*0+i]   & in[4*3+i];
	t04          = t01       & t02;
	t05          = in[4*1+i]   | t03;
	t06          = in[4*0+i]   & in[4*1+i];
	t07          = in[4*3+i]   ^ t04;
	t08          = in[4*2+i]   | t06;
	t09          = in[4*1+i]   ^ t07;
	t10          = in[4*3+i]   & t05;
	t11          = t02       ^ t10;
	out[4*3+i]   = t08       ^ t09;
	t13          = in[4*3+i]   | out[4*3+i];
	t14          = in[4*0+i]   | t07;
	t15          = in[4*1+i]   & t13;
	out[4*2+i]   = t08       ^ t11;
	out[4*0+i]   = t14       ^ t15;
	out[4*1+i]   = t05       ^ t04; 
}

/* InvS3:   0  9 10  7 11 14  6 13  3  5 12  2  4  8 15  1 */

/* depth = 3,6,4,4, Total gates=17 */
static void sb3_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t09, t11, t12, t13, t14, t16, t01;
	t01          = in[4*2+i]   | in[4*3+i];
	t02          = in[4*0+i]   | in[4*3+i];
	t03          = in[4*2+i]   ^ t02;
	t04          = in[4*1+i]   ^ t02;
	t05          = in[4*0+i]   ^ in[4*3+i];
	t06          = t04       & t03;
	t07          = in[4*1+i]   & t01;
	out[4*2+i]   = t05       ^ t06;
	t09          = in[4*0+i]   ^ t03;
	out[4*0+i]   = t07       ^ t03;
	t11          = out[4*0+i]   | t05;
	t12          = t09       & t11;
	t13          = in[4*0+i]   & out[4*2+i];
	t14          = t01       ^ t05;
	out[4*1+i]   = in[4*1+i]   ^ t12;
	t16          = in[4*1+i]   | t13;
	out[4*3+i]   = t14       ^ t16; 
}

/* S4:   1 15  8  3 12  0 11  6  2  5  4 10  9 14  7 13 */

/* depth = 6,7,5,3, Total gates=19 */
static void sb4(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t08, t09, t10, t11, t12, t13, t14, t15, t16, t01;
	t01          = in[4*0+i]   | in[4*1+i];
	t02          = in[4*1+i]   | in[4*2+i];
	t03          = in[4*0+i]   ^ t02;
	t04          = in[4*1+i]   ^ in[4*3+i];
	t05          = in[4*3+i]   | t03;
	t06          = in[4*3+i]   & t01;
	out[4*3+i]   = t03       ^ t06;
	t08          = out[4*3+i]   & t04;
	t09          = t04       & t05;
	t10          = in[4*2+i]   ^ t06;
	t11          = in[4*1+i]   & in[4*2+i];
	t12          = t04       ^ t08;
	t13          = t11       | t03;
	t14          = t10       ^ t09;
	t15          = in[4*0+i]   & t05;
	t16          = t11       | t12;
	out[4*2+i]   = t13       ^ t08;
	out[4*1+i]   = t15       ^ t16;
	out[4*0+i]   =     ~ t14; 
}

/* InvS4:   5  0  8  3 10  9  7 14  2 12 11  6  4 15 13  1 */

/* depth = 6,4,7,3, Total gates=17 */
static void sb4_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t09, t10, t11, t12, t13, t15, t01;
	t01          = in[4*1+i]   | in[4*3+i];
	t02          = in[4*2+i]   | in[4*3+i];
	t03          = in[4*0+i]   & t01;
	t04          = in[4*1+i]   ^ t02;
	t05          = in[4*2+i]   ^ in[4*3+i];
	t06          =     ~ t03;
	t07          = in[4*0+i]   & t04;
	out[4*1+i]   = t05       ^ t07;
	t09          = out[4*1+i]   | t06;
	t10          = in[4*0+i]   ^ t07;
	t11          = t01       ^ t09;
	t12          = in[4*3+i]   ^ t04;
	t13          = in[4*2+i]   | t10;
	out[4*3+i]   = t03       ^ t12;
	t15          = in[4*0+i]   ^ t04;
	out[4*2+i]   = t11       ^ t13;
	out[4*0+i]   = t15       ^ t09; 
}

/* S5:  15  5  2 11  4 10  9 12  0  3 14  8 13  6  7  1 */

/* depth = 4,6,8,6, Total gates=17 */
static void sb5(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t07, t08, t09, t10, t11, t12, t13, t14, t01;
	t01          = in[4*1+i]   ^ in[4*3+i];
	t02          = in[4*1+i]   | in[4*3+i];
	t03          = in[4*0+i]   & t01;
	t04          = in[4*2+i]   ^ t02;
	t05          = t03       ^ t04;
	out[4*0+i]   =     ~ t05;
	t07          = in[4*0+i]   ^ t01;
	t08          = in[4*3+i]   | out[4*0+i];
	t09          = in[4*1+i]   | t05;
	t10          = in[4*3+i]   ^ t08;
	t11          = in[4*1+i]   | t07;
	t12          = t03       | out[4*0+i];
	t13          = t07       | t10;
	t14          = t01       ^ t11;
	out[4*2+i]   = t09       ^ t13;
	out[4*1+i]   = t07       ^ t08;
	out[4*3+i]   = t12       ^ t14; 
}

/* InvS5:   8 15  2  9  4  1 13 14 11  6  5  3  7 12 10  0 */

/* depth = 4,6,9,7, Total gates=17 */
static void sb5_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t07, t08, t09, t10, t12, t13, t15, t16, t01;
	t01          = in[4*0+i]   & in[4*3+i];
	t02          = in[4*2+i]   ^ t01;
	t03          = in[4*0+i]   ^ in[4*3+i];
	t04          = in[4*1+i]   & t02;
	t05          = in[4*0+i]   & in[4*2+i];
	out[4*0+i]   = t03       ^ t04;
	t07          = in[4*0+i]   & out[4*0+i];
	t08          = t01       ^ out[4*0+i];
	t09          = in[4*1+i]   | t05;
	t10          =     ~ in[4*1+i];
	out[4*1+i]   = t08       ^ t09;
	t12          = t10       | t07;
	t13          = out[4*0+i]   | out[4*1+i];
	out[4*3+i]   = t02       ^ t12;
	t15          = t02       ^ t13;
	t16          = in[4*1+i]   ^ in[4*3+i];
	out[4*2+i]   = t16       ^ t15; 
}

/* S6:   7  2 12  5  8  4  6 11 14  9  1 15 13  3 10  0 */

/* depth = 8,3,6,3, Total gates=19 */
static void sb6(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t07, t08, t09, t10, t11, t12, t13, t15, t17, t18, t01;
	t01          = in[4*0+i]   & in[4*3+i];
	t02          = in[4*1+i]   ^ in[4*2+i];
	t03          = in[4*0+i]   ^ in[4*3+i];
	t04          = t01       ^ t02;
	t05          = in[4*1+i]   | in[4*2+i];
	out[4*1+i]   =     ~ t04;
	t07          = t03       & t05;
	t08          = in[4*1+i]   & out[4*1+i];
	t09          = in[4*0+i]   | in[4*2+i];
	t10          = t07       ^ t08;
	t11          = in[4*1+i]   | in[4*3+i];
	t12          = in[4*2+i]   ^ t11;
	t13          = t09       ^ t10;
	out[4*2+i]   =     ~ t13;
	t15          = out[4*1+i]   & t03;
	out[4*3+i]   = t12       ^ t07;
	t17          = in[4*0+i]   ^ in[4*1+i];
	t18          = out[4*2+i]   ^ t15;
	out[4*0+i]   = t17       ^ t18; 
}

/* InvS6:  15 10  1 13  5  3  6  0  4  9 14  7  2 12  8 11 */

/* depth = 5,3,8,6, Total gates=19 */
static void sb6_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t07, t08, t09, t12, t13, t14, t15, t16, t17, t01;
	t01          = in[4*0+i]   ^ in[4*2+i];
	t02          =     ~ in[4*2+i];
	t03          = in[4*1+i]   & t01;
	t04          = in[4*1+i]   | t02;
	t05          = in[4*3+i]   | t03;
	t06          = in[4*1+i]   ^ in[4*3+i];
	t07          = in[4*0+i]   & t04;
	t08          = in[4*0+i]   | t02;
	t09          = t07       ^ t05;
	out[4*1+i]   = t06       ^ t08;
	out[4*0+i]   =     ~ t09;
	t12          = in[4*1+i]   & out[4*0+i];
	t13          = t01       & t05;
	t14          = t01       ^ t12;
	t15          = t07       ^ t13;
	t16          = in[4*3+i]   | t02;
	t17          = in[4*0+i]   ^ out[4*1+i];
	out[4*3+i]   = t17       ^ t15;
	out[4*2+i]   = t16       ^ t14; 
}

/* S7:   1 13 15  0 14  8  2 11  7  4 12 10  9  3  5  6 */

/* depth = 10,7,10,4, Total gates=19 */
static void sb7(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t05, t06, t08, t09, t10, t11, t13, t14, t15, t16, t17, t01;
	t01          = in[4*0+i]   & in[4*2+i];
	t02          =     ~ in[4*3+i];
	t03          = in[4*0+i]   & t02;
	t04          = in[4*1+i]   | t01;
	t05          = in[4*0+i]   & in[4*1+i];
	t06          = in[4*2+i]   ^ t04;
	out[4*3+i]   = t03       ^ t06;
	t08          = in[4*2+i]   | out[4*3+i];
	t09          = in[4*3+i]   | t05;
	t10          = in[4*0+i]   ^ t08;
	t11          = t04       & out[4*3+i];
	out[4*1+i]   = t09       ^ t10;
	t13          = in[4*1+i]   ^ out[4*1+i];
	t14          = t01       ^ out[4*1+i];
	t15          = in[4*2+i]   ^ t05;
	t16          = t11       | t13;
	t17          = t02       | t14;
	out[4*0+i]   = t15       ^ t17;
	out[4*2+i]   = in[4*0+i]   ^ t16; 
}

/* InvS7:   3  0  6 13  9 14 15  8  5 12 11  7 10  1  4  2 */

/* depth = 9,7,3,3, Total gates=18 */
static void sb7_inv(uint8_t* out, const uint8_t* in, uint8_t i){
	uint8_t t02, t03, t04, t06, t07, t08, t09, t10, t11, t13, t14, t15, t16, t01;
	t01          = in[4*0+i]   & in[4*1+i];
	t02          = in[4*0+i]   | in[4*1+i];
	t03          = in[4*2+i]   | t01;
	t04          = in[4*3+i]   & t02;
	out[4*3+i]   = t03       ^ t04;
	t06          = in[4*1+i]   ^ t04;
	t07          = in[4*3+i]   ^ out[4*3+i];
	t08          =     ~ t07;
	t09          = t06       | t08;
	t10          = in[4*1+i]   ^ in[4*3+i];
	t11          = in[4*0+i]   | in[4*3+i];
	out[4*1+i]   = in[4*0+i]   ^ t09;
	t13          = in[4*2+i]   ^ t06;
	t14          = in[4*2+i]   & t11;
	t15          = in[4*3+i]   | out[4*1+i];
	t16          = t01       | t10;
	out[4*0+i]   = t13       ^ t15;
	out[4*2+i]   = t14       ^ t16; 
}

typedef void(*sb_fpt)(uint8_t*, const uint8_t*, uint8_t i);

sb_fpt sf_tab[] PROGMEM = {
	sb0, sb1, sb2, sb3, 
	sb4, sb5, sb6, sb7
};

sb_fpt sinvf_tab[] PROGMEM = {
	sb0_inv, sb1_inv, sb2_inv, sb3_inv, 
	sb4_inv, sb5_inv, sb6_inv, sb7_inv
};

void sbox128(void * w, uint8_t box){
	uint8_t i, buffer[16];
	box &= 0x7;
	
	sb_fpt fp;
	fp = (sb_fpt)pgm_read_word(&(sf_tab[box]));
	for(i=0; i<4; ++i){
		fp(buffer, (uint8_t*)w, i);
	}
	memcpy(w, buffer, 16);
}

void inv_sbox128(void * w, uint8_t box){
	uint8_t i, buffer[16];
	box &= 0x7;
	
	sb_fpt fp;
	fp = (sb_fpt)pgm_read_word(&(sinvf_tab[box]));
	for(i=0; i<4; ++i){
		fp(buffer, (uint8_t*)w, i);
	}
	memcpy(w, buffer, 16);
}








