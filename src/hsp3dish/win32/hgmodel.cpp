
//
//		High performance Graphic Image access (model)
//			onion software/onitama 2004/11
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif

#include "hgio.h"
#include "hgio_dg.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

hgmodel::hgmodel( void )
{
	flag = HGMODEL_FLAG_NONE;
	deftex = -1;
	defattr = 0;
	tpoly = 0;
	cur = 0;
	data = NULL;
	defspeed = 15.0f;
	SetDefaultColScale( 1.0f, 1.0f, 1.0f );
	SetDefaultColParam( 1.0f, 1.0f, 1.0f );
	shademode = 1;
	rotorder = HGMODEL_ROTORDER_ZYX;

	ua_max = 0;				// UV Animation Max count
	ua_cnt = 0;				// UV Animation counter
	ua_time = 0;			// UV Animation timer
	ua_deftime = 0;			// UV Animation default timer
	loadcnt = 0;			// Model loading count
	animcnt = 0;			// Animation loading count

	meshsx = meshsy = 0;
}


hgmodel::~hgmodel( void )
{
	Terminate();
}


void hgmodel::Terminate( void )
{
#if 0
	int i;
	switch( flag ) {
	case HGMODEL_FLAG_SMODEL:
		for(i=0;i<cur;i++) { DeletePolyNode( i ); }
		break;
	case HGMODEL_FLAG_XMODEL:
		{
		CSkinModel *mdl;
		mdl = (CSkinModel *)data;
		mdl->Release();
		delete mdl;
		data = NULL;
		break;
		}
	case HGMODEL_FLAG_EFXMODEL:
		TouchEfx();
		break;
	default:
		break;
	}
#endif
	if ( data != NULL ) { free( data ); data = NULL; }
	flag = HGMODEL_FLAG_NONE;
}


void hgmodel::RegistData( HGMODEL_FLAG p_flag )
{
	flag = p_flag;
	switch( flag ) {
	case HGMODEL_FLAG_2DSPRITE:
	case HGMODEL_FLAG_POLY4:
	case HGMODEL_FLAG_SMODEL:
	case HGMODEL_FLAG_XMODEL:
		break;
#if 0
	case HGMODEL_FLAG_EFXMODEL:
		data = malloc( sizeof(EFXPOLY) );
		memset( data, 0, sizeof(EFXPOLY) );
		break;
#endif
	default:
		return;
	}
	tpoly = 1;
}

/*------------------------------------------------------------*/
/*
		draw routines
*/
/*------------------------------------------------------------*/

#if 0
void hgmodel::DrawSprite( HGMODEL_DRAWPRM *drawprm )
{
	//		拡大回転スプライト(2D)
	//		(x,y)中心座標 / (sx,sy)サイズ / ang=角度
	//
	POLY4 mem_poly4;
	POLY4 *poly;
	IAXIS2 *iv;
	short tx0, ty0, tx1,ty1;
	float x0,y0,x1,y1,ofsx,ofsy;
	float ang;
	short xx,yy;
	short tsx,tsy;

	ang = drawprm->rot.z;

	poly = (POLY4 *)&mem_poly4;
	xx = ((short)(drawprm->spr_x)) + center_x;
	yy = ((short)(drawprm->spr_y)) + center_y;

	tx0 = uv[0];
	ty0 = uv[1];
	tx1 = uv[2];
	ty1 = uv[3];

	ofsx = ((float)(tx1-tx0)) * drawprm->scale.x;
	ofsy = ((float)(ty1-ty0)) * drawprm->scale.y;

	if ( drawprm->ua_flag ) {
		tsx = tx1 - tx0; tsy = ty1 - ty0;
		tx0 += drawprm->ua_ofsx;
		tx1 += drawprm->ua_ofsx;
		ty0 += drawprm->ua_ofsy;
		ty1 += drawprm->ua_ofsy;
	}

	x0=-(float)sin( ang );
	y0=(float)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

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

	poly->tex = GetTexture();
	poly->attr = drawprm->attr;
	poly->npoly = 1;
	poly->color = 0xffffff;
	poly->alpha = (short)(drawprm->efx.x);
	poly->zattr = drawprm->pos.z;

//	io->DrawPolygonTex( poly );
}
#endif

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int hgmodel::Draw( HGMODEL_DRAWPRM *drawprm )
{
	int df;
	switch( flag ) {
	case HGMODEL_FLAG_2DSPRITE:
		hgio_drawsprite( this, drawprm );
		df = tpoly;
		break;
#if 0
	case HGMODEL_FLAG_POLY4:
		DrawPoly4( drawprm );
		df = tpoly;
		break;
	case HGMODEL_FLAG_SMODEL:
		DrawPolyNode( drawprm );
		df = tpoly;
		break;
	case HGMODEL_FLAG_XMODEL:
		df = DrawXMesh( drawprm );
		break;
	case HGMODEL_FLAG_EFXMODEL:
		DrawEfx( drawprm );
		df = tpoly;
		break;
	case HGMODEL_FLAG_OBAQ3D:
		OmkeDrawSetParam( drawprm, 1 );
		OmkeDrawAll( center_x, 1 );
		df = tpoly;
		break;
	case HGMODEL_FLAG_LINES:
		OmkeDrawSetParam( drawprm, 0 );
		if ( drawprm->tex < 0 ) {
			OmkeDrawLineModel( (center_y<<16)|(center_x&0xffff) );
		} else {
			OmkeDrawLineModelTex( center_x, center_y );
		}
		df = tpoly;
		break;
#endif
	}
	return df;
}


void hgmodel::SetDefaultColScale( float fval1, float fval2, float fval3 )
{
	defcolscale[0] = fval1;
	defcolscale[1] = fval2;
	defcolscale[2] = fval3;
}


void hgmodel::SetDefaultColParam( float fval1, float fval2, float fval3 )
{
	defcolparam[0] = fval1;
	defcolparam[1] = fval2;
	defcolparam[2] = fval3;
}


void hgmodel::SetUV( int tx0, int ty0, int tx1, int ty1 )
{
	int sx,sy;
	sx = tx1 - tx0;
	sy = ty1 - ty0;
	uv[0]=(short)tx0; uv[1]=(short)ty0; uv[2]=(short)tx1; uv[3]=(short)ty1;
	SetOffset( sx/2, sy/2 );
	sizex = (float)sx;
	sizey = (float)sy;
}
