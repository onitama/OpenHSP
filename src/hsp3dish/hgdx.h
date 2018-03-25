
//
//	hgdx.cpp structures
//
#ifndef __hgdx_h
#define __hgdx_h

#include "hgio.h"
#include "hgio_dg.h"
#include "hgobj.h"

enum {
CAMMODE_NORMAL = 0,
CAMMODE_LOOKAT,
CAMMODE_LOOKOBJ,
CAMMODE_AUTOMOVE,
CAMMODE_MAX
};

//
//	hgdx.cpp class
//
class hgdx {
public:
	hgdx();
	~hgdx();
	int Reset( void );

	void Terminate( void );
	void SetDest( void *dest, int x, int y, int sx, int sy, int centerx=-1, int centery=-1 );
	void SetCamMode( int mode, int id = 0 );
	void GetBorder( VECTOR *v1, VECTOR *v2 );
	void SetBorder( float x0, float x1, float y0, float y1, float z0, float z1 );
	void SetViewPort( int x, int y, int sx, int sy );

	void DrawStart( int mode );
	int DrawEnd( void );
	int Sync( int wait );

#if 0
	int RegistTexture( char *buf, int srcx, int srcy, int sx, int sy, int sw );
	int RegistIndexTexture( char *buf, char *palette, int srcx, int srcy, int sx, int sy, int sw, int pals );
	int RegistTextureEmpty( int sx, int sy, int tmode );
	int RegistTextureFromFile( char *fname, int sx, int sy );
	int UpdateTexture( int tex, char *buf, int sw );
	void DeleteTexture( int id );
	void ClearTexture( int id, int color );
	void DrawTexString2( HWND hwnd, HFONT font, int tex, int x, int y, int color, char *str );

	void DrawTexOpen( HWND hwnd, char *name, int size, int opt, int id );
	void DrawTexString( int x, int y, int color, char *str );
	void DrawTexClose( void );
	void DrawTextOption( int space, int linespace, int mode );

	void DrawSquareEx( int tex, int color, int attr, int alpha, int *x, int *y, int *texx, int *texy );
	void DrawSpriteEx( int tex, int attr, int alpha, int x, int y, int sx, int sy, double ang, int tx0, int ty0, int tsx, int tsy );
	void DrawRectEx( int color, int alpha, int x, int y, int sx, int sy, double ang );
	void DrawMosaic( int x, int y, int sx, int sy, int divx, int divy, int alpha );
	void DrawLineEx( int color, int alpha, int x, int y, int x2, int y2 );

	void DrawSetBG( int color );
	void DrawSetBGTex( int tex );
	void DrawSetBGBlur( int color );

	void SetFixedFont( int texid, int x, int y, int px, int mode );
	void SetFixedFontAlpha( int alpha );
	void DrawFixedFont( int x, int y, char *mes );

	void SetBGSize( int x1, int y1, int x2, int y2 );
	int GetSurface( int x, int y, int sx, int sy, void *res, int mode );
	int SaveSurface( char *fname, int mode );

	void SetLandAxis( float y ) { landaxis = y; };
	void SetLandLimit( float y ) { landlimit = y; };
	void SetAllMove( int fl ) { allmove = fl; };
#endif

	//		Basic object process
	hgobj *GetObj( int id );
	hgmodel *GetModel( int id );
	hgobj *AddObj( void );
	hgobj *AddObj( int flag );
	hgmodel *AddModel( void );
	hgobj *AddLightObj( void );
	VECTOR *GetObjVectorPrm( int id, int prmid );
	void ScaleModel( int modelid, float sx, float sy, float az );
	void DeleteModel( int id );
	void DeleteObj( int id );
	void DrawObj( hgobj *obj );
	void DrawObjXMesh( hgobj *obj );
	void MoveObj( hgobj *obj, float timepass );
	int DrawObjAll( void );
	void RegistSortObj( hgobj *obj );
	void SortObjAll( void );
	void SortSwap(hgobj **a, hgobj **b);

	int AddObjWithModel( int modelid );
	int AttachObjWithModel( int objid, int modelid );
	void EnumObj( int group );
	int GetEnumObj( void );
	void ObjModeOn( int objid, int p1 ) { GetObj(objid)->mode |= p1; }
	void ObjModeOff( int objid, int p1 ) { GetObj(objid)->mode &= ~p1; }
	void SetObjChild( int parentid, int objid );
	void CutObjChild( int parentid );
	void SetTargetID( int id ) { target_id = id; }
	void SetBoundFactor( float val ) { boundfactor = val; }
	void SetGainFactor( float val, float val2 ) { gainfactor = val; gainfactor2 = val2; }

	//		Additional support
	int UpdateObjColi( int id, float size, int addcol );
	int FindObj( void );
	void StartObjFind( int mode, int group );
	int PutEventObj( int event, VECTOR *pos );
	int StartAnimation( int id, int anim, int mode );
	void SetObjSpeed( int id, float speed );
	void SetModelSpeed( int id, float speed );
	void SetObjLight( int id, int refid, int mode );
	void SetXInfo( hgobj *obj, VECTOR *vec, int id, int opt );
	int GetXInfo( hgobj *obj, char *buf, int id, int opt );
	int GetObjModelId( int objid );
	void SetModelColScale( int id, float scale1, float scale2 , float scale3 );
	void SetModelColParam( int id, float scale1, float scale2 , float scale3 );
	void SetObjProjMode( int id, int mode );
	float ObjAim( int id, int target, int mode, float x, float y, float z );
	int GetTimerTick( int type );
	void SetTimerTick( int val, double max );
	void SetFloorOffsetY( int id, float y );
	int ObjWalk( int id, VECTOR *res, VECTOR *dir );
	int ObjWalk( hgobj *obj, VECTOR *res, VECTOR *dir );
	void GetFloorVector( int id, int mode, VECTOR *vec );
	void GetFloorVectorInt( int id, int mode, int *vec );
	int GetNearestObj( int id, float range, int colgroup );
	float GetGroundY( hgobj *o );
	void ConvertAxis( VECTOR *res, VECTOR *pos, int mode );
	void GroupModify( int group, int param, int calcopt, VECTOR *data );

	//		Event process
	int GetEmptyEventId( void );
	hgevent *GetEvent( int eventid ) { return mem_event[eventid]; }
	hgevent *AddEvent( int eventid );
	void DeleteEvent( int eventid );
	int AddWaitEvent( int eventid, int frame );
	int AddMoveEvent( int eventid, int target, float x, float y, float z, int frame, int sw );
	int AddSplineMoveEvent( int eventid, int target, float x, float y, float z, int frame, int sw );
	int AddPlusEvent( int eventid, int target, float x, float y, float z );
	int AddChangeEvent( int eventid, int target, float x1, float y1, float z1, float x2, float y2, float z2 );
	int AddJumpEvent( int eventid, int gonum, int rate );
	int AddParamEvent( int eventid, int mode, int target, int param );
	int AddUVEvent( int eventid, int ysize, int count );
	int AddRegobjEvent( int eventid, int model, int event );
	int AddEPrimEvent( int eventid, int model, int prm, float x1, float y1 );
	int AddAimEvent( int eventid, int target, int mode, float x, float y, float z );
	int AddAnimEvent( int eventid, int anim, int opt, float speed );
	void ExecuteObjEvent( hgobj *obj, float timepass, int entry );
	int AttachEvent( int objid, int eventid, int entry );

	//		Model Process
	int AddSpriteModel( int tex, int attr, int tx0, int ty0, int tx1, int ty1 );
	int AddPlateModel( int tex, int attr, int color, float sx, float sy, int tx0, int ty0, int tx1, int ty1 );
	int AddPlateNode( int modelid, int tex, int attr, int color, float sx, float sy, int mode );
	void AddPlateNodeSub( int modelid, int node, float sx, float sy, VECTOR *pos, VECTOR *ang );
	int AddMeshNode( int modelid, int tex, int attr, int color, float sx, float sy, int divx, int divy, float uv0, float uv1 );
	int AddDxfNode( int modelid, int color, int mode, void *fileptr );
	void ModifyMeshNodeY( int modelid, int *data, int mode=0, float pow=0.01f );
	void BuildMeshNodeMapUV( int modelid, int *data=NULL, int mode=0 );

	//		Efx Process
	int AddEfxModel( int modelid, int mode, int option );
	void SetEfxParam( int modelid, int param, float value );
	void SetEfxParamColor( int modelid, int param, int value );

	//		XMesh Process
	int AddXMesh( int modelid, char *fname, int mode );
	int SetTextureAnim( int modelid, int id, int subid, char *texname );
	int SetTextureAnimMode( int modelid, int id, int mode, int tex );
	int AddTextureAnim( int modelid, char *fname, int p_wait, int p_loopwait, int bgy );

	//		For Mosaic
	void InitMosaic( int x, int y, int sx, int sy, int divx, int divy, int mode );
	void TermMosaic( void );
	void DrawMosaic( int alpha );

	//		For Emitter
	int GetEmptyEmitterId( void );
	int AddEmitter( int id, int mode, int num );
	void DeleteEmitter( int id );
	hgemitter *GetEmitter( int emitid );
	void SetObjEmitter( int id, int emitid );

	//		Info Data
	//
	int	worktime;
	unsigned long TotalPoly;
	unsigned long OldTime;
	int vramleft;

private:
	//		for internal use
	void DeleteObjSub( int id );
	void SetPolyAxis( POLY4 *poly, int num, int x, int y, int tx, int ty );
	void InitXRender( void );
	void Init2DRender( int mode );
	void UpdateTime( void );
	void UpdateLight( void );
	void UpdateCamera( void );

	void InsertList( int eventid, hgevent *pEvent );
	void DeleteList( int eventid, hgevent *pEvent );
	float GetTimerFromFrame( int frame );
	void StoreNextVector( hgevent *myevent );


	//		Settings
	//
	int hgmode;
	int dest_flag;
	float center_x, center_y;

	int bgclr;			// 背景消去フラグ(0=none)
	int bgcolor;		// 背景色コード
	int bgtex;			// 背景テクスチャコード

	int nDestWidth;		// 描画座標幅
	int nDestHeight;	// 描画座標高さ

	int mestex;			// Font Message TexID
	int mescx;			// Font Message X-size
	int mescy;			// Font Message X-size
	int mespx;			// Font Message X-Forward
	int mesmode;		// Font Message Mode(0=Normal/1=Trans)

	int fntmode;		// 2Dフォントモード
	int fntalpha;		// 2Dフォントalpha値

	int tmes_spacing;	// テクスチャフォント描画spacing
	int tmes_lspacing;	// テクスチャフォント描画行間
	int	tmes_mode;		// テクスチャフォント描画mode

	int bgsizex, bgsizey;
	int bgcsizex, bgcsizey;

	int *mozcolors;
	int mozx,mozy,mozsx,mozsy,mozdx,mozdy,mozmode;

	int	enum_group;
	int enum_id;

	float landaxis;		// 地面のY座標
	float landlimit;	// 地面の上限Y座標
	float boundfactor;	// バウンド係数
	float gainfactor;	// 抵抗係数(空中)
	float gainfactor2;	// 抵抗係数(地上)
	int	target_id;

	//		Data Area
	//
	POLY4 mem_poly4;	// 2D描画座標の一時バッファ

	//		3D Data
	//
	int maxobj;
	int maxmodel;
	int maxevent;
	int maxemitter;
	hgobj **mem_obj;
	hgmodel **mem_model;
	hgevent **mem_event;
	hgemitter **mem_emitter;

	int maxsortobj;
	hgobj **mem_sortobj;	// ソート用のオブジェクトデータ

	//		System Data
	//
	hgobj *camera;
	hgobj *light;
	VECTOR border1;		// BORDER座標1
	VECTOR border2;		// BORDER座標2

/*
	D3DXMATRIX mProjection;
	D3DXMATRIX mProjection2;
	D3DXMATRIX mView;
	D3DVIEWPORT8 viewDataOrg;
	D3DVIEWPORT8 viewData;
*/
	int cammode;
	int allmove;

	int objfind_exmode;
	int objfind_group;
	int objfind_id;

};


#endif
