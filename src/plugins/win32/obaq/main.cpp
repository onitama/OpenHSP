
//
//		HSP3.2 OMKE plugin
//		onion software/onitama 2009/6
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "omkedraw.h"
#include "game.h"
#include "supio.h"

#define PI 3.14159265358979f
#define PI2 6.28318530717958f
#define HPI 1.570796326794895f

/*----------------------------------------------------------------*/

static GAME *game;
static VESSEL *pVessel;
static BMSCR *main_bm;
static HSPEXINFO *main_hei;

static float zoom_sx, zoom_sy;
static float disp_sx, disp_sy;
static float center_x, center_y;
static float offset_x, offset_y;

static int find_id, find_group, find_stat;

const int DEFAULT_LOGMAX = 256;

const float DEFAULT_ZOOM = 4.0f;
const float DEFAULT_OFFSET = 0.0f;
const float DEFAULT_WEIGHT = 6.0f;
const float DEFAULT_MOMENT = 1200.0f;
const float DEFAULT_FRICTION = 1.0f;
const float DEFAULT_DAMPER = 0.7f;
const float DEFAULT_INERTIA = 0.999f;
const float DEFAULT_GRAVITY = 1.0f;

static void OmkeInit( void )
{
	game = NULL;
	pVessel = NULL;
	InitSysReq();
	SetSysReq( SYSREQ_MAXOBJ, MAX_ROCK );
	SetSysReq( SYSREQ_MAXLOG, DEFAULT_LOGMAX );
	SetSysReq( SYSREQ_PHYSICS_RATE, RATE_PHYSICS );

	SetSysReq( SYSREQ_DEFAULT_WEIGHT, DEFAULT_WEIGHT );
	SetSysReq( SYSREQ_DEFAULT_MOMENT, DEFAULT_MOMENT );
	SetSysReq( SYSREQ_DEFAULT_DAMPER, DEFAULT_DAMPER );
	SetSysReq( SYSREQ_DEFAULT_FRICTION, DEFAULT_FRICTION );

}


static void OmkeTerm( void )
{
	if ( game != NULL ) { delete game; game = NULL; }
	if ( pVessel != NULL ) { delete pVessel; pVessel = NULL; }
}


static void OmkeReset( void )
{
	OmkeTerm();
	game = new GAME;
	pVessel = new VESSEL;
	main_bm = NULL;

	game->init(pVessel);
	game->random2.setseed(rand() & 32767);

	pVessel->rockbase.setGravity( 0.0f, 0.005f);	// 剛体, デフォルトは(0.0f, 0.0025f)

	find_id = GetSysReqInt( SYSREQ_MAXOBJ );
}

/*----------------------------------------------------------------*/

static void OmkeBMSCR( BMSCR *bm, float zx, float zy, float ofsx, float ofsy  )
{
	//	draw initalize
	main_bm = bm;

	zoom_sx = zx;
	zoom_sy = zy;
	offset_x = ofsx;
	offset_y = ofsy;

	disp_sx = bm->sx / zoom_sx;
	disp_sy = bm->sy / zoom_sy;
	center_x = disp_sx * 0.5f;
	center_y = disp_sy * 0.5f;
}


static int OmkeCnvX( float x )
{
	//		座標変換(X)
	//
	return (int)(x * zoom_sx + offset_x);
}


static int OmkeCnvY( float y )
{
	//		座標変換(Y)
	//
	return (int)(y * zoom_sy + offset_y);
}


static float OmkeCnvXR( int x )
{
	//		座標逆変換(X)
	//
	return ((float)x - offset_x ) / zoom_sx;
}


static float OmkeCnvYR( int y )
{
	//		座標逆変換(Y)
	//
	return ((float)y - offset_y ) / zoom_sy;
}


static void OmkeDrawDebug( BMSCR *bm, ROCK* pR )
{
	//		オブジェクトのDebug用ワイヤーフレーム描画
	//
	int col;
	int rate = GetSysReqInt(SYSREQ_PHYSICS_RATE);

	col = 0xffffff;
	if(pR->stat == ROCK::stat_sleep) {
		if ( (pR->sleepCount/rate) & 2 ) return;
		col = 0x808080;
	}
	OmkeDrawLineStart( bm, col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1], po;
		for(int j = nvo; j < nv; j ++) {
			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			OmkeDrawLine( bm, OmkeCnvX(p.x), OmkeCnvY(p.y), OmkeCnvX(po.x), OmkeCnvY(po.y) );
			//line2D(p.x * (float)ZOOM, p.y * (float)ZOOM, po.x * (float)ZOOM, po.y * (float)ZOOM, color_edge);
		}
		k ++;
	}

	OmkeDrawLineEnd( bm );

	//		中心dot
	//
	int group = pR->group;
	col = 0x808080;
	if ( group & 1 ) col = 0xff0000;
	if ( group & 2 ) col = 0xff00;
	if ( group & 4 ) col = 0xffff00;
	if ( group & 8 ) col = 0xff;
	if ( group & 16 ) col = 0xff00ff;
	if ( group & 32 ) col = 0xffff;
	if ( group & 64 ) col = 0xffffff;
	if ( group & 128 ) col = 0xff80ff;
	if ( group & 256 ) col = 0x80ffff;
	OmkeDrawCircle( bm, OmkeCnvX(pR->center.x) , OmkeCnvY(pR->center.y) , 4, col );
	//circle2D(pR->center.x * (float)ZOOM + ofx, pR->center.y * (float)ZOOM + ofy, 4.0f, color_center);

}


static void OmkeDrawObjectSprite( BMSCR *bm, ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのスプライト描画
	//
	BMSCR *bm2;

	//		中心からスプライトを描画(イメージ等倍)
	//
	bm2 = (BMSCR *)main_hei->HspFunc_getbmscr( gr->material_id );
	OmkeDrawSetGMode( gr->material_gmode, gr->material_grate, gr->material_offsetx, gr->material_offsety );
	OmkeDrawSprite( bm, bm2, gr->material_subid, OmkeCnvX(pR->center.x), OmkeCnvY(pR->center.y), pR->angle, gr->material_zoomx, gr->material_zoomy );
}


static void OmkeDrawObjectSpriteFit( BMSCR *bm, ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのスプライト描画
	//
	BMSCR *bm2;

	//		中心からスプライトを描画(形状フィット)
	//
	int cx,cy;
	int sx,sy;
	int ofsx,ofsy;
	float zx,zy;

	cx = OmkeCnvX(pR->center.x);
	cy = OmkeCnvY(pR->center.y);
/*
	int x,y,minx,miny,maxx,maxy;
	minx = maxx = 0;
	miny = maxy = 0;
	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->spu.rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p;
		for(int j = nvo; j < nv; j ++) {
			p = pVessel->spu.rockbase.vertex[pR->startVertex + j];
			x = OmkeCnvX(p.x); y = OmkeCnvY(p.y);
			if ( x < minx ) minx = x;
			if ( y < miny ) miny = y;
			if ( x > maxx ) maxx = x;
			if ( y > maxy ) maxy = y;
		}
		k ++;
	}
*/
	sx = OmkeCnvX(gr->maxx) - OmkeCnvX(gr->minx) + 1;
	sy = OmkeCnvY(gr->maxy) - OmkeCnvY(gr->miny) + 1;

	bm2 = (BMSCR *)main_hei->HspFunc_getbmscr( gr->material_id );

	zx = (float)sx / (float)bm2->divsx;
	zy = (float)sy / (float)bm2->divsy;

	ofsx = (int)fabs( gr->minx * zoom_sx );
	ofsy = (int)fabs( gr->miny * zoom_sy );

	OmkeDrawSetGMode( gr->material_gmode, gr->material_grate, ofsx + gr->material_offsetx, ofsy + gr->material_offsety );
	OmkeDrawSprite2( bm, bm2, gr->material_subid, cx, cy, pR->angle, zx * gr->material_zoomx, zy * gr->material_zoomy );
}


static void OmkeDrawObjectLine( BMSCR *bm, ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのワイヤーフレーム描画
	//
	int col;

	col = gr->material_id;
	OmkeDrawLineStart( bm, col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1], po;
		for(int j = nvo; j < nv; j ++) {
			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			OmkeDrawLine( bm, OmkeCnvX(p.x), OmkeCnvY(p.y), OmkeCnvX(po.x), OmkeCnvY(po.y) );
		}
		k ++;
	}

	OmkeDrawLineEnd( bm );
}


static void OmkeDrawObjectLine2( BMSCR *bm, ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのワイヤーフレーム描画(輪郭あり)
	//
	int col;
	int x1,y1,x2,y2;

	//		輪郭
	col = gr->material_subid;
	OmkeDrawLineStart( bm, col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1], po;
		for(int j = nvo; j < nv; j ++) {
			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			x1 = OmkeCnvX(p.x); y1 = OmkeCnvY(p.y); x2 = OmkeCnvX(po.x); y2 = OmkeCnvY(po.y);
			OmkeDrawLine( bm, x1+1, y1, x2+1, y2 );
			OmkeDrawLine( bm, x1-1, y1, x2-1, y2 );
			OmkeDrawLine( bm, x1, y1+1, x2, y2+1 );
			OmkeDrawLine( bm, x1, y1-1, x2, y2-1 );
		}
		k ++;
	}

	OmkeDrawLineEnd( bm );


	//		メイン
	col = gr->material_id;
	OmkeDrawLineStart( bm, col );

	k = 0; nv = 0;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1], po;
		for(int j = nvo; j < nv; j ++) {
			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			x1 = OmkeCnvX(p.x); y1 = OmkeCnvY(p.y); x2 = OmkeCnvX(po.x); y2 = OmkeCnvY(po.y);
			OmkeDrawLine( bm, x1, y1, x2, y2 );
		}
		k ++;
	}

	OmkeDrawLineEnd( bm );
}


static void OmkeDraw( BMSCR *bm, int mode )
{
	//		すべてのオブジェクトを描画
	//
	int i,max;
	int rate = GetSysReqInt(SYSREQ_PHYSICS_RATE);
	int type;
	ROCK* pR;
	GAMEROCK *gr;
	max = GetSysReqInt( SYSREQ_MAXOBJ );
	for( i = 0; i < max; i ++) {
		pR = &pVessel->rockbase.rock[i];
		if(pR->stat != ROCK::stat_reserve) {
			gr = pR->pGameRock;
			if (( gr->material_type & GAMEROCK::mattype_delay ) == 0 ) {
				type = gr->material_type & GAMEROCK::mattype_basetype;
				if( pR->stat == ROCK::stat_sleep ) {
					if ( (pR->sleepCount/rate) & 2 ) type = -1;
				}
				switch( type ) {
				case GAMEROCK::mattype_sprite_fit:
					OmkeDrawObjectSpriteFit( bm, pR, gr );
					break;
				case GAMEROCK::mattype_sprite:
					OmkeDrawObjectSprite( bm, pR, gr );
					break;
				case GAMEROCK::mattype_wire:
					OmkeDrawObjectLine( bm, pR, gr );
					break;
				case GAMEROCK::mattype_wire2:
					OmkeDrawObjectLine2( bm, pR, gr );
					break;
				default:
					break;
				}
			}
		}
	}

	for( i = 0; i < max; i ++) {
		pR = &pVessel->rockbase.rock[i];
		if(pR->stat != ROCK::stat_reserve) {
			gr = pR->pGameRock;
			if ( gr->material_type & GAMEROCK::mattype_delay ) {
				type = gr->material_type & GAMEROCK::mattype_basetype;
				if( pR->stat == ROCK::stat_sleep ) {
					if ( (pR->sleepCount/rate) & 2 ) type = -1;
				}
				switch( type ) {
				case GAMEROCK::mattype_sprite_fit:
					OmkeDrawObjectSpriteFit( bm, pR, gr );
					break;
				case GAMEROCK::mattype_sprite:
					OmkeDrawObjectSprite( bm, pR, gr );
					break;
				case GAMEROCK::mattype_wire:
					OmkeDrawObjectLine( bm, pR, gr );
					break;
				case GAMEROCK::mattype_wire2:
					OmkeDrawObjectLine2( bm, pR, gr );
					break;
				default:
					break;
				}
			}
			if ( mode == 0 ) OmkeDrawDebug( bm, pR );
		}
	}

}


static int AddModelSub( float x1, float y1, float rot, float sx, float sy, void *ptr, int nvertex, int vtype, int colsw, int mygroup, int exgroup, int loggroup )
{
	//		自由設定オブジェクト追加
	//
	int i;
	int val;
	float vertex[ MAX_ROCKBASE_VERTEX * 2 ];
	float *vv;
	int *ip;
	int ival;
	double *dp;
	double dval;
	int start_flag;
	float start_x,start_y;
	float xx,yy;

	vv = vertex;
	dp = (double *)ptr;
	ip = (int *)ptr;
	i = 0;
	start_flag = 0;
	while(1) {
		if ( i >= nvertex ) break;
		switch( vtype ) {
		case TYPE_INUM:
			ival = *ip++;
			xx = (float)ival;
			ival = *ip++;
			yy = (float)ival;
			break;
		case TYPE_DNUM:
			dval = *dp++;
			xx = (float)dval;
			dval = *dp++;
			yy = (float)dval;
			break;
		}
		start_flag++;
		if ( start_flag <= 1 ) {
			start_x = xx; start_y = yy;
		} else {
			if (( xx == start_x )&&( yy == start_y )) {
				xx = yy = FLT_MAX;
				start_flag = 0;
			}
		}
		*vv++ = xx;
		*vv++ = yy;
		i+=2;
	}

	//	terminator
	*vv++ = FLT_MAX; *vv++ = 0.0f;
	*vv++ = FLT_MAX; *vv++ = 0.0f;

	val = -1;
	if ( game != NULL ) {
		//val = game->addShape( type, ROCK::stat_active, ROCK::type_normal, x1, y1, rot, sx, sy, colsw, mygroup, exgroup, loggroup );
		val = game->addRock( ROCK::stat_active, ROCK::type_normal,  x1, y1, rot, sx, sy, 0, (FVECTOR2 *)vertex ,margin_default, colsw, mygroup, exgroup );
		if ( val > 0 ) {
			game->setRockGroup( val, mygroup, exgroup, loggroup );
			game->setRockWeight( val, GetSysReq(SYSREQ_DEFAULT_WEIGHT), GetSysReq(SYSREQ_DEFAULT_MOMENT) );
			game->setRockDamper( val, GetSysReq(SYSREQ_DEFAULT_DAMPER), GetSysReq(SYSREQ_DEFAULT_FRICTION) );
		}
	}
	return val;
}




/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		OmkeInit();
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		OmkeTerm();
	}
	return TRUE ;
}

 /*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI qreset( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	int *act;
	BMSCR *bmscr;

	main_hei = hei;
	act = hei->actscr;
	bmscr = (BMSCR *)hei->HspFunc_getbmscr( *act );

	OmkeReset();
	OmkeBMSCR( bmscr, DEFAULT_ZOOM, DEFAULT_ZOOM, DEFAULT_OFFSET, DEFAULT_OFFSET );

	if ( game == NULL ) return -1;

	//	デフォルトの外壁
	game->setBorder( center_x * -0.9f, center_y * -0.9f, center_x * 0.9f, center_y * 0.9f, center_x, center_y );
	return 0;
}


EXPORT BOOL WINAPI qterm( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	OmkeTerm();
	return 0;
}


EXPORT BOOL WINAPI qexec( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	if ( game == NULL ) return -1;

	//	GAMEの更新
	game->proc();
	game->ExecuteAutoWipe();

	// コリジョンログ取得サンプル
	/*
	COLLISIONLOG* pCL;
	while(pCL = pVessel->spu.rockbase.getCollisionLog(0), pCL) {
		DebugMessage(0, "%08x : position = (%f, %f), normal = (%f, %f), depth = %f\n",
		pCL->rockNum, pCL->position.x, pCL->position.y, pCL->normal.x, pCL->normal.y, pCL->depth);
	}
	*/

	return 0;
}


EXPORT BOOL WINAPI qdraw( BMSCR *p1, int p2, int p3, int p4 )
{
	//	(type$02)
	//		オブジェクトを描画
	//		qdraw mode
	//
	if ( game == NULL ) return -1;

	OmkeDraw( main_bm, p2 );
	return 0;
}


EXPORT BOOL WINAPI qview( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ビュー設定
	//		qview zoomx,zoomy,ofsx,ofsy
	//
	int *act;
	BMSCR *bmscr;
	float zx,zy,ofsx,ofsy;

	zx = (float)hei->HspFunc_prm_getdd(DEFAULT_ZOOM);
	zy = (float)hei->HspFunc_prm_getdd(DEFAULT_ZOOM);
	ofsx = (float)hei->HspFunc_prm_getdd(DEFAULT_OFFSET);
	ofsy = (float)hei->HspFunc_prm_getdd(DEFAULT_OFFSET);

	if ( game == NULL ) return -1;

	act = hei->actscr;
	bmscr = (BMSCR *)hei->HspFunc_getbmscr( *act );
	OmkeBMSCR( bmscr, zx, zy, ofsx, ofsy );
	return 0;
}


EXPORT BOOL WINAPI qsetreq( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		システムリクエスト設定
	//		qsetreq reqid,val
	//
	int type;
	float val;
	type = hei->HspFunc_prm_getdi(0);
	val = (float)hei->HspFunc_prm_getdd(0.0);
	SetSysReq( type, val );
	return 0;
}


EXPORT BOOL WINAPI qgetreq( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		システムリクエスト取得
	//		qgetreq var,reqid
	//
	int type;
	double val;
	PVal *pv;
	APTR ap;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	type = hei->HspFunc_prm_getdi(0);
	val = (double)GetSysReq( type );
	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &val );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI qgetptr( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		内部データポインタを取得
	//		qgetptr var1,option
	//
	PVal *pv;
	APTR ap;
	int resx;
	int opt;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;

	switch( opt ) {
	case 0:
		resx = (int)(game);
		break;
	default:
		resx = 0;
		break;
	}
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &resx );			// 変数に値を代入

	return 0;
}


/*------------------------------------------------------------------------------------*/

EXPORT BOOL WINAPI qborder( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		外壁を設定
	//		qborder x1,y1,x2,y2
	//
	float x1,y1,x2,y2;

	x1 = (float)hei->HspFunc_prm_getdd(-100.0);
	y1 = (float)hei->HspFunc_prm_getdd(-100.0);
	x2 = (float)hei->HspFunc_prm_getdd( 100.0);
	y2 = (float)hei->HspFunc_prm_getdd( 100.0);

	if ( game == NULL ) return -1;
	game->setBorder( x1,y1,x2,y2, center_x, center_y );

	return 0;
}


EXPORT BOOL WINAPI qgravity( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		重力を設定
	//		qgravity gx,gy
	//
	float x1,y1;

	x1 = (float)hei->HspFunc_prm_getdd(0.0);
	y1 = (float)hei->HspFunc_prm_getdd(0.005f);

	if ( game == NULL ) return -1;
	pVessel->rockbase.setGravity( x1, y1 );
	return 0;
}


EXPORT BOOL WINAPI qcnvaxis( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		内部座標を表示座標に変換
	//		qcnvaxis var_x,var_y,x,y,option
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	double x,y;
	int resx,resy;
	int opt;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	x = hei->HspFunc_prm_getdd(0.0);
	y = hei->HspFunc_prm_getdd(0.0);
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;

	switch( opt ) {
	case 1:
		resx = (int)x;
		resy = (int)y;
		x = OmkeCnvXR(resx);
		y = OmkeCnvYR(resy);
		hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &x );			// 変数に値を代入
		hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &y );			// 変数に値を代入
		break;
	default:
		resx = OmkeCnvX((float)x);
		resy = OmkeCnvY((float)y);
		hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &resx );			// 変数に値を代入
		hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &resy );		// 変数に値を代入
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI qgetaxis( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		内部座標を取得
	//		qgetaxis num,var_x,var_y,type
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int id,type;
	int resx,resy;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	type = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	resx = 0;
	resy = 0;
	switch( type ) {
	case 0:
		resx = (int)gr->minx; resy = (int)gr->miny;
		break;
	case 1:
		resx = (int)gr->maxx; resy = (int)gr->maxy;
		break;
	case 2:
		resx = OmkeCnvX(gr->maxx) - OmkeCnvX(gr->minx) + 1;
		resy = OmkeCnvY(gr->maxy) - OmkeCnvY(gr->miny) + 1;
		break;
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &resx );			// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &resy );		// 変数に値を代入
	return 0;
}


/*------------------------------------------------------------------------------------*/

EXPORT BOOL WINAPI qaddpoly( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		多角形オブジェクト追加
	//		qaddpoly var, shape, x, y, r, sx, sy, colsw, mygroup, exgroup, loggroup
	//
	float x1,y1,rot,sx,sy;
	int type,colsw,mygroup,exgroup,loggroup;
	int val;
	PVal *pv;
	APTR ap;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	type = hei->HspFunc_prm_getdi(3);
	x1 = (float)hei->HspFunc_prm_getdd(0.0);
	y1 = (float)hei->HspFunc_prm_getdd(0.0);
	rot = (float)hei->HspFunc_prm_getdd(0.0);
	sx = (float)hei->HspFunc_prm_getdd(10.0);
	sy = (float)hei->HspFunc_prm_getdd(10.0);
	colsw = hei->HspFunc_prm_getdi(1);
	mygroup = hei->HspFunc_prm_getdi(1);
	exgroup = hei->HspFunc_prm_getdi(0);
	loggroup = hei->HspFunc_prm_getdi(0);

	val = -1;
	if ( game != NULL ) {
		val = game->addShape( type, ROCK::stat_active, ROCK::type_normal, x1, y1, rot, sx, sy, colsw, mygroup, exgroup, loggroup );
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &val );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qdel( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		オブジェクト削除
	//		qdel id
	//
	int id;
	GAMEROCK *gr;
	id = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;
	game->delRock( gr );

	return 0;
}


EXPORT BOOL WINAPI qpos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		位置、角度パラメーターを設定
	//		qpos num,x,y,angle
	//
	int id;
	float x,y,dval;

	id = hei->HspFunc_prm_getdi(0);
	x = (float)hei->HspFunc_prm_getdd( 0.0 );
	y = (float)hei->HspFunc_prm_getdd( 0.0 );
	dval = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setRockCenter( id, x, y );
	game->setRockAngle( id, dval );
	return 0;
}


EXPORT BOOL WINAPI qgetpos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		位置、角度パラメーターを変数に取得
	//		qgetpos num,x,y,angle
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	double d1,d2,d3;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	d1 = (double)pR->center.x;
	d2 = (double)pR->center.y;
	d3 = (double)pR->angle;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &d3 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qweight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		重さなどのパラメーターを設定
	//		qweight num,weight,moment
	//
	int id;
	float d1,d2;

	id = hei->HspFunc_prm_getdi(0);
	d1 = (float)hei->HspFunc_prm_getdd( DEFAULT_WEIGHT );
	d2 = (float)hei->HspFunc_prm_getdd( DEFAULT_MOMENT );

	if ( game == NULL ) return -1;
	game->setRockWeight( id, d1, d2 );
	return 0;
}


EXPORT BOOL WINAPI qgetweight( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		重さなどのパラメーターを変数に取得
	//		qgetweight num,weight,moment
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	//PVal *pv3;
	//APTR ap3;
	//PVal *pv4;
	//APTR ap4;
	double d1,d2;
	//double d3,d4;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	//ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数
	//ap4 = hei->HspFunc_prm_getva( &pv4 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	d1 = (double)pR->weight;
	d2 = (double)pR->moment;
	//d3 = (double)pR->friction;
	//d4 = (double)pR->buoyancy;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
	//hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &d3 );	// 変数に値を代入
	//hei->HspFunc_prm_setva( pv4, ap4, TYPE_DNUM, &d4 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qspeed( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		加速パラメーターを設定
	//		qspeed num,px,py,pr,opt
	//
	int id,opt;
	float x,y,dval;

	id = hei->HspFunc_prm_getdi(0);
	x = (float)hei->HspFunc_prm_getdd( 0.0 );
	y = (float)hei->HspFunc_prm_getdd( 0.0 );
	dval = (float)hei->HspFunc_prm_getdd( 0.0 );
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;

	game->setRockSpeed( id, x, y, dval, opt );
	return 0;
}



EXPORT BOOL WINAPI qgetspeed( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		加速パラメーターを変数に取得
	//		qgetspeed num,px,py,pr
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	double d1,d2,d3;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	d1 = (double)pR->speed.x;
	d2 = (double)pR->speed.y;
	d3 = (double)pR->rSpeed;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &d3 );	// 変数に値を代入
	return 0;
}



EXPORT BOOL WINAPI qgroup( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		グループパラメーターを設定
	//		qspeed num,group,exgroup,loggroup
	//
	int id;
	int gr1,gr2,gr3;

	id = hei->HspFunc_prm_getdi(0);
	gr1 = hei->HspFunc_prm_getdi(1);
	gr2 = hei->HspFunc_prm_getdi(0);
	gr3 = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockGroup( id, gr1,gr2,gr3 );
	return 0;
}


EXPORT BOOL WINAPI qgetgroup( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		グループパラメーターを変数に取得
	//		qgetgroup num,group,exgroup,loggroup
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int gr1,gr2,gr3;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	gr1 = (int)pR->group;
	gr2 = (int)pR->exceptGroup;
	gr3 = (int)pR->recordGroup;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &gr1 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &gr2 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_INUM, &gr3 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qtype( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		typeパラメーターを設定
	//		qtype num,type,op
	//		(op:0=set,1=on,2=off)
	//
	int id;
	int type,op;

	id = hei->HspFunc_prm_getdi(0);
	type = hei->HspFunc_prm_getdi(0);
	op = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockType( id, (unsigned int)type, op );
	return 0;
}


EXPORT BOOL WINAPI qgettype( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		typeパラメーターを変数に取得
	//		qgettype num,type
	//
	int id;
	PVal *pv;
	APTR ap;
	int type;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	type = (int)pR->type;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &type );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qstat( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		statパラメーターを設定
	//		qstat num,stat,sleepcount
	//
	int id;
	int stat,count;
	int rate = GetSysReqInt(SYSREQ_PHYSICS_RATE);

	id = hei->HspFunc_prm_getdi(0);
	stat = hei->HspFunc_prm_getdi(0);
	count = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockStat( id, (unsigned int)stat, count * rate );
	return 0;
}


EXPORT BOOL WINAPI qgetstat( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		typeパラメーターを変数に取得
	//		qgetstat num,stat,sleepcount
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int stat,count;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	stat = (int)pR->stat;
	count = (int)pR->sleepCount;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &stat );			// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &count );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qcollision( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		コリジョン取得開始
	//		qcollision num, target
	//
	int id,target;

	id = hei->HspFunc_prm_getdi(0);
	target = hei->HspFunc_prm_getdi(-1);

	if ( game == NULL ) return -1;
	game->getCollision( id, target );

	return 0;
}


EXPORT BOOL WINAPI qgetcol( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		コリジョン取得
	//		qgetcol var,x,y
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	COLLISIONLOG *log;
	int id;
	double x,y;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	log = game->getCollisionLog();
	if ( log == NULL ) {
		id = -1;
		x = y = 0.0;
	} else {
		id = (int)log->rockNum & 0xffff;
		if ( id == game->collision_id ) {
			id = (int)((log->rockNum>>16) & 0xffff);
		}
		x = (double)log->position.x;
		y = (double)log->position.y;
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &id );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &x );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &y );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetcol2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		コリジョン詳細情報取得
	//		qgetcol2 depth,nx,ny
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	COLLISIONLOG *log;
	int id;
	double dep,x,y;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	log = game->curlog;
	if ( log == NULL ) {
		dep = x = y = 0.0;
	} else {
		dep = (double)log->depth;
		x = (double)log->normal.x;
		y = (double)log->normal.y;
		id = (int)log->rockNum & 0xffff;
		if ( id == game->collision_id ) {
			x = -x; y = -y;
		}
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &dep );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &x );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &y );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetcol3( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		コリジョン詳細情報取得
	//		qgetcol3 bound,slide
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	COLLISIONLOG *log;
	double x,y;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数

	if ( game == NULL ) return -1;
	log = game->curlog;
	if ( log == NULL ) {
		x = y = 0.0;
	} else {
		x = (double)log->bound;
		y = (double)log->slide;
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &x );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &y );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qmat( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアルパラメーターを設定
	//		qmat num,type,id,subid
	//
	int id,type,matid,matsub;

	id = hei->HspFunc_prm_getdi(0);
	type = hei->HspFunc_prm_getdi(0);
	matid = hei->HspFunc_prm_getdi(0);
	matsub = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockMaterial( id, type, matid, matsub );
	game->setRockMaterial2( id, 0,0, 1.0f, 1.0f );
	game->setRockMaterial3( id, 2, 256 );

	return 0;
}


EXPORT BOOL WINAPI qmat2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアル詳細パラメーターを設定
	//		qmat2 num,offsetx,offsety,zoomx,zoomy
	//
	int id,offsetx,offsety;
	float zoomx,zoomy;

	id = hei->HspFunc_prm_getdi(0);
	offsetx = hei->HspFunc_prm_getdi(0);
	offsety = hei->HspFunc_prm_getdi(0);
	zoomx = (float)hei->HspFunc_prm_getdd(1.0);
	zoomy = (float)hei->HspFunc_prm_getdd(1.0);

	if ( game == NULL ) return -1;
	game->setRockMaterial2( id, offsetx, offsety, zoomx, zoomy );

	return 0;
}


EXPORT BOOL WINAPI qmat3( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアル表示パラメーターを設定
	//		qmat3 num,gmode,rate
	//
	int id,ip1,ip2;

	id = hei->HspFunc_prm_getdi(0);
	ip1 = hei->HspFunc_prm_getdi(0);
	ip2 = hei->HspFunc_prm_getdi(256);

	if ( game == NULL ) return -1;
	game->setRockMaterial3( id,  ip1, ip2 );

	return 0;
}


EXPORT BOOL WINAPI qgetmat( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアルパラメーターを取得
	//		qgetmat num,type,id,subid
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int id;
	int u1,u2,u3;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	u1 = gr->material_type;
	u2 = gr->material_id;
	u3 = gr->material_subid;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_INUM, &u3 );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetmat2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアル詳細パラメーターを取得
	//		qgetmat2 num,offsetx,offsety,zoomx,zoomy
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	PVal *pv4;
	APTR ap4;
	int id;
	int u1,u2;
	double u3,u4;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数
	ap4 = hei->HspFunc_prm_getva( &pv4 );		// パラメータ変数

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	u1 = gr->material_offsetx;
	u2 = gr->material_offsety;
	u3 = (double)gr->material_zoomx;
	u4 = (double)gr->material_zoomy;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &u3 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv4, ap4, TYPE_DNUM, &u4 );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetmat3( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		マテリアル表示パラメーターを取得
	//		qgetmat3 num,gmode,rate
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int id;
	int u1,u2;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	u1 = gr->material_gmode;
	u2 = gr->material_grate;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI quser( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ユーザー定義データを設定
	//		quser num,user1,user2,user3
	//
	int id;
	int u1,u2,u3;

	id = hei->HspFunc_prm_getdi(0);
	u1 = hei->HspFunc_prm_getdi(0);
	u2 = hei->HspFunc_prm_getdi(0);
	u3 = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockUserData( id, u1, u2, u3 );
	return 0;
}


EXPORT BOOL WINAPI quser2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ユーザー定義データを設定2
	//		quser2 num,user4,user5,user6
	//
	int id;
	float u4,u5,u6;

	id = hei->HspFunc_prm_getdi(0);
	u4 = (float)hei->HspFunc_prm_getdd( 0.0 );
	u5 = (float)hei->HspFunc_prm_getdd( 0.0 );
	u6 = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setRockUserData2( id, u4, u5, u6 );
	return 0;
}


EXPORT BOOL WINAPI qgetuser( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ユーザー定義データを取得
	//		qgetuser var,u1,u2,u3
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int id;
	int u1,u2,u3;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	u1 = gr->user1;
	u2 = gr->user2;
	u3 = gr->user3;

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_INUM, &u3 );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetuser2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ユーザー定義データを取得2
	//		qgetuser2 var,u4,u5,u6
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int id;
	double u4,u5,u6;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ変数

	if ( game == NULL ) return -1;
	GAMEROCK *gr = game->getGameRock( id );
	if ( gr == NULL ) return -1;

	u4 = (double)gr->user4;
	u5 = (double)gr->user5;
	u6 = (double)gr->user6;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &u4 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &u5 );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_DNUM, &u6 );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qdamper( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		吸振などのパラメーターを設定
	//		qdamper num,damper,friction
	//
	int id;
	float d1,d2;

	id = hei->HspFunc_prm_getdi(0);
	d1 = (float)hei->HspFunc_prm_getdd( DEFAULT_DAMPER );
	d2 = (float)hei->HspFunc_prm_getdd( DEFAULT_FRICTION );

	if ( game == NULL ) return -1;
	game->setRockDamper( id, d1, d2 );
	return 0;
}


EXPORT BOOL WINAPI qgetdamper( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		吸振などのパラメーターを変数に取得
	//		qgetdamper num,damper,friction
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	double d1,d2;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	d1 = (double)pR->damper;
	d2 = (double)pR->friction;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qpush( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		任意の場所に力を与える
	//		qpush num,xw,yw,ax,ay,sw
	//
	int id,sw;
	float d1,d2,d3,d4;

	id = hei->HspFunc_prm_getdi(0);
	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d4 = (float)hei->HspFunc_prm_getdd( 0.0 );
	sw = hei->HspFunc_prm_getdi(1);

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;
	if ( sw == 0 ) {
		pR->push( d1, d2, d3, d4 );
	} else {
		pR->pushMove( d1, d2, d3, d4 );
	}

	return 0;
}


EXPORT BOOL WINAPI qblast( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		任意の場所に力を与える
	//		qblast xw,yw,power,near,far
	//
	int sw;
	float d1,d2,d3,d4,d5;

	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 1.0 );
	//sw = hei->HspFunc_prm_getdi(1);
	d4 = (float)hei->HspFunc_prm_getdd( 1.0 );
	d5 = (float)hei->HspFunc_prm_getdd( 9999.0 );

	if ( game == NULL ) return -1;
	sw = 1;
	game->BlastAll( d1, d2, d3, d4, d5, sw );

	return 0;
}


EXPORT BOOL WINAPI qaddmodel( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		自由設定オブジェクト追加
	//		qaddmodel var, shape_var, nvertex, x, y, r, sx, sy, colsw, mygroup, exgroup, loggroup
	//
	float x1,y1,rot,sx,sy;
	int colsw,mygroup,exgroup,loggroup;
	int nvertex,vtype;
	int val;
	//float vertex[ MAX_ROCKBASE_VERTEX * 2 ];
	//float *vv;
	//int *ip;
	//int i,ival;
	//double *dp;
	//double dval;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	//int start_flag;
	//float start_x,start_y;
	//float xx,yy;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ2:変数
	nvertex = hei->HspFunc_prm_getdi(-1);
	x1 = (float)hei->HspFunc_prm_getdd(0.0);
	y1 = (float)hei->HspFunc_prm_getdd(0.0);
	rot = (float)hei->HspFunc_prm_getdd(0.0);
	sx = (float)hei->HspFunc_prm_getdd(10.0);
	sy = (float)hei->HspFunc_prm_getdd(10.0);
	colsw = hei->HspFunc_prm_getdi(1);
	mygroup = hei->HspFunc_prm_getdi(1);
	exgroup = hei->HspFunc_prm_getdi(0);
	loggroup = hei->HspFunc_prm_getdi(0);

	vtype = -1;
	if ( pv2->flag == TYPE_INUM ) vtype = TYPE_INUM;
	if ( pv2->flag == TYPE_DNUM ) vtype = TYPE_DNUM;
	if ( vtype < 0 ) return -1;

	if ( nvertex < 0 ) { nvertex = pv2->len[1]; }
	if ( pv2->len[1] < nvertex ) return -2;
	if ( MAX_ROCKBASE_VERTEX < nvertex ) return -3;

	val = AddModelSub( x1, y1, rot, sx, sy, pv2->pt, nvertex, vtype, colsw, mygroup, exgroup, loggroup );
/*

	vv = vertex;
	dp = (double *)pv2->pt;
	ip = (int *)pv2->pt;
	i = 0;
	start_flag = 0;
	while(1) {
		if ( i >= nvertex ) break;
		switch( vtype ) {
		case TYPE_INUM:
			ival = *ip++;
			xx = (float)ival;
			ival = *ip++;
			yy = (float)ival;
			break;
		case TYPE_DNUM:
			dval = *dp++;
			xx = (float)dval;
			dval = *dp++;
			yy = (float)dval;
			break;
		}
		start_flag++;
		if ( start_flag <= 1 ) {
			start_x = xx; start_y = yy;
		} else {
			if (( xx == start_x )&&( yy == start_y )) {
				xx = yy = FLT_MAX;
				start_flag = 0;
			}
		}
		*vv++ = xx;
		*vv++ = yy;
		i+=2;
	}

	//	terminator
	*vv++ = FLT_MAX; *vv++ = 0.0f;
	*vv++ = FLT_MAX; *vv++ = 0.0f;

	val = -1;
	if ( game != NULL ) {
		//val = game->addShape( type, ROCK::stat_active, ROCK::type_normal, x1, y1, rot, sx, sy, colsw, mygroup, exgroup, loggroup );
		val = game->addRock( ROCK::stat_active, ROCK::type_normal,  x1, y1, rot, sx, sy, 0, (FVECTOR2 *)vertex ,margin_default, colsw, mygroup, exgroup );
		if ( val > 0 ) {
			game->setRockGroup( val, mygroup, exgroup, loggroup );
			game->setRockWeight( val, GetSysReq(SYSREQ_DEFAULT_WEIGHT), GetSysReq(SYSREQ_DEFAULT_MOMENT) );
			game->setRockDamper( val, GetSysReq(SYSREQ_DEFAULT_DAMPER), GetSysReq(SYSREQ_DEFAULT_FRICTION) );
		}
	}
*/
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &val );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qfind( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		オブジェクトの検索
	//		qfind group, stat
	//
	int group,stt;

	group = hei->HspFunc_prm_getdi(-1);
	stt = hei->HspFunc_prm_getdi(0);

	find_id = 0;
	find_group = group;
	find_stat = stt;

	return 0;
}


EXPORT BOOL WINAPI qnext( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		検索されたオブジェクトを取得
	//		qnext var
	//
	PVal *pv;
	APTR ap;
	int id;
	ROCK* pR;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数

	if ( game == NULL ) return -1;

	id = -1;
	while(1) {
		if ( find_id >= GetSysReqInt( SYSREQ_MAXOBJ ) ) break;
		pR = game->getRock( find_id );
		if ( pR != NULL ) {
			if ( pR->group & find_group ) {
				if ( find_stat == 0 ) {
					id = find_id++;
					break;
				} else {
					if ( pR->stat == find_stat ) {
						id = find_id++;
						break;
					}
				}
			}
		}
		find_id++;
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &id );		// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qinertia( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		重さなどのパラメーターを設定
	//		qinertia num,inertia,gravity
	//
	int id;
	float d1,d2;

	id = hei->HspFunc_prm_getdi(0);
	d1 = (float)hei->HspFunc_prm_getdd( DEFAULT_INERTIA );
	d2 = (float)hei->HspFunc_prm_getdd( DEFAULT_GRAVITY );

	if ( game == NULL ) return -1;
	game->setRockInertia( id, d1, d2 );
	return 0;
}


EXPORT BOOL WINAPI qgetinertia( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		吸振などのパラメーターを変数に取得
	//		qgetinertia num,inertia,gravity
	//
	int id;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	double d1,d2;

	id = hei->HspFunc_prm_getdi(0);
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ変数

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	if ( pR == NULL ) return -1;

	d1 = (double)pR->inertia;
	d2 = (double)pR->gravity;

	hei->HspFunc_prm_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qgetversion( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		バージョン文字列を変数に取得
	//		qgetversion var
	//
	PVal *pv;
	APTR ap;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	hei->HspFunc_prm_setva( pv, ap, TYPE_STRING, VERINFO );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI qinner( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		座標がオブジェクト内にあるかを取得
	//		qinner var,x,y,num
	//
	int id,res;
	PVal *pv;
	APTR ap;
	float d1,d2;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ変数
	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	id = hei->HspFunc_prm_getdi(-1);

	if ( game == NULL ) return -1;
	res = game->CheckInsidePosition( id, d1, d2 );

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入
	return 0;
}


#ifdef _TACSCAN

EXPORT BOOL WINAPI qguide( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		加速パラメーターを設定
	//		qguide num,x,y,r,maxsp,maxrsp,opt
	//
	int id,opt;
	float x,y,dval,maxsp,maxrsp;

	id = hei->HspFunc_prm_getdi(0);
	x = (float)hei->HspFunc_prm_getdd( 0.0 );
	y = (float)hei->HspFunc_prm_getdd( 0.0 );
	dval = (float)hei->HspFunc_prm_getdd( 0.0 );
	maxsp = (float)hei->HspFunc_prm_getdd( 1.0 );
	maxrsp = (float)hei->HspFunc_prm_getdd( 0.25 );
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	ROCK* pR = game->getRock( id );
	game->moveRockToTarget( pR, x, y, dval, maxsp, maxrsp, opt );
	return 0;
}


EXPORT BOOL WINAPI qtarget( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットの設定
	//		qtarget id,target_id,x,y,speed,opt
	//
	int id,opt,target;
	float d1,d2,d3;

	id = hei->HspFunc_prm_getdi(0);
	target = hei->HspFunc_prm_getdi(-1);
	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 0.0 );
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->resetRockTarget( id, opt, target, d1, d2, d3 );
	return 0;
}


EXPORT BOOL WINAPI qtargetarea( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットの固有エリア設定
	//		qtargetarea px,py,sx,sy
	//
	float d1,d2,d3,d4;

	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d4 = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setTargetDefault( d1, d2, d3, d4 );
	return 0;
}


EXPORT BOOL WINAPI qtargetadd( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットの固有増分設定
	//		qtargetadd px,py
	//
	float d1,d2;

	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setTargetDefaultAdd( d1, d2 );
	return 0;
}


EXPORT BOOL WINAPI qautoarea( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットの自動移動範囲エリア設定
	//		qautoarea x1,y1,x2,y2
	//
	float d1,d2,d3,d4;

	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d4 = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setTargetBorder( d1, d2, d3, d4 );
	return 0;
}


EXPORT BOOL WINAPI qtargetint( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットインターバルの設定
	//		qtargetint id,timer
	//
	int id,opt;

	id = hei->HspFunc_prm_getdi(0);
	opt = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	game->setRockTargetTimer( id, opt );
	return 0;
}


EXPORT BOOL WINAPI qtargetpos( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲット座標の再設定
	//		qtargetpos id,x1,y1,x2,y2
	//
	int id;
	float d1,d2,d3,d4;

	id = hei->HspFunc_prm_getdi(0);
	d1 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d2 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d3 = (float)hei->HspFunc_prm_getdd( 0.0 );
	d4 = (float)hei->HspFunc_prm_getdd( 0.0 );

	if ( game == NULL ) return -1;
	game->setRockTargetArea( id, d1, d2, d3, d4 );
	return 0;
}


/*-----------------------------------------------------------------------------*/

static int qm_colsw,qm_mygroup,qm_exgroup,qm_loggroup;
static int qm_nvertex, qm_vtype;
static float qm_sx, qm_sy;
static void *qm_ptr;
static int qm_mat, qm_tex, qm_subid, qm_line, qm_user1, qm_user2, qm_user3;

static int newRockMutilSub( float x, float y, float spd, float rot, int opt )
{
	int val;
	val = AddModelSub( x, y, rot, qm_sx, qm_sy, qm_ptr, qm_nvertex, qm_vtype, qm_colsw, qm_mygroup, qm_exgroup, qm_loggroup );
	if ( val < 0 ) return -1;
	game->setRockMaterial( val, qm_mat, qm_tex, qm_subid );
	game->setRockMaterial2( val, 0, 0, 1.0f, (float)qm_line );
	game->setRockUserData( val, qm_user1, qm_user2, qm_user3 );

	game->setRockSpeed( val, sin(rot)*spd, cos(rot)*spd, 0.5f, 1 );
	game->setRockInertia( val, 1.0f, 1.0f );
	game->setRockType( val, ROCK::type_autowipe, 1 );
/*
	;vx=atan( myx-dx, myy-dy )
	;qspeed j,sin(vx)*0.3,cos(vx)*0.3,0.5
	;qinertia j,1
	;qtype j, type_autowipe, 1
*/
	return val;
}


static void newRockMutil( int id, float x, float y, float spd, int target, int num, int opt, int ofs )
{
	int i;
	float xx,yy;
	float xx2,yy2;
	float rot,radd;

	if ( id < 0 ) {
		xx = x;
		yy = y;
	} else {
		ROCK* pR = game->getRock(id);
		if ( pR == NULL ) return;
		rot = pR->angle;
		xx = sin( rot )*y;
		yy = cos( rot )*y;
		rot = pR->angle + HPI;
		xx2 = sin( rot )*x;
		yy2 = cos( rot )*x;
		//Alertf( "%f,%f(%f,%f)",xx,yy,x,y );
		xx = pR->center.x + xx + xx2;
		yy = pR->center.y + yy + yy2;
	}

	rot = 0.0f;
	if ( opt == 0 ) {
		radd = PI2 / num;
	} else {
		radd = PI2 / opt;
	}
	if ( target >= 0 ) {
		ROCK* tr = game->getRock(target);
		rot = atan2( tr->center.x - xx, tr->center.y - yy );
	}
	if ( num > 1 ) {
		rot -= radd * (num/2);
	}
	if ( ofs ) {
		rot += radd * ofs;
	}
	for(i=0;i<num;i++) {
		newRockMutilSub( xx,yy,spd,rot,opt );
		rot+=radd;
	}
}


EXPORT BOOL WINAPI qmulti( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ターゲットに向けてオブジェクト生成
	//		(id:ベースID/target:ターゲットID)
	//		(ofsx,ofsy:座標/speed:速度)
	//		(num:個数/opt:option)
	//		qmulti id,ofsx,ofsy,speed,target,num,opt,offset
	//
	int id,num,opt,target,ofs;
	float x,y,spd;

	id = hei->HspFunc_prm_getdi(0);
	x = (float)hei->HspFunc_prm_getdd( 0.0 );
	y = (float)hei->HspFunc_prm_getdd( 0.0 );
	spd = (float)hei->HspFunc_prm_getdd( 0.1 );
	target = hei->HspFunc_prm_getdi(0);
	num = hei->HspFunc_prm_getdi(1);
	opt = hei->HspFunc_prm_getdi(32);
	ofs = hei->HspFunc_prm_getdi(0);

	if ( game == NULL ) return -1;
	newRockMutil( id, x,y,spd, target, num, opt, ofs );
	return 0;
}


EXPORT BOOL WINAPI qmultiprm( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		自由設定オブジェクト追加
	//		qmultiprm shape_var, nvertex, sx, sy, colsw, mygroup, exgroup, loggroup
	//
	//float x1,y1,rot,sx,sy;
	//int colsw,mygroup,exgroup,loggroup;
	int nvertex,vtype;
	//int val;
	//float vertex[ MAX_ROCKBASE_VERTEX * 2 ];
	//float *vv;
	//int *ip;
	//int i,ival;
	//double *dp;
	//double dval;
	PVal *pv2;
	APTR ap2;
	//int start_flag;
	//float start_x,start_y;
	//float xx,yy;

	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ2:変数
	nvertex = hei->HspFunc_prm_getdi(-1);
	qm_sx = (float)hei->HspFunc_prm_getdd(10.0);
	qm_sy = (float)hei->HspFunc_prm_getdd(10.0);
	qm_colsw = hei->HspFunc_prm_getdi(1);
	qm_mygroup = hei->HspFunc_prm_getdi(1);
	qm_exgroup = hei->HspFunc_prm_getdi(0);
	qm_loggroup = hei->HspFunc_prm_getdi(0);

	vtype = -1;
	if ( pv2->flag == TYPE_INUM ) vtype = TYPE_INUM;
	if ( pv2->flag == TYPE_DNUM ) vtype = TYPE_DNUM;
	if ( vtype < 0 ) return -1;

	if ( nvertex < 0 ) { nvertex = pv2->len[1]; }
	if ( pv2->len[1] < nvertex ) return -2;
	if ( MAX_ROCKBASE_VERTEX < nvertex ) return -3;

	qm_nvertex = nvertex;
	qm_vtype = vtype;
	qm_ptr = pv2->pt;

	return 0;
}


EXPORT BOOL WINAPI qmultiprm2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		qmultiprm2 mat,tex,subid,line, user1, user2, user3
	//
	qm_mat = hei->HspFunc_prm_getdi(0);
	qm_tex = hei->HspFunc_prm_getdi(0);
	qm_subid = hei->HspFunc_prm_getdi(0);
	qm_line = hei->HspFunc_prm_getdi(0);
	qm_user1 = hei->HspFunc_prm_getdi(0);
	qm_user2 = hei->HspFunc_prm_getdi(0);
	qm_user3 = hei->HspFunc_prm_getdi(0);

	return 0;
}


#endif

