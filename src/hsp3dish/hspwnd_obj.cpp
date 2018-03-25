
//
//	HSP3 window object manager
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/dpmread.h"
#include "../hsp3/strbuf.h"
#include "../hsp3/strnote.h"

#include "hgio.h"
#include "supio.h"
#include "hspwnd.h"

/*------------------------------------------------------------*/
/*
		Object callback interface
*/
/*------------------------------------------------------------*/

static int *notice_ptr;

void SetObjectEventNoticePtr( int *ptr )
{
	//		イベント時の値ポインタを設定
	//
	notice_ptr = ptr;
}

static void Object_JumpEvent( HSPOBJINFO *info, int wparam )
{
	if ( info->enableflag == 0 ) return;
	*notice_ptr = info->owsize;

	if ( info->btnset->jumpmode & 1 ) {
		code_call( (unsigned short *)info->btnset->ptr );
	} else {
		code_setpci( (unsigned short *)info->btnset->ptr );
	}

}

static void Object_ButtonDraw( HSPOBJINFO *info )
{
	Bmscr *bm;
	HSP3BTNSET *btn;
	int col1,col2,tcol;
	int x1,x2,y1,y2;

	bm = (Bmscr *)info->bm;
	bm->SetFontInternal( "", 18, 0 );

	x1 = info->x; y1 = info->y;
	x2 = info->x + info->sx - 1; y2 = info->y + info->sy - 1;

	btn = info->btnset;
	if ( btn->messx < 0 ) {
		bm->gmode = 3;
		bm->gfrate = 0;
		bm->cx = x1;
		bm->cy = y1;
		bm->Setcolor( 0 );
		bm->Print( info->btnset->name );
		btn->messx = bm->printsizex;
		btn->messy = bm->printsizey;
	}

	tcol = 0xff000000;
	//tcol = 0xffffffff;
	if ( info->srcid < 0 ) {
		//	グラデーションによる標準ボタン
		if ( info->enableflag ) {
			if ( info->tapflag == 1 ) {
				col1 = 0xff909090; col2 = 0xffc0c0c0;
				//col1 = 0xff202020; col2 = 0xff404040;
			} else {
				col1 = 0xfff0f0f0; col2 = 0xffc0c0c0;
				//col1 = 0xff404040; col2 = 0xff606060;
			}
		} else {
			col1 = 0xff808080; col2 = 0xff606060; tcol = 0xff404040;
		}
		bm->gmode = 0;
		bm->gfrate = 255;
		bm->GradFill( x1+1, y1+1, info->sx-2, info->sy-2, 1, col1, col2 );
		bm->Setcolor( 0xff808080 );
		bm->cx = x1; bm->cy = y1;
		bm->Line( x2, y1 );
		bm->Line( x2, y2 );
		bm->Line( x1, y2 );
		bm->Line( x1, y1 );
	} else {
		//	画像によるボタン
		col1 = btn->normal_x; col2 = btn->normal_y;
		if ( info->enableflag ) {
			if ( info->tapflag == 1 ) {
				col1 = btn->push_x; col2 = btn->push_y;
			}
		} else {
			tcol = 0xff808080;
		}
		bm->gmode = 3;
		bm->gfrate = 255;
		bm->Setcolor( tcol );
		bm->cx = x1;
		bm->cy = y1;
		HspWnd *wnd = (HspWnd *)bm->master_hspwnd;
		bm->Copy( wnd->GetBmscrSafe( info->srcid ), col1, col2, info->sx, info->sy );
	}

	bm->gmode = 3;
	bm->gfrate = 255;
	bm->Setcolor( tcol );
	bm->cx = info->x + ( info->sx - btn->messx )/2;
	bm->cy = info->y + ( info->sy - btn->messy )/2;
	if ( info->tapflag == 1 ) {
		bm->cx++;
		bm->cy++;
	}
	bm->Print( info->btnset->name );
}

/*---------------------------------------------------------------------------*/

void Bmscr::ResetHSPObject( void )
{
	//		すべてのObjectをリセットする
	//
	int i;
	if ( mem_obj != NULL ) {
		for( i=0;i<objmax;i++ ) {
			DeleteHSPObject( i );
		}
		sbFree( mem_obj );
	}
	mem_obj = NULL;
	objmax = 0;
	objlimit = HSPOBJ_LIMIT_DEFAULT;
}


void Bmscr::EnableObject( int id, int sw )
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) throw HSPERR_ILLEGAL_FUNCTION;
	obj->enableflag = sw;
//	EnableWindow( obj->hCld, sw!=0 );
}


void Bmscr::SetObjectMode( int id, int owmode )
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) throw HSPERR_ILLEGAL_FUNCTION;
	if ( owmode <= 0 ) throw HSPERR_ILLEGAL_FUNCTION;
	obj->owmode = owmode;
}


int Bmscr::NewHSPObject( void )
{
	//		空きIDを探す
	//
	int i,id;
	id = 0;
	if ( mem_obj == NULL ) {
		mem_obj = (HSPOBJINFO *) sbAlloc( sizeof(HSPOBJINFO) );
		objmax++;
	} else {
		if ( objmax ) {
			for( i=0; i<objmax; i++ ) {
				if ( mem_obj[i].owmode == HSPOBJ_NONE ) return i;
			}
		}
		if ( objmax >= objlimit ) throw HSPERR_WINDOW_OBJECT_FULL;
		id = objmax++;
		mem_obj = (HSPOBJINFO *) sbExpand( (char *)mem_obj, sizeof(HSPOBJINFO) * objmax );
	}
	return id;
}


HSPOBJINFO *Bmscr::AddHSPObject( int id, int mode )
{
	HSPOBJINFO *obj;
	obj = &mem_obj[id];
	obj->hCld = NULL;
	obj->owmode = mode;
	obj->enableflag = 1;
	obj->bm = GetBMSCR();
	obj->func_notice = NULL;
	obj->func_objprm = NULL;
	obj->func_delete = NULL;
	obj->func_draw = NULL;
	obj->owid = 0;
	obj->owsize = 0;
	obj->btnset = NULL;

	obj->x = this->cx;
	obj->y = this->cy;
	obj->sx = this->ox;
	obj->sy = this->oy;
	obj->tapflag = 0;
	obj->srcid = -1;

	return obj;
}


HSPOBJINFO *Bmscr::GetHSPObject( int id )
{
	return &mem_obj[id];
}


HSPOBJINFO *Bmscr::GetHSPObjectSafe( int id )
{
	if (( id < 0 )||( id >= objmax )) throw HSPERR_ILLEGAL_FUNCTION;
	return &mem_obj[id];
}


void Bmscr::DeleteHSPObject( int id )
{
	//		オブジェクト削除
	//
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );

	if ( obj->owmode == HSPOBJ_NONE ) return;
	if ( obj->func_delete != NULL ) obj->func_delete( obj );
	if ( obj->btnset != NULL ) {
		sbFree( obj->btnset ); obj->btnset = NULL;
	}
	obj->owmode = HSPOBJ_NONE;
}


int Bmscr::AddHSPObjectButton( char *name, int flag, void *callptr )
{
	//		create push button
	//
	int id;
	HSPOBJINFO *obj;
	HSP3BTNSET *btn;

	id = NewHSPObject();
	obj = AddHSPObject( id, HSPOBJ_TAB_ENABLE );

	obj->owid = -1;
	obj->owsize = id;

	btn = (HSP3BTNSET *)sbAlloc( sizeof(HSP3BTNSET) );
	obj->btnset = btn;

	strncpy( btn->name, name, 63 );
	btn->jumpmode = flag;
	btn->ptr = callptr;
	btn->messx = -1;
	btn->messy = -1;

	obj->func_draw = Object_ButtonDraw;
	obj->func_notice = Object_JumpEvent;
	//obj->func_delete = Object_WindowDelete;
	//obj->func_objprm = Object_SetInputBox;
	Posinc( oy );
	return id;
}


void Bmscr::SetButtonImage( int id, int bufid, int x1, int y1, int x2, int y2, int x3, int y3 )
{
	HSPOBJINFO *obj;
	HSP3BTNSET *bset;

	obj = GetHSPObjectSafe( id );

	if ( obj->func_notice != Object_JumpEvent ) throw HSPERR_UNSUPPORTED_FUNCTION;

	obj->srcid = bufid;

	bset = obj->btnset;
	if ( bset != NULL ) {
		bset->normal_x = x1;
		bset->normal_y = y1;
		bset->push_x = x2;
		bset->push_y = y2;
		bset->focus_x = x3;
		bset->focus_y = y3;
	}
}


int Bmscr::DrawAllObjects( void )
{

	//		DishSystemObjectを描画する
	//
	int i;
	int bak_mulcolor;
	HSPOBJINFO *info;
	info = this->mem_obj;
	if ( info == NULL ) return -1;

	bak_mulcolor = this->mulcolor;
	SetMulcolor( 255,255,255 );				// 乗算カラーを標準に戻す

	for( i=0;i<this->objmax;i++ ) {
		if ( info->owmode != HSPOBJ_NONE ) {
			if ( info->func_draw != NULL ) info->func_draw( info );
		}
		info++;
	}

	SetMulcolor( (bak_mulcolor>>16)&0xff, (bak_mulcolor>>8)&0xff, (bak_mulcolor)&0xff );				// 乗算カラーを元に戻す
	return -1;
}


int Bmscr::UpdateAllObjects( void )
{
	//		DishSystemObjectを更新する
	//
	int i,x,y,msx,msy,tap;
	HSPOBJINFO *info;
	HSPOBJINFO *focus;
	int focustap;
	info = this->mem_obj;
	if ( info == NULL ) return -1;

	msx = savepos[BMSCR_SAVEPOS_MOSUEX];
	msy = savepos[BMSCR_SAVEPOS_MOSUEY];

	if ( this->tapinvalid ) {				// ボタンのない場所で押した後は離すまで待つ
		if ( this->tapstat == 0 ) {
			this->tapinvalid = 0;
		}
		return -1;
	}

	if ( this->cur_obj != NULL ) {			// ボタンにフォーカスしている時(押した後)

		//Alertf( "[BtnHold]%d,%d,%d",msx,msy,tapstat );

		info = this->cur_obj;
		tap = -1;
		y = msy - info->y;
		if (( y>=0 )&&( y<info->sy )) {
			x = msx - info->x;
			if (( x>=0 )&&( x<info->sx )) {
				tap = this->tapstat;
			}
		}
		info->tapflag = tap;
		if ( this->tapstat == 0 ) {
			this->cur_obj = NULL;
			if ( tap >= 0 ) {
				info->func_notice( info, tap );
			}
		}
		return -1;
	}

	//		まだ何も押していない状態
	//
	focus = NULL;
	focustap = 0;
	for( i=0;i<this->objmax;i++ ) {
		if ( info->owmode != HSPOBJ_NONE ) {
			if ( info->func_notice != NULL ) {
				tap = -1;
				y = msy - info->y;
				if (( y>=0 )&&( y<info->sy )) {
					x = msx - info->x;
					if (( x>=0 )&&( x<info->sx )) {
						tap = this->tapstat;
						focus = info;
						focustap = tap;
					}
				}
			}
		}
		info++;
	}

	//Alertf( "[BtnPush][%x,%d]%d,%d,%d",focus,msx,msy,tapstat );

	//		押したボタンの判定
	//
	//if ( info->tapflag == 0 ) {
		if ( this->tapstat == 1 ) {
			if ( focus != NULL ) {
				this->tapinvalid = 0;
				this->cur_obj = focus;
				focus->tapflag = focustap;
			} else {
				this->tapinvalid = 1;
			}
		}
	//}

	return -1;
}
