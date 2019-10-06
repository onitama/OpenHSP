
//
//	hgobj.cpp structures
//
#ifndef __hgobj_h
#define __hgobj_h

#include "hgio.h"

#define HGOBJ_FLAG_NONE 0
#define HGOBJ_FLAG_ENTRY 1
#define HGOBJ_FLAG_VISIBLE 2
#define HGOBJ_FLAG_MOVING 4

#define HGOBJ_MODE_HIDE 1
#define HGOBJ_MODE_TREE 2
#define HGOBJ_MODE_XFRONT 4
//#define HGOBJ_MODE_UVANIM 8
//#define HGOBJ_MODE_UVANIM_1SHOT 16
#define HGOBJ_MODE_MOVE 32
#define HGOBJ_MODE_FLIP 64
#define HGOBJ_MODE_BORDER 0x80
#define HGOBJ_MODE_2D 0x100
//#define HGOBJ_MODE_TIMER 0x200
#define HGOBJ_MODE_SORT 0x400
#define HGOBJ_MODE_STATIC 0x800
#define HGOBJ_MODE_GRAVITY 0x1000
//#define HGOBJ_MODE_ZPULL 0x2000
#define HGOBJ_MODE_LATE 0x4000
#define HGOBJ_MODE_FIRST 0x8000
#define HGOBJ_MODE_STAND 0x10000
#define HGOBJ_MODE_GROUND 0x20000
#define HGOBJ_MODE_LAND 0x40000
#define HGOBJ_MODE_LOOKAT 0x80000

#define HGOBJ_MODE_BOUND 0x100000
#define HGOBJ_MODE_ALIEN 0x200000
#define HGOBJ_MODE_WALKCLIP 0x400000
#define HGOBJ_MODE_EMITTER 0x800000
#define HGOBJ_MODE_AUTOROT 0x1000000

//#define HGOBJ_SHADE_LINES	0x80
//#define HGOBJ_SHADE_NOLIGHT	0x100
//#define HGOBJ_SHADE_TOON	0x200

//#define HGOBJ_MOVEMODE_NONE	0
//#define HGOBJ_MOVEMODE_EVENTMOV	1

enum {
MOC_POS = 0,
MOC_ANG,
MOC_SCALE,
MOC_DIR,
MOC_EFX,
MOC_WORK,
MOC_POS2,
MOC_ANG2,
MOC_SCALE2,
MOC_DIR2,
MOC_EFX2,
MOC_WORK2,
MOC_MAX
};

#define HGOBJ_MULTIEVENT_MAX 4

//
//	hgobj.cpp class
//
class hgobj {
public:
	hgobj();
	~hgobj();
	void Reset( void );

	void SetPos( float x, float y, float z ) { prm.pos.x=x;prm.pos.y=y;prm.pos.z=z; }
	void SetRot( float x, float y, float z ) { prm.rot.x=x;prm.rot.y=y;prm.rot.z=z; }
	void SetScale( float x, float y, float z ) { prm.scale.x=x;prm.scale.y=y;prm.scale.z=z; }
	void SetDir( float x, float y, float z ) { prm.dir.x=x;prm.dir.y=y;prm.dir.z=z; }
	void SetEfx( float x, float y, float z ) { prm.efx.x=x;prm.efx.y=y;prm.efx.z=z; }
	void SetWork( float x, float y, float z ) { prm.work.x=x;prm.work.y=y;prm.work.z=z; }
	void SetLandAxis( float y ) { landy = y; }
	void SetAttribute( int attr ) { prm.attr = attr; }
	void SetTexture( int p_id ) { prm.tex = p_id; }
	void SetSpeed( float fval ) { speed = fval; }

//	void SetViewMatrix( MATRIX *mat ) { prm.view = mat; }
//	void SetProjMatrix( MATRIX *mat ) { prm.proj = mat; }
//	void SetLight( VECTOR *vec ) { prm.lightvec = vec; }
//	void SetLightColor( VECTOR *color ) { prm.lightcolor = color; }
//	void SetLightAmbient( VECTOR *color ) { prm.ambcolor = color; }
	void SetUVAnimation( int xofs, int yofs ) { prm.ua_flag = 1; prm.ua_ofsx = xofs; prm.ua_ofsy = yofs; }
	void SetColiGroup( int my, int target ) { mygroup = my; colgroup = target; }
	void SetColScale( float fval1, float fval2, float fval3 );
	void SetColParam( float fval1, float fval2, float fval3 );
	void SetOptInfo( int p1, int p2, int p3 ) { optinfo[0]=p1; optinfo[1]=p2; optinfo[2]=p3; };
	void SetFloorOffsetY( float y ) { colofsy = y; }
	int GetDistance( VECTOR *v, float size );
	float GetDistance( VECTOR *v );
	int GetDistance2D( VECTOR *v, float size );
	float GetDistance2D( VECTOR *v );
	void SetLookObj( int id ) { lookobj = id; }
	int GetLookObj( void ) { return lookobj; }

	int GetEmptyEvent( void );
	void DeleteEvent( int entry );
	hgevent *GetEvent( int entry );
	void SetEvent( hgevent *ev, int entry );
	void StartEvent( hgevent *ev, int entry );
	void SetEmitter( int id );

	VECTOR *GetPos( void ) { return &prm.pos; }
	VECTOR *GetRot( void ) { return &prm.rot; }
	VECTOR *GetScale( void ) { return &prm.scale; }
	VECTOR *GetDir( void ) { return &prm.dir; }
	VECTOR *GetEfx( void ) { return &prm.efx; }
	VECTOR *GetWork( void ) { return &prm.work; }
	VECTOR *GetLandNormal( void ) { return &landnorm; }
	float GetLandAxis( void ) { return landy; }
//	MATRIX *GetMatrix( void ) { return &prm.mat; }
	int GetAttribute( void ) { return prm.attr; }
	int GetTexture( void ) { return prm.tex; }
	int GetColiGroup( void ) { return (int)mygroup; }
	int GetColiTarget( void ) { return (int)colgroup; }
	int *GetOptInfo( void ) { return optinfo; }

	VECTOR *GetVectorPrm( int id );
	void UpdateMatrix( void );
	void UpdateSortZ( void );
	void LookAt( VECTOR *target );
	void UpdateAnimation( float timepass );
	void ApplyAnimation( void );
	void StartAnimation( int anim );
	void NextAnimation( int anim );
	int GetAnimationEnd( void );

	void TreeAt( VECTOR *target );
	HGMODEL_DRAWPRM *GetPrm( void ) { return &prm; }

	//	Parameters (並び順をhgevent.hで参照している)
	//
	int		id;
	int		mode;

	short	flag;
	short	shade;
	short	timer;
	short	mygroup;
	short	colgroup;
	short	colinfo;			// collision info
	short	emitid;				// EmitterID
	short	drawflag;
	short	drawresult;
	short	matupdate;			// materix update flag
	short	evtimer;			// timer for event
	short	landobj;			// 接地オブジェクトID
	short	pushflg;			// collision push flag
	short	meshx,meshy;		// 接地メッシュ座標
	short	pullobj;			// 押し出されたオブジェクトID
	short	pushobj;			// 押し出したオブジェクトID

	void	*parent;			// parent object
	void	*child;				// child object
	void	*sibling;			// sibling object

	float	colscale[3];		// collision scale
	float	colprm[3];			// collision parameters
	float	time[ HGOBJ_MULTIEVENT_MAX ];
	float	speed;
	float	aniframe;

	float	landy;				// 接地面のY座標
	float	colofsy;			// モデル中心面からの接地オフセット
	VECTOR	landnorm;			// 接地面の法線
	int		lookobj;

	int		optinfo[3];			// 汎用オブジェクト情報

	hgmodel *model;
	float	sortz;				// ソート用Z座標
	short	sortdrawflag;
	short	sortoffset;

private:
	//	Settings
	//
	hgevent *event[ HGOBJ_MULTIEVENT_MAX ];			// Event List
	HGMODEL_DRAWPRM prm;

};


#endif
