
//
//	hgmodel.cpp structures
//
#ifndef __hgmodel_h
#define __hgmodel_h

#include "geometry.h"

typedef enum {
HGMODEL_FLAG_NONE = 0,
HGMODEL_FLAG_POLY4,		// １枚ポリゴン(plate)
HGMODEL_FLAG_SMODEL,	// 三角ポリゴンモデル(mx,box,mesh...)
HGMODEL_FLAG_XMODEL,	// Xファイルモデル
HGMODEL_FLAG_2DSPRITE,	// 2Dスプライト
HGMODEL_FLAG_CAMERA,
HGMODEL_FLAG_LIGHT,
HGMODEL_FLAG_EFXMODEL,	// エフェクトモデル
HGMODEL_FLAG_OBAQ3D,	// OBAQ 3Dモデル
HGMODEL_FLAG_LINES,		// 線分モデル
HGMODEL_FLAG_MAX
} HGMODEL_FLAG;


typedef enum {
HGMODEL_ROTORDER_ZYX = 0,
HGMODEL_ROTORDER_XYZ,
HGMODEL_ROTORDER_YXZ
} HGMODEL_ROTORDER;


typedef struct {

	VECTOR pos;
	VECTOR rot;
	VECTOR scale;
	VECTOR dir;
	VECTOR efx;
	VECTOR work;

	VECTOR movetemp[6];		// 上のVECTORと同じ数だけの座標バッファ

	short attr;
	short tex;

	short ua_flag;			// UV Animation flag
	short ua_ofsx;			// UV Animation offset X
	short ua_ofsy;			// UV Animation offset Y
	short ua_opt;			// UV Animation option (not use)

	float	spr_x;			// X-axis (2D)
	float	spr_y;			// Y-axis (2D)

} HGMODEL_DRAWPRM;

typedef struct {
	float x,y,z;
	int color;
	float tu0,tv0;
} HGMODEL_EFXVERTEX;


//
//	hgmodel.cpp class
//

class hgmodel {
public:
	hgmodel();
	~hgmodel();
	void Terminate( void );
	void RegistData( HGMODEL_FLAG p_flag );
	void ExpandData( int size, int oldsize );

	void SetTexture( int id ) { deftex = id; };
	int GetTexture( void ) { return deftex; }
	void SetAttribute( int p ) { defattr = p; }
	int GetAttribute( void ) { return defattr; }
	HGMODEL_FLAG GetFlag( void ) { return flag; }
	void *GetData( void ) { return data; }
	void SetShadeMode( int p );
	void SetRotOrder( int p ) { rotorder = p; }
	int GetRotOrder( void ) { return rotorder; }

	void SetOffset( int x, int y ) { center_x=((float)x)*-1.0f; center_y=((float)y)*-1.0f; }
	void SetUV( int tx0, int ty0, int tx1, int ty1 );
	void SetDefaultSpeed( float fval ) { defspeed = fval; };
	float GetDefaultSpeed( void ) { return defspeed; };
	void SetDefaultColScale( float fval1, float fval2, float fval3 );
	float *GetDefaultColScale( void ) { return defcolscale; };
	void SetDefaultColParam( float fval1, float fval2, float fval3 );
	float *GetDefaultColParam( void ) { return defcolparam; };
	void SetDefaultTexture( int id ) { deftex = id; };
	int GetDefaultTexture( void ) { return deftex; };

	int AddPolyNode( void );
	int GetCurrentPolyNodeID( void );

	int Draw( HGMODEL_DRAWPRM *drawprm );

private:
	void DrawSprite( HGMODEL_DRAWPRM *drawprm );
	void DrawPoly4( HGMODEL_DRAWPRM *drawprm );
	void DrawPolyNode( HGMODEL_DRAWPRM *drawprm );
	float GetFloatColor( int cval );
	
public:
	//		Settings
	//
	int id;
	HGMODEL_FLAG flag;
	int	deftex;				// Default TexID
	int	defattr;			// Default Attribute
	float defspeed;			// Default Animation Speed
	float defcolscale[3];	// Default Collision Scale
	float defcolparam[3];	// Default Collision Param
	int	tpoly;				// num of polygons
	int cur;				// current ID of Poly4
	short shademode;		// Shading mode (1=on/0=off)
	short rotorder;			// Rotating order

	void *data;				// Vertex data
	float center_x;			// 2D Offset(X)
	float center_y;			// 2D Offset(Y)
	short uv[4];			// 2D UV

	float sizex, sizey;		// 2D UV Size

	short ua_max;			// UV Animation Max count
	short ua_cnt;			// UV Animation counter
	short ua_time;			// UV Animation timer
	short ua_deftime;		// UV Animation default timer

	short loadcnt;			// Model loading count
	short animcnt;			// Animation loading count

	short meshsx;			// Mesh size-X
	short meshsy;			// Mesh size-Y
};


#endif
