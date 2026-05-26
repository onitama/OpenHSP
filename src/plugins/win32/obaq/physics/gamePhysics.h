#ifndef GAMEPHYSICS_H

//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

#define ENABLE_COLLISIONLOG

#define DISABLE_FLUID

const int RATE_PHYSICS = 4;

//
// spuvessel
//

const int VWIDTH = 192;
const int VHEIGHT = 108;

const int MAX_GRAIN = 1;		// 最大粒子数

#define VERINFO "OBAQ.DLL ver.090707\r\nCopyright 2009(C) Zener Works, Inc./kuni/onitama"


//#define SURFACE_GRAIN

#define setVpower(pG, pGi, sl, vt2) \
	if(((pG)->type & GRAIN::type_mud) && ((pGi)->type & GRAIN::type_mud) && (pG)->grainID == (pGi)->grainID) {\
		ssl = (1.8f - (sl)) / (sl) / 5.0f;\
		(vt2).x *= (0.04f * 2.0f);\
		(vt2).y *= (0.04f * 2.0f);\
	} else {\
		ssl = (2.0f - (sl)) / (sl) / 5.5f;\
		(vt2).x *= (0.02f * 2.0f);\
		(vt2).y *= (0.02f * 2.0f);\
	}

#define mutationGrainGrain(pG1, pG2, rct) \
	;

#define mutationGrainRock(md, pG, pR) \
	;

#define mutationRockGrain(pR, pG, vt) \
	if((pR)->buoyancy > 0.0f) {\
		(vt).x *= 4.0f;\
		(vt).y *= 4.0f;\
		(pR)->push((pG)->fpos.x, (pG)->fpos.y, (vt).x * (pR)->buoyancy, (vt).y * (pR)->buoyancy);\
	}

#define resistGrain(pG, gravity) \
	if(procCount == 0) {\
		(pG)->speed.x += (gravity).x * (2.0f * (float)RATE_PHYSICS);\
		(pG)->speed.y += (gravity).y * (2.0f * (float)RATE_PHYSICS);\
	}


//
// rock
//

#define MAX_ROCK			512
#define MAX_ROCK_AREA		8
#define MAX_ROCK_WIRE		2

#define MAX_WIRE			8
#define MAX_WIRE_POINT		32

#define MAX_ROCKBASE_AREAVERTEX	MAX_ROCK*86
#define MAX_ROCKBASE_VERTEX		MAX_ROCK*32
#define MAX_ROCKBASE_VERTEXW	MAX_ROCK*32

const float margin_default = 2.0f;
const float margin_collect = 4.0f;
const float margin_attract = 8.0f;

const float maxRockSpeed = 9.9f;


#define mutationRockRock(pR, pR2, d) \
	;

#endif
#define GAMEPHYSICS_H

