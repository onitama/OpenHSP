
//
//		High performance Graphic Image access (draw main)
//			onion software/onitama 2000/11
//
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <math.h>

#include "hgcnf.h"
#include "hginc.h"

#ifdef HGIMG_DIRECTX
	#include "../hgimgx/hgiox.h"
#else
	#include "hgio.h"
	#include "hgiof.h"
#endif

#include "hgdraw.h"
#include "packet.h"
#include "moc.h"
#include "sysreq.h"

// プリミティブの構造体サイズリスト
/*
#ifdef HGIMG_DIRECTX
static primsize[] = {
0,					//	PRIM_NONE
sizeof(P_FILL),		//	PRIM_PFILL
sizeof(P_POINT),	//	PRIM_PPOINT
sizeof(P_LINE),		//	PRIM_PLINE
sizeof(P_SPRITE),	//	PRIM_PSPRITE
sizeof(P_BGSPR),	//	PRIM_BGPSP
sizeof(POLY4GX),	//	PRIM_POLY4F
sizeof(POLY4GX),	//	PRIM_POLY4T
sizeof(POLY4GX),	//	PRIM_POLY4FS
sizeof(POLY4GX),	//	PRIM_POLY4TS
sizeof(POLY4GX),	//	PRIM_POLY4FGS
sizeof(POLY4GX),	//	PRIM_POLY4TGS
sizeof(POLY4GX),	//	PRIM_POLY4EF
sizeof(POLY4GX),	//	PRIM_POLY4ET

sizeof(POLY4GX),	//	PRIM_LINE4F
sizeof(POLY4GX),	//	PRIM_LINE4T
sizeof(POLY4GX),	//	PRIM_LINE4FS
sizeof(POLY4GX),	//	PRIM_LINE4TS
sizeof(POLY4GX),	//	PRIM_LINE4FGS
sizeof(POLY4GX),	//	PRIM_LINE4TGS
sizeof(POLY4GX),	//	PRIM_LINE4EF
sizeof(POLY4GX),	//	PRIM_LINE4ET

sizeof(P_MAT),		//	PRIM_MATSET
sizeof(P_PRM),		//	PRIM_ATTRSET
};
#else
*/
static primsize[] = {
0,					//	PRIM_NONE
sizeof(P_FILL),		//	PRIM_PFILL
sizeof(P_POINT),	//	PRIM_PPOINT
sizeof(P_LINE),		//	PRIM_PLINE
sizeof(P_SPRITE),	//	PRIM_PSPRITE
sizeof(P_BGSPR),	//	PRIM_BGPSP
sizeof(POLY4),		//	PRIM_POLY4F
sizeof(POLY4),		//	PRIM_POLY4T
sizeof(POLY4),		//	PRIM_POLY4FS
sizeof(POLY4),		//	PRIM_POLY4TS
sizeof(POLY4G),		//	PRIM_POLY4FGS
sizeof(POLY4G),		//	PRIM_POLY4TGS
sizeof(POLY4G),		//	PRIM_POLY4EF
sizeof(POLY4G),		//	PRIM_POLY4ET

sizeof(POLY4),		//	PRIM_LINE4F
sizeof(POLY4),		//	PRIM_LINE4T
sizeof(POLY4),		//	PRIM_LINE4FS
sizeof(POLY4),		//	PRIM_LINE4TS
sizeof(POLY4G),		//	PRIM_LINE4FGS
sizeof(POLY4G),		//	PRIM_LINE4TGS
sizeof(POLY4G),		//	PRIM_LINE4EF
sizeof(POLY4G),		//	PRIM_LINE4ET

sizeof(P_MAT),		//	PRIM_MATSET
sizeof(P_PRM),		//	PRIM_ATTRSET
};
//#endif


static void DebugMes( char *mes )
{
	MessageBox( NULL, mes, "error",MB_ICONINFORMATION | MB_OK );
}

/*
	{
	char st[128];
	sprintf( st,"" );
	MessageBox( NULL, st, "error",MB_ICONINFORMATION | MB_OK );
	}
*/

/*------------------------------------------------------------*/
/*
		mmTimer
*/
/*------------------------------------------------------------*/

//-------------------------------------------------------------
//		Sync Timer Routines
//-------------------------------------------------------------

static int	timecnt;
static int	timeres;
static int	timerid;

//
// TimerFunc --- タイマーコールバック関数
//
static void CALLBACK TimerFunc( UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	timecnt++;
}

//
// 高精度タイマーを開始します
//
static void StartTimer( int val )
{
	int sync;
	TIMECAPS caps;
	timeGetDevCaps( &caps, sizeof(caps) );
	timeres=caps.wPeriodMin;
	sync = (val+timeres-1) / timeres;
	sync *= timeres;

	timeBeginPeriod( caps.wPeriodMin );
	timerid =
      timeSetEvent( sync,
					caps.wPeriodMin,
					TimerFunc,
					0,
					(UINT)TIME_PERIODIC );
}

//
// 高精度タイマーを停止します
//
static void StopTimer( void )
{
	TIMECAPS caps;
	if( timerid != 0 )
	{
		timeKillEvent( timerid );
		timerid = 0;
		timeGetDevCaps( &caps, sizeof(caps) );
		timeEndPeriod( caps.wPeriodMin );
   }
}

int hgdraw::GetTimerResolution( void )
{
	return timeres;
}

int hgdraw::GetTimerCount( void )
{
	return timecnt;
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int hgdraw::Startup( int mode )
{
	//		Graphics I/O初期化
	//
	dest_flag = 0;
	hgmode = mode;
#ifdef HGIMG_DIRECTX
		::hgiox_init( &io, GetSysReq(SYSREQ_DXMODE), GetSysReq(SYSREQ_DXWIDTH), GetSysReq(SYSREQ_DXHEIGHT), (HWND)GetSysReq(SYSREQ_DXHWND) );
#else
	switch( hgmode ) {
	case 0:
		::hgiof_init( &io );
		break;
	case 1:
		::hgio_init( &io );
		break;
	case 2:
		break;
	}
#endif

	//		timerの初期化
	//
	StartTimer( 1 );

	//		Startup phase2
	//
	Startup2();

	return 0;
}


void hgdraw::Startup2( void )
{
	//		Startup phase2
	//
	syncwait = -1;

	//		ライブラリ初期化
	//
	pkInit( GetSysReq(SYSREQ_PKTSIZE), OT_ALL );
	GeometryInit();
	pkcurrent = NULL;

	//		MOC初期化
	//
	MocSetMax( ( GetSysReq(SYSREQ_MAXOBJ) + 4 ) * SCN_MOC_MAX );
	MocInit();

	//		テクスチャ情報初期化
	//
	io.TexInit();

	//		カメラ初期化
	//
	cammocnum = 0;
	RegistCam();

	//		ライト初期化
	//
	InitLight();
/*	
	{
	node = MakeNode( PRIM_POLY4T, 4, 4 );
	float sx=64.0f;
	float sy=64.0f;
	VECTOR *fv = node->fv;
	IAXIS *uv = node->uv;
	SetVector( &fv[0], -1.0f, -1.0f, 0.0f, 1.0f );
	SetVector( &fv[1], 1.0f, -1.0f, 0.0f, 1.0f );
	SetVector( &fv[2], 1.0f, 1.0f, 0.0f, 1.0f );
	SetVector( &fv[3], -1.0f, 1.0f, 0.0f, 1.0f );
	setIAXIS( uv[0], sx*4, sy*0 );
	setIAXIS( uv[1], sx*5, sy*0 );
	setIAXIS( uv[2], sx*5, sy*1 );
	setIAXIS( uv[3], sx*4, sy*1 );
	}
*/
}


hgdraw::hgdraw( void )
{
	hgmode = -1;
	dest_flag = 0;
	*(int *)&io = 0;
	InitSysReq();
}


hgdraw::~hgdraw( void )
{
	if ( *(int *)&io != 0 ) {
		Terminate();
	}
}


void hgdraw::Terminate( void )
{
	if ( *(int *)&io != 0 ) {
		releaseDest();
		#ifdef HGIMG_DIRECTX
			::hgiox_term();
		#else
		switch( hgmode ) {
		case 0:
			::hgiof_term();
			break;
		case 1:
			::hgio_term();
			break;
		case 2:
			break;
		}
		#endif
		*(int *)&io = 0;
	}
	if ( hgmode >= 0 ) {
		DeleteCam();
		DeleteLight();
		MocTerm();
		GeometryTerm();
		pkTerm();
		StopTimer();
		hgmode = -1;
	}
}


void hgdraw::Restart( void )
{
	//		Restart System
	//
	if ( *(int *)&io != 0 ) {
		releaseDest();
		io.TexTerm();
	}
	if ( hgmode >= 0 ) {
		DeleteCam();
		DeleteLight();
		MocTerm();
		GeometryTerm();
		pkTerm();
	}
	Startup2();
}


void hgdraw::releaseDest( void )
{
	int i;
	io.releaseDest();
	if ( dest_flag == 0 ) return;

	for(i=0;i<model_buf_max;i++) {
		DeleteModel( i );
	}
	for(i=0;i<scnobj_buf_max;i++) {
		DeleteObj( i );
	}

	free( mem_model );
	free( mem_manim );
	free( mem_obj );
	dest_flag = 0;
}


void hgdraw::Reset( void )
{
	int i;
	float rt;
	float otrt;
	for(i=0;i<model_buf_max;i++) {
		mem_model[i].model = NULL;
	}
	for(i=0;i<scnobj_buf_max;i++) {
		mem_obj[i].flag = 0;
	}

	//		ビューの初期化
	//
	center.x = (float)nDestWidth * 0.5f;
	center.y = (float)nDestHeight * 0.5f;
	center.z = 0.0f;
	center.w = 0.0f;
	rt = center.x / 160.0f;
	otrt = OT_RANGE / 512.0f;

	//		3Dの初期化
	//
	bgclr = 1;
	bgcolor = 0;
	proj = 100.0f * rt;
	xyproj = 100.0f * rt;
	//otzv = 10.0f * otrt;
	otzv = 5.0f;
	//otzv = 10.0f;
	otofs = 0;
	//clipzn = 2;
	clipzf = OT_RANGE-1;
	SetVector( &border1, -50.0f, 0.0f, -50.0f, 0.0f );
	SetVector( &border2,  50.0f, -100.0f,  50.0f, 0.0f );

	//		設定の初期化
	//
	SetSysReq( SYSREQ_COLORKEY, 0);
	ResetLight();
}


void hgdraw::SetZCalc( int otdiv, int ofs )
{
	if ( otdiv == 0 ) {
		otzv = 0.0f;
	}
	else {
		otzv = 5.0f * (float)otdiv / (float)OT_RANGE;
	}
	otofs = ofs;
}

	
void hgdraw::setDest( void *dest, int sx, int sy )
{
	int size;
	releaseDest();
	io.setDest( dest, sx, sy );
	nDestWidth = sx;
	nDestHeight = sy;
	//model_buf_max =	4096;
	//scnobj_buf_max = 512;
	model_buf_max =	GetSysReq(SYSREQ_MAXMODEL);
	scnobj_buf_max = GetSysReq(SYSREQ_MAXOBJ);

	size = sizeof( MODBASE ) * model_buf_max;
	mem_model = (MODBASE *)malloc( size );
	memset( mem_model, 0, size );
	size = sizeof( MDLANM ) * model_buf_max;
	mem_manim = (MDLANM *)malloc( size );
	memset( mem_manim, 0, size );
	size = sizeof( SCNOBJ ) * scnobj_buf_max;
	mem_obj = (SCNOBJ *)malloc( size );
	memset( mem_obj, 0, size );
	Reset();
	dest_flag = 1;
}


void hgdraw::setSrc( void *src, int sx, int sy )
{
	io.SetSrcTex( src, sx, sy );
}


void hgdraw::Sync( int sync )
{
	int b;
	syncwait = sync;
	if (sync>0) {
		b=0;
		while( syncwait > timecnt ) {
			if ( b++ > 100 ) break;
			Sleep(5);
		}
	}
	timecnt=0;
}


void hgdraw::SetUVAnimSize( int sx, int sy )
{
	uva_px = sx;
	uva_py = sy;
}


void hgdraw::SetBorder( float x0, float x1, float y0, float y1, float z0, float z1 )
{
	SetVector( &border1, x0, y0, z0, 0.0f );
	SetVector( &border2, x1, y1, z1, 0.0f );
}


void hgdraw::GetBorder( VECTOR *v1, VECTOR *v2 )
{
	CopyVector( v1, &border1 );
	CopyVector( v2, &border2 );
}


/*------------------------------------------------------------*/
/*
		node process
*/
/*------------------------------------------------------------*/

PNODE *hgdraw::MakeNode( int code, int vnum, int uvs )
{
	//		ノード情報を新規作成
	//
	PNODE *n;
	VECTOR *v;
	IAXIS *uv;
	n = new PNODE;
	memset( n, 0, sizeof(PNODE) );
	n->code = code;
	n->attr = 0;
	n->num  = vnum;
	n->color = 0xffffff;
	if ( vnum ) {
		v = (VECTOR *)malloc( sizeof(VECTOR)*vnum*2 );
		n->fv = v;
		n->fn = v + vnum;
	}
	n->num2 = uvs;
	if ( uvs ) {
		uv = (IAXIS *)malloc( sizeof(IAXIS)*uvs );
		n->uv = uv;
	}
	return n;
}
	

void *hgdraw::SetNodeExData( PNODE *n, int size )
{
	//		ノード情報にExtra Dataを追加
	//
	if ( n->exdata != NULL ) free( n->exdata );
	n->exdata = (void *)malloc( size );
	memset( (char *)n->exdata, 0, size );
	return n->exdata;
}
	
	
void hgdraw::DeleteNode( PNODE *n )
{
	//		ノード情報を破棄
	//
	if ( n->exdata != NULL ) free( n->exdata );
	if ( n->fv != NULL ) free( n->fv );
	if ( n->uv != NULL ) free( n->uv );
	delete n;
}


#ifdef HGIMG_DIRECTX
void hgdraw::NodeToPrim( PNODE *n )
{
	VECTOR pos;
	VECTOR *v;
	float x,z;
	float zot;
	int a;

	zot = 0.0f;

	switch( n->code ) {
	case PRIM_PSPRITE:
		a=0;
		if (( ang2d.x != 1.0f )||( ang2d.y != 1.0f )||( ang2d.z != 0.0f )) a++;
		if ( a==0 )
		{
			//		等倍スプライト
			P_SPRITE *spr = (P_SPRITE *)MakePrim( PRIM_PSPRITE );
			short tx0,ty0,tx1,ty1;
			int ofsx,ofsy;
			spr->attr = n->attr;
			tx0 = n->uv[0].x;
			ty0 = n->uv[0].y + uoffset;
			tx1 = n->uv[1].x;
			ty1 = n->uv[1].y + uoffset;
			ofsx = (int)(vec2d.x);
			ofsy = (int)(vec2d.y);
			ofsx -= ( tx1 - tx0 + 1 )>>1;
			ofsy -= ( ty1 - ty0 + 1 )>>1;
			for(a=0;a<n->num;a++) {
				v = &n->fv[a];
				spr->v[a].x = (int)(v->x) + ofsx;
				spr->v[a].y = (int)(v->y) + ofsy;
			}
			spr->v[0].tx = tx0;
			spr->v[0].ty = ty0;
			spr->v[1].tx = tx1;
			spr->v[1].ty = ty1;
			spr->tex = (char)n->tex;
			spr->alpha = drawmat_alpha;
			a = (int)v->z;
			a += otofs;
			if (( a < OT_RANGE )&&( a>=0 )) AddPrim( spr, a );
		}
		else
		{
			//		拡大回転スプライト
			POLY4 *poly;
			IAXIS2 *iv;
			float ang,x0,y0,x1,y1,ofsx,ofsy;
			short xx,yy,tx0,ty0,tx1,ty1;

			poly = (POLY4 *)MakePrim( PRIM_POLY4T );
			v = &n->fv[0];
			xx = (short)(v->x + vec2d.x);
			yy = (short)(v->y + vec2d.y);

			tx0 = n->uv[0].x;
			ty0 = n->uv[0].y;
			tx1 = n->uv[1].x;
			ty1 = n->uv[1].y;
			ofsx = (float)( tx1 - tx0 + 1 );
			ofsy = (float)( ty1 - ty0 + 1 );
			ang = ang2d.z;
			x0=-(float)sin( ang );
			y0=(float)cos( ang );
			x1 = -y0;
			y1 = x0;

			ofsx *= ang2d.x * -0.5f;
			ofsy *= ang2d.y * -0.5f;
			x0 *= ofsy;
			y0 *= ofsy;
			x1 *= ofsx;
			y1 *= ofsx;
			ty0 += uoffset;
			ty1 += uoffset;

			iv = &poly->v[0];
			iv->x = (short)(-x0+x1) + xx;
			iv->y = (short)(-y0+y1) + yy;
			iv->tx = tx1;
			iv->ty = ty1;

			iv = &poly->v[1];
			iv->x = (short)(x0+x1) + xx;
			iv->y = (short)(y0+y1) + yy;
			iv->tx = tx1;
			iv->ty = ty0;

			iv = &poly->v[2];
			iv->x = (short)(x0-x1) + xx;
			iv->y = (short)(y0-y1) + yy;
			iv->tx = tx0;
			iv->ty = ty0;

			iv = &poly->v[3];
			iv->x = (short)(-x0-x1) + xx;
			iv->y = (short)(-y0-y1) + yy;
			iv->tx = tx0;
			iv->ty = ty1;

			poly->tex = (char)n->tex;
			poly->attr = n->attr;
			poly->npoly = 1;
			poly->color = 0xffffff;
			poly->alpha = drawmat_alpha;
			a = (int)v->z;
			a += otofs;
			if (( a < OT_RANGE )&&( a>=0 )) AddPrim( poly, a );
		}
		break;
	case PRIM_PBGSPR:
		{
		P_BGSPR *bg = (P_BGSPR *)MakePrim( PRIM_PBGSPR );
		P_BGSPR *exdata = (P_BGSPR *)n->exdata;
		v = &n->fv[0];
		bg->tex = (char)n->tex;
		bg->xx = (int)(v->x + vec2d.x);
		bg->yy = (int)(v->y + vec2d.y);
		bg->vx = exdata->vx;
		bg->vy = exdata->vy;
		bg->vram = exdata->vram;
		bg->sx = exdata->sx;
		bg->sy = exdata->sy;
		bg->cx = exdata->cx;
		bg->cy = exdata->cy;
		bg->wx = exdata->wx;
		bg->wy = exdata->wy;
		bg->attr = n->attr;
		bg->alpha = drawmat_alpha;
		a = (int)v->z;
		a += otofs;
		if (( a < OT_RANGE )&&( a>=0 )) AddPrim( bg, a );
		}
		break;
	case PRIM_POLY4F:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4F );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = *(int *)&n->color;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4T:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4T );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = 0xffffff;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4FS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4FS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], n->color );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4TS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4TS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], 0xffffff );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4FGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_POLY4FGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], n->color );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4TGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_POLY4TGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], 0xffffff );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}

	case PRIM_LINE4F:
	case PRIM_LINE4T:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_LINE4F );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = *(int *)&n->color;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_LINE4FS:
	case PRIM_LINE4TS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_LINE4FS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], n->color );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_LINE4FGS:
	case PRIM_LINE4TGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_LINE4FGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], n->color );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}

	}
}
#else
void hgdraw::NodeToPrim( PNODE *n )
{
	VECTOR pos;
	VECTOR *v;
	float x,z;
	float zot;
	int a;

	zot = 0.0f;

	switch( n->code ) {
	case PRIM_PSPRITE:
		a=0;
		if (( ang2d.x != 1.0f )||( ang2d.y != 1.0f )||( ang2d.z != 0.0f )) a++;
		if ( a==0 )
		{
			//		等倍スプライト
			P_SPRITE *spr = (P_SPRITE *)MakePrim( PRIM_PSPRITE );
			short tx0,ty0,tx1,ty1;
			int ofsx,ofsy;
			spr->attr = n->attr;
			tx0 = n->uv[0].x;
			ty0 = n->uv[0].y + uoffset;
			tx1 = n->uv[1].x;
			ty1 = n->uv[1].y + uoffset;
			ofsx = (int)(vec2d.x);
			ofsy = (int)(vec2d.y);
			ofsx -= ( tx1 - tx0 + 1 )>>1;
			ofsy -= ( ty1 - ty0 + 1 )>>1;
			for(a=0;a<n->num;a++) {
				v = &n->fv[a];
				spr->v[a].x = (int)(v->x) + ofsx;
				spr->v[a].y = (int)(v->y) + ofsy;
			}
			spr->v[0].tx = tx0;
			spr->v[0].ty = ty0;
			spr->v[1].tx = tx1;
			spr->v[1].ty = ty1;
			spr->tex = (char)n->tex;
			spr->alpha = drawmat_alpha;
			a = (int)v->z;
			a += otofs;
			if (( a < OT_RANGE )&&( a>=0 )) AddPrim( spr, a );
		}
		else
		{
			//		拡大回転スプライト
			POLY4 *poly;
			IAXIS2 *iv;
			float ang,x0,y0,x1,y1,ofsx,ofsy;
			short xx,yy,tx0,ty0,tx1,ty1;

			poly = (POLY4 *)MakePrim( PRIM_POLY4T );
			v = &n->fv[0];
			xx = (short)(v->x + vec2d.x);
			yy = (short)(v->y + vec2d.y);

			tx0 = n->uv[0].x;
			ty0 = n->uv[0].y;
			tx1 = n->uv[1].x;
			ty1 = n->uv[1].y;
			ofsx = (float)( tx1 - tx0 + 1 );
			ofsy = (float)( ty1 - ty0 + 1 );
			ang = ang2d.z;
			x0=-(float)sin( ang );
			y0=(float)cos( ang );
			x1 = -y0;
			y1 = x0;

			ofsx *= ang2d.x * -0.5f;
			ofsy *= ang2d.y * -0.5f;
			x0 *= ofsy;
			y0 *= ofsy;
			x1 *= ofsx;
			y1 *= ofsx;
			ty0 += uoffset;
			ty1 += uoffset;

			iv = &poly->v[0];
			iv->x = (short)(-x0+x1) + xx;
			iv->y = (short)(-y0+y1) + yy;
			iv->tx = tx1;
			iv->ty = ty1;

			iv = &poly->v[1];
			iv->x = (short)(x0+x1) + xx;
			iv->y = (short)(y0+y1) + yy;
			iv->tx = tx1;
			iv->ty = ty0;

			iv = &poly->v[2];
			iv->x = (short)(x0-x1) + xx;
			iv->y = (short)(y0-y1) + yy;
			iv->tx = tx0;
			iv->ty = ty0;

			iv = &poly->v[3];
			iv->x = (short)(-x0-x1) + xx;
			iv->y = (short)(-y0-y1) + yy;
			iv->tx = tx0;
			iv->ty = ty1;

			poly->tex = (char)n->tex;
			poly->attr = n->attr;
			poly->npoly = 1;
			poly->color = 0xffffff;
			poly->alpha = drawmat_alpha;
			a = (int)v->z;
			a += otofs;
			if (( a < OT_RANGE )&&( a>=0 )) AddPrim( poly, a );
		}
		break;
	case PRIM_PBGSPR:
		{
		P_BGSPR *bg = (P_BGSPR *)MakePrim( PRIM_PBGSPR );
		P_BGSPR *exdata = (P_BGSPR *)n->exdata;
		v = &n->fv[0];
		bg->tex = (char)n->tex;
		bg->xx = (int)(v->x + vec2d.x);
		bg->yy = (int)(v->y + vec2d.y);
		bg->vx = exdata->vx;
		bg->vy = exdata->vy;
		bg->vram = exdata->vram;
		bg->sx = exdata->sx;
		bg->sy = exdata->sy;
		bg->cx = exdata->cx;
		bg->cy = exdata->cy;
		bg->wx = exdata->wx;
		bg->wy = exdata->wy;
		bg->attr = n->attr;
		bg->alpha = drawmat_alpha;
		a = (int)v->z;
		a += otofs;
		if (( a < OT_RANGE )&&( a>=0 )) AddPrim( bg, a );
		}
		break;
	case PRIM_POLY4F:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4F );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = *(int *)&n->color;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4T:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4T );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = 0xffffff;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4FS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4FS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], n->color );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4TS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_POLY4TS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], 0xffffff );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4FGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_POLY4FGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], n->color );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_POLY4TGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_POLY4TGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			poly->v[a].tx = n->uv[a].x;
			poly->v[a].ty = n->uv[a].y + uoffset;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], 0xffffff );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->tex = (short)n->tex;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}

	case PRIM_LINE4F:
	case PRIM_LINE4T:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_LINE4F );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->color = *(int *)&n->color;
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_LINE4FS:
	case PRIM_LINE4TS:
		{
		POLY4 *poly = (POLY4 *)MakePrim( PRIM_LINE4FS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数

			//	simple shade calc
			poly->color = CalcLight( &n->fn[0], n->color );
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}
	case PRIM_LINE4FGS:
	case PRIM_LINE4TGS:
		{
		POLY4G *poly = (POLY4G *)MakePrim( PRIM_LINE4FGS );
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly->v[a].x = (short)x;
			poly->v[a].y = (short)pos.y;
			//	simple shade calc
			poly->color[a] = CalcLight( &n->fn[a], n->color );
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			a += otofs;
			poly->attr = n->attr;
			poly->npoly = n->num >> 2;		// ポリゴン数
			poly->alpha = drawmat_alpha;
			AddPrim( poly, OT_ALL-a );
		}
		break;
		}

	}
}
#endif


void hgdraw::MakeSpriteNode( MODEL *mdl, int tx0, int ty0, int tx1, int ty1, int sw )
{
	PNODE *n;
	VECTOR *fv;
	IAXIS *uv;
	float sx,sy,z;
	n = MakeNode( PRIM_PSPRITE, 2, 2 );
	n->tex = io.GetNextTexID();
	n->attr = sw;
	fv = n->fv;
	uv = n->uv;
	sx = (float)(tx1 - tx0 + 1);
	sy = (float)(ty1 - ty0 + 1);
	z = 0.0f;
	SetVector( &fv[0], 0.0f, 0.0f, (float)z, 1.0f );
	SetVector( &fv[1],   sx,   sy, (float)z, 1.0f );
	setIAXIS( uv[0], tx0, ty0 );
	setIAXIS( uv[1], tx1, ty1 );
	SetModelNode( mdl, n );
}


void hgdraw::MakeBGNode( MODEL *mdl, int sx, int sy, int wx, int wy, int cx, int cy, int sw )
{
	PNODE *n;
	VECTOR *fv;
	P_BGSPR *bg;
	char *vram;
	n = MakeNode( PRIM_PBGSPR, 1, 0 );
	n->tex = io.GetNextTexID();
	fv = n->fv;
	SetVector( &fv[0], 0.0f, 0.0f, 0.0f, 1.0f );
	bg = (P_BGSPR *)SetNodeExData( n, sizeof(P_BGSPR)+(sx*sy*2) );
	vram = (char *)(bg+1);
	bg->sx = sx;
	bg->sy = sy;
	bg->cx = cx;
	bg->cy = cy;
	bg->wx = wx;
	bg->wy = wy;
	bg->xx = 0; bg->yy = 0;
	bg->vram = vram;
	bg->attr = sw;
	SetModelNode( mdl, n );
}


void hgdraw::MakePlaneNodeF( MODEL *mdl, int code, float sx, float sy, int color, VECTOR *pos, VECTOR *ang, int sw )
{
	PNODE *n;
	VECTOR *fv;
	VECTOR v;
	float x,y;
	int a;
	n = MakeNode( code+((sw&3)<<1), 4, 4 );
	n->tex = -1;
	n->attr = (short)sw;
	n->color = color;
	fv = n->fv;
	x = sx * 0.5f; y = sy * 0.5f;
	SetVector( &fv[0], -x, -y, 0.0f, 1.0f );
	SetVector( &fv[1],  x, -y, 0.0f, 1.0f );
	SetVector( &fv[2],  x,  y, 0.0f, 1.0f );
	SetVector( &fv[3], -x,  y, 0.0f, 1.0f );
	for(a=0;a<4;a++) {
		InitMatrix();
		Trans( pos->x, pos->y, pos->z );
		RotZ( ang->z );
		RotY( ang->y );
		RotX( ang->x );
		CopyVector( &v, &fv[a] );
		ApplyMatrix( &fv[a], &v );
	}
	SetModelNode( mdl, n );
}


void hgdraw::MakeBoxNodeF( MODEL *mdl, float sx, float sy, int color, int attr )
{
	VECTOR pos,ang;
	float xx,yy;
	int sw;
	xx = sx * 0.5f;
	yy = sy * 0.5f;
	sw = attr;
	SetVector( &pos, 0.0f, 0.0f, -yy, 0.0f );
	SetVector( &ang, 0.0f, 0.0f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );
	SetVector( &pos, 0.0f, 0.0f, yy, 0.0f );
	SetVector( &ang, PI, 0.0f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );

	SetVector( &pos, xx, 0.0f, 0.0f, 0.0f );
	SetVector( &ang, 0.0f, PI*0.5f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );
	SetVector( &pos, -xx, 0.0f, 0.0f, 0.0f );
	SetVector( &ang, 0.0f, PI*-0.5f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );

	SetVector( &pos, 0.0f, xx, 0.0f, 0.0f );
	SetVector( &ang, PI*-0.5f, 0.0f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );
	SetVector( &pos, 0.0f, -xx, 0.0f, 0.0f );
	SetVector( &ang, PI*0.5f, 0.0f, 0.0f, 0.0f );
	MakePlaneNodeF( mdl, PRIM_POLY4F, sx, sy, color, &pos, &ang, sw );
}

	
void hgdraw::MakePlaneNode( MODEL *mdl, int code, float sx, float sy, int tx0, int ty0, int tx1, int ty1, VECTOR *pos, VECTOR *ang, int sw )
{
	PNODE *n;
	VECTOR *fv;
	VECTOR v;
	IAXIS *uv;
	float x,y;
	int a;
	n = MakeNode( code+((sw&3)<<1), 4, 4 );
	n->tex = io.GetNextTexID();
	n->attr = (short)sw;
	fv = n->fv;
	uv = n->uv;
	x = sx * 0.5f; y = sy * 0.5f;
	SetVector( &fv[0], -x, -y, 0.0f, 1.0f );
	SetVector( &fv[1],  x, -y, 0.0f, 1.0f );
	SetVector( &fv[2],  x,  y, 0.0f, 1.0f );
	SetVector( &fv[3], -x,  y, 0.0f, 1.0f );
	setIAXIS( uv[0], tx1, ty0 );
	setIAXIS( uv[1], tx0, ty0 );
	setIAXIS( uv[2], tx0, ty1 );
	setIAXIS( uv[3], tx1, ty1 );
	for(a=0;a<4;a++) {
		InitMatrix();
		Trans( pos->x, pos->y, pos->z );
		RotZ( ang->z );
		RotY( ang->y );
		RotX( ang->x );
		CopyVector( &v, &fv[a] );
		ApplyMatrix( &fv[a], &v );
	}
	SetModelNode( mdl, n );
}


void hgdraw::MakeBoxNode( MODEL *mdl, float sx, float sy, int tx0, int ty0, int tx1, int ty1, int attr )
{
	VECTOR pos,ang;
	float xx,yy;
	int tsx,tsy,sw;
	xx = sx * 0.5f;
	yy = sy * 0.5f;
	tsx = tx1-tx0+1;
	tsy = ty1-ty0+1;
	sw = attr;

	SetVector( &pos, 0.0f, 0.0f, -yy, 0.0f );
	SetVector( &ang, 0.0f, 0.0f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0+tsy, tx1, ty1+tsy, &pos, &ang, sw );
	SetVector( &pos, 0.0f, 0.0f, yy, 0.0f );
	SetVector( &ang, PI, 0.0f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0+tsy, tx1, ty1+tsy, &pos, &ang, sw );

	tsy += tsy;

	SetVector( &pos, xx, 0.0f, 0.0f, 0.0f );
	SetVector( &ang, 0.0f, PI*0.5f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0+tsy, tx1, ty1+tsy, &pos, &ang, sw );
	SetVector( &pos, -xx, 0.0f, 0.0f, 0.0f );
	SetVector( &ang, 0.0f, PI*-0.5f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0+tsy, tx1, ty1+tsy, &pos, &ang, sw );

	SetVector( &pos, 0.0f, xx, 0.0f, 0.0f );
	SetVector( &ang, PI*-0.5f, 0.0f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0, tx1, ty1, &pos, &ang, sw );
	SetVector( &pos, 0.0f, -xx, 0.0f, 0.0f );
	SetVector( &ang, PI*0.5f, 0.0f, 0.0f, 0.0f );
	MakePlaneNode( mdl, PRIM_POLY4T, sx, sy, tx0, ty0, tx1, ty1, &pos, &ang, sw );
}

	
void hgdraw::MakeMeshNode( MODEL *mdl, float sx, float sy, int divx, int divy, int tx, int ty, int sw )
{
	VECTOR pos,ang;
	int i,j,tx0,ty0,tx1,ty1;
	float xx,yy,px,py;
	xx = sx * -0.5f;
	yy = sy * -0.5f;
	px = sx / (float)divx;
	py = sy / (float)divy;
	SetVector( &ang, 0.0f, PI, 0.0f, 0.0f );

	for(j=0;j<divy;j++) {
		if ( sw&16 ) {
				ty0=0;ty1=ty-1;
		}
		else {
			ty0 = ty * j / divy;
			ty1 = ( j + 1 ) * ty / divy;
		}
		for(i=0;i<divx;i++) {
			if ( sw&16 ) { tx0=0;tx1=tx-1; }
			else {
				tx1 = tx * i / divx;
				tx0 = ( i + 1 ) * tx / divx;
			}
			SetVector( &pos, xx+( px * (float)i ), yy+( py * (float)j ), 0.0f, 0.0f );
			MakePlaneNode( mdl, PRIM_POLY4T, px, py, tx1, ty0, tx0, ty1, &pos, &ang, sw&1 );
		}
	}
}

/*	
void hgdraw::MakeMeshNode( MODEL *mdl, float sx, float sy, int divx, int divy, int tx, int ty, int sw )
{
	VECTOR pos,ang;
	int i,j,tx0,ty0,tx1,ty1;
	float xx,yy,px,py,xx2,yy2;
	px = sx / (float)divx;
	py = sy / (float)divy;
	SetVector( &ang, 0.0f, PI, 0.0f, 0.0f );

	yy = sy * -0.5f;
	for(j=0;j<divy;j++) {
		if ( sw&16 ) {
				ty0=0;ty1=ty-1;
		}
		else {
			ty0 = ty * j / divy;
			ty1 = ( j + 1 ) * ty / divy;
		}
		xx = sx * -0.5f;
		yy2 = yy+( sy * (float)(j+1) / (float)divy );
		for(i=0;i<divx;i++) {
			if ( sw&16 ) { tx0=0;tx1=tx-1; }
			else {
				tx1 = tx * i / divx;
				tx0 = ( i + 1 ) * tx / divx;
			}
			xx2 = xx+( sx * (float)(i+1) / (float)divx );
			SetVector( &pos, xx, yy, 0.0f, 0.0f );
			MakePlaneNode( mdl, PRIM_POLY4T, xx2-xx, yy2-yy, tx1, ty0, tx0, ty1, &pos, &ang, sw );
			xx=xx2;
		}
		yy=yy2;
	}
}
*/
	
/*------------------------------------------------------------*/
/*
		model process
*/
/*------------------------------------------------------------*/

MODEL *hgdraw::MakeModel( void )
{
	//		モデル情報を新規作成(mem)
	//
	MODEL *m;
	PNODE **pn;

	m = (MODEL *)malloc( sizeof(MODEL) );
	memset( m, 0, sizeof(MODEL) );
	pn = (PNODE **)malloc( sizeof(PNODE *) * MDL_NODE_MAX );
	m->node = pn;
	m->bound = 0.1f;
	m->nummax = MDL_NODE_MAX;
	m->aindex = 0;

	//	初期値
	SetVector( &m->pos, 0.0f, 0.0f, 0.0f, 1.0f );
	SetVector( &m->ang, 0.0f, 0.0f, 0.0f, 1.0f );
	SetVector( &m->scale, 1.0f, 1.0f, 1.0f, 1.0f );

	return m;
}
	
	
int hgdraw::RegistModel( void )
{
	//		モデル情報を新規作成(buffer)
	//
	MODEL *m;
	MODBASE *base;
	int i,sel;

	sel = -1;
	for(i=0;i<model_buf_max;i++) {
		if ( mem_model[i].model == NULL ) { sel = i; break; }
	}
	if ( sel < 0 ) return NULL;

	m = MakeModel();
	base = &mem_model[sel];
	base->model = m;
	base->num = 1;
	return sel;
}


void hgdraw::RebuildModelTreeSub( MODBASE *base, MODEL *m, char *output )
{
	MODEL **mbuf;
	char tmp[128];
	char *p;
	int i;
	if ( base->num >= 64 ) return;
	mbuf = (MODEL **)&base->model;
	mbuf[base->num] = m;
	m->id = base->num;
	if ( output!=NULL ) {
		p = tmp;
		for(i=0;i<tree_depth;i++) { *p++=32; }
		sprintf( p,"(%d)\n",m->id );
		strcat( output, tmp );
	}
	tree_depth++;
	base->num++;
	if ( m->child != NULL ) RebuildModelTreeSub( base, (MODEL *)m->child, output );
	tree_depth--;
	if ( m->sibling != NULL ) RebuildModelTreeSub( base, (MODEL *)m->sibling, output );
}


int hgdraw::RebuildModelTree( int id, char *output )
{
	//		モデル情報を新規作成(buffer)
	//
	MODBASE *base;
	tree_depth = 0;
	base = &mem_model[id];
	if ( base->model == NULL ) return 0;
	base->num = 0;
	RebuildModelTreeSub( base, base->model, output );
	if ( base->num >= 64 ) {
		base->num = 1;
		base->model->child = NULL;
		base->model->sibling = NULL;
		return -1;
	}
	if ( output!=NULL ) {
		char tmp[128];
		sprintf( tmp,"total:%d\n",base->num );
		strcat( output, tmp );
		}
	return 0;
}


void hgdraw::ArrangeModelTree( int modelid, int subid, int newid, int mode )
{
	//		階層構造の変更
	//			(mode:0=child/1=sibling)
	//
	MODEL *m;
	MODEL *m2;
	MODEL **mbuf;
	MODBASE *base;

	base = &mem_model[modelid];
	mbuf = (MODEL **)&base->model;

	m = GetModelEx( modelid, subid );
	if ( m==NULL ) return;
	m2 = GetModel( newid );
	if ( modelid != newid ) {
		mbuf[base->num] = m2;
		m->id = base->num;
		base->num++;
	}
	if ( mode==0 ) m->child = m2; else m->sibling = m2;
	RebuildModelTree( modelid, NULL );
}


int hgdraw::GetModelSubID( int id, MODEL *model )
{
	//		modelのアドレスに該当するモデルSubIDを得る
	//
	int i,max;
	MODBASE *base;
	MODEL **mbuf;
	if ( model==NULL ) return -1;
	base = &mem_model[id];
	if ( base->model == NULL ) return -1;
	max = base->num;
	mbuf = (MODEL **)&base->model;
	for(i=0;i<max;i++) {
		if ( mbuf[i]==model ) return i;
	}
	return -1;
}


int hgdraw::GetModelSubMax( int id )
{
	//		SubIDの数を得る
	//
	return mem_model[ id ].num;
}


MODEL *hgdraw::GetModel( int id )
{
	//		IDからモデルを取得(buffer)
	//
	MODEL *m;
	MODEL **mbuf;
	if ( id>=0x10000 ) {
		mbuf = (MODEL **)&mem_model[ id & 0xffff ].model;
		m = mbuf[ id>>16 ];
	} else {
		m = mem_model[id].model;
	}
	if ( m == NULL ) return NULL;
	if ( m->nummax == 0 ) return NULL;
	return m;
}


MODEL *hgdraw::GetModelEx( int id, int sub )
{
	//		ID、サブIDからモデルを取得(buffer)
	//
	MODEL *m;
	MODEL **mbuf;
	mbuf = (MODEL **)&mem_model[id].model;
	m = mbuf[sub];
	if ( m == NULL ) return NULL;
	if ( m->nummax == 0 ) return NULL;
	return m;
}


void hgdraw::SetModelAnimFrame( int id, int frame, int opt )
{
	//		IDのモデルにアニメーションフレーム数を設定
	//		( SetModelAnim の直後のみ )
	//
	int a;
	MODEL *m;
	MDLANM *ma;
	m = mem_model[id].model;
	if ( m == NULL ) return;
	if ( m->anum == 0 ) return;
	ma = &mem_manim[ id ];
	a = m->aindex - 1;
	ma->animax[a] = frame;
	ma->aniopt[a] = opt;
}


MDLANM *hgdraw::GetModelAnimFrame( int id )
{
	//		IDのモデルアニメーションフレーム情報を取得
	//
	return &mem_manim[ id ];
}


void hgdraw::SetModelAnimDef( int id, int ua_mode, int ua_wait, int ua_times )
{
	//		IDのモデルUVアニメーション情報を設定
	//
	MDLANM *ma;
	ma = &mem_manim[ id ];
	ma->ua_mode = ua_mode;
	ma->ua_wait = ua_wait;
	ma->ua_times = ua_times;
	ma->uva_py = uva_py;
	ma->mode |= OBJ_MODE_UVANIM;
}


void hgdraw::SetModelModeDef( int id, int mode )
{
	//		IDのモデルデフォルトモードを設定
	//
	MDLANM *ma;
	ma = &mem_manim[ id ];
	ma->mode |= mode;
}


void hgdraw::SetModelTimerDef( int id, int timer )
{
	//		IDのモデルデフォルトタイマーを設定
	//
	MDLANM *ma;
	ma = &mem_manim[ id ];
	ma->timer = timer;
	ma->mode |= OBJ_MODE_TIMER;
}


P_BGSPR *hgdraw::GetBG( MODEL *m )
{
	//		Modelに関連付けられているBGのアドレスを取得
	//
	PNODE *n;
	if ( m == NULL ) return NULL;
	n = m->node[ 0 ];
	if ( n->code != PRIM_PBGSPR ) return NULL;
	return (P_BGSPR *)n->exdata;
}


char *hgdraw::GetBGVram( MODEL *m )
{
	P_BGSPR *bg;
	bg = GetBG( m );
	if ( bg == NULL ) return NULL;
	return bg->vram;
}

	
void hgdraw::SetBGView( MODEL *m, int x, int y )
{
	//		Modelに関連付けられているBGのMAP表示位置を設定
	//
	//int xx,yy,mx,my;
	P_BGSPR *bg;
	bg = GetBG( m );
	if ( bg == NULL ) return;

/*
	//			( x,yが画面中央にくるように設定 )
	mx = ( bg->sx - bg->wx ) * bg->cx;
	my = ( bg->sy - bg->wy ) * bg->cy;
	xx = x - (( bg->wx>>1 ) * bg->cx );
	yy = y - (( bg->wy>>1 ) * bg->cy );
	if ( xx > mx ) xx=mx;
	if ( yy > my ) yy=my;
	if ( xx < 0 ) xx=0;
	if ( yy < 0 ) yy=0;
	bg->vx = xx;
	bg->vy = yy;
*/
	bg->vx = x;
	bg->vy = y;
}


void hgdraw::DeleteModelSub( MODEL *m )
{
	int a;
	if ( m->nummax > 0 ) {
		for(a=0;a<m->num;a++) {
			DeleteNode( m->node[a] );
		}
		free( m->node );
	}

	if ( m->anum ) {
		free( m->ani );
		m->aindex = 0;
		m->anum = 0;
	}
		
	if ( m->child != NULL ) DeleteModelSub( (MODEL *)m->child );
	if ( m->sibling != NULL ) DeleteModelSub( (MODEL *)m->sibling );
	m->nummax = 0;
}


void hgdraw::DeleteModel( int id )
{
	//		モデル情報を破棄
	//
	MODEL *m;
	m = mem_model[id].model;
	if ( m == NULL ) return;
	DeleteModelSub( m );
	mem_model[id].model = NULL;
}


void hgdraw::SetModelPos( MODEL *m, VECTOR *v )
{
	CopyVector( &m->pos, v );
}
	
	
void hgdraw::SetModelAng( MODEL *m, VECTOR *v )
{
	CopyVector( &m->ang, v );
}
	
	
void hgdraw::SetModelScale( MODEL *m, VECTOR *v )
{
	CopyVector( &m->scale, v );
}


void hgdraw::SetModelChild( MODEL *m, MODEL *child )
{
	m->child = child;
}


void hgdraw::SetModelSibling( MODEL *m, MODEL *sibling )
{
	m->sibling = sibling;
}


void hgdraw::SetModelUV( MODEL *m, int nodeid, int *uv )
{
	PNODE *n;
	int a,i;
	if ( m == NULL ) return;
	n = m->node[ nodeid ];
	if ( n->num2 == 0 ) return;
	i=0;
	for(a=0;a<4;a++) {
		n->uv[a].x = uv[i++];
		n->uv[a].y = uv[i++];
	}
}


void hgdraw::GetModelUV( MODEL *m, int nodeid, int *uv )
{
	PNODE *n;
	int a,i;
	if ( m == NULL ) return;
	n = m->node[ nodeid ];
	if ( n->num2 == 0 ) return;
	i=0;
	for(a=0;a<4;a++) {
		uv[i++] = n->uv[a].x;
		uv[i++] = n->uv[a].y;
	}
}


void hgdraw::GetModelVector( MODEL *m, int nodeid, int mode, float *axis )
{
	VECTOR *v;
	PNODE *n;
	int a,i;
	if ( m == NULL ) return;
	n = m->node[ nodeid ];
	if ( n->num == 0 ) return;
	i=0;
	for(a=0;a<4;a++) {
		v = (VECTOR *)&axis[a*4];
		if ( mode==0 ) CopyVector( v, &n->fv[a] );
					else CopyVector( v, &n->fn[a] );
	}
}


void hgdraw::SetModelVector( MODEL *m, int nodeid, int mode, float *axis )
{
	VECTOR *v;
	PNODE *n;
	int a,i;
	if ( m == NULL ) return;
	n = m->node[ nodeid ];
	if ( n->num == 0 ) return;
	i=0;
	for(a=0;a<4;a++) {
		v = (VECTOR *)&axis[a*4];
		if ( mode==0 ) CopyVector( &n->fv[a], v );
					else CopyVector( &n->fn[a], v );
	}
}


void hgdraw::SetModelTexEx( MODEL *m, int nodeid, int tex, int shade )
{
	PNODE *n;
	TEXINF *ti;
	int sx,sy;
	int uvs[8];
	if ( m == NULL ) return;
	n = m->node[ nodeid ];
	n->tex = tex;
	if ( tex>=0 ) {
		ti = io.GetTex(tex);
		sx = ti->sx; sy = ti->sy;
		uvs[0]=0;uvs[1]=0;
		uvs[2]=sx;uvs[3]=0;
		uvs[4]=sx;uvs[5]=sy;
		uvs[6]=0;uvs[7]=sy;
		SetModelUV( m, nodeid, uvs );
		n->code = PRIM_POLY4T + (shade<<1);
	}
	else {
		n->code = PRIM_POLY4F + (shade<<1);
	}
}


int hgdraw::SetModelTex( MODEL *m, int nodeid, int tex )
{
	PNODE *n;
	if ( m == NULL ) return NULL;
	n = m->node[ nodeid ];
	if ( tex<0 ) return n->tex;
	n->tex = tex;
	return 0;
}


PNODE *hgdraw::GetModelNode( MODEL *m, int nodeid )
{
	if ( m == NULL ) return NULL;
	return m->node[ nodeid ];
}


void hgdraw::SetModelNode( MODEL *m, PNODE *n )
{
	//		モデルにNODEを追加
	//
	int i;
	i = m->num;
	if ( i >= m->nummax ) {				// NODE bufferを拡張
		int a;
		PNODE **pn;
		m->nummax += MDL_NODE_MAX;
		pn = (PNODE **)malloc( sizeof(PNODE *) * m->nummax );
		for(a=0;a<m->num;a++) pn[a] = m->node[a];
		free( m->node );
		m->node = pn;
	}
	m->node[ i ] = n;
	m->num++;
}


void hgdraw::SetModelAnim( MODEL *m, ANMINF *ani, int frame )
{
	//		モデルにAnimationを追加
	//
	int a,i;
	ANMINF *p;
	ANMINF *pw;
	ANMINF *pold;
	i = m->anum;
	if ( i == 0 ) {						// 新規バッファ
		p = (ANMINF *)malloc( sizeof(ANMINF) * frame );
		pw = p;
	}
	else {								// バッファ拡張
		pold = m->ani;
		p = (ANMINF *)malloc( sizeof(ANMINF) * (i+frame) );
		for(a=0;a<i;a++) {
			p[a] = pold[a];
		}
		free( pold );
		pw = &p[i];
	}

	for(a=0;a<frame;a++) {
		pw[a] = ani[a];					// データをコピー
	}

	m->ani = p;
	m->anifr[ m->aindex ] = i;
	m->animax[ m->aindex ] = frame;
	m->aindex++;
	i += frame;
	m->anum = i;

}

/*-----------------------------------------------------------------*/

void hgdraw::DrawModelSub( MODEL *m )
{
	int a;
	int frmax;
	ANMINF *ani;
	PushMatrix();

	if ( aniflag == 0 ) {						// 通常時
		Trans( m->pos.x, m->pos.y, m->pos.z );
		RotZ( m->ang.z );
		RotY( m->ang.y );
		RotX( m->ang.x );
		Scale( m->scale.x ,m->scale.y, m->scale.z );
	}
	else {										// アニメーション時
		if ( m->anum ) {
			frmax = m->animax[ curanim ];
			a = curframe;if ( a >= frmax ) a = frmax-1;
			ani = &m->ani[ m->anifr[ curanim ] + a ];
			Trans( ani->tx, ani->ty, ani->tz );
			RotZ( ani->rz );
			RotY( ani->ry );
			RotX( ani->rx );
			Scale( ani->sx, ani->sy, ani->sz );
		}
	}

	if ( m->num ) {
//		#ifdef HGIMG_DIRECTX
//			io.DrawNode( GetCurrentMatrixPtr(), m->num, m->node );
//		#endif
		for(a=0;a<m->num;a++) {
			NodeToPrim( m->node[a] );
		}
	}

	if ( m->child != NULL ) this->DrawModelSub( (MODEL *)m->child );
	PopMatrix();
	if ( m->sibling != NULL ) this->DrawModelSub( (MODEL *)m->sibling );
}


void hgdraw::DrawModel( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx )
{
	if ( m==NULL ) return;

//	SetCurrentMatrix( &m_camera );		// T&L実行時のみ
	SetCurrentMatrix( &m_view );
	Trans( pos->x, pos->y, pos->z );
	RotX( ang->x );
	RotY( ang->y );
	RotZ( ang->z );
	Scale( scale->x ,scale->y, scale->z );
	drawmat_alpha = (int)efx->x;

	aniflag = 0;							// アニメなし描画
	DrawModelSub( m );
}


void hgdraw::DrawModelAnim( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx )
{
	if ( m==NULL ) return;

	SetCurrentMatrix( &m_view );
	Trans( pos->x, pos->y, pos->z );
	RotX( ang->x );
	RotY( ang->y );
	RotZ( ang->z );
	Scale( scale->x ,scale->y, scale->z );
	drawmat_alpha = (int)efx->x;

	aniflag = 1;
	DrawModelSub( m );
}


int hgdraw::UpdateModelAnim( int id, int anim, int frame )
{
	int maxframe;
	MDLANM *ma;

	curanim = anim;
	curframe = frame;
	ma = GetModelAnimFrame( id );
	maxframe = ma->animax[anim];
	curframe++;
	if ( curframe == maxframe ) curframe = 0;
	return curframe;
}


void hgdraw::DrawModel2D( MODEL *m, VECTOR *pos, VECTOR *ang, VECTOR *scale, VECTOR *efx )
{
	int a;
	if ( m==NULL ) return;

	SetVector( &vec2d, center.x + pos->x, center.y + pos->y, pos->z, 0.0f );
	SetVector( &ang2d, scale->x, scale->y, ang->z, 0.0f );

	if ( m->num ) {
		drawmat_alpha = (int)efx->x;
		for(a=0;a<m->num;a++) {
			NodeToPrim( m->node[a] );
		}
	}
}

/*-----------------------------------------------------------------*/

void hgdraw::ModelOffsetSub( MODEL *m, VECTOR *offset )
{
	int a,i;
	PNODE *node;
	VECTOR *fv;

	if ( m->nummax > 0 ) {
		for(a=0;a<m->num;a++) {
			node = this->GetModelNode( m, a );
			fv = node->fv;
			for(i=0;i<node->num;i++) {
				AddVector( fv, fv, offset );
				fv++;
			}
		}
	}
	if ( m->child != NULL ) ModelOffsetSub( (MODEL *)m->child, offset );
	if ( m->sibling != NULL ) ModelOffsetSub( (MODEL *)m->sibling, offset );
}


void hgdraw::ModelOffset( int modelid, VECTOR *offset )
{
	MODEL *m;
	m = this->GetModel( modelid );
	ModelOffsetSub( m, offset );
}

/*-----------------------------------------------------------------*/

void hgdraw::ModelScaleSub( MODEL *m, VECTOR *scale )
{
	int a,i;
	PNODE *node;
	VECTOR *fv;

	if ( m->nummax > 0 ) {
		for(a=0;a<m->num;a++) {
			node = this->GetModelNode( m, a );
			fv = node->fv;
			for(i=0;i<node->num;i++) {
				MulVector( fv, fv, scale );
				fv++;
			}
		}
	}
	if ( m->child != NULL ) ModelOffsetSub( (MODEL *)m->child, scale );
	if ( m->sibling != NULL ) ModelOffsetSub( (MODEL *)m->sibling, scale );
}


void hgdraw::ModelScale( int modelid, VECTOR *scale )
{
	MODEL *m;
	m = this->GetModel( modelid );
	ModelScaleSub( m, scale );
}

/*-----------------------------------------------------------------*/

void hgdraw::ModelSetShadeSub( MODEL *m, int mode )
{
	int a,i;
	PNODE *node;
	if ( m->nummax > 0 ) {
		for(a=0;a<m->num;a++) {
			node = this->GetModelNode( m, a );
			i = node->code;
			if ((i>=PRIM_POLY4F)&&(i<PRIM_ATTRSET) ) {
				i = (i - PRIM_POLY4F ) & 1;
				i += mode<<1;
				node->code = PRIM_POLY4F + i;
			}
		}
	}
	if ( m->child != NULL ) ModelSetShadeSub( (MODEL *)m->child, mode );
	if ( m->sibling != NULL ) ModelSetShadeSub( (MODEL *)m->sibling, mode );
}


void hgdraw::ModelSetShade( int modelid, int mode )
{
	MODEL *m;
	m = this->GetModel( modelid );
	ModelSetShadeSub( m, mode );
}


void hgdraw::MakeNodeNormals( MODEL *m, int node, int mode )
{
	//		Nodeの法線を設定する
	//
	PNODE *n;
	PNODE *n2;
	VECTOR *fv;
	VECTOR *fn;
	VECTOR *fv2;
	VECTOR *fn2;
	VECTOR normal;
	VECTOR v1,v2;
	int a,b,i;

	n = GetModelNode( m, node );
	fv = n->fv;
	fn = n->fn;
	SubVector( &v1, &fv[0], &fv[1] );
	SubVector( &v2, &fv[2], &fv[1] );
	OuterProduct( &normal, &v1, &v2 );
	UnitVector( &normal );
	if ( mode==0 ) {
		for(a=0;a<n->num;a++) {
			CopyVector( &fn[a], &normal );
		}
		return;
	}
	for(a=0;a<n->num;a++) {
		for(i=0;i<m->num;i++) {
			n2 = GetModelNode( m, i );
			fv2 = n2->fv;
			fn2 = n2->fn;
			for(b=0;b<n2->num;b++) {
				if (( fv2[b].x == fv[a].x )
				  &&( fv2[b].y == fv[a].y )
				  &&( fv2[b].z == fv[a].z )) {
					AddVector( &fn2[b], &fn2[b], &normal );
				}
			}
		}
	}
}


void hgdraw::MakeModelNormals( int modelid, int mode )
{
	//		モデルの法線を設定する
	//			( mode:0=flat/1=gouraud )
	//
	int i,a,b,max;
	MODEL *m;
	PNODE *n;
	VECTOR *fn;
	VECTOR *fv;
	max = mem_model[modelid].num;
	for(i=0;i<max;i++) {
		m = GetModelEx( modelid, i );
		if ( m!=NULL ) if ( m->nummax > 0 ) {
			if ( mode ) {
				for(a=0;a<m->num;a++) {
					n = GetModelNode( m, a );
					fn = n->fn;
					for(b=0;b<n->num;b++) {
						SetVector( &fn[b], 0.0f, 0.0f, 0.0f, 0.0f );
					}
				}
			}
			for(a=0;a<m->num;a++) {
				MakeNodeNormals( m, a, mode );
			}
			if ( mode ) {
				for(a=0;a<m->num;a++) {
					n = GetModelNode( m, a );
					fn = n->fn;	fv = n->fv;
					for(b=0;b<n->num;b++) {
						UnitVector( &fn[b] );
					}
				}
			}
		}
	}
}


/*------------------------------------------------------------*/
/*
		draw process
*/
/*------------------------------------------------------------*/
/*
void hgdraw::DrawFontEx( int x, int y, char *mes, char *buf, int sx, int sy )
{
	//		指定したバッファに文字列を描画
	//
	char *tmp_dest;
	int tmp_sx,tmp_sy,tmp_size;
	IAXIS tmp_clip1;
	IAXIS tmp_clip2;

	tmp_dest = lpDest;
	tmp_sx = nDestWidth;
	tmp_sy = nDestHeight;
	tmp_size = DestSize;
	tmp_clip1 = sprclip1;
	tmp_clip2 = sprclip2;

	lpDest = buf;
	nDestWidth = sx;
	nDestHeight = sy;
	DestSize = sx*sy;

	setIAXIS( sprclip1, 0, 0 );
	setIAXIS( sprclip2, sx, sy );
	DrawFont( x, y, mes );

	lpDest = tmp_dest;
	nDestWidth = tmp_sx;
	nDestHeight = tmp_sy;
	DestSize = tmp_size;
	sprclip1 = tmp_clip1;
	sprclip2 = tmp_clip2;
}
*/
	
void hgdraw::DrawFont( int x, int y, int alp, char *mes )
{
	//		文字列を描画
	//
	io.DrawFontSprite( x, y, mescx, mescy, mespx, mes, mesmode|alp, mestex );
}


void hgdraw::SetFont( int tex, int cx, int cy, int px, int mode )
{
	mestex = tex;
	mescx = cx;
	mescy = cy;
	mespx = px;
	if ( mespx == 0 ) mespx = mescx;
	mesmode = mode;
}


void hgdraw::DrawPrim( void *prim )
{
	char *p;
	char code;

	p = (char *)prim;
	code = *p;
	switch( code ) {
	case PRIM_PSPRITE:
		io.DrawSpriteTex( (P_SPRITE *)prim );
		break;
	case PRIM_PBGSPR:
		io.DrawBGSpriteTex( (P_BGSPR *)prim );
		break;
	case PRIM_POLY4F:
	case PRIM_POLY4FS:
		io.DrawPolygonF4( (POLY4 *)prim );
		break;
	case PRIM_POLY4T:
	case PRIM_POLY4TS:
		io.DrawPolygonTex( (POLY4 *)prim );
		break;
	case PRIM_POLY4FGS:
		io.DrawPolygonF4G( (POLY4G *)prim );
		break;
	case PRIM_POLY4TGS:
		io.DrawPolygonTexG( (POLY4G *)prim );
		break;
	case PRIM_LINE4F:
	case PRIM_LINE4T:
	case PRIM_LINE4FS:
	case PRIM_LINE4TS:
		io.DrawLineF4( (POLY4 *)prim );
		break;
	case PRIM_LINE4FGS:
	case PRIM_LINE4TGS:
		io.DrawLineF4G( (POLY4G *)prim );
		break;
	}
}


void hgdraw::DrawOT( void )
{
	Packet *p;
	void *prim;
	io.ChangeTex( -1 );
	p = pkGetTable();
	while( p != NULL ) {
		prim = p->adr;
		if ( prim != NULL ) DrawPrim( prim );
		p = (Packet *)p->next;
	}
}


void hgdraw::DrawOTSync( void )
{
	Packet *p;
	void *prim;
	io.ChangeTex( -1 );
	if ( pkcurrent != NULL ) {
		p = (Packet *)pkcurrent;
	}
	else {
		p = pkGetTable();
	}

	while( p != NULL ) {
		prim = p->adr;
		if ( prim != NULL ) DrawPrim( prim );
		p = (Packet *)p->next;
		if ( syncwait <= timecnt ) {		// 時間内に描画できない
			pkcurrent = p; return;
		}
	}
	pkcurrent = NULL;
}


void hgdraw::DrawStart( void )
{
	if ( pkcurrent == NULL ) {
		pkBeginBuffer();		// ダブルパケットバッファを初期化
		if ( bgclr ) {
			io.ClearDest( bgclr,bgcolor,bgtex );
		}
	}
	else {						// OT描画中はパケット再初期化
		pkRestartBuffer();
	}
}


void hgdraw::DrawSetBG( int color )
{
	if ( color<0 ) { bgclr = 0; return; }
	bgclr = 1;
	bgcolor = color;
}


void hgdraw::DrawSetBGTex( int tex )
{
	bgclr = 2;
	if ( tex<0 ) return;
	bgtex = tex;
}


void hgdraw::DrawSetBGBlur( int color )
{
	bgclr = 3;
	bgcolor = color;
}


int hgdraw::DrawEnd( void )
{
	CamProc();
	UpdateLight();
	UpdateViewMatrix();
	DrawObjAll();

#ifdef HGIMG_DIRECTX
	::hgiox_render_start();
#endif

	if ( syncwait < 0 ) {
		DrawOT();					// OTの描画
	}
	else {
		DrawOTSync();				// OTの描画(Sync)
	}
	syncflag = ( pkcurrent == NULL );

	return syncflag;
}


void hgdraw::UpdateScreen( void )
{
#ifdef HGIMG_DIRECTX
	::hgiox_render_end();
#endif
}
	
	
void hgdraw::CopyPrimPacket( void *prim, int ot )
{
	int size;
	char *p;
	char *wbuf;
	p = (char *)prim;
	size = primsize[*p];
	wbuf = (char *)pkAllocBuffer( size );
	memcpy( wbuf, p, size );
	pkInsertPacket( ot, wbuf );
}


void hgdraw::AddPrim( void *prim, int ot )
{
	pkInsertPacket( ot, prim );
}


void *hgdraw::MakePrim( int code )
{
	int size;
	char *p;
	size = primsize[code];
	p = (char *)pkAllocBuffer( size );
	p[0] = (char)code;
	p[1] = (char)size;
	return p;
}


/*------------------------------------------------------------*/
/*
		camera process
*/
/*------------------------------------------------------------*/

void hgdraw::RegistCam( void )
{
	VECTOR v;
	cammocnum = SCN_MOC_MAX;
	cammoc = MocRegist( cammocnum );		// MOCを確保
	cammode = 0;

	//	初期値
	SetVector( &v, 0.0f, 0.0f, 4.0f, 1.0f );
	SetCamPos( &v );
	SetVector( &v, 0.0f, 0.0f, 0.0f, 0.0f );
	SetCamAng( &v );

	//	ver2.61追加
	SetVector( &v, 0.0f, 1.0f, 1.0f, 0.0f );
	MocSetKeyFV( cammoc + SCN_MOC_DIR, &v );

	UpdateCam();
}


void hgdraw::DeleteCam( void )
{
	int a;
	if ( cammocnum ) {
		for( a=0;a<SCN_MOC_MAX;a++) MocDelete( cammoc+a );
		cammocnum = 0;
	}
}


int hgdraw::GetCamMoc( void )
{
	return cammoc;
}


void hgdraw::UpdateCam( void )
{
	VECTOR campos;
	VECTOR camang;
	MocGetKeyFV( cammoc + SCN_MOC_ANG, &camang );
	MocGetKeyFV( cammoc + SCN_MOC_POS, &campos );

	InitMatrix();
	RotX( camang.x );
	RotY( camang.y );
	RotZ( camang.z );
	Trans( -campos.x, -campos.y, -campos.z );
	GetCurrentMatrix( &m_camera );
}


void hgdraw::CamProc( void )
{
	MocMove( cammoc, cammocnum );

	if ( cammode == CAM_MODE_LOOKAT ) {
		CalcCamInt();
	}
	UpdateCam();
}


void hgdraw::SetCamMode( int mode )
{
	cammode = mode;
}


void hgdraw::GetTargetAngle( VECTOR *ang, VECTOR *src, VECTOR *target )
{
	//--------------------------------------------------
	//	カメラのポジション->注視点の回転角を求める
	//--------------------------------------------------
	VECTOR vec;
	/* 注視点->ポジションのベクトル */
	vec.x = src->x - target->x;
	vec.y = src->y - target->y;
	vec.z = src->z - target->z;
	/* 回転角 */
	ang->y = (float)-atan2( vec.x, vec.z );
	ang->x = (float)-atan2( vec.y, sqrt(vec.x*vec.x + vec.z*vec.z) );
	ang->z = 0.0f;
	ang->w = 0.0f;
}
	

void hgdraw::CalcCamInt( void )
{
//--------------------------------------------------
//	カメラの注視点からポジションの回転角を求める
//--------------------------------------------------
	VECTOR vec;
	VECTOR ang;
	VECTOR CamTrans;
	VECTOR CamInt;

	MocGetKeyFV( cammoc + SCN_MOC_POS, &CamTrans );
	MocGetKeyFV( cammoc + SCN_MOC_SCALE, &CamInt );

	/* 注視点->ポジションのベクトル */
	vec.x = CamTrans.x - CamInt.x;
	vec.y = CamTrans.y - CamInt.y;
	vec.z = CamTrans.z - CamInt.z;

	/* 回転角 */
	ang.y = (float)atan2(vec.x, vec.z);
	ang.x = (float)-atan2(vec.y, sqrt(vec.x*vec.x + vec.z*vec.z));
	ang.z = 0.0f;
	ang.w = 0.0f;
	MocSetKeyFV( cammoc + SCN_MOC_ANG, &ang );
}


void hgdraw::SetCamPos( VECTOR *pos )
{
	MocSetKeyFV( cammoc + SCN_MOC_POS, pos );
	UpdateCam();
}


void hgdraw::SetCamAng( VECTOR *ang )
{
	MocSetKeyFV( cammoc + SCN_MOC_ANG, ang );
	UpdateCam();
}


/*------------------------------------------------------------*/
/*
		light process
*/
/*------------------------------------------------------------*/

int inline hgdraw::CalcLight( VECTOR *normal, int color )
{
	VECTOR pos;
	LIGHTINF *lg;
	unsigned char *cc;
	float ip;
	int a1,a2,a3,col;

	cc = (unsigned char *)&color;
	lg = &light[0];					// LIGHTINF ID#0を使用

	ApplyMatrixRot( &pos, normal );
	
//	ip = pos.x * 0.5f + pos.y * -0.5f;

//	SetVector( &ldir, 0.5f, -0.5f, 0.0f, 0.0f );
	ip = ( pos.x * lg->x + pos.y * lg->y + ( pos.z * 2.8f * lg->z ) ) * 256.0f;
	if ( ip<0.0f ) ip=0.0f;
	col = (int)ip;

//	ip = ip * (128.0f/PI);	
//	ip = ip * 128.0f + 128.0f + 32.0f;

	a1 = (( col * lg->color[2] * cc[2] )>>16) + lg->ambient[2];
	if ( a1 > 255 ) a1 = 255;
	a2 = (( col * lg->color[1] * cc[1] )>>16) + lg->ambient[1];
	if ( a2 > 255 ) a2 = 255;
	a3 = (( col * lg->color[0] * cc[0] )>>16) + lg->ambient[0];
	if ( a3 > 255 ) a3 = 255;

//	a1 = (((int)( ip*lg->color[2] )) + cc[2] + lg->ambient[2] ) >> 1;
//	a2 = (((int)( ip*lg->color[1] )) + cc[1] + lg->ambient[1] ) >> 1;
//	a3 = (((int)( ip*lg->color[0] )) + cc[0] + lg->ambient[0] ) >> 1;

//	a1 = ( col*(lg->color[2] + cc[2] + lg->ambient[2] ) ) >> 9;
//	a2 = ( col*(lg->color[1] + cc[1] + lg->ambient[1] ) ) >> 9;
//	a3 = ( col*(lg->color[0] + cc[0] + lg->ambient[0] ) ) >> 9;
//	col = (int)ip;
//	a1 = (a1*cc[2])>>8;
//	a2 = (a2*cc[1])>>8;
//	a3 = (a3*cc[0])>>8;
	return (a1<<16)|(a2<<8)|a3;
}


LIGHTINF *hgdraw::GetLight( int id )
{
	return &light[id];
}


void hgdraw::InitLight( void )
{
	int i;
	LIGHTINF *lg;
	for(i=0;i<LIGHT_MAX;i++) {
		lg=GetLight(i);
		memset( lg,0,sizeof(LIGHTINF) ); 
	}
	ResetLight();
}


void hgdraw::DeleteLight( void )
{
	int a,i;
	LIGHTINF *lg;
	for(i=0;i<LIGHT_MAX;i++) {
		lg=GetLight(i);
		if ( lg->flag ) {
			for( a=0;a<LIGHT_MOC_MAX;a++) MocDelete( lg->id+a );
			lg->flag = 0;
		}
	}
}


void hgdraw::ResetLight( void )
{
	VECTOR v;
	LIGHTINF *lg;
	lg=GetLight(0);
	lg->flag = 1;
	lg->mode = 0;
	lg->id = MocRegist( LIGHT_MOC_MAX );		// MOCを確保

	//	初期値
	SetVector( &v, 0.0f, 0.0f, 0.0f, 0.0f );
	MocSetKeyFV( lg->id + LIGHT_MOC_POS, &v );
	MocSetKeyFV( lg->id + LIGHT_MOC_FALLOFF, &v );
	SetVector( &v, 3.2f, -0.3f, 0.0f, 0.0f );
	MocSetKeyFV( lg->id + LIGHT_MOC_ANG, &v );
	SetVector( &v, 255.0f, 255.0f, 255.0f, 0.0f );
	MocSetKeyFV( lg->id + LIGHT_MOC_COLOR, &v );
	SetVector( &v, 32.0f, 32.0f, 32.0f, 0.0f );
	MocSetKeyFV( lg->id + LIGHT_MOC_AMBIENT, &v );
}


void hgdraw::UpdateLightSub( int id )
{
	VECTOR v;
	VECTOR ang;
	LIGHTINF *lg;
	lg=GetLight(id);
	MocMove( lg->id, LIGHT_MOC_MAX );
	MocGetKeyFV( lg->id + LIGHT_MOC_ANG, &ang );
	SetVector( &v, 0.0f, 0.0f, 1.0f, 1.0f );
	InitMatrix();
	RotZ( ang.z );
	RotY( ang.y );
	RotX( ang.x );
	ApplyMatrix( &ang, &v );
	lg->x = ang.x;
	lg->y = ang.y;
	lg->z = ang.z;
	MocGetKeyFV( lg->id + LIGHT_MOC_AMBIENT, &v );
	lg->ambient[0]=(short)v.x;
	lg->ambient[1]=(short)v.y;
	lg->ambient[2]=(short)v.z;
	MocGetKeyFV( lg->id + LIGHT_MOC_COLOR, &v );
	lg->color[0]=(short)v.x;
	lg->color[1]=(short)v.y;
	lg->color[2]=(short)v.z;
}

void hgdraw::UpdateLight( void )
{
	int i;
	for(i=0;i<LIGHT_MAX;i++) {
		if ( light[i].flag ) UpdateLightSub( i );
	}
}

int hgdraw::GetLightMoc( int id )
{
	LIGHTINF *lg;
	lg=GetLight(id);
	return lg->id;
}

void hgdraw::SetLightParam( int id, int type, VECTOR *v )
{
	LIGHTINF *lg;
	lg=GetLight(id);
	MocSetKeyFV( lg->id + type, v );
}
