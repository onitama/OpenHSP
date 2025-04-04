
//
//	hgdraw.cpp structures
//
#ifndef __hgdraw_h
#define __hgdraw_h

#define OT_ALL 2048
#define OT_RANGE 1024

// アニメーション情報
typedef struct
{
short flag;			// enable flag(0=end/1=go/2=ret)
short time;			// times
float sx,sy,sz;		// scale
float rx,ry,rz;		// rotate
float tx,ty,tz;		// translate
} ANMINF;


// モデルアニメーション情報
typedef struct
{
int mode;			// default mode
short timer;		// default timer
short mygroup;		// my collision group (16bit)
short colgroup;		// target collision group (16bit)
short opt;			// (No use)

short animax[16];	// Animation Frame Max Index (option)
short aniopt[16];	// Animation Frame Option (option)

short ua_mode;		// UV-Animation Default Mode
short ua_wait;		// UV-Animation Default WaitTime
short ua_times;		// UV-Animation Default Times
short uva_py;		// UV-Animation Default Y-Size
} MDLANM;


#define LIGHT_MAX 4

// ライト情報
typedef struct
{
short flag;			// enable flag(0=none/1=enable)
short mode;			// mode (0=direct/1=point/2=spot)
int	id;				// moc ID ( pos,dir,color,amb,falloff )
float x,y,z;		// pre-calc value
short color[4];		// pre-calc light color
short ambient[4];	// pre-calc ambient color
} LIGHTINF;


#define MDL_NODE_MAX 16

// シーンモデル情報
typedef struct
{
VECTOR pos;				// local Position
VECTOR ang;				// local Angle
VECTOR scale;			// local Scale
short id;				// Model ID
short opt;				// option (no use)
short num;				// num of NODE
short nummax;			// Maximum num of NODE(0=none)
PNODE **node;			// Node ptr
void *sibling;			// Sibling Object
void *child;			// Child Object
float bound;			// Bounding Box Max Length

short aindex;			// Animation Max Index ( 0 = No Anim)
short anum;				// Animation Frame Max ( 0 = No Anim )
ANMINF *ani;			// Animation Data Array (option)
short anifr[16];		// Animation Frame Start Index (option)
short animax[16];		// Animation Frame Max Index (option)

} MODEL;


// シーンマスターモデル情報
typedef struct
{
MODEL *model;			// Base model for tree
MODEL *sub[62];			// Sub model for tree
int num;				// number of models ( include bone )
} MODBASE;


// シーンのマクロ
enum {
SCN_FLAG_OFF = 0,
SCN_FLAG_ON,
};

enum {
SCN_MOC_POS = 0,
SCN_MOC_ANG,
SCN_MOC_SCALE,
SCN_MOC_DIR,
SCN_MOC_EFX,
SCN_MOC_MAX
};

enum {
CAM_MODE_NORMAL = 0,
CAM_MODE_LOOKAT,
CAM_MODE_MAX
};

enum {
EVENT_MODEL_BOM = 0,
EVENT_MODEL_BOM2,
EVENT_MODEL_BOM3,
EVENT_MODEL_MAX
};

enum {
LIGHT_MOC_POS = 0,
LIGHT_MOC_ANG,
LIGHT_MOC_COLOR,
LIGHT_MOC_AMBIENT,
LIGHT_MOC_FALLOFF,
LIGHT_MOC_MAX
};

#define OBJ_MODE_HIDE 1
#define OBJ_MODE_TREE 2
#define OBJ_MODE_XFRONT 4
#define OBJ_MODE_UVANIM 8
#define OBJ_MODE_UVANIM_1SHOT 16
#define OBJ_MODE_MOVE 32
#define OBJ_MODE_FLIP 64
#define OBJ_MODE_BORDER 0x80
#define OBJ_MODE_2D 0x100
#define OBJ_MODE_TIMER 0x200
#define OBJ_MODE_WIPEBOM 0x400
#define OBJ_MODE_NOSORT 0x800
#define OBJ_MODE_GRAVITY 0x1000
#define OBJ_MODE_SKY 0x2000
#define OBJ_MODE_GROUND 0x4000

// シーンオブジェクト情報
typedef struct
{
short flag;				// enable flag (0=none)
short mid;				// Model ID (-1=N/A)

int mode;				// mode flag

short mocnum;			// hold num of MOC
short mocid;			// MOC ID
MODEL *model;			// model ptr

short aniflag;			// Model Animation Flag ( 0=OFF )
short anim;				// Model Animation ID
int aniframe;			// Model Animation Frame

short mygroup;			// my collision group (16bit)
short colgroup;			// target collision group (16bit)
short colinfo;			// collision info
short timer;			// count down timer to death
short otbase;			// OT Base
short opt;				// (no use)

short ua_ofs;			// UV Animation offset
short ua_sy;			// UV Animation offset U size
short ua_max;			// UV Animation Max count
short ua_cnt;			// UV Animation counter
short ua_time;			// UV Animation timer
short ua_deftime;		// UV Animation default timer
} SCNOBJ;


// オブジェクト検索情報
typedef struct
{
	int id;				// Scan ObjID
	int model;			// Scan ModelID
	int subm;			// Scan SubModelID
	int node;			// Scan NodeID
	int zot;			// Scan Z-OT
	int mode;			// Scan Mode (0=2DPoint/1=3DLine)
	VECTOR result;		// Scan Result (Collision Point)
	VECTOR point;		// Scan target point
	VECTOR vector;		// Scan target vector ( for line )
} SCANOBJ;


//
//	hgdraw.cpp functions
//

class hgdraw {
public:
	HGProc io;			// hgio interface
	hgdraw();
	~hgdraw();
	int Startup( int mode );
	void Startup2( void );
	void Terminate( void );
	void Restart( void );
	void releaseDest( void );
	void setDest( void *dest, int sx, int sy );
	void setSrc( void *src, int sx, int sy );
	void Reset( void );

	int GetTimerResolution( void );
	int GetTimerCount( void );
	void Sync( int sync );
	void SetBorder( float x0, float x1, float y0, float y1, float z0, float z1 );
	void GetBorder( VECTOR *v1, VECTOR *v2 );
	void SetZCalc( int otdiv, int otofs );
	void SetUVAnimSize( int sx, int sy );

	void DrawStart( void );
	int DrawEnd( void );
	void UpdateScreen( void );
	void DrawPrim( void *prim );
	void DrawOT( void );
	void DrawOTSync( void );
	void DrawSetBG( int color );
	void DrawSetBGTex( int tex );
	void DrawSetBGBlur( int color );

	void *MakePrim( int code );
	void AddPrim( void *prim, int ot );
	void CopyPrimPacket( void *prim, int ot );

	PNODE *MakeNode( int code, int vnum, int uvs );
	void *SetNodeExData( PNODE *n, int size );
	void DeleteNode( PNODE *n );
	void NodeToPrim( PNODE *n );
	void MakePlaneNode( MODEL *mdl, int code, float sx, float sy, int tx0, int ty0, int tx1, int ty1, VECTOR *pos, VECTOR *ang, int sw );
	void MakePlaneNodeF( MODEL *mdl, int code, float sx, float sy, int color, VECTOR *pos, VECTOR *ang, int sw );
	void MakeBoxNode( MODEL *mdl, float sx, float sy, int tx0, int ty0, int tx1, int ty1, int sw );
	void MakeBoxNodeF( MODEL *mdl, float sx, float sy, int color, int sw );
	void MakeSpriteNode( MODEL *mdl, int tx0, int ty0, int tx1, int ty1, int sw );
	void MakeBGNode( MODEL *mdl, int sx, int sy, int wx, int wy, int cx, int cy, int sw );
	void MakeMeshNode( MODEL *mdl, float sx, float sy, int divx, int divy, int tx, int ty, int sw );

	int RegistModel( void );
	MODEL *MakeModel( void );
	MODEL *GetModel( int id );
	PNODE *GetModelNode( MODEL *m, int nodeid );
	void DeleteModel( int id );
	void SetModelAnimFrame( int id, int frame, int opt );
	MDLANM *GetModelAnimFrame( int id );
	void SetModelPos( MODEL *m, VECTOR *v );
	void SetModelAng( MODEL *m, VECTOR *v );
	void SetModelScale( MODEL *m, VECTOR *v );
	void SetModelChild( MODEL *m, MODEL *child );
	void SetModelSibling( MODEL *m, MODEL *sibling );
	int SetModelTex( MODEL *m, int nodeid, int tex );
	void SetModelNode( MODEL *m, PNODE *n );
	void SetModelAnim( MODEL *m, ANMINF *ani, int frame );
	void DrawModel( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx );
	void DrawModelAnim( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx );
	void DrawModel2D( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx );
	P_BGSPR *GetBG( MODEL *m );
	char *GetBGVram( MODEL *m );
	void SetBGView( MODEL *m, int x, int y );
	int UpdateModelAnim( int id, int anim, int frame );
	void SetModelAnimDef( int id, int ua_mode, int ua_wait, int ua_times );
	void SetModelModeDef( int id, int mode );
	void SetModelTimerDef( int id, int timer );
	void ModelOffset( int modelid, VECTOR *offset );
	void ModelSetShade( int modelid, int mode );
	void ModelScale( int modelid, VECTOR *scale );

	SCNOBJ *GetObj( int id ) { return &mem_obj[id]; };
	int RegistObj( int mode, MODEL *model );
	int RegistObj( int mode, int modelid );
	void SetObjModel( int id, int modelid );
	int GetObjModel( int id );
	void DeleteObj( int id );
	void DeleteObj( SCNOBJ *o );
	void DrawObj( int id, int sw );
	void DrawObjAll( void );
	void UpdateViewMatrix( void );
	void UpdateObjAll( void );
	int GetObjMoc( int id );
	void SetObjPos( int id, VECTOR *v );
	void SetObjAng( int id, VECTOR *v );
	void SetObjScale( int id, VECTOR *v );
	void SetObjDir( int id, VECTOR *v );
	void SetObjEfx( int id, VECTOR *v );
	void GetObjPos( int id, VECTOR *v );
	void GetObjAng( int id, VECTOR *v );
	void GetObjScale( int id, VECTOR *v );
	void GetObjDir( int id, VECTOR *v );
	void GetObjEfx( int id, VECTOR *v );
	void GetObjColi( int id, int *mygroup, int *enegroup );
	void SetObjColi( int id, int mygroup, int enegroup );
	int GetObjDistance( SCNOBJ *o, VECTOR *v, float size );
	int UpdateObjColi( int id, float size );
	int FindObj( void );
	void StartObjFind( int mode, int group );
	int PutEventObj( int event, VECTOR *pos );

	int UpdateUVAnim( SCNOBJ *o );
	void SetObjUVAnim( int id, int mode, int wait, int times, int py );
	void SetObjAnim( int id, int anim );
	void SetObjMode( int id, int mode, int sw );
	void SetEventModel( int id, int model );

	void RegistCam( void );
	void DeleteCam( void );
	int GetCamMoc( void );
	void UpdateCam( void );
	void CamProc( void );
	void SetCamPos( VECTOR *pos );
	void SetCamAng( VECTOR *ang );
	void CalcCamInt( void );
	void SetCamMode( int mode );
	void GetTargetAngle( VECTOR *ang, VECTOR *src, VECTOR *target );

	void SetFont( int tex, int cx, int cy, int px, int mode );
	void DrawFont( int x, int y, int alp, char *mes );
//	void DrawFontEx( int x, int y, char *mes, char *buf, int sx, int sy );

	MODEL *GetModelEx( int id, int sub );
	int RebuildModelTree( int id, char *output );
	void ArrangeModelTree( int modelid, int subid, int newid, int mode );
	int GetModelSubID( int id, MODEL *model );
	int GetModelSubMax( int id );
	void ProcessObj( int id );
	void *ProcessObjAll( int mode, float x, float y, float z );
	void MakeModelNormals( int modelid, int mode );
	void MakeNodeNormals( MODEL *m, int node, int mode );

	void SetModelTexEx( MODEL *m, int nodeid, int tex, int shade );
	void SetModelUV( MODEL *m, int nodeid, int *uv );
	void GetModelUV( MODEL *m, int nodeid, int *uv );
	void GetModelVector( MODEL *m, int nodeid, int mode, float *axis );
	void SetModelVector( MODEL *m, int nodeid, int mode, float *axis );

	LIGHTINF *GetLight( int id );
	int inline CalcLight( VECTOR *normal, int color );
	void InitLight( void );
	void DeleteLight( void );
	void ResetLight( void );
	void UpdateLightSub( int id );
	void UpdateLight( void );
	void SetLightParam( int id, int type, VECTOR *v );
	int GetLightMoc( int id );

	//		for Debug
	//
	int	debug;
private:
	//		Settings
	//
	int hgmode;
	int dest_flag;
	int model_buf_max;
	int scnobj_buf_max;

	VECTOR center;		// 画面の中心点(3D)
	int bgclr;			// 背景消去フラグ(0=none)
	int bgcolor;		// 背景色コード
	int bgopt;			// (no use)
	int bgtex;			// 背景テクスチャコード
	int syncwait;		// 更新待ち時間(*10ms)
	int syncflag;		// 処理落ちフラグ

	float proj;			// 画角
	float xyproj;		// 画角のXY補正値
	int cammode;		// カメラモード(0=normal/1=int)
	int cammoc;			// カメラのmocid
	int cammocnum;		// カメラのmoc数
	MATRIX m_camera;	// カメラのマトリクス
	MATRIX m_view;		// ビューマトリクス
	float otzv;			// OTZのための補正値
	float clipznear;	// Near Clip Z
	int clipzf;			// Far Clip Z(OT)
	int otofs;			// OTのオフセット

	VECTOR vec2d;		// 2D表示用オフセット
	VECTOR ang2d;		// 2D表示用スケールXY,Z-Rot

	void *pkcurrent;	// sync中のパケット

	int nDestWidth;		// 描画座標幅
	int nDestHeight;	// 描画座標高さ


	//		Data Area
	//
	SCNOBJ *mem_obj;
	MODBASE *mem_model;
	MDLANM *mem_manim;
	LIGHTINF light[LIGHT_MAX];

	//		Work Area
	//
	int	uoffset;		// UVアニメーション用オフセット
	VECTOR border1;		// BORDER座標1
	VECTOR border2;		// BORDER座標2
	int aniflag;		// Model Animation Flag ( 0=OFF )
	int curanim;		// current Animation ID
	int curframe;		// current Animation Frame
	int evmodel[EVENT_MODEL_MAX];	// Event Model ID
	int uva_px;			// UV Anim X size Default
	int uva_py;			// UV Anim Y size Default
	int mestex;			// Font Message TexID
	int mescx;			// Font Message X-size
	int mescy;			// Font Message X-size
	int mespx;			// Font Message X-Forward
	int mesmode;		// Font Message Mode(0=Normal/1=Trans)
	int objfind_exmode;
	int objfind_group;
	int objfind_id;
	int tree_depth;

	//		Draw Material Info
	//
	int	drawmat_alpha;		// bit0-7:val
							// bit8-9: 0=alpha blending
							//         1=n/c
							//         2=modulate
							//         3=substract
	int	drawmat_efx1;		// EFX1 flag (0=none)
	int	drawmat_efx2;		// EFX1 flag (0=none)
	int	drawmat_efx3;		// EFX1 flag (0=none)

	SCANOBJ seobj;		// SCANOBJ for scan
	SCANOBJ seres;		// SCANOBJ for scan (result)

	void DrawModelSub( MODEL *m );
	void DeleteModelSub( MODEL *m );
	void ModelOffsetSub( MODEL *m, VECTOR *offset );
	void ModelScaleSub( MODEL *m, VECTOR *scale );
	void ModelSetShadeSub( MODEL *m, int mode );
	int ProcessNode( PNODE *n );
	void ProcessModelSub( MODEL *m );
	void RebuildModelTreeSub( MODBASE *base, MODEL *m, char *output );

};


#endif
