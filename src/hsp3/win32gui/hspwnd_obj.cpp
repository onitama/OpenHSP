
//
//	HSP3 window manager
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "../hsp3config.h"
#include "../hsp3debug.h"
#include "../hspwnd.h"
#include "../dpmread.h"
#include "../strbuf.h"
#include "../strnote.h"
#include "../supio.h"

#include <Commctrl.h>


#ifndef _tstof
#ifdef HSPUTF8
#define _tstof(prm)		_wtof(prm)
#else
#define _tstof(prm)		atof(prm)
#endif
#endif


/*------------------------------------------------------------*/
/*
		Object callback interface
*/
/*------------------------------------------------------------*/

static int *notice_ptr;
static int bmscr_obj_ival;
static double bmscr_obj_dval;
static WNDPROC DefEditProc;
static WNDPROC DefButtonProc;

extern HspWnd *curwnd;

LRESULT CALLBACK MyEditProc( HWND hwnd , UINT msg , WPARAM wp , LPARAM lp ) {
	if ( msg==WM_CHAR ) {
		if (( wp==13 )||( wp==9 )) {
			return 0;
		}
	}
	return CallWindowProc( DefEditProc , hwnd , msg , wp , lp);
}

static void UpdateCustomButton( HWND hwnd, int flag )
{
	int id;
	HDC disthdc;
	HWND hw;
	Bmscr *bm;
	HSPOBJINFO *obj;

	hw = (HWND)GetWindowLongPtr( hwnd , GWLP_HWNDPARENT );
	id = (int)GetWindowLongPtr( hw, GWLP_USERDATA );
	bm = curwnd->GetBmscrSafe( id );

	id = (int)GetWindowLongPtr( hwnd, GWLP_USERDATA );
	obj = bm->GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) return;
	if ( hwnd != obj->hCld ) return;

	if ( flag ) {
		if ( obj->option & 0x100 ) return;

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;
		_TrackMouseEvent(&tme);

		obj->option |= 0x100;
	} else {
		if (( obj->option & 0x100 ) == 0 ) return;
		obj->option &= ~0x100;
	}

	disthdc=GetDC( hwnd );
	bm->DrawHSPCustomButton( obj, disthdc, flag );
	ReleaseDC( hwnd, disthdc );
}

LRESULT CALLBACK MyButtonProc( HWND hwnd , UINT msg , WPARAM wp , LPARAM lp ) {
	switch( msg ) {
	case WM_MOUSEMOVE:
		UpdateCustomButton( hwnd, 1 );
		break;
	case WM_MOUSELEAVE:
		UpdateCustomButton( hwnd, 0 );
		break;
	}
	return CallWindowProc( DefButtonProc , hwnd , msg , wp , lp);
}

void SetObjectEventNoticePtr( int *ptr )
{
	//		イベント時の値ポインタを設定
	//
	notice_ptr = ptr;
}

static void Object_WindowDelete( HSPOBJINFO *info )
{
	DestroyWindow( info->hCld );
	info->hCld = NULL;
}

static void Object_JumpEvent( HSPOBJINFO *info, int wparam )
{
	*notice_ptr = info->owsize;
	if ( info->option & 1 ) {
		code_call( (unsigned short *)info->varset.ptr );
	} else {
		code_setpci( (unsigned short *)info->varset.ptr );
	}
}

static void Object_SendSetVar( HSPOBJINFO *obj )
{
	code_setva( obj->varset.pval, obj->varset.aptr, obj->varset.type, obj->varset.ptr );
}

static void Object_CheckBox( HSPOBJINFO *info, int wparam )
{
	HWND hwnd;
	BMSCR *bm;
	int val, cid;

	bm = (BMSCR *)info->bm;
	hwnd = bm->hwnd;
	cid = GetDlgCtrlID( info->hCld );
	val = IsDlgButtonChecked( hwnd, cid );
	bmscr_obj_ival = val;
	Object_SendSetVar( info );
}

static void Object_IntInput( HSPOBJINFO *info, int wparam )
{
	HWND hwnd;
	BMSCR *bm;
	TCHAR minp[64];
	int val, cid, notify;

	bm = (BMSCR *)info->bm;
	hwnd = bm->hwnd;
	notify = wparam>>16;
	if ( notify != EN_UPDATE ) return;
	cid = GetDlgCtrlID( info->hCld );
	val = GetDlgItemText( hwnd, cid, minp, 63 );
	if ( val == 0 ) {
		bmscr_obj_ival = 0;
	} else {
		bmscr_obj_ival = _ttoi( minp );
	}
	info->varset.ptr = (void *)&bmscr_obj_ival;
	Object_SendSetVar( info );
}

static void Object_DoubleInput( HSPOBJINFO *info, int wparam )
{
	HWND hwnd;
	BMSCR *bm;
	TCHAR minp[64];
	int val, cid, notify;

	bm = (BMSCR *)info->bm;
	hwnd = bm->hwnd;
	notify = wparam>>16;
	if ( notify != EN_UPDATE ) return;
	cid = GetDlgCtrlID( info->hCld );
	val = GetDlgItemText( hwnd, cid, minp, 63 );
	if ( val == 0 ) {
		bmscr_obj_dval = 0.0;
	} else {
		bmscr_obj_dval = _tstof( minp );
	}
	info->varset.ptr = (void *)&bmscr_obj_dval;
	Object_SendSetVar( info );
}

static void Object_StrInput( HSPOBJINFO *info, int wparam )
{
	HWND hwnd;
	BMSCR *bm;
	TCHAR minp[0x8000];
	TCHAR *bigbuf;
	int val, cid, notify, size;
	HSPCHAR *hctmp1 = 0;

	bm = (BMSCR *)info->bm;
	hwnd = bm->hwnd;
	notify = wparam>>16;
	if ( notify != EN_UPDATE ) return;

	size = (int)SendMessage( info->hCld, WM_GETTEXTLENGTH,0,0L );
	cid = GetDlgCtrlID( info->hCld );
	
	if ( size < 0x8000 ) {
		bigbuf = minp;
		val = GetDlgItemText( hwnd, cid, minp, 0x7fff );
	} else {
		bigbuf = (HSPAPICHAR*)sbAlloc( size+1 );
		val = GetDlgItemText( hwnd, cid, bigbuf, size );
	}

	if ( val == 0 ) {
		bmscr_obj_ival = 0;
		info->varset.ptr = (void *)&bmscr_obj_ival;
	} else {
		apichartohspchar(bigbuf,&hctmp1);
		info->varset.ptr = hctmp1;
	}
	Object_SendSetVar( info );

	if ( size >= 0x8000 ) sbFree( bigbuf );
	freehc(&hctmp1);
}

static void Object_ComboBox( HSPOBJINFO *info, int wparam )
{
	int notify;
	notify = wparam>>16;
	if ( notify != CBN_SELENDOK ) return;
	bmscr_obj_ival = (int)SendMessage( info->hCld, CB_GETCURSEL,0,0L );
	Object_SendSetVar( info );
}

static void Object_ListBox( HSPOBJINFO *info, int wparam )
{
	bmscr_obj_ival = (int)SendMessage( info->hCld, LB_GETCURSEL,0,0L );
	Object_SendSetVar( info );
}

static void Object_SetMultiBox( HSPOBJINFO *info, int type, void *ptr )
{
	int i,max;
	char res[256];
	UINT m_ini,m_add;
	HWND hw;
	CStrNote note;
	HSPAPICHAR *hactmp1 = 0;

	hw = info->hCld;

	switch( type ) {
	case TYPE_STRING:
		if ( info->owid ) {
			m_ini=CB_RESETCONTENT;
			m_add=CB_ADDSTRING;
		} else {
			m_ini=LB_RESETCONTENT;
			m_add=LB_ADDSTRING;
		}
		note.Select( (HSPCHAR*)ptr );
		max = note.GetMaxLine();
		SendMessage( hw, m_ini, 0, 0L );
		for( i=0;i<max;i++ ) {
			note.GetLine( res, i, 255 );
			SendMessage( hw, m_add, 0, (LPARAM)chartoapichar(res,&hactmp1) );
			freehac(&hactmp1);
		}
		break;
	case TYPE_INUM:
		if ( info->owid ) {
			m_ini=CB_SETCURSEL;
		} else {
			m_ini=LB_SETCURSEL;
		}
		bmscr_obj_ival = *(int *)ptr;
		SendMessage( hw, m_ini, bmscr_obj_ival, 0L );
		Object_SendSetVar( info );
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
}


static void Object_SetInputBox( HSPOBJINFO *info, int type, void *ptr )
{
	HWND hw;
	HSPAPICHAR *hactmp1 = 0;
	hw = info->hCld;
	switch( type ) {
	case TYPE_STRING:
		SetWindowText( hw, chartoapichar((char*)ptr,&hactmp1) );
		freehac(&hactmp1);
		break;
	case TYPE_INUM:
	case TYPE_DNUM:
		SetWindowText( hw, chartoapichar((char *)HspVarCoreCnv( type, TYPE_STRING, ptr ),&hactmp1) );
		freehac(&hactmp1);
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
}


static void Object_SetInputMesBox( HSPOBJINFO *info, int type, void *ptr )
{
	HWND hw;
	hw = info->hCld;
	Object_SetInputBox( info, type, ptr );
	Object_StrInput( info, EN_UPDATE<<16 );	// 即時反映させる
}


static void Object_SetCheckBox( HSPOBJINFO *info, int type, void *ptr )
{
	HWND const hw = info->hCld;
	HSPAPICHAR *hactmp1 = 0;
	switch ( type ) {
	case HSPVAR_FLAG_STR:
		SetWindowText( hw, 
			chartoapichar(static_cast< char * >( ptr ),&hactmp1) );
		break;
	case HSPVAR_FLAG_INT:
		SendMessage( hw, BM_SETCHECK,
		 ( *static_cast< int * >( ptr ) ? 1 : 0 ), 0 );
		Object_CheckBox( info, 0 );
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
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


int Bmscr::ActivateHSPObject( int id )
{
	//		フォーカスをONにする
	//
	int a,cid;
	HWND ow;
	HSPOBJINFO *obj;

	if (id>=0) {
		obj = GetHSPObjectSafe( id );
		ow = obj->hCld;
		if ( ow == NULL ) return -2;

		if ( obj->owmode & HSPOBJ_TAB_SELALLTEXT ) {
			SendMessage( ow, EM_SETSEL, 0, -1);
		}
		SetFocus( ow );
		return 0;
	}
	cid = -1;
	ow = GetFocus();
	if ( ow != NULL) {
		for( a=0; a<objmax; a++ ) {
			if ( ow == mem_obj[ a ].hCld ) cid = a;
		}
	}
	return cid;
}



void Bmscr::EnableObject( int id, int sw )
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) throw HSPERR_ILLEGAL_FUNCTION;
	EnableWindow( obj->hCld, sw!=0 );
}


void Bmscr::SetObjectMode( int id, int owmode )
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) throw HSPERR_ILLEGAL_FUNCTION;
	if ( owmode <= 0 ) throw HSPERR_ILLEGAL_FUNCTION;
	obj->owmode = owmode;
}


void Bmscr::NextObject( int plus )
{
	//		[TAB]を押して次のobjectを選択
	//
	HWND hw;
	HSPOBJINFO *obj;
	int a,cid,ts;

	cid = ActivateHSPObject(-1);
	obj = &mem_obj[ cid ];
	if ( cid>=0 ) if ( ( obj->owmode & 3 ) == HSPOBJ_TAB_DISABLE ) return;

	for( a=0; a<objmax; a++ ) {
		cid+=plus;
		if ( cid>=objmax ) cid = 0;
		if ( cid<0 ) cid = objmax - 1;
		obj = &mem_obj[ cid ];
		hw = obj->hCld;
		if ( hw != NULL ) {
			ts = obj->owmode & 3;
			if ( ts != HSPOBJ_TAB_SKIP ) {
				if ( IsWindowEnabled( hw ) ) {
					if ( obj->owmode & HSPOBJ_TAB_SELALLTEXT ) {
						SendMessage( hw, EM_SETSEL, 0, -1);
					}
					SetFocus( hw );
					return;
				}
			}
		}
	}
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


HSPOBJINFO *Bmscr::AddHSPObject( int id, HWND handle, int mode )
{
	HSPOBJINFO *obj;
	obj = &mem_obj[id];
	obj->hCld = handle;
	obj->owmode = mode;
	obj->option = 0;
	obj->bm = GetBMSCR();
	obj->func_notice = NULL;
	obj->func_objprm = NULL;
	obj->func_delete = NULL;
	obj->owid = 0;
	obj->owsize = 0;
	SetHSPObjectFont( id );
	return obj;
}


HSPOBJINFO *Bmscr::AddHSPJumpEventObject( int id, HWND handle, int mode, int val, void *ptr )
{
	HSPOBJINFO *obj;
	obj = AddHSPObject( id, handle, mode );
	obj->owid = -1;
	obj->owsize = id;
	obj->option = val;
	obj->varset.ptr = ptr;
	obj->func_notice = Object_JumpEvent;

	SetWindowLongPtr( handle, GWLP_USERDATA, id );

	return obj;
}


void Bmscr::SetButtonImage( int id, int bufid, int x1, int y1, int x2, int y2, int x3, int y3 )
{
	HSPOBJINFO *obj;
	HSP3BTNSET *bset;

	obj = GetHSPObjectSafe( id );

	if ( obj->func_notice != Object_JumpEvent ) throw HSPERR_UNSUPPORTED_FUNCTION;

	obj->owid = bufid;
	SetWindowLong( obj->hCld, GWL_STYLE, GetWindowLong( obj->hCld, GWL_STYLE ) | BS_OWNERDRAW );

	DefButtonProc = (WNDPROC)GetWindowLongPtr( obj->hCld , GWLP_WNDPROC );

	if ( DefButtonProc != MyButtonProc ) {
		SetWindowLongPtr( obj->hCld , GWLP_WNDPROC , (LONG_PTR)MyButtonProc );
	}

	bset = (HSP3BTNSET *)(&obj->varset);
	bset->normal_x = x1;
	bset->normal_y = y1;
	bset->push_x = x2;
	bset->push_y = y2;
	bset->focus_x = x3;
	bset->focus_y = y3;
}


HSPOBJINFO *Bmscr::AddHSPVarEventObject( int id, HWND handle, int mode, PVal *pval, APTR aptr, int type, void *ptr )
{
	HSPOBJINFO *obj;
	obj = AddHSPObject( id, handle, mode );
	obj->varset.pval = pval;
	obj->varset.aptr = aptr;
	obj->varset.type = type;
	obj->varset.ptr = ptr;
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
	HFONT hf;
	HSPOBJINFO *obj;

	obj = GetHSPObjectSafe( id );

	if ( obj->owmode == HSPOBJ_NONE ) return;
	if ( obj->hCld != NULL ) {
		if ( obj->owmode & HSPOBJ_OPTION_SETFONT ) {
			hf=(HFONT)SendMessage( obj->hCld, WM_GETFONT, 0, 0 );
			if (hf!=NULL) DeleteObject( hf );
		}
		if ( obj->func_delete != NULL ) obj->func_delete( obj );
	}
	obj->owmode = HSPOBJ_NONE;
}


void Bmscr::UpdateHSPObject( int id, int type, void *ptr )
{
	//		オブジェクトに値を設定する
	//
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe( id );
	if ( obj->func_objprm != NULL ) {
		obj->func_objprm( obj, type, ptr );
	} else {
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
}


void Bmscr::SetHSPObjectFont( int id )
{
	int a;
	HWND hw;
	HFONT hf;
	PLOGFONT plf;
	HSPOBJINFO *obj;

	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) return;
	if (( obj->owmode & HSPOBJ_OPTION_SETFONT ) == 0 ) return;

	a = objmode;
	if (a) {
		hw = obj->hCld;
		if ( hw == NULL ) return;
		if (a==1) hf=(HFONT)GetStockObject(17);
		else {
			plf=(PLOGFONT) &logfont;
			hf=CreateFontIndirect( plf );
		}
		PostMessage( hw,WM_SETFONT,(WPARAM)hf,TRUE );
	}
}


void Bmscr::SendHSPObjectNotice( int wparam )
{
	//		オブジェクトの通知処理
	//
	int id;
	HWND hw;
	HSPOBJINFO *obj;
	id = wparam & (MESSAGE_HSPOBJ-1);
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) return;
	hw = obj->hCld;
	if ( hw == NULL ) return;

	if ( obj->func_notice != NULL ) {
		obj->func_notice( obj, wparam );
	}
}


void Bmscr::DrawHSPCustomButton( HSPOBJINFO *obj, HDC drawhdc, int flag )
{
	//		オーナードローの描画処理
	//		(flag:0=通常、1=フォーカス、2=押下)
	//
	int xx,yy;
	Bmscr *src;
	HspWnd *wnd;
	HSP3BTNSET *bset;
	RECT rect;
	HFONT hFont;
	COLORREF col;
	HSPAPICHAR *hactmp1 = 0;

	char msgtmp[256];

	GetClientRect( obj->hCld, &rect );
	if ( obj->owid < 0 ) {

	} else {
		bset = (HSP3BTNSET *)(&obj->varset);
		wnd = (HspWnd *)master_hspwnd;
		src = wnd->GetBmscrSafe( obj->owid );
		switch( flag ) {
		case 1:
			xx = bset->focus_x;
			yy = bset->focus_y;
			break;
		case 2:
			xx = bset->push_x;
			yy = bset->push_y;
			break;
		default:
			xx = bset->normal_x;
			yy = bset->normal_y;
			break;
		}
		BitBlt( drawhdc, rect.left, rect.top, rect.right, rect.bottom, src->hdc, xx, yy, SRCCOPY );
	}

	if ( obj->option & 0x200 ) {			// キーボードフォーカスあり
		InflateRect( &rect, -3, -3 );
		DrawFocusRect( drawhdc, &rect );
		GetClientRect( obj->hCld, &rect );
	}

	SendMessage( obj->hCld, WM_GETTEXT, 255, (LPARAM)msgtmp );
	hFont = (HFONT)SelectObject(drawhdc, (HGDIOBJ)SendMessage( obj->hCld, WM_GETFONT, 0, 0));
	col = RGB(0,0,0);
	SetBkMode( drawhdc,TRANSPARENT );
	SetTextColor( drawhdc, col );
	DrawText( drawhdc, chartoapichar(msgtmp,&hactmp1), -1, &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	freehac(&hactmp1);
	SelectObject( drawhdc, hFont );
}


void Bmscr::SendHSPObjectDraw( int wparam, LPDRAWITEMSTRUCT lparam )
{
	//		オーナードローの描画処理を呼び出す(wparam,lparam処理)
	//
	int id,flag;
	HWND hw;
	HSPOBJINFO *obj;
	id = wparam & (MESSAGE_HSPOBJ-1);
	obj = GetHSPObjectSafe( id );
	if ( obj->owmode == HSPOBJ_NONE ) return;
	hw = obj->hCld;
	if ( hw != lparam->hwndItem ) return;

	if ( lparam->itemState & ODS_FOCUS ) {
		obj->option |= 0x200;
	} else {
		obj->option &= ~0x200;
	}

	flag = 0;
	if ( obj->option & 0x100 ) flag = 1;
	if ( lparam->itemState & ODS_SELECTED ) flag = 2;
	DrawHSPCustomButton( obj, lparam->hDC, flag );
}


int Bmscr::AddHSPObjectButton( char *name, int flag, void *callptr )
{
	//		create push button
	//
	HWND hw;
	int id,ws;
	HSPOBJINFO *obj;
	HSPAPICHAR *hactmp1 = 0;

	id = NewHSPObject();
	ws = objstyle | BS_PUSHBUTTON;
/*
	rev 43
	mingw : warning : 変数型から大きさの異なるポインタ型への変換
	に対処
	以降4ヶ所も同様。
*/
	hw = CreateWindow( TEXT("button"), chartoapichar(name,&hactmp1), ws,
				cx, cy, ox, oy, hwnd,
				reinterpret_cast< HMENU >( static_cast< WORD >( MESSAGE_HSPOBJ + id ) ), hInst, NULL );
	freehac(&hactmp1);

	// ダブルクリックの受付を抑制 
	SetClassLong( hw, GCL_STYLE, GetClassLong(hw, GCL_STYLE) & ~CS_DBLCLKS );

	obj = AddHSPJumpEventObject( id, hw, HSPOBJ_TAB_ENABLE|HSPOBJ_OPTION_SETFONT, flag, callptr );
	obj->func_delete = Object_WindowDelete;
	obj->func_objprm = Object_SetInputBox;
	Posinc( oy );
	return id;
}


int Bmscr::AddHSPObjectCheckBox( char *name, PVal *pval, APTR aptr )
{
	//		create push button
	//
	HWND hw;
	int id,ws;
	int *iptr;
	HSPOBJINFO *obj;
	HSPAPICHAR *hactmp1 = 0;

	id = NewHSPObject();
	ws = objstyle | BS_AUTOCHECKBOX;
	hw = CreateWindow( TEXT("button"), chartoapichar(name,&hactmp1), ws,
				cx, cy, ox, oy, hwnd,
				reinterpret_cast< HMENU >( static_cast< WORD >( MESSAGE_HSPOBJ + id ) ), hInst, NULL );
	freehac(&hactmp1);

	obj = AddHSPVarEventObject( id, hw, HSPOBJ_TAB_ENABLE|HSPOBJ_OPTION_SETFONT, pval, aptr, TYPE_INUM, (void *)&bmscr_obj_ival );
	obj->func_notice = Object_CheckBox;
	obj->func_objprm = Object_SetCheckBox;
	obj->func_delete = Object_WindowDelete;
	Posinc( oy );

	iptr = (int *)HspVarCorePtrAPTR( pval, aptr );
	if ( *iptr ) SendMessage( hw, BM_SETCHECK, 1, 0 );

	return id;
}


int Bmscr::AddHSPObjectInput( PVal *pval, APTR aptr, int sizex, int sizey, char *defval, int limit, int mode )
{
	//		create input box
	//
	HWND hwedit;
	int id,ws,ws2,max,tabstop,type,subcl;
	HSPOBJINFO *obj;
	HSPAPICHAR *hactmp1 = 0;

	id = NewHSPObject();
	ws = objstyle;
	ws2 = WS_EX_CLIENTEDGE;
	max = 16;
	subcl = 0;
	tabstop = HSPOBJ_TAB_ENABLE;
	type = mode & 15;
	if ( type == HSPOBJ_INPUT_STR ) max = limit;

	if ( mode & HSPOBJ_INPUT_HSCROLL ) ws|=WS_HSCROLL;
	if ( mode & HSPOBJ_INPUT_MULTILINE ) {
		ws|=ES_LEFT|ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL|ES_AUTOVSCROLL;
		//tabstop = HSPOBJ_TAB_DISABLE;
		tabstop = HSPOBJ_TAB_SKIP;
		max = 0;
	}
	else {
		//if ( bm->objmode & 32 ) ws2=WS_EX_WINDOWEDGE;
		ws|=ES_AUTOHSCROLL;
		tabstop |= HSPOBJ_TAB_SELALLTEXT;
		subcl = 1;
	}

	if ( mode & HSPOBJ_INPUT_READONLY ) {
		ws|=ES_READONLY;
		tabstop = HSPOBJ_TAB_SKIP;
	}
	else {
		ws|=WS_TABSTOP;
	}

	hwedit = CreateWindowEx( ws2, TEXT("edit"), NULL, ws,
					cx, cy, sizex, sizey,
					hwnd, reinterpret_cast< HMENU >( static_cast< WORD >( MESSAGE_HSPOBJ + id ) ), hInst, NULL );

	if ( subcl ) {
		DefEditProc = (WNDPROC)GetWindowLongPtr( hwedit , GWLP_WNDPROC );
		SetWindowLongPtr(hwedit, GWLP_WNDPROC, (LONG_PTR)MyEditProc);
	}

	obj = AddHSPVarEventObject( id, hwedit, tabstop|HSPOBJ_OPTION_SETFONT, pval, aptr, type, (void *)&bmscr_obj_ival );
	switch( type ) {
	case HSPOBJ_INPUT_STR:
		obj->func_notice = Object_StrInput;
		break;
	case HSPOBJ_INPUT_DOUBLE:
		obj->func_notice = Object_DoubleInput;
		break;
	case HSPOBJ_INPUT_INT:
		obj->func_notice = Object_IntInput;
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}

	obj->func_delete = Object_WindowDelete;
	obj->func_objprm = Object_SetInputBox;

	if ( !subcl ) {			// mesbox用のobjprm更新を追加(2011/1/6)
		obj->func_objprm = Object_SetInputMesBox;
	}

	Posinc( sizey );

	SendMessage( hwedit, EM_LIMITTEXT, limit, 0L );
	SetWindowText( hwedit,chartoapichar(defval,&hactmp1) );
	freehac(&hactmp1);
	if ( focflg == 0 ) {
		ActivateHSPObject( id );
	}
	focflg++;

	return id;
}


int Bmscr::AddHSPObjectMultiBox( PVal *pval, APTR aptr, int psize, char *defval, int mode )
{
	//		create combo/list box
	//
	HWND hw;
	HSPOBJINFO *obj;
	int id,sizex,sizey;
	int *iptr;

	id = NewHSPObject();
	sizex = ox; sizey = oy;
	iptr = (int *)HspVarCorePtrAPTR( pval, aptr );

	if ( mode ) {
		hw = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("combobox"), TEXT(""),
			objstyle|WS_VSCROLL|CBS_DROPDOWNLIST,
			cx, cy, sizex, sizey + psize, hwnd,
			reinterpret_cast< HMENU >( static_cast< WORD >( MESSAGE_HSPOBJ + id ) ), hInst, NULL );
	} else {
		sizey += psize;
		hw = CreateWindowEx( WS_EX_CLIENTEDGE, TEXT("listbox"), TEXT(""),
			objstyle|WS_VSCROLL|LBS_NOTIFY,
			cx, cy, sizex, sizey, hwnd,
			reinterpret_cast< HMENU >( static_cast< WORD >( MESSAGE_HSPOBJ + id ) ), hInst, NULL );
	}

	obj = AddHSPVarEventObject( id, hw, HSPOBJ_TAB_ENABLE|HSPOBJ_OPTION_SETFONT, pval, aptr, TYPE_INUM, (void *)&bmscr_obj_ival );
	obj->owid = mode;

	if ( mode ) {
		obj->func_notice = Object_ComboBox;
	} else {
		obj->func_notice = Object_ListBox;
	}
	obj->func_objprm = Object_SetMultiBox;
	obj->func_delete = Object_WindowDelete;
	Posinc( sizey );
	Object_SetMultiBox( obj, TYPE_STRING, defval );
	Object_SetMultiBox( obj, TYPE_INUM, iptr );
	return id;
}

