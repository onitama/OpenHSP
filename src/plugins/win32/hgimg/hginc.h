//
//	hgimg system structures
//
#ifndef __hginc_h
#define __hginc_h
#include "geometry.h"

// プリミティブコード
enum {
PRIM_NONE = 0,
PRIM_PFILL,
PRIM_PPOINT,
PRIM_PLINE,
PRIM_PSPRITE,
PRIM_PBGSPR,
PRIM_POLY4F,
PRIM_POLY4T,
PRIM_POLY4FS,
PRIM_POLY4TS,
PRIM_POLY4FGS,
PRIM_POLY4TGS,
PRIM_POLY4EF,
PRIM_POLY4ET,
PRIM_LINE4F,
PRIM_LINE4T,
PRIM_LINE4FS,
PRIM_LINE4TS,
PRIM_LINE4FGS,
PRIM_LINE4TGS,
PRIM_LINE4EF,
PRIM_LINE4ET,
PRIM_MATSET,
PRIM_ATTRSET,
PRIM_MAX
};

// FILL primitive
typedef struct
{
short code;			// primitive code
short opt;			// option ID
int color;			// color code
IAXIS v[2];			// XY Vertices
} P_FILL;


// POINT primitive
typedef struct
{
short code;			// primitive code
short opt;			// option ID
int color;			// color code
int x,y;			// XY Vertices
} P_POINT;


// LINE primitive
typedef struct
{
short code;			// primitive code
short opt;			// option ID
int color;			// color code
IAXIS v[2];			// XY Vertices
} P_LINE;


// SPRITE primitive
typedef struct
{
short code;			// primitive code
short tex;			// texture ID
short attr;			// attr SW
short alpha;		// Alpha channel
float zx,zy;		// XY Scale
float rot;			// Z-Rotate
IAXIS2 v[2];		// XY Vertices
} P_SPRITE;


// BG primitive
typedef struct
{
short code;			// primitive code
short attr;			// attr SW
short tex;			// texture ID
short alpha;		// alpha blend value (opt)
char *vram;			// Virtual Vram image
short sx,sy;		// Vram Size X,Y
short cx,cy;		// Vram Chip Size X,Y
short wx,wy;		// Window Size X,Y
int vx,vy;			// Vram Pick point X,Y
int xx,yy;			// XY Vertices
} P_BGSPR;


// 四角形ポリゴンprimitive
#define POLY4N 4
typedef struct
{
short code;				// primitive code
short attr;				// attr SW
short tex;				// texture ID
short npoly;			// Num of polygon
IAXIS2 v[POLY4N];		// XY&UV Vertices
int color;				// color code
short alpha;			// alpha blend value (opt)
short specular;			// specular value (opt)
} POLY4;


// 四角形ポリゴンprimitive(enhance)
typedef struct
{
short code;				// primitive code
short attr;				// attr SW
short tex;				// texture ID
short npoly;			// Num of polygon
IAXIS2 v[POLY4N];		// XY&UV Vertices
int color[POLY4N];		// Vertices Color
short alpha;			// alpha blend value (opt)
short specular;			// specular value (opt)
} POLY4G;


// 四角形ポリゴンprimitive(T&L support)
typedef struct AXP4GX {
	float x,y,z;
	int color;
	float tu0,tv0;
}AXP4GX;

typedef struct
{
short code;				// primitive code
short attr;				// attr SW
short tex;				// texture ID
short alpha;			// alpha blend value (opt)
AXP4GX v[4];			// vertices info
} POLY4GX;


// マトリクス設定primitive
typedef struct
{
short code;				// primitive code
short id;				// matrix ID
MATRIX mat;				// matrix data
} P_MAT;


// マテリアル設定primitive
typedef struct
{
short code;				// primitive code
short prm;				// primitive parameter (word)
int	data;				// primitive parameter (long)
} P_PRM;


// 3Dノード情報
typedef struct
{
short code;				// primitive code
short attr;				// attribute code (material)
int color;				// color data
short tex;				// TexID
short num;				// num of VECTOR
VECTOR *fv;				// XYZ Vertices
VECTOR *fn;				// XYZ Normals
short num2;				// num of UV
IAXIS *uv;				// UV Vertices
void *exdata;			// Extra data field (option)
} PNODE;

//		attrコード
//
#define NODE_ATTR_COLKEY 0x8000
#define NODE_ATTR_USEALPHA 0x4000
#define NODE_ATTR_USEMATERIAL 0x400	// MaterialID(bit0-10)を参照する
#define NODE_ATTR_GLOBALMAT 0x800	// グローバルのMaterialIDを使用する
#define NODE_ATTR_SHADE_NONE 0
#define NODE_ATTR_SHADE_FLAT 1
#define NODE_ATTR_SHADE_GOURAUD 2
#define NODE_ATTR_SHADE_ADVANCED 3
#define NODE_ATTR_SHADE_LNONE 4
#define NODE_ATTR_SHADE_LFLAT 5
#define NODE_ATTR_SHADE_LGOURAUD 6
#define NODE_ATTR_SHADE_LADVANCED 7



// スキャンライン情報
typedef struct
{
int minX, maxX;		// left,right edge
int minTx, minTy;	// Tex UV of (minX,y)
int maxTx, maxTy;	// Tex UV of (maxX,y)
} SCANDATA;


#define TEXINF_MAX 512
// テクスチャ情報
typedef struct
{
short flag;			// enable flag
short mode;			// mode
short sx;			// x-size
short sy;			// y-size
short width;		// real x-size
short height;		// real y-size
char *data;			// real data
} TEXINF;



#endif
