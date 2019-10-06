
//
//		High performance Graphic Image access (draw main)
//		onion software/onitama 2011/11
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hgdx.h"
//#include "hgtex.h"
//#include "omkedraw.h"

#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif

#include "sysreq.h"
#include "supio.h"

#define AlertMsgf Alertf

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

int hgdx::Reset( void )
{
	//		Graphics I/O初期化
	//
	int i;
	Terminate();
	hgmode = 1;

	//		Startup phase2
	//
	GeometryInit();

	//		各種パラメーター初期化
	//
	hgmode = 2;
	bgclr = 1;
	bgcolor = 0;
	fntmode = -1;
	fntalpha = 0;
	mozcolors = NULL;
	landaxis = 0.0f;
	landlimit = -15.0f;
	tmes_mode = 0;
	tmes_spacing = 0;
	tmes_lspacing = 0;
	allmove = 1;

	maxobj = GetSysReq( SYSREQ_MAXOBJ );
	mem_obj = (hgobj **)malloc( maxobj * sizeof(void *) );
	mem_sortobj = (hgobj **)malloc( maxobj * sizeof(void *) );
	for(i=0;i<maxobj;i++) { mem_obj[i]=NULL; }

	maxmodel = GetSysReq(  SYSREQ_MAXMODEL );
	mem_model = (hgmodel **)malloc( maxmodel * sizeof(void *) );
	for(i=0;i<maxmodel;i++) { mem_model[i]=NULL; }

	maxevent = GetSysReq( SYSREQ_MAXEVENT );
	mem_event = (hgevent **)malloc( maxevent * sizeof(void *) );
	for(i=0;i<maxevent;i++) { mem_event[i]=NULL; }

	maxemitter = GetSysReq( SYSREQ_MAXEMITTER );
	mem_emitter = (hgemitter **)malloc( maxemitter * sizeof(void *) );
	for(i=0;i<maxemitter;i++) { mem_emitter[i]=NULL; }

	camera = AddObj();
	camera->SetPos( 0.0f, 0.0f, 50.0f );
	camera->SetDir(  0.0f, 0.0f, 0.0f );
	camera->SetEfx( 0.25f*3.141592654f, 0.5f, 768.0f );			// fov,near,far

	SetCamMode( CAMMODE_NORMAL );

	//SetCamMode( CAMMODE_LOOKAT );
	light = AddObj();
	light->SetRot( -1.0f, 1.0f, 0.0f );
	light->SetScale( 255.0f, 255.0f, 255.0f );
	light->SetDir( 30.0f, 30.0f, 30.0f );
	//light->SetDir( 30.0f, 30.0f, 30.0f );
	//light->SetRot( -3.2f, 2.7f, 0.0f );
	//light->SetRot( 1.0f, -0.5f, 0.5f );

	SetVector( &border1, -50.0f, 0.0f, -50.0f, 0.0f );
	SetVector( &border2,  50.0f, -100.0f,  50.0f, 0.0f );

	SetTargetID( 0 );
	SetBoundFactor( 1.0f );
	SetGainFactor( 0.95f, 0.95f );
//	SetViewPort( 0,0, 0,0 );

	return 0;
}


hgdx::hgdx( void )
{
	hgmode = 0;
	light = NULL;
	camera = NULL;
	mem_obj = NULL;
	mem_sortobj = NULL;
	mem_model = NULL;
	mem_event = NULL;
	mem_emitter = NULL;
}


hgdx::~hgdx( void )
{
	Terminate();
}


void hgdx::Terminate( void )
{
	int i;
	if ( hgmode == 0 ) return;
	if ( hgmode > 1 ) {
		//		システムの破棄
		//
		if ( mem_obj != NULL ) {
			for(i=0;i<maxobj;i++) { DeleteObj( i ); }
			free( mem_obj );
			mem_obj = NULL;
		}
		if ( mem_sortobj != NULL ) {
			free( mem_sortobj );
			mem_sortobj = NULL;
		}
		if ( mem_model != NULL ) {
			for(i=0;i<maxmodel;i++) { DeleteModel( i ); }
			free( mem_model );
			mem_model = NULL;
		}
		if ( mem_event != NULL ) {
			for(i=0;i<maxevent;i++) { DeleteEvent( i ); }
			free( mem_event );
			mem_event = NULL;
		}
		if ( mem_emitter != NULL ) {
			for(i=0;i<maxemitter;i++) { DeleteEmitter( i ); }
			free( mem_emitter );
			mem_emitter = NULL;
		}
		GeometryTerm();
	}
	hgmode = 1;
}


void hgdx::SetDest( void *dest, int x, int y, int sx, int sy, int centerx, int centery )
{
	center_x = centerx;
	if ( center_x < 0 ) center_x = (float)(( sx / 2 ) + x);
	center_y = centery;
	if ( center_y < 0 ) center_y = (float)(( sy / 2 ) + y);
	nDestWidth = sx;
	nDestHeight = sy;
	hgio_setcenter( center_x, center_y );
	//io.setDest( dest, x, y, sx, sy );
}


void hgdx::DeleteObjSub( int id )
{
	hgobj *obj;
	hgobj *obj2;
	obj = mem_obj[id];
	if ( obj != NULL ) {
		if ( obj->child != NULL ) {
			obj2 = (hgobj *)obj->child;
			DeleteObjSub( obj2->id );
		}
		if ( obj->sibling != NULL ) {
			obj2 = (hgobj *)obj->sibling;
			DeleteObjSub( obj2->id );
		}
		delete obj;
		mem_obj[id] = NULL;
	}
}


void hgdx::DeleteObj( int id )
{
	hgobj *obj;
	obj = mem_obj[id];
	if ( obj != NULL ) {
		if ( obj->parent != NULL ) {		// 親がある場合は、親から切り離す
			hgobj *tmpobj;
			tmpobj = (hgobj *)obj->parent;
			tmpobj->child = NULL;
			return;
		}
		DeleteObjSub( id );
	}
}


void hgdx::DeleteModel( int id )
{
	hgmodel *model;
	model = GetModel( id );
	if ( model != NULL ) {
		delete model;
		mem_model[id] = NULL;
	}
}


int hgdx::AddSpriteModel( int tex, int attr, int tx0, int ty0, int tx1, int ty1 )
{
	hgmodel *model;
	model = AddModel();
	model->RegistData( HGMODEL_FLAG_2DSPRITE );
	model->SetAttribute( attr );
	model->SetTexture( tex );
//	model->SetOffset( center_x, center_y );
	model->SetUV( tx0, ty0, tx1, ty1 );
	return model->id;
}


int hgdx::AttachObjWithModel( int objid, int modelid )
{
	hgobj *obj;
	hgmodel *model;
	float *fval;

	model = GetModel( modelid );
	if ( model == NULL ) return -1;

	if ( objid < 0 ) { obj = AddObj(); } else { obj = GetObj(objid); }
	if ( obj == NULL ) return -1;
	obj->model = model;
	obj->SetAttribute( obj->model->GetAttribute() );
	obj->SetTexture( obj->model->GetTexture() );
	obj->SetSpeed( model->GetDefaultSpeed() );
	fval = model->GetDefaultColScale();
	obj->SetColScale( fval[0], fval[1], fval[2] );
	fval = model->GetDefaultColParam();
	obj->SetColParam( fval[0], fval[1], fval[2] );
	return obj->id;
}


int hgdx::GetObjModelId( int objid )
{
	int i;
	hgobj *obj;
	hgmodel *model;

	obj = GetObj(objid);
	if ( obj == NULL ) return -1;

	model = obj->model;
	if ( model == NULL ) return -1;

	for(i=0;i<maxmodel;i++) {
		if ( model == GetModel(i) ) return i;
	}
	return -1;
}


int hgdx::AddObjWithModel( int modelid )
{
	return AttachObjWithModel( -1, modelid );
}


hgobj *hgdx::AddObj( void )
{
	int i,sel;
	hgobj *obj;

	sel = -1;
	for(i=0;i<maxobj;i++) {
		if ( mem_obj[i] == NULL ) { sel=i; break; }
	}
	if ( sel >= 0 ) {
		obj = new hgobj;
		obj->id = sel;
//		obj->SetProjMatrix( (MATRIX *)&mProjection );
//		obj->SetViewMatrix( (MATRIX *)&mView );
//		obj->SetLight( light->GetRot() );
//		obj->SetLightColor( light->GetScale() );
//		obj->SetLightAmbient( light->GetDir() );
		mem_obj[sel] = obj;
		return obj;
	}
	return NULL;
}


hgmodel *hgdx::AddModel( void )
{
	int i,sel;
	hgmodel *model;

	sel = -1;
	for(i=0;i<maxmodel;i++) {
		if ( mem_model[i] == NULL ) { sel=i; break; }
	}
	if ( sel >= 0 ) {
		model = new hgmodel;
		model->id = sel;
//		model->SetHGProc( &io );
		mem_model[sel] = model;
		return model;
	}
	return NULL;
}


VECTOR *hgdx::GetObjVectorPrm( int id, int prmid )
{
	hgobj *obj;
	if (( id < 0 )||( id >= maxobj )) return NULL;
	obj = GetObj( id );
	if ( obj == NULL ) return NULL;
	return obj->GetVectorPrm(prmid);
}


hgmodel *hgdx::GetModel( int id )
{
	if (( id < 0 )||( id >= maxmodel )) { AlertMsgf( "Invalid ModelID#%d",id ); return NULL; }
	return mem_model[id];
}


hgobj *hgdx::GetObj( int id )
{
	if (( id < 0 )||( id >= maxobj )) { AlertMsgf( "Invalid ObjID#%d",id ); return NULL; }
	return mem_obj[id];
}


/*------------------------------------------------------------*/
/*
		event process
*/
/*------------------------------------------------------------*/

int hgdx::AttachEvent( int objid, int eventid, int entry )
{
	hgevent *ev;
	hgobj *obj;
	int i;

	obj = GetObj( objid );
	if ( obj == NULL ) return -1;

	if ( eventid < 0 ) {
		obj->DeleteEvent( entry );
		return entry;
	}

	ev = GetEvent( eventid );
	if ( ev == NULL ) return -1;

	i = entry;
	if ( i < 0 ) {
		i = obj->GetEmptyEvent();
		if ( i < 0 ) return -1;
	}
	obj->StartEvent( ev, i );
	return i;
}


int hgdx::GetEmptyEventId( void )
{
	int sel,i;
	sel = -1;
	for(i=0;i<maxevent;i++) {
		if ( mem_event[i] == NULL ) { sel = i; break; }
	}
	return sel;
}


hgevent *hgdx::AddEvent( int eventid )
{
	int sel;
	hgevent *ev;
	hgevent *tmp;
	hgevent *ev_next;
	sel = eventid;
	if ( sel < 0 ) {
		sel = GetEmptyEventId();
		if ( sel < 0 ) return NULL;
	}
	ev = new hgevent;
	ev->id = sel;
	if ( mem_event[ sel ] == NULL ) {			// 新規イベントの追加
		mem_event[ sel ] = ev;
	} else {
		tmp = mem_event[ sel ];
		while(1) {								// 既存のイペントに追加する
			ev_next = tmp->GetNextEvent();
			if ( ev_next == NULL ) { tmp->SetNextEvent( ev ); break; }
			tmp = ev_next;
		}
	}
	return ev;
}


void hgdx::DeleteEvent( int eventid )
{
	hgevent *ev;
	hgevent *nextev;
	ev = mem_event[ eventid ];
	while(1) {
		if ( ev == NULL ) break;
		nextev = ev->GetNextEvent();
		delete ev;
		ev = nextev;
	}
	mem_event[ eventid ] = NULL;
}


float hgdx::GetTimerFromFrame( int frame )
{
	return 1.0f / ((float)frame);
}


int hgdx::AddWaitEvent( int eventid, int frame )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_WAIT;
	if ( frame >= 0 ) {
		ev->timer = GetTimerFromFrame( frame );
	} else {
		ev->source = HGEVENT_SOURCE_WORK;
	}
	return ev->id;
}


int hgdx::AddJumpEvent( int eventid, int gonum, int rate )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_JUMP;
	ev->target = gonum;
	ev->value = rate;
	return ev->id;
}


int hgdx::AddParamEvent( int eventid, int mode, int target, int param )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_PRMSET + mode;
	ev->target = target & HGEVENT_MOCOPT_TARGET_BIT;
	ev->value = param;
	return ev->id;
}


int hgdx::AddMoveEvent( int eventid, int target, float x, float y, float z, int frame, int sw )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	if ( sw == 0 ) {
		ev->flag = HGEVENT_FLAG_MOVETARGET;
	} else {
		ev->flag = HGEVENT_FLAG_MOVETARGET3;
	}
	ev->target = target & HGEVENT_MOCOPT_TARGET_BIT;
	if ( target & HGEVENT_MOCOPT_SRCWORK ) ev->source = HGEVENT_SOURCE_WORK;
	ev->SetEventVector( x, y, z );
	ev->timer = GetTimerFromFrame( frame );
	return ev->id;
}


int hgdx::AddSplineMoveEvent( int eventid, int target, float x, float y, float z, int frame, int sw )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	if ( sw == 0 ) {
		ev->flag = HGEVENT_FLAG_MOVETARGET2;
	} else {
		ev->flag = HGEVENT_FLAG_MOVETARGET4;
	}
	ev->target = target & HGEVENT_MOCOPT_TARGET_BIT;
	if ( target & HGEVENT_MOCOPT_SRCWORK ) {
		ev->source = HGEVENT_SOURCE_WORK;
	}
	ev->SetEventVector( x, y, z );
	ev->timer = GetTimerFromFrame( frame );
	return ev->id;
}


int hgdx::AddPlusEvent( int eventid, int target, float x, float y, float z )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_PLUSTARGET;
	ev->target = target & HGEVENT_MOCOPT_TARGET_BIT;
	if ( target & HGEVENT_MOCOPT_SRCWORK ) ev->source = HGEVENT_SOURCE_WORK;
	ev->SetEventVector( x, y, z );
	return ev->id;
}


int hgdx::AddChangeEvent( int eventid, int target, float x1, float y1, float z1, float x2, float y2, float z2 )
{
	float f;
	hgevent *ev;
	f = 1.0f / 1024.0f;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_SETTARGET;
	ev->target = target & HGEVENT_MOCOPT_TARGET_BIT;
	ev->SetEventVector( x1, y1, z1 );
	ev->SetEventVector2( (x2-x1)*f, (y2-y1)*f, (z2-z1)*f );
	return ev->id;
}


int hgdx::AddUVEvent( int eventid, int ysize, int count )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_UVANIM;
	ev->target = ysize;
	ev->value = count;
	return ev->id;
}


int hgdx::AddRegobjEvent( int eventid, int model, int event )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_REGOBJ;
	ev->target = model;
	ev->value = event;
	return ev->id;
}


int hgdx::AddEPrimEvent( int eventid, int model, int prm, float x1, float y1 )
{
	float f;
	hgevent *ev;
	f = 1.0f / 1024.0f;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_SETEPRIM;
	ev->target = model;
	ev->value = prm;
	ev->SetEventVector( x1, (y1-x1)*f, 0.0f );
	return ev->id;
}


int hgdx::AddAimEvent( int eventid, int target, int mode, float x, float y, float z )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_AIM;
	ev->target = target;
	ev->value = mode;
	ev->SetEventVector( x, y, z );
	return ev->id;
}


int hgdx::AddAnimEvent( int eventid, int anim, int opt, float speed )
{
	hgevent *ev;
	ev = AddEvent( eventid );
	ev->flag = HGEVENT_FLAG_OBJACT;
	ev->target = anim;
	ev->value = opt;
	ev->SetEventVector( speed, 0.0f, 0.0f );
	return ev->id;
}


void hgdx::StoreNextVector( hgevent *myevent )
{
	//		次のmovevectorを検索して補完する
	//
	int flag,fl;
	int target;
	hgevent *lastjp;
	hgevent *ev;
	hgevent *nextev;
	target = myevent->target;
	lastjp = NULL;

	ev = myevent->GetNextEvent();
	while(1) {
		if ( ev == NULL ) break;
		if ( ev == myevent ) break;
		flag = ev->flag;
		if (( flag == HGEVENT_FLAG_MOVETARGET2 )||( flag == HGEVENT_FLAG_MOVETARGET4 )) {			// スプライン移動か?
			if ( ev->target == target ) {															// 自分と同じターゲットか?
				myevent->nextvec = ev->GetEventVector(); return;
			}
		}
		if ( flag == HGEVENT_FLAG_JUMP ) {
			nextev = mem_event[ ev->id ];
			for(fl=0;fl<ev->target;fl++) { nextev = nextev->GetNextEvent(); }
			ev = nextev;
			if ( lastjp == ev ) break;
			lastjp = ev;
		} else {
			ev = ev->GetNextEvent();
		}
	}

	myevent->nextvec = myevent->GetEventVector();			// 次のデータがない場合は自分のvectorを入れる
}


void hgdx::ExecuteObjEvent( hgobj *obj, float timepass, int entry )
{
	//		イベント実行
	//
	hgevent *ev;
	hgevent *nextev;
	VECTOR *v0;
	VECTOR *v1;
	VECTOR *v2;
	VECTOR *v3;
	VECTOR *v4;
	VECTOR *pdir;
	VECTOR tmp;
	VECTOR tmp2;
	float tbase;
	float tval;
	short *p;
	int fl;
	ev = obj->GetEvent(entry);
	tbase = obj->time[entry];
	obj->time[entry] += timepass;

	while( ev != NULL ) {
		switch( ev->flag ) {
		case HGEVENT_FLAG_WAIT:
			if ( ev->source != HGEVENT_SOURCE_VEC ) {
				v1 = obj->GetVectorPrm( ev->source );
				if ( v1->x == 0.0f ) tval = 1.0f; else tval = tbase / v1->x;
			} else {
				tval = tbase * ev->timer;
			}
			if ( tval >= 1.0f ) {
				obj->SetEvent( ev->GetNextEvent(), entry );		// 次のイベントへ進む
				return;
			}
			return;
		case HGEVENT_FLAG_JUMP:
			if ( ev->value ) {
				fl = rand()%100;
				if ( fl >= ev->value ) {
					obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
					break;
				}
			}
			nextev = mem_event[ ev->id ];
			for(fl=0;fl<ev->target;fl++) { nextev = nextev->GetNextEvent(); }
			obj->SetEvent( nextev, entry );
			return;
		case HGEVENT_FLAG_PRMSET:
			if ( ev->target == 0 ) { obj->mode = ev->value; break; }
			p = (short *)&obj->flag;
			p[ ev->target - 1 ] = (short)ev->value;
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
		case HGEVENT_FLAG_PRMON:
			if ( ev->target == 0 ) { obj->mode |= ev->value; break; }
			p = (short *)&obj->flag;
			p[ ev->target - 1 ] |= (short)ev->value;
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
		case HGEVENT_FLAG_PRMOFF:
			fl = ~ev->value;
			if ( ev->target == 0 ) { obj->mode &= fl; break; }
			p = (short *)&obj->flag;
			p[ ev->target - 1 ] &= (short)fl;
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
		case HGEVENT_FLAG_UVANIM:
			{
			HGMODEL_DRAWPRM *prm;
			prm = obj->GetPrm();
			prm->ua_flag = 1;
			prm->ua_ofsx = ev->target;
			prm->ua_ofsy = ev->value;
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
			}
		case HGEVENT_FLAG_MOVETARGET:
			// targetにlinear補間
			v0 = obj->GetVectorPrm( ev->target );
			v1 = obj->GetVectorPrm( MOC_POS2 + ev->target );
			if ( ev->source == HGEVENT_SOURCE_VEC ) v2 = ev->GetEventVector();
			else {
				v2 = obj->GetVectorPrm( ev->source );
			}
			if ( tbase == 0.0f ) {
				CopyVector( v1, v0 );
			}
			tval = tbase * ev->timer;
			if ( tval > 1.0f ) tval = 1.0f;
			SubVector( &tmp, v2, v1 );
			ScaleVector( &tmp, &tmp, tval );
			AddVector( v0, v1, &tmp );
			break;
		case HGEVENT_FLAG_MOVETARGET2:
			{
			// targetにspline補間
			VECTOR	key0;
			VECTOR	key1;
			VECTOR	key2;
			VECTOR	key3;
			VECTOR	fa;
			VECTOR	fb;
			VECTOR	*nextv;
			float t1,t2,t3;
			if ( ev->nextvec == NULL ) StoreNextVector( ev );
			v0 = obj->GetVectorPrm( ev->target );
			v1 = obj->GetVectorPrm( MOC_POS2 + ev->target );
			nextv = ev->nextvec;
			if ( ev->source == HGEVENT_SOURCE_VEC ) v2 = ev->GetEventVector();
			else {
				v2 = obj->GetVectorPrm( ev->source );
				nextv = v2;
			}
			if ( tbase == 0.0f ) {
				CopyVector( v1, v0 );
			}
			tval = tbase * ev->timer;
			if ( tval > 1.0f ) tval = 1.0f;
			t1 = tval;
			t2 = t1 * t1;
			t3 = t2 * t1;

			//	r0 = kframe[i];
			//	r1 = kframe[i+1];
			//	r2 = kframe[i+1]-kframe[i];
			//	r3 = kframe[i+2]-kframe[i+1];
			CopyVector( &key0 , v1 );
			CopyVector( &key1 , v2 );
			SubVector(  &key2 , &key1, &key0 );
			SubVector(  &key3 , nextv, &key1 );

			//	fa = 2.0f * r0 - 2.0f * r1 + r2 + r3;
			ScaleVector( &fa , &key0, 2.0f );
			ScaleVector( &tmp , &key1, 2.0f );
			SubVector( &fa , &fa, &tmp );
			AddVector( &fa , &fa, &key2 );
			AddVector( &fa , &fa, &key3 );

			//	fb = -3.0f * r0 + 3.0f * r1 - 2.0f * r2 - r3;
			ScaleVector( &fb , &key0, -3.0f );
			ScaleVector( &tmp , &key1, 3.0f );
			AddVector( &fb , &fb, &tmp );
			ScaleVector( &tmp , &key2, 2.0f );
			SubVector( &fb , &fb, &tmp );
			SubVector( &fb , &fb, &key3 );

			//	fc = r2;
			//	fd = r0;
			//	res = fa * t3 + fb * t2 + fc * t + fd;
			ScaleVector( &fa, &fa, t3 );
			ScaleVector( &fb, &fb, t2 );
			ScaleVector( &key2, &key2, t1 );
			AddVector( &tmp, &fa, &fb );
			AddVector( &tmp, &tmp, &key2 );
			AddVector( v0, &tmp, &key0 );
			break;
			}
		case HGEVENT_FLAG_MOVETARGET3:
			// targetにlinear補間(相対)
			v0 = obj->GetVectorPrm( ev->target );
			v1 = obj->GetVectorPrm( MOC_POS2 + ev->target );
			if ( ev->source == HGEVENT_SOURCE_VEC ) v2 = ev->GetEventVector();
			else {
				v2 = obj->GetVectorPrm( ev->source );
			}
			if ( tbase == 0.0f ) {
				CopyVector( v1, v0 );
			}
			tval = tbase * ev->timer;
			if ( tval > 1.0f ) tval = 1.0f;
			ScaleVector( &tmp, v2, tval );
			AddVector( v0, v1, &tmp );
			break;
		case HGEVENT_FLAG_MOVETARGET4:
			{
			// targetにspline補間(相対)
			VECTOR	key0;
			VECTOR	key1;
			VECTOR	key2;
			VECTOR	key3;
			VECTOR	fa;
			VECTOR	fb;
			VECTOR	*nextv;
			float t1,t2,t3;
			if ( ev->nextvec == NULL ) StoreNextVector( ev );
			v0 = obj->GetVectorPrm( ev->target );
			v1 = obj->GetVectorPrm( MOC_POS2 + ev->target );
			nextv = ev->nextvec;
			if ( ev->source == HGEVENT_SOURCE_VEC ) v2 = ev->GetEventVector();
			else {
				v2 = obj->GetVectorPrm( ev->source );
				nextv = v2;
			}
			if ( tbase == 0.0f ) {
				CopyVector( v1, v0 );
			}
			tval = tbase * ev->timer;
			if ( tval > 1.0f ) tval = 1.0f;
			t1 = tval;
			t2 = t1 * t1;
			t3 = t2 * t1;

			//	r0 = kframe[i];
			//	r1 = kframe[i+1];
			//	r2 = kframe[i+1]-kframe[i];
			//	r3 = kframe[i+2]-kframe[i+1];
			CopyVector( &key0 , v1 );
			CopyVector( &key1 , v1 ); AddVector( &key1, &key1, v2 );
			CopyVector( &key2 , v2 );
			CopyVector( &key3 , nextv );

			//	fa = 2.0f * r0 - 2.0f * r1 + r2 + r3;
			ScaleVector( &fa , &key0, 2.0f );
			ScaleVector( &tmp , &key1, 2.0f );
			SubVector( &fa , &fa, &tmp );
			AddVector( &fa , &fa, &key2 );
			AddVector( &fa , &fa, &key3 );

			//	fb = -3.0f * r0 + 3.0f * r1 - 2.0f * r2 - r3;
			ScaleVector( &fb , &key0, -3.0f );
			ScaleVector( &tmp , &key1, 3.0f );
			AddVector( &fb , &fb, &tmp );
			ScaleVector( &tmp , &key2, 2.0f );
			SubVector( &fb , &fb, &tmp );
			SubVector( &fb , &fb, &key3 );

			//	fc = r2;
			//	fd = r0;
			//	res = fa * t3 + fb * t2 + fc * t + fd;
			ScaleVector( &fa, &fa, t3 );
			ScaleVector( &fb, &fb, t2 );
			ScaleVector( &key2, &key2, t1 );
			AddVector( &tmp, &fa, &fb );
			AddVector( &tmp, &tmp, &key2 );
			AddVector( v0, &tmp, &key0 );
			break;
			}
		case HGEVENT_FLAG_PLUSTARGET:				
			// target+=dir:dir+=vec を実行する
			v0 = obj->GetVectorPrm( ev->target );
			if ( ev->source == HGEVENT_SOURCE_VEC ) {
				v1 = ev->GetEventVector();
			} else {
				v1 = obj->GetVectorPrm( ev->source );
			}
			AddVector( v0, v0, v1 );
			break;
		case HGEVENT_FLAG_SETTARGET:
			{
			// targetを再設定する
			float t1,t2,t3;
			v0 = obj->GetVectorPrm( ev->target );
			v1 = ev->GetEventVector();
			v2 = ev->GetEventVector2();
			if ( v2->x == 0.0f ) { v0->x = v1->x; }
			else {
				t1 = (float)( rand() & 1023 );
				v0->x = v2->x * t1 + v1->x;
			}
			if ( v2->y == 0.0f ) { v0->y = v1->y; }
			else {
				t2 = (float)( rand() & 1023 );
				v0->y = v2->y * t2 + v1->y;
			}
			if ( v2->z == 0.0f ) { v0->z = v1->z; }
			else {
				t3 = (float)( rand() & 1023 );
				v0->z = v2->z * t3 + v1->z;
			}
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
			}
		case HGEVENT_FLAG_SUICIDE:
			// イベント自体を消滅させる
			obj->SetEvent( NULL, entry );
			fl = ev->id;
			DeleteEvent( fl );
			return;
		case HGEVENT_FLAG_REGOBJ:
			{
			//	新しいOBJを設定
			int objid;
			VECTOR	tmp;
			objid = AddObjWithModel( ev->target );
			AttachEvent( objid, ev->value, -1 );
			v0 = obj->GetVectorPrm( 0 );
			v1 = ev->GetEventVector();
			AddVector( &tmp, obj->GetPos(), obj->GetWork() );
			GetObj(objid)->SetPos( tmp.x, tmp.y, tmp.z );
			//
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
			}
#if 0
		case HGEVENT_FLAG_SETEPRIM:
			{
			//	eprimを設定
			hgmodel *model;
			float t1,val;
			if ( ev->target < 0 ) {
				model = obj->model;
			} else {
				model = GetModel( ev->target );
			}
			if ( model != NULL ) {
				v1 = ev->GetEventVector();
				if ( v1->y == 0.0f ) {
					val = v1->x;
				} else {
					t1 = (float)( rand() & 1023 );
					val = t1 * v1->y + v1->x;
				}
				if ( ev->value >= 16 ) {
					model->SetEfxColorFloat( ev->value - 16, val );
				} else {
					model->SetEfxInfo( ev->value, val );
				}
			}
			//obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
			}
#endif
		case HGEVENT_FLAG_AIM:
			{
			//	向きを設定
			int i,j,val,evpass;
			hgobj *obj2;
			evpass = 1;
			v0 = obj->GetVectorPrm( ev->target );
			v1 = ev->GetEventVector();
			val = ev->value;
			if ( val & 0xf0000 ) {
				obj2 = GetObj( val & 0xffff );
				if (( obj2->flag & HGOBJ_FLAG_ENTRY ) == 0 ) break;
				v2 = obj2->GetVectorPrm( ev->target );
				val = (val>>16) - 1;
			} else {
				v2 = obj->GetWork();
			}
			switch( val ) {
			case 0:
				SubVector( &tmp, v2, v0 );
				if ( tmp.x > v1->x ) tmp.x = v1->x;
				if ( tmp.y > v1->y ) tmp.y = v1->y;
				if ( tmp.z > v1->z ) tmp.z = v1->z;
				if ( tmp.x < -v1->x ) tmp.x = -v1->x;
				if ( tmp.y < -v1->y ) tmp.y = -v1->y;
				if ( tmp.z < -v1->z ) tmp.z = -v1->z;
				AddVector( v0, v0, &tmp );
				GetTargetAngle( &tmp, v0, v2 );
				obj->SetRot( -tmp.x, -tmp.y, -tmp.z );
				evpass = 0;
				break;
			case 1:
				SubVector( &tmp, v2, v0 );
				if ( tmp.x > v1->x ) tmp.x = v1->x;
				if ( tmp.y > v1->y ) tmp.y = v1->y;
				if ( tmp.z > v1->z ) tmp.z = v1->z;
				if ( tmp.x < -v1->x ) tmp.x = -v1->x;
				if ( tmp.y < -v1->y ) tmp.y = -v1->y;
				if ( tmp.z < -v1->z ) tmp.z = -v1->z;
				obj->SetDir( tmp.x, tmp.y, tmp.z );
				break;
			case 2:
				GetTargetAngle( &tmp, v0, v2 );
				//obj->SetRot( 0.0f, -tmp.y, 0.0f );
				InitMatrix();
				RotY( tmp.y + obj->GetWork()->x );
				ApplyMatrix( &tmp, v1 );
				obj->SetDir( -tmp.x, obj->GetWork()->y, -tmp.z );
				break;
			case 3:
				if ( obj->evtimer >= 5 ) break;
			case 4:
				GetTargetAngle( &tmp, v0, v2 );
				obj->SetRot( -tmp.x, -tmp.y, -tmp.z );
				InitMatrix();
				RotY( tmp.y );
				RotX( -tmp.x );
				ApplyMatrix( &tmp, v1 );
				obj->SetDir( -tmp.x, -tmp.y, -tmp.z );
				obj->evtimer++;
				break;
			case 5:
				if ( obj->evtimer >= 20 ) break;
			case 6:
				GetTargetAngle( &tmp, v0, v2 );

				if ( obj->evtimer > 0 ) {
					//DebugMsg( "#%f => %f",tmp.y, obj->GetRot()->y );
					v3 = obj->GetWork();
					v4 = obj->GetRot();

					tmp2.x = tmp.x - v4->x;
					if ( abs( tmp.x - (v4->x+PI2)) < abs(tmp2.x) ) tmp2.x = tmp.x - (v4->x+PI2);
					tmp2.y = tmp.y - v4->y;
					if ( abs( tmp.y - (v4->y+PI2)) < abs(tmp2.y) ) tmp2.y = tmp.y - (v4->y+PI2);
					tmp2.z = tmp.z - v4->z;
					if ( abs( tmp.z - (v4->z+PI2)) < abs(tmp2.z) ) tmp2.z = tmp.z - (v4->z+PI2);

					if ( tmp2.y > v3->y ) tmp2.y = v3->y;
					if ( tmp2.y < -v3->y ) tmp2.y = -v3->y;
					if ( tmp2.x > v3->x ) tmp2.x = v3->x;
					if ( tmp2.x < -v3->x ) tmp2.x = -v3->x;
					if ( tmp2.z > v3->z ) tmp2.z = v3->z;
					if ( tmp2.z < -v3->z ) tmp2.z = -v3->z;

					AddVector( v4, v4, &tmp2 );
				}

				obj->SetRot( tmp.x, tmp.y, tmp.z );
				InitMatrix();
				RotY( tmp.y );
				RotX( -tmp.x );
				ApplyMatrix( &tmp, v1 );

				pdir = obj->GetDir();
				pdir->x = -tmp.x;
				pdir->z = -tmp.z;
				//obj->SetDir( -tmp.x, -tmp.y, -tmp.z );
				obj->evtimer++;
				break;
			case 7:
				//		special missile
				if ( obj->evtimer >= 20 ) break;
				//
				StartObjFind( 0, (int)(v2->y) );
				obj2 = NULL;
				for(i=0;i<(int)(v2->x);i++) {
					j = FindObj();
					if ( j >= 0 ) obj2 = GetObj( j );
				}
				if ( obj2 == NULL ) {
					obj->evtimer = 999;
					break;
				}
				//obj2 = GetObj( (int)(v2->x) );
				if (( obj2->flag & HGOBJ_FLAG_ENTRY ) == 0 ) {
					obj->evtimer = 999;
					break;
				}
				v2 = obj2->GetVectorPrm( ev->target );
				//
				GetTargetAngle( &tmp, v0, v2 );
				obj->SetRot( tmp.x, tmp.y, tmp.z );
				InitMatrix();
				RotY( tmp.y );
				RotX( -tmp.x );
				ApplyMatrix( &tmp, v1 );
				obj->SetDir( -tmp.x, -tmp.y, -tmp.z );
				obj->evtimer++;
				break;
			default:
				break;
			}
			if ( evpass ) {
				obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			}
			break;
			}
#if 0
		case HGEVENT_FLAG_OBJACT:
			v1 = ev->GetEventVector();
			StartAnimation( obj->id, ev->target, ev->value );
			if ( v1->x != 0.0f ) {
				SetObjSpeed( obj->id, v1->x );
			}
			obj->SetEvent( ev->GetNextEvent(), entry );	// イベントをパスする
			break;
#endif
		default:
			AlertMsgf( "Invalid event:%d",ev->flag );
			obj->SetEvent( NULL, entry );
			return;
		}
		ev = ev->GetNextEvent();
	}
	obj->SetEvent( NULL, entry );
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgdx::MoveObj( hgobj *obj, float timepass )
{
	//	オブジェクト動作
	//
	int mode,i;
//	VECTOR dum;
	VECTOR *v0;
	VECTOR *v1;
//	VECTOR *tpos;
//	VECTOR *vnorm;
//	VECTOR v2;
	hgobj *refobj;
//	float dot;
//	float dist;

	//	オブジェクトイベントの処理
	//
	for(i=0;i<HGOBJ_MULTIEVENT_MAX;i++) {
		if ( obj->GetEvent(i) != NULL ) ExecuteObjEvent( obj, timepass, i );
	}

	//	モードごとの動作チェック
	//
	mode = obj->mode;

	{										
		// 通常の場合
		if ( mode & HGOBJ_MODE_MOVE ) {
			v0 = obj->GetVectorPrm( MOC_POS );
			v1 = obj->GetVectorPrm( MOC_DIR );
			AddVector( v0, v0, v1 );
		}
		if ( mode & HGOBJ_MODE_LAND ) {
			refobj = (hgobj *)obj->parent;
			if ( refobj != NULL ) {
				v0 = obj->GetPos();
				v1 = refobj->GetPos();
				v0->x = v1->x;
				v0->y = landaxis;
				v0->z = v1->z;
			}
		}
	}

	if ( mode & HGOBJ_MODE_LOOKAT ) {
		v1 = obj->GetVectorPrm( MOC_DIR );
		obj->LookAt( v1 );
	}
	if ( mode & HGOBJ_MODE_XFRONT ) {
		v0 = obj->GetRot();
		v1 = camera->GetRot();
		v0->x = v1->x;
		v0->y = v1->y;
	}
	if ( mode & HGOBJ_MODE_TREE ) {
		v0 = obj->GetRot();
		v1 = camera->GetRot();
		v0->y = v1->y;
	}
	if ( mode & HGOBJ_MODE_EMITTER ) {
		hgemitter *emi;
		emi = GetEmitter( obj->emitid );
		if ( emi != NULL ) {
			emi->Emit( this, obj->GetVectorPrm( MOC_POS ) );
		}
	}
	if ( mode & HGOBJ_MODE_AUTOROT ) {
		v0 = obj->GetRot();
		v0->x += ((float)(obj->GetPrm()->ua_ofsx)) * 0.002f;
		v0->y += ((float)(obj->GetPrm()->ua_ofsy)) * 0.002f;
	}

	//		自動範囲クリップ
	//
	if ( mode & HGOBJ_MODE_BORDER ) {
		VECTOR *pos;
		VECTOR *dir;
		int cflag,thru;
		pos = obj->GetVectorPrm( MOC_POS );
		dir = obj->GetVectorPrm( MOC_DIR );
		cflag = 0;
		thru = GetSysReq( SYSREQ_THROUGHFLAG );
		if ( thru & 1 ) {
			if ( pos->x < border1.x ) {
				pos->x = border2.x - ( border1.x - pos->x );
			}
			if ( pos->x > border2.x ) {
				pos->x = border1.x + ( pos->x - border2.x );
			}
		}
		if ( thru & 2 ) {
			if ( pos->y < border1.y ) {
				pos->y = border2.y - ( border1.y - pos->y );
			}
			if ( pos->y > border2.y ) {
				pos->y = border1.y + ( pos->y - border2.y );
			}
		}
		if ( thru & 4 ) {
			if ( pos->z < border1.z ) {
				pos->z = border2.z - ( border1.z - pos->z );
			}
			if ( pos->z > border2.z ) {
				pos->z = border1.z + ( pos->z - border2.z );
			}
		}
		if ( mode & HGOBJ_MODE_FLIP ) {
			if ( pos->x < border1.x ) {
				pos->x = border1.x;
				dir->x *= -1.0f;
			}
			if ( pos->x > border2.x ) {
				pos->x = border2.x;
				dir->x *= -1.0f;
			}
			if ( pos->y < border1.y ) {
				pos->y = border1.y;
				dir->y *= -1.0f;
			}
			if ( pos->y > border2.y ) {
				pos->y = border2.y;
				dir->y *= -1.0f;
			}
			if ( pos->z < border1.z ) {
				pos->z = border1.z;
				dir->z *= -1.0f;
			}
			if ( pos->z > border2.z ) {
				pos->z = border2.z;
				dir->z *= -1.0f;
			}
		}
		else {
			if ( pos->x < border1.x ) {
				cflag++;
			}
			if ( pos->x > border2.x ) {
				cflag++;
			}
			if ( pos->y < border1.y ) {
				cflag++;
			}
			if ( pos->y > border2.y ) {
				cflag++;
			}
			if ( pos->z < border1.z ) {
				cflag++;
			}
			if ( pos->z > border2.z ) {
				cflag++;
			}
			if ( cflag ) {
				//if ( mode & OBJ_MODE_WIPEBOM ) {
				//	a = RegistObj( 0, evmodel[EVENT_MODEL_BOM] );
				//	SetObjPos( a, &pos );
				//}
				obj->flag = HGOBJ_FLAG_NONE;
				return;
			}
		}
	}

	//	最後にアニメーションフレームを更新する
	//
	obj->UpdateAnimation( 1.0f );
}


void hgdx::DrawObj( hgobj *obj )
{
	//	モデル表示
	//
	hgmodel *model = obj->model;
	obj->UpdateMatrix();
	obj->drawresult = model->Draw( obj->GetPrm() );
	obj->drawflag = 0;
}


void hgdx::RegistSortObj( hgobj *obj )
{
	mem_sortobj[maxsortobj] = obj;
	maxsortobj++;
	obj->UpdateSortZ();
	obj->sortdrawflag = obj->drawflag;
	obj->drawflag = 0;
}

/*
static int compare_hgobj(hgobj *a, hgobj *b)
{
//	if ( a->GetPos()->z > b->GetPos()->z ) return 1;
	if ( a->sortz > b->sortz ) return 1;
    return -1;
}
*/

void hgdx::SortSwap(hgobj **a, hgobj **b)
{
	hgobj *c = *a;
	*a = *b;
	*b = c;
}


void hgdx::SortObjAll( void )
{
	int i, j;
	if ( maxsortobj <= 0 ) return;
	for (i = 0; i < maxsortobj - 1; i++) {
	  for (j = maxsortobj - 1; j >= i + 1; j--) {
		if ( mem_sortobj[j]->sortz > mem_sortobj[j-1]->sortz ) SortSwap(&mem_sortobj[j], &mem_sortobj[j-1]);
	  }
	}

	//qsort(mem_sortobj, maxsortobj, sizeof(void *), (int (*)(const void*, const void*))compare_hgobj);
}


int hgdx::DrawObjAll( void )
{
	int i;
	int flag;
	int total;
	short df;
	float t;
	hgobj **obj;
	hgobj *pobj;
	hgmodel *model;

	//	表示処理
	//
	total = 0;
	t = 1.0f;
	obj = mem_obj;
	maxsortobj = 0;

	for(i=0;i<maxobj;i++) {
		pobj = *obj;
		if ( pobj != NULL ) {
			flag = pobj->flag;
			if ( flag != HGOBJ_FLAG_NONE ) {
				pobj->matupdate = 0;			// マトリクス計算フラグをリセット
				if (( flag & HGOBJ_FLAG_MOVING )&&( allmove )) MoveObj( pobj, t );

				df = 0;
				if ( flag & HGOBJ_FLAG_VISIBLE) {
					model = pobj->model;
					if ( model != NULL ) {
						if ( pobj->mode & HGOBJ_MODE_HIDE ) df = 0; else {
							df = model->GetFlag();
						}
					}
				}
				total += pobj->drawresult;
				pobj->drawflag = df;
				pobj->drawresult = 0;
			} else {							// オブジェクトは削除される
				DeleteObj( pobj->id );
			}
		}
		obj++;
	}

	//
	//		先に描画するもの(HGOBJ_MODE_FIRSTあり)
	//
	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		pobj = *obj;
		if ( pobj != NULL ) {
			if ( pobj->mode & HGOBJ_MODE_FIRST ) {
				if ( pobj->drawflag ) {
					DrawObj( pobj );	// 通常のオブジェクト
				}
			}
		}
		obj++;
	}

	//
	//		２番目に描画するもの(OBJ_LATEなし)
	//
	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		pobj = *obj;
		if ( pobj != NULL ) {
			if (( pobj->mode & HGOBJ_MODE_LATE ) == 0 ) {
				if ( pobj->drawflag ) {
					if ( pobj->mode & HGOBJ_MODE_SORT ) {
						RegistSortObj( pobj );
					} else {
						DrawObj( pobj );	// 通常のオブジェクト
					}
				}
			}
		}
		obj++;
	}

	//
	//		Zソートオブジェクトの処理
	//
#if 0
	SortObjAll();
	obj = mem_sortobj;							// ソートしたオブジェクトの検索
	for(i=0;i<maxsortobj;i++) {
		pobj = *obj;
		DrawObj( pobj );	// 通常のオブジェクト
		pobj->sortdrawflag = 0;
		obj++;
	}
#endif
	//
	//		後で描画するもの(OBJ_LATEあり)
	//
	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		pobj = *obj;
		if ( pobj != NULL ) {
//			if ( pobj->mode & HGOBJ_MODE_LATE ) {
				if ( pobj->drawflag ) {
					DrawObj( pobj );	// 2Dオブジェクト
				}
//			}
		}
		obj++;
	}

	return total;
}


void hgdx::EnumObj( int group )
{
	enum_group = group;
	enum_id = 0;
}


int hgdx::GetEnumObj( void )
{
	int i;
	hgobj *obj;
	if ( enum_id >= maxobj ) return -1;
	for(i=enum_id;i<maxobj;i++) {
		obj = GetObj( i );
		if ( obj != NULL ) {
			if ( obj->mygroup & enum_group ) { enum_id = i+1; return i; }
		}
	}
	return -1;
}


void hgdx::CutObjChild( int parentid )
{
	hgobj *obj;
	hgobj *obj2;
	hgobj *p_obj;
	obj = GetObj( parentid );
	if ( obj == NULL ) return;
	if ( obj->child == NULL ) return;
	p_obj = obj;
	obj = (hgobj *)p_obj->child;
	p_obj->child = NULL;
	while(1) {
		obj->parent = NULL;
		obj2 = (hgobj *)obj->sibling;
		if ( obj2 == NULL ) break;
		obj->sibling = NULL;
		obj = obj2;
	}
}


void hgdx::SetObjChild( int parentid, int objid )
{
	hgobj *obj;
	hgobj *obj2;
	hgobj *p_obj;
	obj = GetObj( parentid );
	obj2 = GetObj( objid );
	if (( obj == NULL )||( obj2 == NULL )) return;
	if ( obj2->parent != NULL ) return;
	p_obj = obj;
	if ( obj->child == NULL ) {
		obj->child = obj2;
		obj2->parent = obj;
		return;
	}
	obj = (hgobj *)obj->child;
	while(1) {
		if ( obj->sibling == NULL ) {
			obj->sibling = obj2;
			obj2->parent = p_obj;
			break;
		}
		obj = (hgobj *)obj->sibling;
	}
}


void hgdx::UpdateLight( void )
{
	// ライト動作
	//
	//VECTOR lightvec,lightcolor,ambientcolor;
	//CopyVector( &lightvec, light->GetRot() );
	//CopyVector( &lightcolor, light->GetDir() );
	//ScaleVector( &lightcolor, light->GetDir(), 1.0f / 256.0f );			// ライト色を設定する
	//ScaleVector( &ambientcolor, light->GetEfx(), 1.0f / 256.0f );		// アンビエント色を設定する
	//io.SetLight( &lightvec );
}


void hgdx::SetCamMode( int mode, int id )
{
	cammode = mode;
	camera->SetLookObj( id );
}


void hgdx::UpdateCamera( void )
{
}


void hgdx::SetBorder( float x0, float x1, float y0, float y1, float z0, float z1 )
{
	SetVector( &border1, x0, y0, z0, 0.0f );
	SetVector( &border2, x1, y1, z1, 0.0f );
}


void hgdx::GetBorder( VECTOR *v1, VECTOR *v2 )
{
	CopyVector( v1, &border1 );
	CopyVector( v2, &border2 );
}


int hgdx::UpdateObjColi( int id, float size, int addcol )
{
	int i;
	int flg_2d;
	int chkgroup;
	VECTOR *pos;
	hgobj *ene;
	hgobj **obj;
	hgobj *o;

	//	表示処理
	//
	o = mem_obj[id];
	if ( o == NULL ) return -1;

	o->colinfo = -1;
	if ( addcol == 0 ) {
		chkgroup = o->colgroup;
	} else {
		chkgroup = addcol;
	}
	pos = o->GetPos();

	if ( o->flag == HGOBJ_FLAG_NONE ) return -1;
	if ( chkgroup == 0 ) return -1;

	hgmodel *model = o->model;
	if ( model == NULL ) return -1;

	flg_2d = 0;
	if ( model->GetFlag() == HGMODEL_FLAG_2DSPRITE ) flg_2d = 1;

	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		ene = *obj;
		if ( ene != NULL ) {
			if ( ene->flag & HGOBJ_FLAG_VISIBLE ) {

				if (( ene->mygroup & chkgroup )&&( i != id )) {
					if ( flg_2d ) {
						if ( ene->GetDistance2D( pos, size ) ) {
							o->colinfo = i;
							return i;
						}
					} else {
						if ( ene->GetDistance( pos, size ) ) {
							o->colinfo = i;
							return i;
						}
					}
				}

			}
		}
		obj++;
	}

	return -1;
}


void hgdx::StartObjFind( int mode, int group )
{
	//		オブジェクト検索設定
	//			mode:除外するモード
	//			group:対象となるcollision group
	//
	objfind_exmode = mode;
	objfind_group = group;
	objfind_id = 0;
}


int hgdx::FindObj( void )
{
	//		オブジェクト検索
	//			(ret:-1=検索終了)
	//
	int i;
	hgobj *o;
	while(1) {
		if ( objfind_id >= maxobj ) return -1;
		o = mem_obj[objfind_id];
		if ( o != NULL ) {
			if ( o->flag != HGOBJ_FLAG_NONE ) {
				if ( o->mygroup & objfind_group ) {
					if (( o->mode & objfind_exmode )==0 ) {
						break;
					}
				}
			}
		}
		objfind_id++;
	}
	i = objfind_id;
	objfind_id++;
	return i;
}

hgobj *hgdx::AddLightObj( void )
{
	hgobj *lg;
	lg = AddObj();
	lg->SetRot( 0.0f, 1.0f, 0.0f );
	lg->SetScale( 255.0f, 255.0f, 255.0f );
	lg->SetDir( 0.0f, 0.0f, 0.0f );
	return lg;
}


void hgdx::SetObjLight( int id, int refid, int mode )
{
	hgobj *o;
	hgobj *ref;
	o = GetObj( id );
	if ( o == NULL ) return;
	ref = GetObj( refid );
	if ( ref == NULL ) return;
//	if ( mode & 1 ) o->SetLight( ref->GetRot() );
//	if ( mode & 2 ) o->SetLightColor( ref->GetScale() );
//	if ( mode & 4 ) o->SetLightAmbient( ref->GetDir() );
}

float hgdx::ObjAim( int id, int target, int mode, float x, float y, float z )
{
	hgobj *obj;
	VECTOR *v0;
	VECTOR *v1;
	VECTOR *v2;
	VECTOR *v3;
	VECTOR vec;
	VECTOR tmp;
	int val;
	float rval;

	rval = 0.0f;
	obj = GetObj( id );
	if ( obj == NULL ) return rval;

	v0 = obj->GetVectorPrm( target );
	v1 = &vec;
	SetVector( &vec, x, y, z, 1.0 );

	val = mode;
	if ( val & 0xf0000 ) {
		v2 = GetObj( val & 0xffff )->GetVectorPrm( target );
		val = (val>>16) - 1;
	} else {
		v2 = obj->GetWork();
	}
	switch( val ) {
	case 0:
		SubVector( &tmp, v2, v0 );
		if ( tmp.x > v1->x ) tmp.x = v1->x;
		if ( tmp.y > v1->y ) tmp.y = v1->y;
		if ( tmp.z > v1->z ) tmp.z = v1->z;
		if ( tmp.x < -v1->x ) tmp.x = -v1->x;
		if ( tmp.y < -v1->y ) tmp.y = -v1->y;
		if ( tmp.z < -v1->z ) tmp.z = -v1->z;
		if ( tmp.y != 0.0f ) {
		//	AlertMsgf( "%f,%f,%f---%f,%f,%f",tmp.x,tmp.y,tmp.z,v0->x, v0->y, v0->z );
		}
		AddVector( v0, v0, &tmp );
		rval = GetVectorDistance( v0, v2 );
		break;
	case 1:
		SubVector( &tmp, v2, v0 );
		if ( tmp.x > v1->x ) tmp.x = v1->x;
		if ( tmp.y > v1->y ) tmp.y = v1->y;
		if ( tmp.z > v1->z ) tmp.z = v1->z;
		if ( tmp.x < -v1->x ) tmp.x = -v1->x;
		if ( tmp.y < -v1->y ) tmp.y = -v1->y;
		if ( tmp.z < -v1->z ) tmp.z = -v1->z;
		obj->SetDir( tmp.x, tmp.y, tmp.z );
		rval = GetVectorDistance( v0, v2 );
		break;
	case 2:
		GetTargetAngle( &tmp, v0, v2 );
		obj->SetRot( 0.0f, -tmp.y + vec.y, 0.0f );
		rval = GetVectorDistance( v0, v2 );
		break;
	case 3:
	case 4:
		GetTargetAngle( &tmp, v0, v2 );
		obj->SetRot( tmp.x, tmp.y, tmp.z );
		InitMatrix();
		RotY( tmp.y );
		RotX( -tmp.x );
		ApplyMatrix( &tmp, v1 );
		obj->SetDir( -tmp.x, -tmp.y, -tmp.z );
		rval = GetVectorDistance( v0, v2 );
		break;
	case 5:
	case 6:
		GetTargetAngle( &tmp, v0, v2 );
		//if ( obj->evtimer > 0 ) {
			SubVector( &tmp, &tmp, obj->GetRot() );
			v3 = obj->GetWork();
			if ( tmp.x > v3->x ) tmp.x = v3->x;
			if ( tmp.y > v3->y ) tmp.y = v3->y;
			if ( tmp.z > v3->z ) tmp.z = v3->z;
			if ( tmp.x < -v3->x ) tmp.x = -v3->x;
			if ( tmp.y < -v3->y ) tmp.y = -v3->y;
			if ( tmp.z < -v3->z ) tmp.z = -v3->z;
			AddVector( &tmp, &tmp, obj->GetRot() );
		//}
		obj->SetRot( tmp.x, tmp.y, tmp.z );
		InitMatrix();
		RotY( tmp.y );
		RotX( -tmp.x );
		ApplyMatrix( &tmp, v1 );
		obj->SetDir( -tmp.x, -tmp.y, -tmp.z );
		rval = GetVectorDistance( v0, v2 );
		break;
	default:
		break;
	}
	return rval;
}


int hgdx::GetNearestObj( int id, float range, int colgroup )
{
	int i;
	int flg_2d;
	int res;
	VECTOR *pos;
	hgobj *ene;
	hgobj **obj;
	hgobj *o;
	hgmodel *model;
	float maxrange;
	float dist;

	//	表示処理
	//
	o = mem_obj[id];
	if ( o == NULL ) return -1;

	pos = o->GetPos();

	if ( o->flag == HGOBJ_FLAG_NONE ) return -1;
	if ( colgroup == 0 ) return -1;

	model = o->model;
	if ( model == NULL ) return -1;

	res = -1;
	maxrange = range;

	flg_2d = 0;
	if ( model->GetFlag() == HGMODEL_FLAG_2DSPRITE ) flg_2d = 1;

	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		ene = *obj;
		if (( ene != NULL )&&( i != id )) {
			if ( ene->flag & HGOBJ_FLAG_VISIBLE ) {

				if ( ene->mygroup & colgroup ) {
					if ( flg_2d ) {
						dist = ene->GetDistance2D( pos );
					} else {
						dist = ene->GetDistance( pos );
					}
					if ( dist < maxrange ) {
						res = i; maxrange = dist;
					}
				}

			}
		}
		obj++;
	}
	return res;
}

/*------------------------------------------------------------*/
/*
		for Emitter
*/
/*------------------------------------------------------------*/

hgemitter *hgdx::GetEmitter( int emitid )
{
	hgemitter *emi;
	if (( emitid < 0 )||( emitid >= maxemitter )) return NULL;
	emi = mem_emitter[emitid];
	return emi;
}


int hgdx::GetEmptyEmitterId( void )
{
	int sel,i;
	sel = -1;
	for(i=0;i<maxemitter;i++) {
		if ( mem_emitter[i] == NULL ) { sel = i; break; }
	}
	return sel;
}


int hgdx::AddEmitter( int id, int mode, int num )
{
	int sel;
	hgemitter *emi;
	sel = id;
	if ( sel < 0 ) {
		sel = GetEmptyEmitterId();
		if ( sel < 0 ) return -1;
	}
	DeleteEmitter( sel );
	emi = new hgemitter;
	emi->mode = mode;
	emi->out_num = num;
	mem_emitter[ sel ] = emi;
	return sel;
}

void hgdx::DeleteEmitter( int id )
{
	hgemitter *emi;
	emi = mem_emitter[ id ];
	if ( emi == NULL ) return;
	delete emi;
	mem_emitter[ id ] = NULL;
}


void hgdx::SetObjEmitter( int id, int emitid )
{
	hgobj *o;
	o = GetObj( id );
	if ( o == NULL ) return;
	o->SetEmitter( emitid );
}


void hgdx::GroupModify( int group, int param, int calcopt, VECTOR *data )
{
	int i;
	hgobj **obj;
	hgobj *ene;
	VECTOR *vec;

	obj = mem_obj;
	for(i=0;i<maxobj;i++) {
		ene = *obj;
		if ( ene != NULL ) {
			if ( ene->flag & HGOBJ_FLAG_VISIBLE ) {
				if ( ene->mygroup & group ) {

					vec = ene->GetVectorPrm( param );
					switch( calcopt ) {
					case 0:
						CopyVector( vec, data );
						break;
					case 1:
						AddVector( vec, vec, data );
						break;
					case 2:
						SubVector( vec, vec, data );
						break;
					case 3:
						MulVector( vec, vec, data );
						break;
					}

				}
			}
		}
		obj++;
	}
}


void hgdx::SetModelColScale( int id, float scale1, float scale2 , float scale3 )
{
	hgmodel *mdl;
	mdl = GetModel( id );
	if ( mdl != NULL ) {
		mdl->SetDefaultColScale( scale1, scale2, scale3 );
	}
}


void hgdx::SetModelColParam( int id, float scale1, float scale2 , float scale3 )
{
	hgmodel *mdl;
	mdl = GetModel( id );
	if ( mdl != NULL ) {
		mdl->SetDefaultColParam( scale1, scale2, scale3 );
	}
}



