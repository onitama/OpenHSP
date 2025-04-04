
//
//		High performance Graphic Image access (object)
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

/*------------------------------------------------------------*/
/*
		scene object process
*/
/*------------------------------------------------------------*/

int hgdraw::RegistObj( int mode, MODEL *model )
{
	//		オブジェクト情報を新規作成
	//
	SCNOBJ *o;
	VECTOR v;
	int sel,i;

	sel = -1;
	for(i=0;i<scnobj_buf_max;i++) {
		if ( mem_obj[i].flag == 0 ) { sel = i; break; }
	}
	if ( sel < 0 ) return -1;

	o = &mem_obj[sel];
	memset( o, 0, sizeof(SCNOBJ) );
	o->flag = SCN_FLAG_ON;
	o->mode = mode;
	o->model = model;
	o->mocnum = SCN_MOC_MAX;
	o->mocid = MocRegist( o->mocnum );		// MOCを確保
	o->mid = -1;

	//	初期値
	SetVector( &v, 0.0f, 0.0f, 0.0f, 1.0f );
	SetObjPos( sel, &v );
	SetObjAng( sel, &v );
	SetObjDir( sel, &v );
	SetVector( &v, 1.0f, 1.0f, 1.0f, 1.0f );
	SetObjScale( sel, &v );
	SetVector( &v, 256.0f, 0.0f, 0.0f, 0.0f );
	SetObjEfx( sel, &v );

	return sel;
}


int hgdraw::GetObjModel( int id )
{
	SCNOBJ *o;
	o = &mem_obj[id];
	return o->mid;
}

	
void hgdraw::SetObjModel( int id, int modelid )
{
	MDLANM *ma;
	SCNOBJ *o;
	MODEL *model;
	int defmode;
	o = &mem_obj[id];
	o->mid = modelid;
	model = GetModel( modelid );
	o->model = model;
	ma = (MDLANM *)GetModelAnimFrame( modelid );
	defmode = ma->mode;
	o->mode |= defmode;
	o->timer = ma->timer;
	o->mygroup = ma->mygroup;
	o->colgroup = ma->colgroup;
	
	if ( (o->mode) & (OBJ_MODE_GROUND | OBJ_MODE_GROUND) ) {
		o->otbase = 0+OT_RANGE;
	}
	if ( defmode & OBJ_MODE_UVANIM ) {
		SetObjUVAnim( id, ma->ua_mode, ma->ua_wait, ma->ua_times, ma->uva_py );
	}
}


int hgdraw::RegistObj( int mode, int modelid )
{
	//		オブジェクト情報を新規作成(id)
	//
	int sel;
	MODEL *model;
	model = GetModel( modelid );
	sel = RegistObj( mode, model );
	SetObjModel( sel, modelid );
	return sel;
}


void hgdraw::DeleteObj( SCNOBJ *o )
{
	//		オブジェクト情報を破棄
	//
	int a;
	if ( o->flag == 0 ) return;
	if ( o->mocnum ) {
		for( a=0;a<(o->mocnum);a++) MocDelete( (o->mocid)+a );
	}
	o->flag = 0;
}

	
void hgdraw::DeleteObj( int id )
{
	//		オブジェクト情報を破棄
	//
	int a;
	SCNOBJ *o;
	o = &mem_obj[id];
	if ( o->flag == 0 ) return;
	if ( o->mocnum ) {
		for( a=0;a<(o->mocnum);a++) MocDelete( (o->mocid)+a );
	}
	o->flag = 0;
}

	
int hgdraw::GetObjMoc( int id )
{
	//		オブジェクトのMOCIDを取得
	//
	SCNOBJ *o;
	o = &mem_obj[id];
	return o->mocid;
}


void hgdraw::GetObjPos( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocGetKeyFV( o->mocid + SCN_MOC_POS, v );
}
	
	
void hgdraw::GetObjAng( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocGetKeyFV( o->mocid + SCN_MOC_ANG, v );
}
	
	
void hgdraw::GetObjScale( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocGetKeyFV( o->mocid + SCN_MOC_SCALE, v );
}


void hgdraw::GetObjDir( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocGetKeyFV( o->mocid + SCN_MOC_DIR, v );
}


void hgdraw::GetObjEfx( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocGetKeyFV( o->mocid + SCN_MOC_EFX, v );
}


void hgdraw::SetObjPos( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocSetKeyFV( o->mocid + SCN_MOC_POS, v );
}
	
	
void hgdraw::SetObjAng( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocSetKeyFV( o->mocid + SCN_MOC_ANG, v );
}
	
	
void hgdraw::SetObjScale( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocSetKeyFV( o->mocid + SCN_MOC_SCALE, v );
}


void hgdraw::SetObjDir( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocSetKeyFV( o->mocid + SCN_MOC_DIR, v );
}


void hgdraw::SetObjEfx( int id, VECTOR *v )
{
	SCNOBJ *o = &mem_obj[id];
	MocSetKeyFV( o->mocid + SCN_MOC_EFX, v );
}


void hgdraw::SetObjAnim( int id, int anim )
{
	SCNOBJ *o = &mem_obj[id];
	if ( anim < 0 ) {
		o->aniflag = 0;
		return;
	}
	o->aniflag = 1;
	o->anim = anim;
	o->aniframe = 0;
}


void hgdraw::SetObjMode( int id, int mode, int sw )
{
	SCNOBJ *o = &mem_obj[id];
	switch(sw) {
	case 0:
		o->mode |= mode;
		break;
	case 1:
		o->mode &= ~mode;
		break;
	default:
		o->mode = mode;
		break;
	}
}


void hgdraw::SetObjColi( int id, int mygroup, int enegroup )
{
	SCNOBJ *o = &mem_obj[id];
	o->mygroup = mygroup;
	o->colgroup = enegroup;
}


void hgdraw::GetObjColi( int id, int *mygroup, int *enegroup )
{
	SCNOBJ *o = &mem_obj[id];
	*mygroup = o->mygroup;
	*enegroup = o->colgroup;
}


int hgdraw::GetObjDistance( SCNOBJ *o, VECTOR *v, float size )
{
	VECTOR pos;
	MocGetKeyFV( o->mocid + SCN_MOC_POS, &pos );
	if ( fabs( pos.x - v->x ) < size ) {
		if ( fabs( pos.z - v->z ) < size ) {
			if ( fabs( pos.y - v->y ) < size ) {
				return 1;
			}
		}
	}
	return 0;
}


int hgdraw::UpdateObjColi( int id, float size )
{
	int i;
	int chkgroup;
	VECTOR pos;
	SCNOBJ *ene;
	SCNOBJ *o = &mem_obj[id];
	o->colinfo = -1;
	chkgroup = o->colgroup;
	if ( o->flag == 0 ) return -1;
	if ( chkgroup == 0 ) return -1;

	MocGetKeyFV( o->mocid + SCN_MOC_POS, &pos );

	for(i=0;i<scnobj_buf_max;i++) {
		ene = &mem_obj[i];
		if ( ene->flag ) {
			if ( ene->mygroup & chkgroup ) {
				if ( GetObjDistance( ene, &pos, size ) ) {
					o->colinfo = i;
					return i;
				}
			}
		}
	}
	return -1;
}


void hgdraw::StartObjFind( int mode, int group )
{
	//		オブジェクト検索設定
	//			mode:除外するモード
	//			group:対象となるcollision group
	//
	objfind_exmode = mode;
	objfind_group = group;
	objfind_id = 0;
}


int hgdraw::FindObj( void )
{
	//		オブジェクト検索
	//			(ret:-1=検索終了)
	//
	int i;
	SCNOBJ *o;
	while(1) {
		if ( objfind_id >= scnobj_buf_max ) return -1;
		o = &mem_obj[objfind_id];
		if ( o->flag ) {
			if ( o->mygroup & objfind_group ) {
				if (( o->mode & objfind_exmode )==0 ) {
					break;
				}
			}
		}
		objfind_id++;
	}

	i = objfind_id;
	objfind_id++;
	return i;
}


int hgdraw::PutEventObj( int event, VECTOR *pos )
{
	int a;
	a = RegistObj( 0, evmodel[event] );
	SetObjPos( a, pos );
	return a;
}


void hgdraw::DrawObj( int id, int sw )
{
	//		オブジェクト描画
	//			sw:表示スイッチ(0=OFF/1=ON)
	//
	VECTOR pos;
	VECTOR ang;
	VECTOR scale;
	VECTOR dir;
	VECTOR efx;
	int mocid;
	int mode;
	int cflag;
	int a;
	float ytmp;
	SCNOBJ *o;
	o = &mem_obj[id];

	uoffset = 0;						// UVAnimation Offset reset
	mode = o->mode;
	if ( o->flag==0 ) return;
	if ( mode & OBJ_MODE_HIDE ) return;
	if ( mode & OBJ_MODE_UVANIM ) UpdateUVAnim( o );

	mocid = o->mocid;
	MocMove( mocid, o->mocnum );		// 移動オブジェクト制御

	MocGetKeyFV( mocid + SCN_MOC_POS, &pos );
	MocGetKeyFV( mocid + SCN_MOC_ANG, &ang );
	MocGetKeyFV( mocid + SCN_MOC_SCALE, &scale );
	MocGetKeyFV( mocid + SCN_MOC_EFX, &efx );

	//		オート消去タイマー
	//
	if ( mode & OBJ_MODE_TIMER ) {
		if ( o->timer == 0 ) {
			if ( mode & OBJ_MODE_WIPEBOM ) {
				PutEventObj( EVENT_MODEL_BOM, &pos );
			}
			DeleteObj( id );
			return;
		}
		o->timer--;
	}

	//		自動相対移動
	//
	if ( mode & OBJ_MODE_MOVE ) {
		MocGetKeyFV( mocid + SCN_MOC_DIR, &dir );
		AddVector( &pos, &pos, &dir );
		if ( mode & OBJ_MODE_GRAVITY ) {	// 重力付き
			if ( pos.y > 0.0f ) {
				pos.y = 0.0f;
				dir.y = -(dir.y * 0.8f );
			}
			else {
				dir.y += 0.2f;
			}
			MocSetKeyFV( mocid + SCN_MOC_DIR, &dir );
		}
		MocSetKeyFV( mocid + SCN_MOC_POS, &pos );
	}

	//		自動範囲クリップ
	//
	if ( mode & OBJ_MODE_BORDER ) {
		MocGetKeyFV( mocid + SCN_MOC_DIR, &dir );
		cflag = 0;
		if ( pos.x < border1.x ) {
			pos.x = border1.x;
			dir.x *= -1.0f;
			cflag++;
		}
		if ( pos.x > border2.x ) {
			pos.x = border2.x;
			dir.x *= -1.0f;
			cflag++;
		}
		if ( pos.y < border1.y ) {
			pos.y = border1.y;
			dir.y *= -1.0f;
			cflag++;
		}
		if ( pos.y > border2.y ) {
			pos.y = border2.y;
			dir.y *= -1.0f;
			cflag++;
		}
		if ( pos.z < border1.z ) {
			pos.z = border1.z;
			dir.z *= -1.0f;
			cflag++;
		}
		if ( pos.z > border2.z ) {
			pos.z = border2.z;
			dir.z *= -1.0f;
			cflag++;
		}
		if ( cflag ) {
			MocSetKeyFV( mocid + SCN_MOC_POS, &pos );
			MocSetKeyFV( mocid + SCN_MOC_DIR, &dir );
			if (( mode & OBJ_MODE_FLIP )== 0 ) {
				if ( mode & OBJ_MODE_WIPEBOM ) {
					a = RegistObj( 0, evmodel[EVENT_MODEL_BOM] );
					SetObjPos( a, &pos );
				}
				DeleteObj( id );
				return;
			}
		}
	}

	//		アニメーションのフレーム更新
	//
	if ( o->aniflag ) {
		o->aniframe = UpdateModelAnim( o->mid, o->anim, o->aniframe );
	}

	//		モデルを描画
	//
	if ( sw ) {
		otofs = o->otbase;
		if ( mode & OBJ_MODE_SKY ) {		// 空属性
			SetZCalc( 0, OT_RANGE + clipzf );
			ytmp = pos.y;
			MocGetKeyFV( cammoc + SCN_MOC_POS, &pos );
			pos.x *= 0.75f; pos.y = ytmp; pos.z *= 0.75f;
		}
		if ( mode & OBJ_MODE_2D ) {
			DrawModel2D( o->model, &pos, &ang, &scale, &efx );	// Draw 2D Model
			return;
		}
		if ( mode & OBJ_MODE_TREE ) {		// 正面属性(木)
			ang.y = -MocGetKey( cammoc + SCN_MOC_ANG, 1 );
		}
		if ( mode & OBJ_MODE_XFRONT ) {		// 正面属性
			ang.x = -MocGetKey( cammoc + SCN_MOC_ANG, 0 );
		}
		if ( o->aniflag ) {
			DrawModelAnim( o->model, &pos, &ang, &scale, &efx );
		}
		else {
			DrawModel( o->model, &pos, &ang, &scale, &efx );
		}
		if ( mode & OBJ_MODE_SKY ) {		// 空属性(後始末)
			SetZCalc( OT_RANGE, 0 );
		}
	}

}


void hgdraw::UpdateViewMatrix( void )
{
	VECTOR camprm;
	MocGetKeyFV( cammoc + SCN_MOC_DIR, &camprm );
	clipznear = camprm.z;
	xyproj = proj * camprm.y;

	InitMatrix();
	//PerspectiveWithZBuffer( -1.0f/3.0f, 0.0f, proj, 1.0f, 0.0f );
	PerspectiveWithZBuffer( (-1.0f/3.0f)+camprm.x, 0.0f, proj * camprm.y, 1.0f, 0.0f );
	MulMatrix( &m_camera );
	GetCurrentMatrix( &m_view );
}
	

void hgdraw::DrawObjAll( void )
{
	int i;
	for(i=0;i<scnobj_buf_max;i++) {
		if ( mem_obj[i].flag ) {
			DrawObj( i, 1 );
		}
	}
}


void hgdraw::UpdateObjAll( void )
{
	int i;
	for(i=0;i<scnobj_buf_max;i++) {
		if ( mem_obj[i].flag ) {
			DrawObj( i, 0 );
		}
	}
}


int hgdraw::UpdateUVAnim( SCNOBJ *o )
{
	uoffset = o->ua_ofs;					// UV offset setup
	if ( o->ua_time ) {
		o->ua_time--;return 0;
	}
	o->ua_time = o->ua_deftime;
	o->ua_cnt ++;
	if ( o->ua_cnt >= o->ua_max ) {
		if ( o->mode & OBJ_MODE_UVANIM_1SHOT ) {
			DeleteObj( o );					// 1shot object wipe
			//o->flag = 0;					// 1shot object wipe
			return 1;
		}
		o->ua_cnt = 0;
		o->ua_ofs = 0;
	}
	else {
		o->ua_ofs += o->ua_sy;
	}
	return 0;
}


void hgdraw::SetObjUVAnim( int id, int mode, int wait, int times, int py )
{
	//		mode : 0=repeat animation / 1=one shot animation
	//
	SCNOBJ *o;
	MODEL *model;
	PNODE *node;
	TEXINF *texinf;
	o = &mem_obj[id];
	o->mode |= OBJ_MODE_UVANIM;
	if ( mode ) o->mode |= OBJ_MODE_UVANIM_1SHOT;
	model = o->model;
	if ( model->node == NULL ) return;
	node = model->node[0];
	texinf = io.GetTex( node->tex );
	o->ua_ofs = 0;
	o->ua_cnt = 0;
	o->ua_max = times;
	o->ua_sy = py;
	o->ua_time = wait;
	o->ua_deftime = wait;
}


void hgdraw::SetEventModel( int id, int model )
{
	evmodel[id] = model;
}


//
//		拡張オブジェクト検索ルーチン
//

int hgdraw::ProcessNode( PNODE *n )
{
	VECTOR pos;
	VECTOR *v;
	VECTOR poly[4];
	float x,z;
	float zot;
	int a;

	zot = 0.0f;

	if (( n->code >= PRIM_POLY4F )&&( n->code < PRIM_ATTRSET )) {
		for(a=0;a<n->num;a++) {
			v = &n->fv[a];
			ApplyMatrix( &pos, v );
			if ( pos.z < 1.0f ) return 0;
			zot += pos.w;
			z = 1.0f / pos.z;
			pos.x = pos.x * z * xyproj + center.x;
			pos.y = pos.y * z * xyproj + center.y;
			x = pos.x;
			if ( x <= -32000.0 ) x = -32000.0f;
			if ( x >= 32000.0 ) x = 32000.0f;
			poly[a].x = x;
			poly[a].y = pos.y;
			poly[a].z = 0.0f;
		}
		a = ((int)(zot * otzv))>>2;			// 頂点の平均を取る
		if ( a <= clipzf ) {
			seobj.zot = a;
			if ( a < seres.zot ) {
				if ( HasPoint2D( seobj.point.x, seobj.point.y, &poly[0] )) {
					seres = seobj;
					return -1;
				}
			}
		}
	}
	return 0;
}
	

void hgdraw::ProcessModelSub( MODEL *m )
{
	int a;
	PushMatrix();
	Trans( m->pos.x, m->pos.y, m->pos.z );
	RotZ( m->ang.z );
	RotY( m->ang.y );
	RotX( m->ang.x );
	Scale( m->scale.x ,m->scale.y, m->scale.z );
	if ( m->num ) {
		seobj.subm = m->id;
		for(a=0;a<m->num;a++) {
			seobj.node = a;
			ProcessNode( m->node[a] );
		}
	}
	if ( m->child != NULL ) this->ProcessModelSub( (MODEL *)m->child );
	PopMatrix();
	if ( m->sibling != NULL ) this->ProcessModelSub( (MODEL *)m->sibling );
}


void hgdraw::ProcessObj( int id )
{
	//		オブジェクト検索プロセス
	//
	VECTOR pos;
	VECTOR ang;
	VECTOR scale;
	int mocid;
	int mode;
	SCNOBJ *o;
	o = &mem_obj[id];
	mode = o->mode;
	if ( o->flag==0 ) return;
	if ( mode & (OBJ_MODE_HIDE|OBJ_MODE_SKY|OBJ_MODE_2D) ) return;

	mocid = o->mocid;
	MocGetKeyFV( mocid + SCN_MOC_POS, &pos );
	MocGetKeyFV( mocid + SCN_MOC_ANG, &ang );
	MocGetKeyFV( mocid + SCN_MOC_SCALE, &scale );

	//		モデルを描画
	//
	otofs = o->otbase;
	if ( mode & OBJ_MODE_TREE ) {		// 正面属性(木)
		ang.y = -MocGetKey( cammoc + SCN_MOC_ANG, 1 );
	}
	if ( mode & OBJ_MODE_XFRONT ) {		// 正面属性
		ang.x = -MocGetKey( cammoc + SCN_MOC_ANG, 0 );
	}
	seobj.id = id;
	seobj.model = o->mid;

	SetCurrentMatrix( &m_view );
	Trans( pos.x, pos.y, pos.z );
	RotX( ang.x );
	RotY( ang.y );
	RotZ( ang.z );
	Scale( scale.x ,scale.y, scale.z );
	ProcessModelSub( o->model );
}


void *hgdraw::ProcessObjAll( int mode, float x, float y, float z )
{
	int i;
	seres.id = -1;
	seres.zot = clipzf;
	seobj.point.x = x;
	seobj.point.y = y;
	seobj.point.z = z;
	seobj.mode = mode;
	for(i=0;i<scnobj_buf_max;i++) {
		if ( mem_obj[i].flag ) {
			ProcessObj( i );
		}
	}
	return (void *)&seres;
}


