#ifndef TYPES_H

//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

struct FVECTOR2 {
	float	x;
	float	y;
};

struct CVECTOR2 {
	char	x;
	char	y;
};

struct SVECTOR2 {
	short	x;
	short	y;
};

struct FSEQ {
	float	a;
	float	b;
	float	c;

	float	l;
	float	l2;
};

const float F_PI = 3.141592653f;

const unsigned long	PS3_PAD_UP			= (0x01 << 0);	//	↑
const unsigned long	PS3_PAD_DOWN		= (0x01 << 1);	//	↓
const unsigned long	PS3_PAD_LEFT		= (0x01 << 2);	//	←
const unsigned long	PS3_PAD_RIGHT		= (0x01 << 3);	//	→
const unsigned long	PS3_PAD_SQUARE		= (0x01 << 4);	//	□
const unsigned long	PS3_PAD_CROSS		= (0x01 << 5);	//	×
const unsigned long	PS3_PAD_CIRCLE		= (0x01 << 6);	//	○
const unsigned long	PS3_PAD_TRIANGLE	= (0x01 << 7);	//	△
const unsigned long	PS3_PAD_START		= (0x01 << 8);	//	スタート
const unsigned long	PS3_PAD_SELECT		= (0x01 << 9);	//	セレクト
const unsigned long	PS3_PAD_L1			= (0x01 << 10);	//	Ｌ１
const unsigned long	PS3_PAD_R1			= (0x01 << 11);	//	Ｒ１
const unsigned long	PS3_PAD_L2			= (0x01 << 12);	//	Ｌ２
const unsigned long	PS3_PAD_R2			= (0x01 << 13);	//	Ｒ２
const unsigned long	PS3_PAD_L3			= (0x01 << 14);	//	Ｌ３
const unsigned long	PS3_PAD_R3			= (0x01 << 15);	//	Ｒ３

#endif
#define TYPES_H
