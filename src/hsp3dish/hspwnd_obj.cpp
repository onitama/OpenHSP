
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
		Object base interface
*/
/*------------------------------------------------------------*/

Hsp3ObjBase::Hsp3ObjBase(void)
{
	messx = -1;
	messy = -1;
	value = 0;
}

Hsp3ObjBase::~Hsp3ObjBase(void)
{
}

/*------------------------------------------------------------*/
/*
		Object callback interface
*/
/*------------------------------------------------------------*/

static bool bmscr_obj_drawflag;
static int *notice_ptr;
static int bmscr_obj_ival;
static double bmscr_obj_dval;

void SetObjectEventNoticePtr( int *ptr )
{
	//		イベント時の値ポインタを設定(stat値)
	//
	notice_ptr = ptr;
}

static void Object_JumpEvent(HSPOBJINFO *info, int wparam)
{
	if (info->enableflag == 0) return;

	if (wparam != HSPOBJ_NOTICE_CLICK_END) return;

	*notice_ptr = info->owsize;				// statを更新

	Hsp3ObjButton *btn = (Hsp3ObjButton *)info->btnset;
	if (btn == NULL) return;

	if (btn->jumpmode & 1) {
		code_call((unsigned short *)btn->ptr);
	}
	else {
		code_setpci((unsigned short *)btn->ptr);
	}

}

static void Object_SendSetVar(HSPOBJINFO *obj)
{
	//	オブジェクトに関連付けされた変数の値を更新する
	//
	HSP3VARSET *var = obj->varset;
	if (var == NULL) return;
	code_setva(var->pval, var->aptr, var->type, var->ptr);
}

static void Object_SendSetVar(HSPOBJINFO *obj, int type, void *ptr)
{
	//	オブジェクトに関連付けされた変数の値を更新する
	//	(任意の型とポインタによる更新)
	//
	HSP3VARSET *var = obj->varset;
	if (var == NULL) return;

	if (var->type == type) {
		//	同じタイプの場合はそのまま代入する
		code_setva(var->pval, var->aptr, var->type, ptr);
		return;
	}
	//	型の変換
	char *p = (char *)HspVarCoreCnv(type, var->type, ptr);
	code_setva(var->pval, var->aptr, var->type, p);
}

static void Object_ApplyFont(HSPOBJINFO *info)
{
	Bmscr *bm;
	char *fn = "";

	bm = (Bmscr *)info->bm;
#ifdef HSPWIN
	if (info->fontmode & HSPOBJ_FONTMODE_GUIFONT) {
		fn = "MS UI Gothic";
	}
#endif
	if (info->owmode & HSPOBJ_OPTION_SETFONT) {
		if (info->fontname) {
			fn = (char *)info->fontname->c_str();
		}
	}
	bm->SetFontInternal(fn, info->fontsize, info->fontstyle);
}

static void Object_ButtonDraw( HSPOBJINFO *info )
{
	Bmscr *bm;
	int col1,col2,tcol;
	int x1,x2,y1,y2;

	bm = (Bmscr *)info->bm;
	Object_ApplyFont(info);

	Hsp3ObjButton *btn = (Hsp3ObjButton *)info->btnset;
	if (btn == NULL) return;

	x1 = info->x; y1 = info->y;
	x2 = info->x + info->sx - 1; y2 = info->y + info->sy - 1;

	tcol = info->fontcolor & 0xffffff;
	//tcol = 0xffffff;
	if ( info->srcid < 0 ) {
		//	グラデーションによる標準ボタン
		if ( info->enableflag ) {
			if ( info->tapflag == 1 ) {
				col1 = 0x909090; col2 = 0xc0c0c0;
				//col1 = 0xff202020; col2 = 0xff404040;
			} else {
				col1 = 0xf0f0f0; col2 = 0xc0c0c0;
				//col1 = 0xff404040; col2 = 0xff606060;
			}
		} else {
			col1 = 0x808080; col2 = 0x606060; tcol = 0x404040;
		}
		bm->gmode = 0;
		bm->gfrate = 255;
		bm->GradFill( x1+1, y1+1, info->sx-2, info->sy-2, 1, col1, col2 );
		bm->Setcolor( 0x808080 );
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
			tcol = 0x808080;
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
	bm->cx = info->x;
	bm->cy = info->y;
	bm->printoffsetx = info->sx;
	bm->printoffsety = info->sy;
	if ( info->tapflag == 1 ) {
		bm->cx++;
		bm->cy++;
	}
	bm->Print( (char *)info->btnset->name.c_str(), 0 );
}

static void Object_CheckBoxDraw(HSPOBJINFO *info)
{
	Bmscr *bm;
	int tcol;
	int bgcol;
	int x1, x2, y1, y2;
	int fontmode;

	Object_ApplyFont(info);

	bm = (Bmscr *)info->bm;
	Hsp3ObjButton *btn = (Hsp3ObjButton *)info->btnset;
	if (btn == NULL) return;

	tcol = info->fontcolor;
	if (info->enableflag == 0) tcol = 0xff808080;
	//if (info->tapflag == 1) tcol = 0xff808080;

	x1 = info->x; y1 = info->y;
	x2 = info->x + info->sx - 1; y2 = info->y + info->sy - 1;

	bgcol = 0;
	fontmode = info->fontmode & 3;
	switch (fontmode) {
	case HSPOBJ_FONTMODE_OPAQUE:
		bgcol = 0xff000000 | (info->backcolor & 0xffffff);
		break;
	case HSPOBJ_FONTMODE_TRANSPARENT:
		bgcol = 0x80000000 | ( info->backcolor & 0xffffff );
		break;
	default:
		break;
	}

	if (bgcol != 0) {
		bm->gmode = 3;
		bm->gfrate = ( bgcol >> 24 ) & 0xff;
		bm->Setcolor(bgcol);
		bm->Boxfill(x1, y1, x2, y2, 1);
	}

	//bm->gmode = 0;
	//bm->gfrate = 255;
	//bm->GradFill(x1 + 1, y1 + 1, info->sx - 2, info->sy - 2, 1, col1, col2);
	//bm->Setcolor(tcol);

	bm->gmode = 3;
	bm->gfrate = 255;
	bm->Setcolor(tcol);
	bm->cx = x1+ ( info->fontsize * 2 );
	bm->cy = y1;
	bm->printoffsety = info->sy;
	bm->Print((char *)info->btnset->name.c_str(), 0);

	int fsize = info->fontsize;
	int y = (info->sy - bm->printsizey)/2;
	if (y < 0) y = 0;
	x1 = x1 + fsize / 2;
	y1 = y1+y;
	x2 = x1 + fsize;
	y2 = y1 + bm->printsizey;

	bm->Boxfill(x1 - 1, y1 - 1, x2 + 1, y1);
	bm->Boxfill(x1 - 1, y2, x2 + 1, y2 + 1);
	bm->Boxfill(x1 - 1, y1 - 1, x1, y2 + 1);
	bm->Boxfill(x2, y1 - 1, x2 + 1, y2 + 1);

	if (btn->value) {
		bm->Boxfill(x1+2, y1+2, x2-2, y2-2);
	}

}

static void Object_CheckBox(HSPOBJINFO *info, int wparam)
{
	if (info->enableflag == 0) return;

	if (wparam != HSPOBJ_NOTICE_CLICK_END) return;

	Hsp3ObjBase *btn = info->btnset;
	if (btn == NULL) return;

	if (btn->value) {
		bmscr_obj_ival = 0;
	}
	else {
		bmscr_obj_ival = 1;
	}
	btn->value = bmscr_obj_ival;
	Object_SendSetVar(info);

}

static void Object_InputBoxDrawBoxf(Bmscr *bm, texmesPos *tpos, int clip_x, int clip_y, int p_x1, int p_y1, int p_x2, int p_y2, int sw )
{
	//	クリッピング付きのboxf
	//
	int x1, y1, x2, y2;
	int cx1, cy1, cx2, cy2;

	cx1 = clip_x;
	cy1 = clip_y;
	cx2 = clip_x + tpos->sx;
	cy2 = clip_y + tpos->sy;

	x1 = p_x1; y1 = p_y1;
	if (x1 > cx2) return;
	if (y1 > cy2) return;
	if (x1 < cx1) x1 = cx1;
	if (y1 < cy1) y1 = cy1;

	x2 = p_x2; y2 = p_y2;
	if (x2 < cx1) return;
	if (y2 < cy1) return;
	if (x2 >= cx2) x2 = cx2-1;
	if (y2 >= cy2) y2 = cy2-1;

	bm->Boxfill(x1, y1, x2, y2, sw);
}


static void Object_InputBoxDrawSub(Bmscr *bm, int x, int y, texmesPos *tpos, int color, bool cursor)
{
	//	1行描画+カーソル表示
	//
	int x1 = x+2;
	int y1 = y+2;

	//	透明度の設定
	bm->gmode = 3;
	bm->gfrate = 255;

	//	文字を描画
	bm->Setcolor(color);
	bm->cx = x1;
	bm->cy = y1;
	bm->Print(tpos);

	int py = tpos->printysize;
	x1 = tpos->lastcx;
	y1 = tpos->lastcy;
	if (cursor) {
		//	カーソルを描画
		int offset = tpos->getCaretX();
		tpos->caret_cnt+=bm->passed_time;
		if ((tpos->caret_cnt & 512) == 0) {
			//bm->Boxfill(x1+ offset, y1, x1 + offset + 2, y1 + py);
			Object_InputBoxDrawBoxf(bm,tpos, x1, y1, x1 + offset, y1, x1 + offset + 2, y1 + py,0);
		}
	}

	int sel_1, sel_2;
	bool selok = tpos->getSelection(&sel_1,&sel_2);
	if (selok) {
		bm->gmode = 3;
		bm->gfrate = 64;
		int xx1 = tpos->pos[sel_1];
		int xx2 = tpos->pos[sel_2];
		bm->Setcolor(0xff);
		//bm->Boxfill(x1+xx1, y1, x1+xx2, y1 + py, 1);
		Object_InputBoxDrawBoxf(bm, tpos, x1, y1, x1 + xx1, y1, x1 + xx2, y1 + py, 1);
	}
}

static void Object_InputBoxDraw(HSPOBJINFO *info)
{
	Bmscr *bm;
	int tcol;
	int bgcol;
	int x1, x2, y1, y2;
	int fontmode;

	Object_ApplyFont(info);

	bm = (Bmscr *)info->bm;
	Hsp3ObjInput *btn = (Hsp3ObjInput *)info->btnset;
	if (btn == NULL) return;

	tcol = info->fontcolor;
	if (info->enableflag == 0) tcol = 0xff808080;

	x1 = info->x; y1 = info->y;
	x2 = info->x + info->sx - 1; y2 = info->y + info->sy - 1;

	bgcol = 0;
	fontmode = info->fontmode & 3;
	switch (fontmode) {
	case HSPOBJ_FONTMODE_OPAQUE:
		bgcol = 0xff000000 | (info->backcolor & 0xffffff);
		break;
	case HSPOBJ_FONTMODE_TRANSPARENT:
		bgcol = 0x80000000 | (info->backcolor & 0xffffff);
		break;
	default:
		break;
	}

	btn->tpos.setSize(info->sx-4, info->sy-4);

	if (bgcol != 0) {
		//	背景を描画
		bm->gmode = 3;
		bm->gfrate = (bgcol >> 24) & 0xff;
		bm->Setcolor(bgcol);
		bm->Boxfill(x1, y1, x2, y2, 1);

		bm->Setcolor(0);
		bm->Boxfill(x1, y1, x2, y1+1);
		bm->Boxfill(x1, y2-1, x2, y2);
		bm->Boxfill(x1, y1, x1+1, y2);
		bm->Boxfill(x2-1, y1, x2, y2);
	}

	Object_InputBoxDrawSub(bm, x1, y1, &btn->tpos, tcol, bmscr_obj_drawflag);
}


static void Object_InputBoxApplyVAR(HSPOBJINFO *info)
{
	//	editの内容をvarに反映させる
	Bmscr *bm = (Bmscr *)info->bm;
	Hsp3ObjInput *edit = (Hsp3ObjInput *)info->btnset;
	if (edit == NULL) return;
	HSP3VARSET *var = info->varset;
	if (var == NULL) return;

}


static int Object_InputBoxApplyFuncKey(HSPOBJINFO *info, int code)
{
	//		ファンクションキーを処理する
	//		(HSPOBJ_NOTICE_KEY_F* を送信する)
	//		(返値 0=OK、1=更新あり、マイナス値はエラー)
	//
	if ((code < HSPOBJ_NOTICE_KEY_F1) || (code > HSPOBJ_NOTICE_KEY_F12)) return -1;
/*
	switch (code) {
	default:
		break;
	}
*/
//	Alertf("FUNC");
	return 0;
}


static int Object_InputBoxApplySpecialKey(HSPOBJINFO *info, int code)
{
	//		コントロールキーを処理する
	//		(ctrl+A～Zを1～26として送信する)
	//		0=OK、それ以外はNG
	//
	int key;
	Bmscr *bm = (Bmscr *)info->bm;
	Hsp3ObjInput *edit = (Hsp3ObjInput *)info->btnset;
	if (edit == NULL) return -1;

	if ((code >= HSPOBJ_NOTICE_KEY_F1) && (code <= HSPOBJ_NOTICE_KEY_F12)) {
		return Object_InputBoxApplyFuncKey(info, code);
	}

	if ((code < 1) || (code > 26)) return -1;
	key = code + '@';
	switch (key) {
	case 'A':
		edit->tpos.allSelection();
		return 0;
	case 'C':
	case 'X':
	{
		std::string selstr;
		int res = edit->tpos.getSelectionString(selstr);
		if (res) {
			hgio_editputclip((BMSCR *)bm,(char *)selstr.c_str());
		}
		if (key == 'X') {
			edit->tpos.deleteStringSelection();
		}
		return 0;
	}
	case 'V':
	{
		char *buf = hgio_editgetclip((BMSCR *)bm);
		if (buf) {
			edit->tpos.addStringFromCaret((char *)buf);
		}
		return 0;
	}
	default:
		break;
	}

	return 0;
}


static void Object_InputBox(HSPOBJINFO *info, int wparam)
{
	if (info->enableflag == 0) return;

	Bmscr *bm = (Bmscr *)info->bm;
	Hsp3ObjInput *edit = (Hsp3ObjInput *)info->btnset;
	if (edit == NULL) return;

	bool update = false;
	switch (wparam) {
	case HSPOBJ_NOTICE_CLICK:
	{
		bm->SelectEditHSPObject();
		int cur = edit->tpos.getPosFromX(bm->tapobj_posx);
		edit->tpos.setCaret(cur);
		edit->sel_start = cur;
		edit->sel_end = cur;
	break;
	}
	case HSPOBJ_NOTICE_CLICK_END:
	case HSPOBJ_NOTICE_CLICK_MOVE:
	{
		int cur = edit->tpos.getPosFromX(bm->tapobj_posex);
		edit->tpos.setCaret(cur);
		edit->sel_end = cur;
		break;
	}

	case HSPOBJ_NOTICE_KEY_SLEFT:
		edit->tpos.moveCaret(-1,true);
		break;
	case HSPOBJ_NOTICE_KEY_LEFT:
		edit->tpos.moveCaret(-1);
		break;
	case HSPOBJ_NOTICE_KEY_SRIGHT:
		edit->tpos.moveCaret(1, true);
		break;
	case HSPOBJ_NOTICE_KEY_RIGHT:
		edit->tpos.moveCaret(1);
		break;
	case HSPOBJ_NOTICE_KEY_SHOME:
		edit->tpos.setCaretHome(true);
		break;
	case HSPOBJ_NOTICE_KEY_HOME:
		edit->tpos.setCaretHome();
		break;
	case HSPOBJ_NOTICE_KEY_SEND:
		edit->tpos.setCaretEnd(true);
		break;
	case HSPOBJ_NOTICE_KEY_END:
		edit->tpos.setCaretEnd();
		break;
	case HSPOBJ_NOTICE_KEY_INS:
		edit->tpos.toggleInsertMode();
		break;
	case HSPOBJ_NOTICE_KEY_BS:
		edit->tpos.deleteStringFromCaret(true);
		update = true;
		break;
	case HSPOBJ_NOTICE_KEY_DEL:
		edit->tpos.deleteStringFromCaret(false);
		update = true;
		break;

	case HSPOBJ_NOTICE_KEY_UP:
	case HSPOBJ_NOTICE_KEY_DOWN:
	case HSPOBJ_NOTICE_KEY_SCROLL_UP:
	case HSPOBJ_NOTICE_KEY_SCROLL_DOWN:
	case HSPOBJ_NOTICE_KEY_SUP:
	case HSPOBJ_NOTICE_KEY_SDOWN:
	case HSPOBJ_NOTICE_KEY_SSCROLL_UP:
	case HSPOBJ_NOTICE_KEY_SSCROLL_DOWN:
	case HSPOBJ_NOTICE_KEY_TAB:
	case HSPOBJ_NOTICE_KEY_CR:
		break;
	case HSPOBJ_NOTICE_KEY_BUFFER:
		edit->tpos.addStringFromCaret((char *)bm->keybuf);
		update = true;
		break;

	default:
		if (wparam & HSPOBJ_NOTICE_KEY_CTRLADD) {
			int code = wparam & (HSPOBJ_NOTICE_KEY_CTRLADD - 1);
			if (Object_InputBoxApplySpecialKey(info, code ) > 0) {
				update = true;
			}
			break;
		}
		if ((wparam < 0x100)&&((wparam >= 32))) {
			unsigned char *buf = bm->keybuf;
			buf[0] = (unsigned char)wparam;
			buf[1] = 0;
			edit->tpos.addStringFromCaret((char *)buf);
			update = true;
		}
		break;
	}

	if (update) {
		char *buf = edit->tpos.getString();
		Object_SendSetVar(info, HSPVAR_FLAG_STR,buf);
	}
}

static void Object_SetInputBox(HSPOBJINFO *info, int type, void *ptr)
{
	Bmscr *bm = (Bmscr *)info->bm;
	Hsp3ObjInput *edit = (Hsp3ObjInput *)info->btnset;
	if (edit == NULL) return;

	char *p = (char *)HspVarCoreCnv(type, TYPE_STRING, ptr);
	edit->tpos.setString(p);
	edit->tpos.setCaret();

	char *buf = edit->tpos.getString();
	Object_SendSetVar(info, HSPVAR_FLAG_STR, buf);
}

static void Object_SetValueName(HSPOBJINFO *info, int type, void *ptr)
{
	Hsp3ObjBase *btn = info->btnset;
	if (btn == NULL) return;

	switch (type) {
	case TYPE_STRING:
		btn->name = (char *)ptr;
		break;
	case TYPE_INUM:
	case TYPE_DNUM:
		btn->name = (char *)HspVarCoreCnv(type, TYPE_STRING, ptr);
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
}

static void Object_SetValue(HSPOBJINFO *info, int type, void *ptr)
{
	Hsp3ObjBase *btn = info->btnset;
	if (btn == NULL) return;

	switch (type) {
	case TYPE_STRING:
		btn->name = (char *)ptr;
		break;
	case TYPE_INUM:
		btn->value = *(int *)ptr;
		bmscr_obj_ival = btn->value;
		Object_SendSetVar(info);
		break;
	case TYPE_DNUM:
		btn->dval = *(HSPREAL *)ptr;
		btn->value = (int)btn->dval;
		bmscr_obj_ival = btn->value;
		Object_SendSetVar(info);
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
}

static void Object_LayerDelete(HSPOBJINFO *info)
{
	info->hspctx->iparam = info->owsize;
	info->hspctx->wparam = info->owid;
	info->hspctx->lparam = HSPOBJ_LAYER_CMD_TERM;
	code_callback((unsigned short *)info->hCld);
}

static void Object_LayerNotice(HSPOBJINFO *info, int wparam)
{
	info->hspctx->iparam = info->exinfo2;
	info->hspctx->wparam = info->owid;
	info->hspctx->lparam = wparam;
	code_callback((unsigned short *)info->hCld);
	info->exinfo2++;
}

static void Object_SetLayerObject(HSPOBJINFO *info, int type, void *ptr)
{
	int ptype = HSPOBJ_LAYER_CMD_PRMI;
	int iparam = 0;
	switch (type) {
	case TYPE_STRING:
		ptype = HSPOBJ_LAYER_CMD_PRMS;
		strncpy(info->hspctx->refstr, (char *)ptr, HSPCTX_REFSTR_MAX - 1);
		break;
	case TYPE_INUM:
		iparam = *(int *)ptr;
		break;
	case TYPE_DNUM:
		ptype = HSPOBJ_LAYER_CMD_PRMD;
		info->hspctx->refdval = *(HSPREAL *)ptr;
		iparam = (int)info->hspctx->refdval;
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
	info->hspctx->iparam = iparam;
	info->hspctx->wparam = info->owid;
	info->hspctx->lparam = ptype;
	code_callback((unsigned short *)info->hCld);
}


/*---------------------------------------------------------------------------*/

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
	obj->varset = NULL;
	obj->exinfo1 = 0;
	obj->exinfo2 = 0;

	HspWnd *wnd = (HspWnd *)this->master_hspwnd;
	if (wnd) {
		obj->hspctx = wnd->GetHSPCTX();
	}

	obj->x = this->cx;
	obj->y = this->cy;
	obj->sx = this->ox;
	obj->sy = this->oy;
	obj->tapflag = 0;
	obj->srcid = -1;

	obj->fontmode = HSPOBJ_FONTMODE_OPAQUE;
	obj->fontedit = 0;
	obj->fontsize = 0;
	obj->fontstyle = 0;
	obj->fontname = NULL;

	if (objmode & 4) {
		obj->fontcolor = objcolor;
		obj->backcolor = color;
	}
	else {
		obj->fontcolor = 0;
		obj->backcolor = 0xe1e1e1;
	}

	if (objmode & 8) {
		obj->fontmode = HSPOBJ_FONTMODE_TRANSPARENT;
	}
	if (objmode & 16) {
		obj->fontmode = HSPOBJ_FONTMODE_NONE;
	}

	SetHSPObjectFont(id);

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
	if (obj->btnset != NULL) {
		delete obj->btnset; obj->btnset = NULL;
	}
	if (obj->varset != NULL) {
		sbFree(obj->varset); obj->varset = NULL;
	}
	if (obj->fontname != NULL) {
		delete obj->fontname; obj->fontname = NULL;
	}
	obj->owmode = HSPOBJ_NONE;
}


void Bmscr::UpdateHSPObject(int id, int type, void *ptr)
{
	//		オブジェクトに値を設定する
	//
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe(id);
	if (obj->owmode == HSPOBJ_NONE) return;

	if (obj->func_objprm != NULL) {
		obj->func_objprm(obj, type, ptr);
	}
	else {
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
}


HSPOBJINFO *Bmscr::AddHSPVarEventObject(int id, int mode, PVal *pval, APTR aptr, int type, void *ptr)
{
	HSPOBJINFO *obj;
	HSP3VARSET *vset;
	obj = AddHSPObject(id, mode);

	vset = (HSP3VARSET *)sbAlloc(sizeof(HSP3VARSET));
	obj->varset = vset;

	vset->pval = pval;
	vset->aptr = aptr;
	vset->type = type;
	vset->ptr = ptr;
	return obj;
}


void Bmscr::SetButtonImage( int id, int bufid, int x1, int y1, int x2, int y2, int x3, int y3 )
{
	HSPOBJINFO *obj;
	Hsp3ObjButton *bset;

	obj = GetHSPObjectSafe( id );

	if ( obj->func_notice != Object_JumpEvent ) throw HSPERR_UNSUPPORTED_FUNCTION;

	obj->srcid = bufid;

	bset = (Hsp3ObjButton *)obj->btnset;
	if ( bset != NULL ) {
		bset->normal_x = x1;
		bset->normal_y = y1;
		bset->push_x = x2;
		bset->push_y = y2;
		bset->focus_x = x3;
		bset->focus_y = y3;
	}
}


int Bmscr::AddHSPObjectButton(char *name, int flag, void *callptr)
{
	//		create push button
	//
	int id;
	HSPOBJINFO *obj;
	Hsp3ObjButton *btn;

	id = NewHSPObject();
	obj = AddHSPObject(id, HSPOBJ_TAB_ENABLE | HSPOBJ_OPTION_SETFONT);

	obj->owid = -1;
	obj->owsize = id;

	btn = new Hsp3ObjButton;
	obj->btnset = btn;

	btn->name = name;
	btn->jumpmode = flag;
	btn->ptr = callptr;

	obj->func_draw = Object_ButtonDraw;
	obj->func_notice = Object_JumpEvent;
	obj->func_objprm = Object_SetValueName;
	//obj->func_delete = Object_WindowDelete;
	Posinc(oy);
	return id;
}


int Bmscr::AddHSPObjectCheckBox(char *name, PVal *pval, APTR aptr)
{
	//		create push button
	//
	int id;
	int *iptr;
	HSPOBJINFO *obj;
	Hsp3ObjBase *btn;

	id = NewHSPObject();
	obj = AddHSPVarEventObject(id, HSPOBJ_TAB_ENABLE | HSPOBJ_OPTION_SETFONT, pval, aptr, TYPE_INUM, (void *)&bmscr_obj_ival);

	btn = new Hsp3ObjButton;
	obj->btnset = btn;
	btn->name = name;

	obj->func_draw = Object_CheckBoxDraw;
	obj->func_notice = Object_CheckBox;
	obj->func_objprm = Object_SetValue;
	//obj->func_delete = Object_WindowDelete;
	Posinc(oy);

	iptr = (int *)HspVarCorePtrAPTR(pval, aptr);
	if (*iptr) btn->value = 1;

	return id;
}


int Bmscr::AddHSPObjectInput(PVal *pval, APTR aptr, int sizex, int sizey, char *defval, int limit, int mode)
{
	//		create input box
	//
	int id, max, tabstop, type;
	HSPOBJINFO *obj;
	Hsp3ObjInput *btn;

	id = NewHSPObject();
	max = 16;
	tabstop = HSPOBJ_TAB_ENABLE;
	type = mode & 15;
	if (type == HSPOBJ_INPUT_STR) max = limit;

//	if (mode & HSPOBJ_INPUT_HSCROLL) ws |= WS_HSCROLL;
	if (mode & HSPOBJ_INPUT_MULTILINE) {
		tabstop = HSPOBJ_TAB_DISABLE;
		//tabstop |= HSPOBJ_TAB_SKIP;
		max = 0;
	}
	else {
		tabstop |= HSPOBJ_TAB_SELALLTEXT;
	}

	if (mode & HSPOBJ_INPUT_READONLY) {
		tabstop = HSPOBJ_TAB_SKIP;
	}

	obj = AddHSPVarEventObject(id, tabstop | HSPOBJ_OPTION_SETFONT | HSPOBJ_OPTION_EDITSEL,
		pval, aptr, type, (void *)&bmscr_obj_ival);

	btn = new Hsp3ObjInput;
	obj->btnset = btn;

	//btn->name = defval;
	btn->tpos.setMaxLength(max);
	btn->tpos.setString(defval);
	btn->tpos.setCaret();
	btn->tpos.mode = TEXMES_MODE_CENTERY;

	btn->sel_start = 0;
	btn->sel_end = 0;
	btn->tpos.setSelection( &btn->sel_start );

	obj->func_draw = Object_InputBoxDraw;
	obj->func_notice = Object_InputBox;
	obj->func_objprm = Object_SetInputBox;
	//obj->func_delete = Object_WindowDelete;

	Posinc(sizey);
	return id;
}


int Bmscr::AddHSPObjectLayer(int sizex, int sizey, int layer, int val, int mode, void *callptr)
{
	//		create screen layer object
	//
	int id, lay;
	HSPOBJINFO *obj;

	lay = layer & 0xff;
	if (lay < HSPOBJ_OPTION_LAYER_MIN) lay = HSPOBJ_OPTION_LAYER_MIN;
	if (lay > HSPOBJ_OPTION_LAYER_MAX) lay = HSPOBJ_OPTION_LAYER_MAX;

	if (( layer & HSPOBJ_OPTION_LAYER_MULTI ) == 0) {		// 重複登録を検出する
		obj = mem_obj;
		for (int i = 0; i < objmax; i++) {
			if (obj->owmode != HSPOBJ_NONE) {
				if (obj->owmode & HSPOBJ_OPTION_LAYEROBJ) {
					if (obj->hCld == callptr) return -1;
				}
			}
			obj++;
		}
	}

	id = NewHSPObject();

	obj = AddHSPObject(id, HSPOBJ_OPTION_LAYEROBJ | HSPOBJ_TAB_SKIP);
	obj->owid = id;
	obj->owsize = this->wid;

	obj->enableflag = lay;
	obj->exinfo1 = val;
	obj->exinfo2 = 0;

	obj->x = cx;
	obj->y = cy;
	obj->sx = sizex;
	obj->sy = sizey;
	obj->hCld = callptr;

	obj->func_notice = Object_LayerNotice;
	obj->func_delete = Object_LayerDelete;
	obj->func_objprm = Object_SetLayerObject;
	Posinc(sizey);

	obj->hspctx->iparam = val;
	obj->hspctx->wparam = id;
	obj->hspctx->lparam = HSPOBJ_LAYER_CMD_INIT;
	code_callback((unsigned short *)callptr);

	return id;
}



//-------------------------------------------------------
//	Object Draw Service
//-------------------------------------------------------

void Bmscr::ResetHSPObject(void)
{
	//		すべてのObjectをリセットする
	//
	int i;
	if (mem_obj != NULL) {
		for (i = 0; i < objmax; i++) {
			DeleteHSPObject(i);
		}
		sbFree(mem_obj);
	}
	mem_obj = NULL;
	objmax = 0;
	objlimit = HSPOBJ_LIMIT_DEFAULT;

	tapstat = 0;
	tapinvalid = 0;
	cur_obj = NULL;
	cur_objid = -1;
	cur_mo_obj = NULL;
}


int Bmscr::ActivateHSPObject(int id)
{
	//		フォーカスをONにする
	//
	HSPOBJINFO *obj;

	if (id < 0) {
		return cur_objid;
	}
	if (id >= objmax) {
		cur_objid = -1;
		return -1;
	}
	obj = GetHSPObject(id);
	if (obj->owmode == HSPOBJ_NONE) {
		cur_objid = -1;
		return -2;
	}
	cur_objid = id;
	SelectEditHSPObject();
	return 0;
}


void Bmscr::EnableObject(int id, int sw)
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe(id);
	if (obj->owmode == HSPOBJ_NONE) throw HSPERR_ILLEGAL_FUNCTION;
	obj->enableflag = sw;
	//	EnableWindow( obj->hCld, sw!=0 );
}


void Bmscr::SetObjectMode(int id, int owmode)
{
	HSPOBJINFO *obj;
	obj = GetHSPObjectSafe(id);
	if (obj->owmode == HSPOBJ_NONE) throw HSPERR_ILLEGAL_FUNCTION;
	if (owmode <= 0) throw HSPERR_ILLEGAL_FUNCTION;
	obj->owmode = owmode;
}


void Bmscr::SetHSPObjectFont(int id)
{
	int a;
	HSPOBJINFO *obj;

	obj = GetHSPObjectSafe(id);
	if (obj->owmode == HSPOBJ_NONE) return;

	a = objmode & 3;
	switch (a) {
	case 0:
	default:
		obj->fontsize = 18;
		break;
	case 1:
		obj->fontmode |= HSPOBJ_FONTMODE_GUIFONT;
		obj->fontsize = 12;
		break;
	case 2:
		obj->fontsize = font_cursize;
		obj->fontstyle = font_curstyle;
		obj->owmode |= HSPOBJ_OPTION_SETFONT;
		obj->fontname = new std::string(font_curname);
		break;
	}
}


int Bmscr::DrawAllObjects( void )
{

	//		DishSystemObjectを描画する
	//
	int i;
	int bak_mulcolor;
	HSPOBJINFO *info;

	if (this->objmax == 0) return 0;

	info = this->mem_obj;
	if ( info == NULL ) return -1;

	bak_mulcolor = this->mulcolor;
	SetMulcolor( 255,255,255 );				// 乗算カラーを標準に戻す

	for( i=0;i<this->objmax;i++ ) {
		if ( info->owmode != HSPOBJ_NONE ) {
			if (info->func_draw != NULL) {
				bmscr_obj_drawflag = (i==cur_objid);
				if (window_active == 0) bmscr_obj_drawflag = false;
				info->func_draw(info);
			}
		}
		info++;
	}

	SetMulcolor( (bak_mulcolor>>16)&0xff, (bak_mulcolor>>8)&0xff, (bak_mulcolor)&0xff );				// 乗算カラーを元に戻す
	return 0;
}


void Bmscr::SendHSPObjectNotice(int wparam)
{
	//		DishSystemObjectに通知コード(HSPOBJ_NOTICE_*)を送る
	//
	HSPOBJINFO *info;

	if (this->objmax == 0) return;
	if (this->cur_objid < 0) return;

	info = GetHSPObject(this->cur_objid);
	if (info == NULL) return;
	if (info->owmode != HSPOBJ_NONE) {
		if ((info->owmode & HSPOBJ_OPTION_LAYEROBJ) == 0) {
			if (info->func_notice != NULL) {
				info->func_notice(info, wparam);
			}
		}
	}
}


void Bmscr::SendHSPLayerObjectNotice(int layer, int cmd)
{
	//		レイヤーオブジェクトの通知処理
	//
	int i;
	HSPOBJINFO *obj;
	obj = mem_obj;
	for (i = 0; i < objmax; i++) {
		if (obj->owmode) {
			if (obj->owmode & HSPOBJ_OPTION_LAYEROBJ) {
				if (obj->enableflag == layer) {
					if (obj->func_notice != NULL) {
						obj->func_notice(obj, cmd);
					}
				}
			}
		}
		obj++;
	}
}


void Bmscr::SelectEditHSPObject(void)
{
	//		オブジェクトの文字列選択をリセットする
	//		(新規の選択時に必ず呼ぶこと)
	int i;
	HSPOBJINFO *info;
	info = this->mem_obj;
	if (info == NULL) return;

	for (i = 0; i < this->objmax; i++) {
		if (info->owmode != HSPOBJ_NONE) {
			if (info->owmode & HSPOBJ_OPTION_EDITSEL) {
				Hsp3ObjInput *edit = (Hsp3ObjInput *)info->btnset;
				if (edit) { edit->sel_start = 0; edit->sel_end = 0; }
			}
		}
		info++;
	}
}


int Bmscr::UpdateAllObjects( void )
{
	//		DishSystemObjectを更新する
	//
	int i,x,y,msx,msy,tap;
	HSPOBJINFO *info;
	HSPOBJINFO *focus;
	int focustap;
	int focusid;
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
				if ((this->tapobj_posex != x) || (this->tapobj_posey != y)) {
					this->tapobj_posex = x;
					this->tapobj_posey = y;
					if (info->func_notice != NULL) {
						info->func_notice(info, HSPOBJ_NOTICE_CLICK_MOVE);
					}
				}
			}
		}
		info->tapflag = tap;
		if ( this->tapstat == 0 ) {
			this->cur_obj = NULL;
			if ( tap >= 0 ) {
				if (info->func_notice != NULL) {
					info->func_notice(info, HSPOBJ_NOTICE_CLICK_END);
				}
			}
		}
		return -1;
	}

	//		まだ何も押していない状態
	//
	focus = NULL;
	focusid = -1;
	focustap = 0;
	this->cur_mo_obj = NULL;
	for( i=0;i<this->objmax;i++ ) {
		if (info->owmode != HSPOBJ_NONE) {
			if ((info->owmode & HSPOBJ_OPTION_LAYEROBJ) == 0) {
				tap = -1;
				y = msy - info->y;
				if ((y >= 0) && (y < info->sy)) {
					x = msx - info->x;
					if ((x >= 0) && (x < info->sx)) {
						tap = this->tapstat;
						focus = info;
						focustap = tap;
						focusid = i;
						this->cur_mo_obj = info;
						this->tapobj_posx = x;
						this->tapobj_posy = y;
						this->tapobj_posex = x;
						this->tapobj_posey = y;
					}
				}
			}
		}
		info++;
	}

	//Alertf( "[BtnPush][%x,%d]%d,%d,%d",focus,msx,msy,tapstat );

	//		押したボタンの判定
	//
	if ( this->tapstat == 1 ) {
		if ( focus != NULL ) {
			this->tapinvalid = 0;
			this->cur_obj = focus;
			cur_objid = focusid;
			focus->tapflag = focustap;
			if (focus->func_notice != NULL) {
				focus->func_notice(focus, HSPOBJ_NOTICE_CLICK);
			}
		} else {
			this->tapinvalid = 1;
		}
	}

	return -1;
}


