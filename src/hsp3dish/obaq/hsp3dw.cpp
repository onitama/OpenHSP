
//
//	OBAQ hsp3 class
//	onion software/onitama 2011/03
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../hsp3/hsp3config.h"

#include "../../hsp3/hsp3code.h"
#include "../../hsp3/dpmread.h"
#include "../../hsp3/hsp3debug.h"
#include "../../hsp3/strbuf.h"

#include "../sysreq.h"
#include "../supio.h"
#include "../hspwnd.h"
#include "hsp3dw.h"

#include "game.h"
#include "omkedraw.h"

#define MOVEMODE_LINEAR 0
#define MOVEMODE_SPLINE 1
#define MOVEMODE_LINEAR_REL 2
#define MOVEMODE_SPLINE_REL 3

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HSPCTX *hspctx;		// Current Context
static HSPEXINFO *exinfo;
static int *type;
static int *val;
static int *exflg;

static int p1,p2,p3,p4,p5,p6,p7,p8;
static int curtex;
static double dp1,dp2,dp3;
static PVal *p_pval;
static APTR p_aptr;

#define CnvIntRot(val) ((float)val)*(PI2/256.0f)
#define CnvRotInt(val) ((int)(val*(256.0f/PI2)))

static GAME *game;
static VESSEL *pVessel;

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

/*------------------------------------------------------------*/
/*
		service
*/
/*------------------------------------------------------------*/

static void OmkeInit( void )
{
	game = NULL;
	pVessel = NULL;
	//InitSysReq();
	SetSysReq( SYSREQ_MAXOBAQ, MAX_ROCK );
	SetSysReq( SYSREQ_MAXLOG, DEFAULT_LOGMAX );
	SetSysReq( SYSREQ_PHYSICS_RATE, RATE_PHYSICS );

	SetSysReqF( SYSREQ_DEFAULT_WEIGHT, DEFAULT_WEIGHT );
	SetSysReqF( SYSREQ_DEFAULT_MOMENT, DEFAULT_MOMENT );
	SetSysReqF( SYSREQ_DEFAULT_DAMPER, DEFAULT_DAMPER );
	SetSysReqF( SYSREQ_DEFAULT_FRICTION, DEFAULT_FRICTION );

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

	game->init(pVessel);
	game->random2.setseed(rand() & 32767);

	pVessel->rockbase.setGravity( 0.0f, 0.005f);	// 剛体, デフォルトは(0.0f, 0.0025f)

	find_id = GetSysReq( SYSREQ_MAXOBAQ );
}

/*----------------------------------------------------------------*/

static void OmkeBMSCR( BMSCR *bm, float zx, float zy, float ofsx, float ofsy  )
{
	//	draw initalize
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


static void OmkeDrawDebug( ROCK* pR )
{
	//		オブジェクトのDebug用ワイヤーフレーム描画
	//
	int col;
	int rate = GetSysReq(SYSREQ_PHYSICS_RATE);

	col = 0xffffff;
	if(pR->stat == ROCK::stat_sleep) {
		if ( (pR->sleepCount/rate) & 2 ) return;
		col = 0x808080;
	}
	OmkeDrawLineStart( col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1];
		OmkeDrawLineStrip( OmkeCnvX(p.x), OmkeCnvY(p.y) );
		for(int j = nvo; j < nv; j ++) {
//			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			OmkeDrawLineStrip( OmkeCnvX(p.x), OmkeCnvY(p.y) );
			//OmkeDrawLine( OmkeCnvX(p.x), OmkeCnvY(p.y), OmkeCnvX(po.x), OmkeCnvY(po.y) );
		}
		k ++;
	}

	OmkeDrawLineEnd();

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
	OmkeDrawCircle( OmkeCnvX(pR->center.x) , OmkeCnvY(pR->center.y) , 4, col );

}


static void OmkeDrawObjectLine( ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのワイヤーフレーム描画
	//
	int col;

	col = gr->material_id;
	OmkeDrawLineStart( col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1];
		OmkeDrawLineStrip( OmkeCnvX(p.x), OmkeCnvY(p.y) );
		for(int j = nvo; j < nv; j ++) {
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			OmkeDrawLineStrip( OmkeCnvX(p.x), OmkeCnvY(p.y) );
		}
		k ++;
	}

	OmkeDrawLineEnd();
}


static void OmkeDrawObjectLine2( ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのワイヤーフレーム描画(輪郭あり)
	//
	int col;
	int x1,y1,x2,y2;

	//		輪郭
	col = gr->material_subid;
	OmkeDrawLineStart( col );

	int k = 0, nv = 0, nvo;
	while(1) {
		nvo = nv;
		nv = pVessel->rockbase.areaVertex[pR->startAreaVertex + k];
		if(nv == 0) {
			break;
		}
		FVECTOR2 p = pVessel->rockbase.vertexW[pR->startVertexW + nv - 1], po;
		OmkeDrawLineStrip( OmkeCnvX(p.x)-1, OmkeCnvY(p.y) );
		for(int j = nvo; j < nv; j ++) {
			po = p;
			p = pVessel->rockbase.vertexW[pR->startVertexW + j];
			x1 = OmkeCnvX(p.x); y1 = OmkeCnvY(p.y); x2 = OmkeCnvX(po.x); y2 = OmkeCnvY(po.y);

			OmkeDrawLineStrip( x2-1, y2  );
			OmkeDrawLineStrip( x2+1, y2  );
			OmkeDrawLineStrip( x1+1, y1 );
			OmkeDrawLineStrip( x1-1, y1-1 );

		}
		k ++;
	}

	OmkeDrawLineEnd();
	OmkeDrawObjectLine( pR, gr );
}


static void OmkeDrawObjectSpriteFit( ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのスプライト描画
	//

	//		中心からスプライトを描画(形状フィット)
	//
	int cx,cy;
	int sx,sy;
	int ofsx,ofsy;
	float zx,zy;
	BMSCR *bm2;

	cx = OmkeCnvX(pR->center.x);
	cy = OmkeCnvY(pR->center.y);

	sx = OmkeCnvX(gr->maxx) - OmkeCnvX(gr->minx) + 1;
	sy = OmkeCnvY(gr->maxy) - OmkeCnvY(gr->miny) + 1;

	bm2 = (BMSCR *)exinfo->HspFunc_getbmscr( gr->material_id );

	zx = (float)sx / (float)bm2->divsx;
	zy = (float)sy / (float)bm2->divsy;

	ofsx = (int)fabs( gr->minx * zoom_sx );
	ofsy = (int)fabs( gr->miny * zoom_sy );
	ofsx =0;
	ofsy =0;

	OmkeDrawSetGMode( gr->material_gmode, gr->material_grate, ofsx + gr->material_offsetx, ofsy + gr->material_offsety );
	OmkeDrawCelPut( cx, cy, bm2, gr->material_subid, zx * gr->material_zoomx, zy * gr->material_zoomy, pR->angle );
}



static void OmkeDrawObjectSprite( ROCK* pR, GAMEROCK *gr )
{
	//		オブジェクトのスプライト描画
	//
	BMSCR *bm2;

	//		中心からスプライトを描画(イメージ等倍)
	//
	bm2 = (BMSCR *)exinfo->HspFunc_getbmscr( gr->material_id );
	OmkeDrawSetGMode( gr->material_gmode, gr->material_grate, gr->material_offsetx, gr->material_offsety );
	OmkeDrawCelPut( OmkeCnvX(pR->center.x), OmkeCnvY(pR->center.y), bm2, gr->material_subid, gr->material_zoomx, gr->material_zoomy, pR->angle );
}




static void OmkeDraw( int mode )
{
	//		すべてのオブジェクトを描画
	//
	int i,max;
	int rate = GetSysReq(SYSREQ_PHYSICS_RATE);
	int type;
	ROCK* pR;
	GAMEROCK *gr;
	max = GetSysReq( SYSREQ_MAXOBAQ );
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
					OmkeDrawObjectSpriteFit( pR, gr );
					break;
				case GAMEROCK::mattype_sprite:
					OmkeDrawObjectSprite( pR, gr );
					break;
				case GAMEROCK::mattype_wire:
					OmkeDrawObjectLine( pR, gr );
					break;
				case GAMEROCK::mattype_wire2:
					OmkeDrawObjectLine2( pR, gr );
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
					OmkeDrawObjectSpriteFit( pR, gr );
					break;
				case GAMEROCK::mattype_sprite:
					OmkeDrawObjectSprite( pR, gr );
					break;
				case GAMEROCK::mattype_wire:
					OmkeDrawObjectLine( pR, gr );
					break;
				case GAMEROCK::mattype_wire2:
					OmkeDrawObjectLine2( pR, gr );
					break;
				default:
					break;
				}
			}
			if ( mode == 0 ) OmkeDrawDebug( pR );
		}
	}

}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static int _qaddmodel( void )
{
	//		自由設定オブジェクト追加
	//		qaddmodel var, shape_var, nvertex, x, y, r, sx, sy, colsw, mygroup, exgroup, loggroup
	//
	float x1,y1,rot,sx,sy;
	int colsw,mygroup,exgroup,loggroup;
	int i,nvertex,vtype;
	int val;
	float vertex[ MAX_ROCKBASE_VERTEX * 2 ];
	float *vv;
	int *ip;
	int ival;
	double *dp;
	double dval;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int start_flag;
	float start_x,start_y;
	float xx,yy;

	ap = code_getva( &pv );			// パラメータ1:変数
	ap2 = code_getva( &pv2 );		// パラメータ2:変数
	nvertex = code_getdi(-1);
	x1 = (float)code_getdd(0.0);
	y1 = (float)code_getdd(0.0);
	rot = (float)code_getdd(0.0);
	sx = (float)code_getdd(10.0);
	sy = (float)code_getdd(10.0);
	colsw = code_getdi(1);
	mygroup = code_getdi(1);
	exgroup = code_getdi(0);
	loggroup = code_getdi(0);

	vtype = -1;
	if ( pv2->flag == TYPE_INUM ) vtype = TYPE_INUM;
	if ( pv2->flag == TYPE_DNUM ) vtype = TYPE_DNUM;
	if ( vtype < 0 ) return -1;

	if ( nvertex < 0 ) { nvertex = pv2->len[1]; }
	if ( pv2->len[1] < nvertex ) return -2;
	if ( MAX_ROCKBASE_VERTEX < nvertex ) return -3;

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
			game->setRockWeight( val, GetSysReqF(SYSREQ_DEFAULT_WEIGHT), GetSysReqF(SYSREQ_DEFAULT_MOMENT) );
			game->setRockDamper( val, GetSysReqF(SYSREQ_DEFAULT_DAMPER), GetSysReqF(SYSREQ_DEFAULT_FRICTION) );
		}
	}

	code_setva( pv, ap, TYPE_INUM, &val );	// 変数に値を代入
	return 0;
}

static int cmdfunc_extcmd( int cmd )
{
	//		cmdfunc : TYPE_EXTCMD
	//		(内蔵GUIコマンド)
	//
	//Alertf( "OmkeCMD:%x",cmd );
	code_next();							// 次のコードを取得(最初に必ず必要です)
	switch( cmd ) {							// サブコマンドごとの分岐

	//	HGIMG part
	//
	case 0x00:								// qreset
		{
		BMSCR *bm;
		OmkeReset();
		bm = (BMSCR *)exinfo->HspFunc_getbmscr( 0 );
		OmkeBMSCR( bm, DEFAULT_ZOOM, DEFAULT_ZOOM, DEFAULT_OFFSET, DEFAULT_OFFSET );
		if ( game == NULL ) return -1;
		//	デフォルトの外壁
		game->setBorder( center_x * -0.9f, center_y * -0.9f, center_x * 0.9f, center_y * 0.9f, center_x, center_y );
		break;
		}

	case 0x01:								// qterm
		OmkeTerm();
		break;
	case 0x02:								// qexec
		//	GAMEの更新
		{
		//float xx,yy,zz, ang, px,py;
		//gb_getaccel( &xx, &yy, &zz );

		//	ang = xx * 3.14;
		//	px = sinf( ang ); py = cosf( ang );
		//px = xx; py = -yy;
		//pVessel->rockbase.setGravity( px*0.02f, py*0.02f );

		//gb_color( 0,255,0 );
		//xx = 160.0f; yy = 240.0f;
		//gb_line( xx,yy, xx+px*30.0f, yy+py*30.0f );
					
		if ( game == NULL ) return -1;
		game->proc();
		break;
		}
	case 0x03:								// qdraw
		p1 = code_getdi( 0 );
		if ( game == NULL ) return -1;
		OmkeDraw( p1 );
		break;
	case 0x04:								// qview
		{
		BMSCR *bm;
		bm = (BMSCR *)exinfo->HspFunc_getbmscr( 0 );
		float zx,zy,ofsx,ofsy;
		zx = (float)code_getdd(DEFAULT_ZOOM);
		zy = (float)code_getdd(DEFAULT_ZOOM);
		ofsx = (float)code_getdd(DEFAULT_OFFSET);
		ofsy = (float)code_getdd(DEFAULT_OFFSET);
		if ( game == NULL ) return -1;
		OmkeBMSCR( bm, zx, zy, ofsx, ofsy );
		break;
		}
	case 0x05:								// qsetreq
		{
		int type;
		float val;
		type = code_getdi(0);
		val = (float)code_getdd(0.0);
		SetSysReqF( type, val );
		break;
		}
	case 0x06:								// qgetreq
		{
		int type;
		double val;
		PVal *pv;
		APTR ap;
		ap = code_getva( &pv );		// パラメータ1:変数
		type = code_getdi(0);
		val = (double)GetSysReqF( type );
		code_setva( pv, ap, TYPE_DNUM, &val );	// 変数に値を代入
		break;
		}

	case 0x07:								// qborder
		{
		float x1,y1,x2,y2;
		x1 = (float)code_getdd(-100.0);
		y1 = (float)code_getdd(-100.0);
		x2 = (float)code_getdd( 100.0);
		y2 = (float)code_getdd( 100.0);
		if ( game == NULL ) return -1;
		game->setBorder( x1,y1,x2,y2, center_x, center_y );
		break;
		}
	case 0x08:								// qgravity
		{
		float x1,y1;
		x1 = (float)code_getdd(0.0);
		y1 = (float)code_getdd(0.005f);
		if ( game == NULL ) return -1;
		pVessel->rockbase.setGravity( x1, y1 );
		break;
		}
	case 0x09:								// qcnvaxis
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		double x,y;
		int resx,resy;
		int opt;
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		x = code_getdd(0.0);
		y = code_getdd(0.0);
		opt = code_getdi(0);
		if ( game == NULL ) return -1;
		switch( opt ) {
		case 1:
			resx = (int)x;
			resy = (int)y;
			x = OmkeCnvXR(resx);
			y = OmkeCnvYR(resy);
			code_setva( pv, ap, TYPE_DNUM, &x );			// 変数に値を代入
			code_setva( pv2, ap2, TYPE_DNUM, &y );			// 変数に値を代入
			break;
		default:
			resx = OmkeCnvX((float)x);
			resy = OmkeCnvY((float)y);
			code_setva( pv, ap, TYPE_INUM, &resx );			// 変数に値を代入
			code_setva( pv2, ap2, TYPE_INUM, &resy );		// 変数に値を代入
			break;
		}
		break;
		}
	case 0x0a:								// qgetaxis
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		int id,type;
		int resx,resy;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		type = code_getdi(0);
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
		code_setva( pv, ap, TYPE_INUM, &resx );			// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &resy );		// 変数に値を代入
		break;
		}
	case 0x0b:								// qaddpoly
		{
		float x1,y1,rot,sx,sy;
		int type,colsw,mygroup,exgroup,loggroup;
		int val;
		PVal *pv;
		APTR ap;
		ap = code_getva( &pv );		// パラメータ1:変数
		type = code_getdi(3);
		x1 = (float)code_getdd(0.0);
		y1 = (float)code_getdd(0.0);
		rot = (float)code_getdd(0.0);
		sx = (float)code_getdd(10.0);
		sy = (float)code_getdd(10.0);
		colsw = code_getdi(1);
		mygroup = code_getdi(1);
		exgroup = code_getdi(0);
		loggroup = code_getdi(0);
		val = -1;
		if ( game != NULL ) {
			val = game->addShape( type, ROCK::stat_active, ROCK::type_normal, x1, y1, rot, sx, sy, colsw, mygroup, exgroup, loggroup );
		}
		//Alertf( "qaddpoly:%d",val );
		code_setva( pv, ap, TYPE_INUM, &val );	// 変数に値を代入
		break;
		}
	case 0x0c:								// qdel
		{
		int id;
		GAMEROCK *gr;
		id = code_getdi(0);
		if ( game == NULL ) return -1;
		gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		game->delRock( gr );
		break;
		}
	case 0x0d:								// qpos
		{
		int id;
		float x,y,dval;
		id = code_getdi(0);
		x = (float)code_getdd( 0.0 );
		y = (float)code_getdd( 0.0 );
		dval = (float)code_getdd( 0.0 );
		if ( game == NULL ) return -1;
		game->setRockCenter( id, x, y );
		game->setRockAngle( id, dval );
		break;
		}
	case 0x0e:								// qgetpos
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		double d1,d2,d3;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		d1 = (double)pR->center.x;
		d2 = (double)pR->center.y;
		d3 = (double)pR->angle;
		code_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &d3 );	// 変数に値を代入
		break;
		}
	case 0x0f:								// qweight
		{
		int id;
		float d1,d2;
		id = code_getdi(0);
		d1 = (float)code_getdd( DEFAULT_WEIGHT );
		d2 = (float)code_getdd( DEFAULT_MOMENT );
		if ( game == NULL ) return -1;
		game->setRockWeight( id, d1, d2 );
		break;
		}
	case 0x10:								// qgetweight
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		double d1,d2;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		d1 = (double)pR->weight;
		d2 = (double)pR->moment;
		code_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
		break;
		}
	case 0x11:								// qspeed
		{
		int id,opt;
		float x,y,dval;
		id = code_getdi(0);
		x = (float)code_getdd( 0.0 );
		y = (float)code_getdd( 0.0 );
		dval = (float)code_getdd( 0.0 );
		opt = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockSpeed( id, x, y, dval, opt );
		break;
		}
	case 0x12:								// qgetspeed
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		double d1,d2,d3;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		d1 = (double)pR->speed.x;
		d2 = (double)pR->speed.y;
		d3 = (double)pR->rSpeed;
		code_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &d3 );	// 変数に値を代入
		break;
		}
	case 0x13:								// qgroup
		{
		int id;
		int gr1,gr2,gr3;
		id = code_getdi(0);
		gr1 = code_getdi(1);
		gr2 = code_getdi(0);
		gr3 = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockGroup( id, gr1,gr2,gr3 );
		break;
		}
	case 0x14:								// qgetgroup
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		int gr1,gr2,gr3;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		gr1 = (int)pR->group;
		gr2 = (int)pR->exceptGroup;
		gr3 = (int)pR->recordGroup;
		code_setva( pv, ap, TYPE_INUM, &gr1 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &gr2 );	// 変数に値を代入
		code_setva( pv3, ap3, TYPE_INUM, &gr3 );	// 変数に値を代入
		break;
		}
	case 0x15:								// qtype
		{
		int id;
		int type,op;
		id = code_getdi(0);
		type = code_getdi(0);
		op = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockType( id, (unsigned int)type, op );
		break;
		}
	case 0x16:								// qgettype
		{
		int id;
		PVal *pv;
		APTR ap;
		int type;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		type = (int)pR->type;
		code_setva( pv, ap, TYPE_INUM, &type );		// 変数に値を代入
		break;
		}
	case 0x17:								// qstat
		{
		int id;
		int stat,count;
		int rate = GetSysReq(SYSREQ_PHYSICS_RATE);
		id = code_getdi(0);
		stat = code_getdi(0);
		count = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockStat( id, (unsigned int)stat, count * rate );
		break;
		}
	case 0x18:								// qgetstat
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		int stat,count;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		stat = (int)pR->stat;
		count = (int)pR->sleepCount;
		code_setva( pv, ap, TYPE_INUM, &stat );			// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &count );		// 変数に値を代入
		break;
		}
	case 0x19:								// qcollision
		{
		int id,target;
		id = code_getdi(0);
		target = code_getdi(-1);
		if ( game == NULL ) return -1;
		game->getCollision( id, target );
		break;
		}
	case 0x1a:								// qgetcol
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		COLLISIONLOG *log;
		int id;
		double x,y;
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
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
		code_setva( pv, ap, TYPE_INUM, &id );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &x );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &y );		// 変数に値を代入
		break;
		}
	case 0x1b:								// qgetcol2
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		COLLISIONLOG *log;
		int id;
		double dep,x,y;
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
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
		code_setva( pv, ap, TYPE_DNUM, &dep );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &x );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &y );		// 変数に値を代入
		break;
		}
	case 0x1c:								// qgetcol3
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		COLLISIONLOG *log;
		double x,y;
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		log = game->curlog;
		if ( log == NULL ) {
			x = y = 0.0;
		} else {
			x = (double)log->bound;
			y = (double)log->slide;
		}
		code_setva( pv, ap, TYPE_DNUM, &x );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &y );		// 変数に値を代入
		break;
		}
	case 0x1d:								// qmat
		{
		int id,type,matid,matsub;
		id = code_getdi(0);
		type = code_getdi(0);
		matid = code_getdi(0);
		matsub = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockMaterial( id, type, matid, matsub );
		game->setRockMaterial2( id, 0,0, 1.0f, 1.0f );
		game->setRockMaterial3( id, 2, 256 );
		break;
		}
	case 0x1e:								// qmat2
		{
		int id,offsetx,offsety;
		float zoomx,zoomy;
		id = code_getdi(0);
		offsetx = code_getdi(0);
		offsety = code_getdi(0);
		zoomx = (float)code_getdd(1.0);
		zoomy = (float)code_getdd(1.0);
		if ( game == NULL ) return -1;
		game->setRockMaterial2( id, offsetx, offsety, zoomx, zoomy );
		break;
		}
	case 0x1f:								// qmat3
		{
		int id,ip1,ip2;
		id = code_getdi(0);
		ip1 = code_getdi(0);
		ip2 = code_getdi(256);
		if ( game == NULL ) return -1;
		game->setRockMaterial3( id,  ip1, ip2 );
		break;
		}
	case 0x20:								// qgetmat
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		int id;
		int u1,u2,u3;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		GAMEROCK *gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		u1 = gr->material_type;
		u2 = gr->material_id;
		u3 = gr->material_subid;
		code_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_INUM, &u3 );		// 変数に値を代入
		break;
		}
	case 0x21:								// qgetmat2
		{
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
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		ap4 = code_getva( &pv4 );		// パラメータ変数
		if ( game == NULL ) return -1;
		GAMEROCK *gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		u1 = gr->material_offsetx;
		u2 = gr->material_offsety;
		u3 = (double)gr->material_zoomx;
		u4 = (double)gr->material_zoomy;
		code_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &u3 );		// 変数に値を代入
		code_setva( pv4, ap4, TYPE_DNUM, &u4 );		// 変数に値を代入
		break;
		}
	case 0x22:								// qgetmat3
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		int id;
		int u1,u2;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		GAMEROCK *gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		u1 = gr->material_gmode;
		u2 = gr->material_grate;
		code_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
		break;
		}
	case 0x23:								// quser
		{
		int id;
		int u1,u2,u3;
		id = code_getdi(0);
		u1 = code_getdi(0);
		u2 = code_getdi(0);
		u3 = code_getdi(0);
		if ( game == NULL ) return -1;
		game->setRockUserData( id, u1, u2, u3 );
		break;
		}
	case 0x24:								// quser2
		{
		int id;
		float u4,u5,u6;
		id = code_getdi(0);
		u4 = (float)code_getdd( 0.0 );
		u5 = (float)code_getdd( 0.0 );
		u6 = (float)code_getdd( 0.0 );
		if ( game == NULL ) return -1;
		game->setRockUserData2( id, u4, u5, u6 );
		break;
		}
	case 0x25:								// qgetuser
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		int id;
		int u1,u2,u3;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		GAMEROCK *gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		u1 = gr->user1;
		u2 = gr->user2;
		u3 = gr->user3;
		code_setva( pv, ap, TYPE_INUM, &u1 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_INUM, &u2 );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_INUM, &u3 );		// 変数に値を代入
		break;
		}
	case 0x26:								// qgetuser2
		{
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		PVal *pv3;
		APTR ap3;
		int id;
		double u4,u5,u6;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		ap3 = code_getva( &pv3 );		// パラメータ変数
		if ( game == NULL ) return -1;
		GAMEROCK *gr = game->getGameRock( id );
		if ( gr == NULL ) return -1;
		u4 = (double)gr->user4;
		u5 = (double)gr->user5;
		u6 = (double)gr->user6;
		code_setva( pv, ap, TYPE_DNUM, &u4 );		// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &u5 );		// 変数に値を代入
		code_setva( pv3, ap3, TYPE_DNUM, &u6 );		// 変数に値を代入
		break;
		}
	case 0x27:								// qdamper
		{
		int id;
		float d1,d2;
		id = code_getdi(0);
		d1 = (float)code_getdd( DEFAULT_DAMPER );
		d2 = (float)code_getdd( DEFAULT_FRICTION );
		if ( game == NULL ) return -1;
		game->setRockDamper( id, d1, d2 );
		break;
		}
	case 0x28:								// qgetdamper
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		double d1,d2;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		d1 = (double)pR->damper;
		d2 = (double)pR->friction;
		code_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
		break;
		}
	case 0x29:								// qpush
		{
		int id,sw;
		float d1,d2,d3,d4;
		id = code_getdi(0);
		d1 = (float)code_getdd( 0.0 );
		d2 = (float)code_getdd( 0.0 );
		d3 = (float)code_getdd( 0.0 );
		d4 = (float)code_getdd( 0.0 );
		sw = code_getdi(1);
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		if ( sw == 0 ) {
			pR->push( d1, d2, d3, d4 );
		} else {
			pR->pushMove( d1, d2, d3, d4 );
		}
		break;
		}
	case 0x2a:								// qblast
		{
		int sw;
		float d1,d2,d3,d4,d5;
		d1 = (float)code_getdd( 0.0 );
		d2 = (float)code_getdd( 0.0 );
		d3 = (float)code_getdd( 1.0 );
		//sw = code_getdi(1);
		d4 = (float)code_getdd( 1.0 );
		d5 = (float)code_getdd( 9999.0 );
		if ( game == NULL ) return -1;
		sw = 1;
		game->BlastAll( d1, d2, d3, d4, d5, sw );
		break;
		}
	case 0x2b:								// qaddmodel
		return _qaddmodel();
	case 0x2c:								// qfind
		{
		int group,stt;
		group = code_getdi(-1);
		stt = code_getdi(0);
		find_id = 0;
		find_group = group;
		find_stat = stt;
		break;
		}
	case 0x2d:								// qnext
		{
		PVal *pv;
		APTR ap;
		int id;
		ROCK* pR;
		ap = code_getva( &pv );			// パラメータ変数
		if ( game == NULL ) return -1;
		id = -1;
		while(1) {
			if ( find_id >= GetSysReq( SYSREQ_MAXOBAQ ) ) break;
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
		code_setva( pv, ap, TYPE_INUM, &id );		// 変数に値を代入
		break;
		}
	case 0x2e:								// qinertia
		{
		int id;
		float d1,d2;
		id = code_getdi(0);
		d1 = (float)code_getdd( DEFAULT_INERTIA );
		d2 = (float)code_getdd( DEFAULT_GRAVITY );
		if ( game == NULL ) return -1;
		game->setRockInertia( id, d1, d2 );
		break;
		}
	case 0x2f:								// qgetinertia
		{
		int id;
		PVal *pv;
		APTR ap;
		PVal *pv2;
		APTR ap2;
		double d1,d2;
		id = code_getdi(0);
		ap = code_getva( &pv );			// パラメータ変数
		ap2 = code_getva( &pv2 );		// パラメータ変数
		if ( game == NULL ) return -1;
		ROCK* pR = game->getRock( id );
		if ( pR == NULL ) return -1;
		d1 = (double)pR->inertia;
		d2 = (double)pR->gravity;
		code_setva( pv, ap, TYPE_DNUM, &d1 );	// 変数に値を代入
		code_setva( pv2, ap2, TYPE_DNUM, &d2 );	// 変数に値を代入
		break;
		}
	case 0x30:								// qgetversion
		{
		PVal *pv;
		APTR ap;
		ap = code_getva( &pv );			// パラメータ変数
		code_setva( pv, ap, TYPE_STRING, VERINFO );	// 変数に値を代入
		break;
		}
	case 0x31:								// qinner
		{
		int id,res;
		PVal *pv;
		APTR ap;
		float d1,d2;
		ap = code_getva( &pv );			// パラメータ変数
		d1 = (float)code_getdd( 0.0 );
		d2 = (float)code_getdd( 0.0 );
		id = code_getdi(-1);
		if ( game == NULL ) return -1;
		res = game->CheckInsidePosition( id, d1, d2 );
		code_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入
		break;
		}

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//		(内蔵GUI)
	//
	OmkeTerm();
	return 0;
}

void hsp3typeinit_dw_extcmd( HSP3TYPEINFO *info )
{
	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	hspctx = info->hspctx;

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		Application initalize
	//
	OmkeInit();
	OmkeDrawSetBMSCR( (BMSCR *)exinfo->HspFunc_getbmscr( 0 ) );


}

void hsp3typeinit_dw_extfunc( HSP3TYPEINFO *info )
{
}


