
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
#include "hgobj.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		hgobj interface
*/
/*------------------------------------------------------------*/

hgobj::hgobj( void )
{
	Reset();
}


hgobj::~hgobj( void )
{
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgobj::Reset( void )
{
	int i,size;
	float *p;
	model = NULL;

	p = (float *)&prm;
	size = sizeof(HGMODEL_DRAWPRM) / sizeof(float);
	for(i=0;i<size;i++) { p[i]=0.0f; }

	SetEfx( 256.0f, 0.0f, 0.0f );
	SetScale( 1.0f, 1.0f, 1.0f );
	SetAttribute( NODE_ATTR_NONE );
	SetTexture( -1 );
//	SetProjMatrix( NULL );
//	SetViewMatrix( NULL );
	SetUVAnimation( 0, 0 );

	flag = HGOBJ_FLAG_ENTRY | HGOBJ_FLAG_VISIBLE | HGOBJ_FLAG_MOVING;
	mode = 0;
	shade = 0;
	timer = 0;
	mygroup = 0;
	colgroup = 0;
	colinfo = -1;
	emitid = 0;

	SetColScale( 1.0f, 1.0f, 1.0f );
	SetColParam( 1.0f, 1.0f, 1.0f );
	SetOptInfo( 0,0,0 );

//	option = 0;
	aniframe = 0.0f;
	speed = 1.0f;
//	speed = 1.0f / 30.0f;
	drawflag = 0;
	drawresult = 0;
	parent = NULL;
	child = NULL;
	sibling = NULL;
	matupdate = 0;
	evtimer = 0;
	lookobj = 0;
	colofsy = 0.0f;
	landy = 0.0f;

#if 0
	xanim.anim = 0;
	xanim.animnext = 0;
	xanim.animend = 0;
	xanim.m_1st_time = 0;
	xanim.fCurTime = 0.0f;
#endif

	//	イベントのリセット
	for(i=0;i<HGOBJ_MULTIEVENT_MAX;i++) {
		time[i] = 0.0f;
		event[i] = NULL;
	}
}


VECTOR *hgobj::GetVectorPrm( int id )
{
	return ((VECTOR *)&prm) + id;
}


void hgobj::LookAt( VECTOR *target )
{
	//--------------------------------------------------
	//	ポジション->注視点の回転角を求める
	//--------------------------------------------------
	VECTOR vec;

	/* 注視点->ポジションのベクトル */
	vec.x = prm.pos.x - target->x;
	vec.y = -prm.pos.y - target->y;
	vec.z = prm.pos.z - target->z;
	/* 回転角 */
	prm.rot.y = (float)atan2( vec.x, vec.z );
	prm.rot.x = (float)-atan2( vec.y, sqrt(vec.x*vec.x + vec.z*vec.z) );
	prm.rot.z = 0.0f;
}


void hgobj::TreeAt( VECTOR *target )
{
	//--------------------------------------------------
	//	ポジション->注視点の回転角を求める(Y軸のみ)
	//--------------------------------------------------
	VECTOR vec;

	/* 注視点->ポジションのベクトル */
	vec.x = prm.pos.x - target->x;
	vec.y = -prm.pos.y - target->y;
	vec.z = prm.pos.z - target->z;
	/* 回転角 */
	prm.rot.y = (float)atan2( vec.x, vec.z );
}


void hgobj::UpdateSortZ( void )
{
#if 0
	D3DXMATRIX m;
	D3DXMATRIX *p1;
	D3DXMATRIX *p2;
	VECTOR res;
	p1 = (D3DXMATRIX *)prm.view;
	p2 = (D3DXMATRIX *)prm.proj;
	m =  (*p1) * (*p2);
	D3DXVec3TransformCoord( (D3DXVECTOR3 *)&res, (D3DXVECTOR3 *)&(prm.pos), &m );
	sortz = res.z;
#endif
}


void hgobj::UpdateMatrix( void )
{
	int flag;
//	float rt;
	hgobj *tmpobj;
	VECTOR tmppos;
	VECTOR *pos;
	hgobj *pobj = this;

	flag = model->GetFlag();
	if ( flag == HGMODEL_FLAG_2DSPRITE ) {
		if ( matupdate ) return;
		tmpobj = (hgobj *)(pobj->parent);
		if ( tmpobj != NULL ) {
			tmpobj->UpdateMatrix();
			prm.spr_x = (float)(tmpobj->prm.pos.x + prm.pos.x );
			prm.spr_y = (float)(tmpobj->prm.pos.y + prm.pos.y );
		} else {
			prm.spr_x = (float)prm.pos.x;
			prm.spr_y = (float)prm.pos.y;
		}
		matupdate = 1;		// 計算済みフラグ
		return;
	}
	if ( matupdate ) return;

	pos = &prm.pos;

/*
	if ( mode & HGOBJ_MODE_LAND ) {
		VECTOR tmppos;
		tmpobj = (hgobj *)(pobj->parent);
		if ( tmpobj != NULL ) {
			VECTOR *v0;
			v0 = tmpobj->GetPos();
			tmppos.x = v0->x;
			tmppos.z = v0->z;
			if ( mode & HGOBJ_MODE_STAND ) {
				tmppos.y = prm.landy + pos->y;
			} else {
				tmppos.y = pos->y;
			}
		} else {
			tmppos.x = 0.0f;
			tmppos.y = 0.0f;
			tmppos.z = 0.0f;
		}
		pos = &tmppos;
	}
*/


	if ( mode & (HGOBJ_MODE_LOOKAT|HGOBJ_MODE_XFRONT|HGOBJ_MODE_TREE) ) {
		tmpobj = (hgobj *)(pobj->parent);
		if ( tmpobj != NULL ) {
			AddVector( &tmppos, pos, tmpobj->GetPos() );
			pos = &tmppos;
		}
	}

#if 0

	// 重心移動
	D3DXMATRIXA16 m;
	D3DXMatrixIdentity( &m );
	D3DXMatrixTranslation( &m, pos->x, -pos->y, pos->z );

	// 回転(z-y-x)
	D3DXMATRIXA16 xrot;
	D3DXMATRIXA16 yrot;
	D3DXMATRIXA16 zrot;
	D3DXMatrixRotationX( &xrot, prm.rot.x );
	D3DXMatrixRotationY( &yrot, prm.rot.y );
	D3DXMatrixRotationZ( &zrot, prm.rot.z );

	// スケール
	D3DXMATRIXA16 sc;
	D3DXMatrixScaling( &sc, prm.scale.x, prm.scale.y, prm.scale.z );

	switch( flag ) {
	case HGMODEL_FLAG_XMODEL:
		CSkinModel *mdl;
		mdl = (CSkinModel *)model->GetData();
		mdl->SetView( (D3DXMATRIX *)prm.view );
		mdl->SetProjection( (D3DXMATRIX *)prm.proj );
		mdl->SetTrans(m);
		switch( model->GetRotOrder() ) {
		case HGMODEL_ROTORDER_ZYX:
			m = xrot * yrot * zrot;
			break;
		case HGMODEL_ROTORDER_XYZ:
			m = zrot * yrot * xrot;		// もとの回転順序
			break;
		case HGMODEL_ROTORDER_YXZ:
			m = zrot * xrot * yrot;
			break;
		}
		mdl->SetRot(m);
		mdl->SetScale(sc);
		rt = 1.0f / 255.0f;
		mdl->SetLight( (D3DXVECTOR3 *)prm.lightvec, prm.lightcolor->x * rt, prm.lightcolor->y * rt, prm.lightcolor->z * rt );
		// アンビエント設定
		mdl->SetAmbient( prm.ambcolor->x * rt, prm.ambcolor->y * rt, prm.ambcolor->z * rt );
		return;

	default:
		break;
	}

	switch( model->GetRotOrder() ) {
	case HGMODEL_ROTORDER_ZYX:
		*((D3DXMATRIXA16 *)&prm.mat) = xrot * yrot * zrot * sc * m;		// もとの回転順序
		break;
	case HGMODEL_ROTORDER_XYZ:
		*((D3DXMATRIXA16 *)&prm.mat) = zrot * yrot * xrot * sc * m;
		break;
	case HGMODEL_ROTORDER_YXZ:
		*((D3DXMATRIXA16 *)&prm.mat) = zrot * xrot * yrot * sc * m;
		break;
	}

	if ( mode & (HGOBJ_MODE_LAND|HGOBJ_MODE_LOOKAT|HGOBJ_MODE_XFRONT|HGOBJ_MODE_TREE) ) {
		matupdate = 1;		// 計算済みフラグ
		return;
	}

	while(1) {
		pobj = (hgobj *)(pobj->parent);
		if ( pobj == NULL ) break;
		pobj->UpdateMatrix();
		*((D3DXMATRIXA16 *)&prm.mat) *= *((D3DXMATRIXA16 *)pobj->GetMatrix());
	}
#endif

	matupdate = 1;		// 計算済みフラグ
}


void hgobj::UpdateAnimation( float timepass )
{
	aniframe += speed * timepass;
}


void hgobj::ApplyAnimation( void )
{
#if 0
	int i;
	if ( model != NULL ) {
		i = model->UpdateAnimationFrame( aniframe, &xanim );
		if ( i ) {
			if ( i < 0 ) {
				SetSpeed( 0.0f );
			} else {
				SetSpeed( model->GetDefaultSpeed() );
			}
		}
	}
#endif
}


int hgobj::GetEmptyEvent( void )
{
	int i;
	for(i=0;i<HGOBJ_MULTIEVENT_MAX;i++) {
		if ( event[i] == NULL ) return i;
	}
	return -1;
}


void hgobj::DeleteEvent( int entry )
{
	event[entry] = NULL;
}


void hgobj::StartEvent( hgevent *ev, int entry )
{
	evtimer = 0;
	SetEvent( ev, entry );
}


void hgobj::SetEvent( hgevent *ev, int entry )
{
	int i;
	if ( entry < 0 ) {
		i = GetEmptyEvent();
		if ( i < 0 ) return;
		event[i] = ev;
		time[i] = 0.0f;
	} else {
		event[entry] = ev;
		time[entry] = 0.0f;
	}
}


hgevent *hgobj::GetEvent( int entry )
{
	return event[entry];
}


void hgobj::SetColScale( float fval1, float fval2, float fval3 )
{
	colscale[0] = fval1;
	colscale[1] = fval2;
	colscale[2] = fval3;
}


void hgobj::SetColParam( float fval1, float fval2, float fval3 )
{
	colprm[0] = fval1;
	colprm[1] = fval2;
	colprm[2] = fval3;
}


int hgobj::GetDistance( VECTOR *v, float size )
{
	float sz;
	sz = size * colscale[0];
	if ( fabs( prm.pos.x - v->x ) < sz ) {
		sz = size * colscale[2];
		if ( fabs( prm.pos.z - v->z ) < sz ) {
			sz = size * colscale[1];
			if ( fabs( prm.pos.y - v->y ) < sz ) {
				return 1;
			}
		}
	}
	return 0;
}


float hgobj::GetDistance( VECTOR *v )
{
	return GetVectorDistance( &prm.pos, v );
}


int hgobj::GetDistance2D( VECTOR *v, float size )
{
	float sz;
	sz = size * colscale[0];
	if ( fabs( prm.pos.x - v->x ) < sz ) {
		sz = size * colscale[1];
		if ( fabs( prm.pos.y - v->y ) < sz ) {
			return 1;
		}
	}
	return 0;
}


float hgobj::GetDistance2D( VECTOR *v )
{
	float px,py;
	px = abs( prm.pos.x - v->x );
	py = abs( prm.pos.y - v->y );
	return sqrt( px * px + py * py );
}


void hgobj::StartAnimation( int anim )
{
#if 0
	if ( model != NULL ) {
		if ( anim < 0 ) {
			speed = 0.0f;
		} else {
			speed = model->GetDefaultSpeed();
			aniframe = 0.0f;
			xanim.m_1st_time = 0;
			xanim.anim = anim;
			xanim.animnext = anim;
			xanim.animend = 0;
		}
	}
#endif
}


void hgobj::NextAnimation( int anim )
{
#if 0
	if ( model != NULL ) {
		xanim.animnext = anim;
		xanim.animend = 0;
	}
#endif
}


int hgobj::GetAnimationEnd( void )
{
#if 0
	if ( model != NULL ) {
		return xanim.animend;
	}
#endif
	return -1;
}


void hgobj::SetEmitter( int id )
{
	if ( id < 0 ) {
		mode &= ~HGOBJ_MODE_EMITTER;
	} else {
		emitid = id;
		mode |= HGOBJ_MODE_EMITTER;
	}
}


