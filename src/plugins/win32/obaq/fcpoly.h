//
//	fcpoly system structures
//
#ifndef __fcpoly_h
#define __fcpoly_h

#define PI 3.14159265358979f
#define PI2 6.28318530717958f
#define HPI 1.570796326794895f

// 整数AXIS2型(UV付き)
typedef struct {
short x,y,tx,ty;
} IAXIS2;

#define setIAXIS( a,b,c ) a.x=(int)b;a.y=(int)c;
#define setIAXIS2( a,b,c,d,e ) a.x=(int)b;a.y=(int)c;a.tx=(int)d;a.ty=(int)e;

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


//		attrコード
//
#define NODE_ATTR_COLKEY 0x8000

// スキャンライン情報
typedef struct
{
int minX, maxX;		// left,right edge
int minTx, minTy;	// Tex UV of (minX,y)
int maxTx, maxTy;	// Tex UV of (maxX,y)
} SCANDATA;


//
//	function prototypes
//
void hgiof_init( void );
void hgiof_term( void );
void SetPolyDest( void *dest, int sx, int sy );
void SetPolySource( void *src, int sx, int sy );
void SetPolyColorKey( int color );
void DrawPolygonF4( POLY4 *lpPolyData );
void DrawPolygonF4P( POLY4 *lpPolyData );
void DrawPolygonTex( POLY4 *lpPolyData );
void DrawPolygonTexP( POLY4 *lpPolyData );

void TexInit( void );
void TexTerm( void );



#endif
