
//
//		High performance Graphic Image access (emitter)
//			onion software/onitama 2009/12
//

#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif

#ifdef HSPWIN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hgdx.h"
#include "supio.h"

#define PI2_4096 (6.28318530717958f / 4096.0f)
#define FRND_4096 (1.0f / 4096.0f)

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		hgemitter interface
*/
/*------------------------------------------------------------*/

hgemitter::hgemitter( void )
{
	Reset();
}


hgemitter::~hgemitter( void )
{
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgemitter::Reset( void )
{
	mode = EMITMODE_NONE;
	speed = 1.0f; speedopt = 0.0f;

	SetSize( 0.0f, 0.0f, 0.0f );
	SetAngleMul( 1.0f, 1.0f, 1.0f );
	SetAngleOpt( 0.0f, 0.0f, 0.0f );

	out_model = 0;
	out_modelnum = 0;
	out_efx = 0x100;
	out_event = -1;
	out_eventnum = 0;
	out_num = 8;
	out_numopt = 0;
	mygroup = enegroup = 0;

	rotorder = HGMODEL_ROTORDER_ZYX;
}


void hgemitter::GetBasePosition( VECTOR *res, VECTOR *in )
{
	VECTOR padd;
	if ( size.x == 0.0f ) padd.x = 0.0f; else padd.x = ((float)(rand()&4095) * FRND_4096 ) * size.x;
	if ( size.y == 0.0f ) padd.y = 0.0f; else padd.y = ((float)(rand()&4095) * FRND_4096 ) * size.y;
	if ( size.z == 0.0f ) padd.z = 0.0f; else padd.z = ((float)(rand()&4095) * FRND_4096 ) * size.z;
	AddVector( res, in, &padd );
}


void hgemitter::GetAngle( VECTOR *res )
{
	VECTOR padd;
	if ( angmul.x == 0.0f ) padd.x = 0.0f; else padd.x = ((float)(rand()&4095) * PI2_4096 ) * angmul.x;
	if ( angmul.y == 0.0f ) padd.y = 0.0f; else padd.y = ((float)(rand()&4095) * PI2_4096 ) * angmul.y;
	if ( angmul.z == 0.0f ) padd.z = 0.0f; else padd.z = ((float)(rand()&4095) * PI2_4096 ) * angmul.z;
	AddVector( res, &padd, &angopt );
}


void hgemitter::GetCircleAngle( VECTOR *res, float rot )
{
	VECTOR padd;
	padd.x = angmul.x * rot;
	padd.y = angmul.y * rot;
	padd.z = angmul.z * rot;
	AddVector( res, &padd, &angopt );
}


void hgemitter::SetDirectionFromAngle( int flag, VECTOR *res, VECTOR *ang )
{
	float spd;
	VECTOR v;

	spd = speed;
	if ( spd != 0.0f ) spd += ((float)(rand()&4095) * FRND_4096 ) * speedopt;

	if ( flag == HGMODEL_FLAG_2DSPRITE ) {
		SetVector( res, sin(ang->z) * spd, cos(ang->z) * spd, 0.0f, 0.0f );
		return;
	}

	SetVector( &v, 0.0f, 0.0f, spd, 0.0f );
	InitMatrix();
	switch( rotorder ) {
	case HGMODEL_ROTORDER_ZYX:
		RotZ( ang->z );
		RotY( ang->y );
		RotX( ang->x );
		break;
	case HGMODEL_ROTORDER_XYZ:
		RotX( ang->x );
		RotY( ang->y );
		RotZ( ang->z );
		break;
	case HGMODEL_ROTORDER_YXZ:
		RotY( ang->y );
		RotX( ang->x );
		RotZ( ang->z );
		break;
	}
	ApplyMatrix( res, &v );
}


int hgemitter::Emit( hgdx *hg, VECTOR *in )
{
	int i,n,total,type;
	int id,modelid,eventid;
	float rot,prot;
	short df;
	VECTOR vec;
	hgobj *obj;

	total = 0;
	type = mode & 3;
	if ( type == EMITMODE_NONE ) return 0;

	n = out_num;
	if ( out_numopt > 0 ) n += rand()%out_numopt;
	if ( n <= 0 ) return 0;

	if ( type  == EMITMODE_CIRCLE ) {
		rot = 0.0f;
		prot = PI2 / (float)n;
	}

	for(i=0;i<n;i++) {
		modelid = out_model;
		if ( out_modelnum > 0 ) {
			modelid += rand()%out_modelnum;
			//AlertMsgf( "%d===%d",out_modelnum,modelid );
		}
		id = hg->AddObjWithModel( modelid );
		if ( id < 0 ) break;

		obj = hg->GetObj( id );
		df = obj->model->GetFlag();
		obj->mode |= objmode;
		obj->SetEfx( (float)out_efx, 0.0f, 0.0f );
		GetBasePosition( obj->GetPos(), in );

		switch( type ) {
		case EMITMODE_CIRCLE:
			GetCircleAngle( &vec, rot );
			SetDirectionFromAngle( df, obj->GetDir(), &vec );
			rot += prot;
			break;
		case EMITMODE_RANDOM:
			GetAngle( &vec );
			SetDirectionFromAngle( df, obj->GetDir(), &vec );
			break;
		default:
			break;
		}
		if ( mode & EMITMODE_LOOKAT ) {
			CopyVector( obj->GetRot(), &vec );
		}

		if ( out_event >= 0 ) {
			eventid = out_event;
			if ( out_eventnum > 0 ) eventid += rand()%out_eventnum;
			obj->SetEvent( hg->GetEvent(eventid)  ,-1 );
		}
		obj->SetColiGroup( mygroup, enegroup );
		total++;
	}
	return total;
}


