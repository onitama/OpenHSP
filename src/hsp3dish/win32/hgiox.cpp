//
//		Draw lib (directX8+TEXMES)
//			onion software/onitama 2001/6
//			               onitama 2011/5
//
#include <stdio.h>
#define STRICT
#include <windows.h>
#include <math.h>
#include <d3d8.h>
#include <d3dx8.h>

#include "../hgio.h"
#include "../supio.h"
#include "../sysreq.h"

#include "hgtex.h"
#include "filedlg.h"

#include "dx11_renderer.h"

#include "../texmes.h"
#include "../emscripten/fontsystem.h"
void hgio_fontsystem_win32_init(HWND wnd);

/*------------------------------------------------------------*/
/*
		HSP File Service
*/
/*------------------------------------------------------------*/

#define MFPTR_MAX 8
static char *mfptr[MFPTR_MAX];
static int mfptr_depth;

void InitMemFile( void )
{
	mfptr_depth = 0;
	mfptr[0] = NULL;
}


int OpenMemFilePtr( char *fname )
{
	int fsize;
	fsize = (int)dpm_exist( fname );		// ファイルのサイズを取得
	if ( fsize <= 0 ) {
		return -1;
	}
	mfptr_depth++;
	if ( mfptr_depth >= MFPTR_MAX ) return -1;
	mfptr[mfptr_depth] = (char *)malloc( fsize );
	dpm_read( fname, mfptr[mfptr_depth], fsize, 0 );	// ファイル読み込み
	return fsize;
}


char *GetMemFilePtr( void )
{
	return mfptr[mfptr_depth];
}


void CloseMemFilePtr( void )
{
	if ( mfptr_depth == 0 ) return;
	if ( mfptr[mfptr_depth] != NULL ) {
		free( mfptr[mfptr_depth] ); mfptr[mfptr_depth]=NULL;
		mfptr_depth--;
	}
}

/*------------------------------------------------------------*/
/*
		DirectX11 Service
*/
/*------------------------------------------------------------*/

//		Settings
//
HGIO_DX11_RENDERER *render = NULL;

static		char *lpDest;		// 描画画面のバッファ
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ

static		HWND master_wnd;	// 表示対象Window
static		int drawflag;		// レンダー開始フラグ
static		bool need_vsync;	// VSync有効フラグ

static		texmesManager tmes;	// テキストメッセージマネージャー

static		BMSCR *mainbm;		// メインスクリーンのBMSCR
static		char m_tfont[256];	// テキスト使用フォント
static		int m_tsize;		// テキスト使用フォントのサイズ
static		int m_tstyle;		// テキスト使用フォントのスタイル指定
static		float center_x, center_y;

static		int lineColor = 0;
static		float lineBaseX = 0.0f;
static		float lineBaseY = 0.0f;

static		BMSCR *backbm;		// 背景消去用のBMSCR(null=NC)

static		HSPREAL infoval[GINFO_EXINFO_MAX];

static		MATRIX mat_proj;	// プロジェクションマトリクス
static		MATRIX mat_unproj;	// プロジェクション逆変換マトリクス

static		HCURSOR cursor_arrow;	// 通常カーソル
static		HCURSOR cursor_ibeam;	// テキストエリア用カーソル

#define CIRCLE_DIV 32
#define DEFAULT_FONT_NAME ""
#define DEFAULT_FONT_SIZE 18
#define DEFAULT_FONT_STYLE 0


static int Get2N(int val)
{
	int res = 1;
	while (1) {
		if (res >= val) break;
		res <<= 1;
	}
	return res;
}

int GetSurface( int x, int y, int sx, int sy, int px, int py, void *res, int mode )
{
	//	VRAMの情報を取得する
	//
	return 0;
}


static void ClearDest( int mode, int color, int tex )
{
	switch ( mode ) {
	case CLSMODE_NONE:
		//消去(Zバッファのみ)
		//d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		break;
	case CLSMODE_SOLID:
		//塗りつぶして消去
		//d3ddev->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,color,1.0f,0);	// T&L使用時
		break;
	case CLSMODE_TEXTURE:
		{
		//テクスチャで消去
		}
	case CLSMODE_BLUR:
		{
		//blur clear
		break;
		}
	}
}


static void InitTexture(void)
{
	//		テクスチャ情報初期化
	//
	TexSetParam(render);
	TexInit();


	//		テキストを初期化
	//
	tmes.texmesInit(SYSREQ_MESCACHE_MAX);
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgio_init( int mode, int sx, int sy, void *hwnd )
{
	//		ファイルサービス設定
	//
	InitMemFile();

	//		設定の初期化
	//
	GeometryInit();
	SetSysReq( SYSREQ_RESULT, 0 );
	SetSysReq( SYSREQ_RESVMODE, 0 );

	master_wnd = (HWND)hwnd;
	mainbm = NULL;
	backbm = NULL;
	drawflag = 0;
	nDestWidth = sx;
	nDestHeight = sy;
	need_vsync = false;

	hgio_fontsystem_win32_init(master_wnd);

	//		バッファ初期化
	//
	lpDest = NULL;

	//	Direct3Dオブジェクトの取得
	//
	render = new HGIO_DX11_RENDERER;
	if(render == NULL ) {
		SetSysReq( SYSREQ_RESULT, 1 );
		return;
	}

	// DX11デバイスとスワップチェーンを作成する。
	if (!render->Create(master_wnd, sx, sy,
		GetSysReq(SYSREQ_DXMODE) != 0, GetSysReq(SYSREQ_DXVSYNC) == 0)) {
		SetSysReq(SYSREQ_RESULT, 1);
		return;
	}

	//		デバイス初期化
	//
	InitTexture();

	//		infovalをリセット
	//
	int i;
	for(i=0;i<GINFO_EXINFO_MAX;i++) {
		infoval[i] = 0.0;
	}

	//		カーソル読み込み
	//
	cursor_arrow = LoadCursor(NULL, IDC_ARROW);
	cursor_ibeam = LoadCursor(NULL, IDC_IBEAM);
}


void hgio_clsmode( int mode, int color, int tex )
{
	SetSysReq( SYSREQ_CLSMODE, mode );
	SetSysReq( SYSREQ_CLSCOLOR, color );
	SetSysReq( SYSREQ_CLSTEX, tex );
}


void hgio_resize_window(int x, int y)
{
	if (render == NULL) return;
	if (render->Resize(x, y)) { nDestWidth = x; nDestHeight = y; }
}


int hgio_device_ready(void)
{
	return 0;
}


int hgio_device_restore( void )
{
	//	デバイスの修復
	//		(0=OK/1=NG)
	//
	return 0;
}


void hgio_resume( void )
{
	hgio_device_restore();
}


int hgio_render_end( void )
{
	HRESULT hr;
	int res;

	if ( drawflag == 0 ) return 0;

	res = 0;

	//シーンレンダー終了
	hr = render->EndFrame(GetSysReq(SYSREQ_DXVSYNC) == 0);
	if (FAILED(hr)) {
		res = -1;
	}
	SetSysReq(SYSREQ_DEVLOST, res);
	drawflag = 0;

	if (res == 0) {
		tmes.texmesProc();
	}
	return res;
}


int hgio_render_start( void )
{
	//static D3DXMATRIX InvViewport;
	if ( drawflag ) {
		hgio_render_end();
	}

	//	標準の投影マトリクスを設定する
	UnitMatrix();
	OrthoMatrix(0.0f, 0.0f, static_cast<float>(nDestWidth), static_cast<float>(nDestHeight), 0.0f, 1.0f);
	GetCurrentMatrix(&mat_proj);
	render->SetViewMatrix(&mat_proj);

	hgio_setfilter(0, 0);

	// DX11フレームを開始する。テクスチャ／ブラー背景は従来のClearDestで続けて描く。
	render->BeginFrame(GetSysReq(SYSREQ_CLSCOLOR)|0xff000000,
		GetSysReq(SYSREQ_CLSMODE) != CLSMODE_NONE);

	//render->BeginFrame(0xff202838, true);
	render->SetBlend(HGIO_BLEND_COPY, false);

	//	画面クリア
	int bgtex = -1;
	if (backbm != NULL) { bgtex = backbm->texid; }
	//ClearDest(GetSysReq(SYSREQ_CLSMODE), GetSysReq(SYSREQ_CLSCOLOR), bgtex);

	//	ユーザー設定の投影マトリクスを設定する
	if (mainbm) hgio_setview(mainbm);
	//render->SetViewMatrix(&mymat);

	//シーンレンダー開始
	TexReset();
	drawflag = 1;

	return 0;
}


void hgio_setback( BMSCR *bm )
{
	//		背景画像の設定
	//		(NULL=なし)
	//
	backbm = bm;
}


void hgio_screen( BMSCR *bm )
{
	//		スクリーン再設定
	//		(cls相当)
	//
	drawflag = 0;
	if (bm->type == HSPWND_TYPE_MAIN) {
		mainbm = bm;
	}
	hgio_font( DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE );
}


void hgio_delscreen( BMSCR *bm )
{
	//		スクリーンを破棄
	//		(Bmscrクラスのdelete時)
	//
	if ( bm->flag == BMSCR_FLAG_NOUSE ) return;
	if ( bm->texid != -1 ) {
		DeleteTex( bm->texid );
		bm->texid = -1;
	}
}


int hgio_getWidth( void )
{
	return nDestWidth;
}


int hgio_getHeight( void )
{
	return nDestHeight;
}


void hgio_term( void )
{
	hgio_render_end();
	tmes.texmesTerm();
	TexTerm();
	render->Destroy();
	GeometryTerm();
}


int hgio_stick( int actsw )
{
	//		stick用の入力を返す
	//
	HWND hwnd;
	int ckey = 0;

	if ( actsw ) {
		hwnd = GetActiveWindow();
		if ( hwnd != master_wnd ) return 0;
	}

	if ( GetAsyncKeyState(37)&0x8000 ) ckey|=1;		// [left]
	if ( GetAsyncKeyState(38)&0x8000 ) ckey|=2;		// [up]
	if ( GetAsyncKeyState(39)&0x8000 ) ckey|=4;		// [right]
	if ( GetAsyncKeyState(40)&0x8000 ) ckey|=8;		// [down]
	if ( GetAsyncKeyState(32)&0x8000 ) ckey|=16;	// [spc]
	if ( GetAsyncKeyState(13)&0x8000 ) ckey|=32;	// [ent]
	if ( GetAsyncKeyState(17)&0x8000 ) ckey|=64;	// [ctrl]
	if ( GetAsyncKeyState(27)&0x8000 ) ckey|=128;	// [esc]
	if ( GetAsyncKeyState(1)&0x8000 )  ckey|=256;	// mouse_l
	if ( GetAsyncKeyState(2)&0x8000 )  ckey|=512;	// mouse_r
	if ( GetAsyncKeyState(9)&0x8000 )  ckey|=1024;	// [tab]

	if (GetAsyncKeyState(90) & 0x8000)  ckey |= 1<<11;	// [z]
	if (GetAsyncKeyState(88) & 0x8000)  ckey |= 1<<12;	// [x]
	if (GetAsyncKeyState(67) & 0x8000)  ckey |= 1<<13;	// [c]

	if (GetAsyncKeyState(65) & 0x8000)  ckey |= 1 << 14;	// [a]
	if (GetAsyncKeyState(87) & 0x8000)  ckey |= 1 << 15;	// [w]
	if (GetAsyncKeyState(68) & 0x8000)  ckey |= 1 << 16;	// [d]
	if (GetAsyncKeyState(83) & 0x8000)  ckey |= 1 << 17;	// [s]

	return ckey;
}


int hgio_redraw( BMSCR *bm, int flag )
{
	//		redrawモード設定
	//		(必ずredraw 0～redraw 1をペアにすること)
	//
	if ( bm == NULL ) return -1;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;

	if ( flag & 1 ) {
		hgio_render_end();
		int curtick = hgio_gettick();
		if (bm->prevtime) {
			bm->passed_time = curtick - bm->prevtime;
		}
		bm->prevtime = curtick;
		return 0;
	} else {
		hgio_render_start();
	}

	//	ウインドウアクティブの更新
	//
	HWND hwnd;
	hwnd = GetActiveWindow();
	if (hwnd != master_wnd) {
		bm->window_active = 0;
	}
	else {
		bm->window_active = 1;
	}

	//	カーソルの更新
	//
	HSPOBJINFO *info = bm->cur_mo_obj;
	HCURSOR hc = cursor_arrow;

	if (info) {
		if (info->owmode & (HSPOBJ_OPTION_EDITSEL| HSPOBJ_OPTION_MULTISEL)) {
			hc = cursor_ibeam;
		}
	}
	SetCursor(hc);
	SetClassLongPtr(hwnd, -12, (LONG_PTR)hc);

	return 0;
}

int hgio_dialog_ex(HSPCTX* ctx, Bmscr* bmscr, int mode, char* str1, char* str2)
{
	HWND hwnd;
	int i, res;
	i = 0;

	hwnd = master_wnd;

	if (mode >= 64) {
		return 0;
	}
	if (mode & 16) {
		res = fd_dialog(hwnd, mode & 3, str1, str2);
		if (res == 0) {
			ctx->refstr[0] = 0;
		}
		else {
			strncpy(ctx->refstr, fd_getfname(), HSPCTX_REFSTR_MAX - 1);
		}
		return res;
	}
	if (mode & 32) {
		i = (int)fd_selcolor(hwnd, mode & 1);
		if (i == -1) return 0;
		bmscr->Setcolor2( i );
		return 1;
	}
	return hgio_dialog(mode,str1,str2);
}

int hgio_dialog( int mode, char *str1, char *str2 )
{
	//		dialog表示
	//
	int res;
	int i;
	int p1, p2;
	char* ptr;
	char* ps;
	char stmp[0x4000];
	HSPAPICHAR* hactmp1 = 0;
	HSPAPICHAR* hactmp2 = 0;
	ptr = str1;
	strncpy(stmp, ptr, 0x4000 - 1);
	p1 = mode;
	ps = str2;
	p2 = 0;
	res = 0;

	if (p1 >= 64) {
		return res;
	}
	if (p1 & 16) {
		res = fd_dialog(master_wnd, p1 & 3, stmp, ps);
		if (res == 0) {
			//ctx->refstr[0] = 0;
		}
		else {
			//strncpy(ctx->refstr, fd_getfname(), HSPCTX_REFSTR_MAX - 1);
		}
	}
	else if (p1 & 32) {
		i = (int)fd_selcolor(master_wnd, p1 & 1);
		if (i == -1) res = 0;
		else {
			mainbm->color = i;
			res = 1;
		}
	}
	else {
		i = 0;
		if (p1 & 1) i |= MB_ICONEXCLAMATION; else i |= MB_ICONINFORMATION;
		if (p1 & 2) i |= MB_YESNO; else i |= MB_OK;
		res = MessageBox(master_wnd,
			chartoapichar(stmp, &hactmp1), chartoapichar(ps, &hactmp2), i);
		freehac(&hactmp1);
		freehac(&hactmp2);
	}
	return res;
}


int hgio_title( char *str1 )
{
	//		title変更
	//
	HSPAPICHAR* hactmp1 = 0;
	SetWindowText(master_wnd, chartoapichar(str1, &hactmp1));
	freehac(&hactmp1);
	return 0;
}


int hgio_texload( BMSCR *bm, char *fname )
{
	//		テクスチャ読み込み
	//
	int i,fsize;
	TEXINF *tex;

	fsize = OpenMemFilePtr( fname );				// HSPリソースを含めて検索する
	i = RegistTex( GetMemFilePtr(), fsize );
	CloseMemFilePtr();
	if ( i < 0 ) return i;

	tex = GetTex( i );
	bm->sx = tex->width;
	bm->sy = tex->height;
	bm->texid = i;

	return 0;
}


char* hgio_texmaskbuffer(BMSCR* bm, char* resname)
{
	//		マスクバッファ作成
	//
	char* p;
	int fsize, xsize, ysize;
	fsize = OpenMemFilePtr(resname);				// HSPリソースを含めて検索する
	p = GetPixelMaskBuffer(GetMemFilePtr(), fsize, &xsize, &ysize);
	CloseMemFilePtr();
	if (p) {
		if ((xsize == bm->sx) || (ysize == bm->sy)) {
			return p;
		}
		free(p);
	}
	return NULL;
}


int hgio_gsel( BMSCR *bm )
{
	//		gsel(描画先変更)
	//
	hgio_render_end();
	return 0;
}


int hgio_buffer(BMSCR* bm)
{
	//		buffer(描画用画面作成)
	//
	int texid = RegistTexEmpty(bm->sx, bm->sy, 1);
	if (texid >= 0) {
		bm->texid = texid;
	}
	return 0;
}


int hgio_bufferop(BMSCR* bm, int mode, char *ptr)
{
	//		オフスクリーンバッファを操作
	//
	int texid = bm->texid;
	if (texid < 0) return -1;

	if (mode & 0x1000) {
		return UpdateTexStar(texid, mode & 0xfff);
	}

	switch (mode) {
	case 0:
	case 1:
		return UpdateTex32(texid, ptr, mode);
	case 16:
	case 17:
		GetSurface(0, 0, bm->sx, bm->sy, 1, 1, ptr, mode & 15);
		return 0;
	default:
		return -2;
	}
	return 0;
}


/*------------------------------------------------------------*/
/*
		Polygon Draw Routines
*/
/*------------------------------------------------------------*/

// Converts the HSP packed color to the renderer vertex color format.
static unsigned int ColorWithAlpha(int color, int alpha)
{
	return (static_cast<unsigned int>(alpha & 255) << 24) |
		(static_cast<unsigned int>(color) & 0x00ffffff);
}

static void DrawLine(float x0, float y0, float x1, float y1, unsigned int color)
{
	const HGIO_DX11_VERTEX v[] = {
		{ x0, y0, 0.0f, color, 0.0f, 0.0f },
		{ x1, y1, 0.0f, color, 0.0f, 0.0f },
	};
	render->Draw(HGIO_TOPOLOGY_LINE_LIST, v, ARRAYSIZE(v), false);
}

static void DrawQuad(const HGIO_DX11_VERTEX* vertices, bool textured)
{
	render->Draw(HGIO_TOPOLOGY_TRIANGLE_FAN, vertices, 4, textured);
}

static void SetAlphaMode(int mode)
{
	switch (mode) {
	case 0:
		render->SetBlend(HGIO_BLEND_COPY, false);
		break;
	case 5: render->SetBlend(HGIO_BLEND_ADD, true); break;
	case 6: render->SetBlend(HGIO_BLEND_SUB, true); break;
	case 7: render->SetBlend(HGIO_BLEND_INVSRCALPHA, true); break;
	default:
		render->SetBlend(HGIO_BLEND_ALPHA, true);
		break;
	}
}

static int CopyAlpha(BMSCR* bm, bool forceTextureAlpha)
{
	int mode = bm->gmode;
	if (forceTextureAlpha) {
		if (mode < 3) mode = 3;
		SetAlphaMode(mode);
	} else {
		SetAlphaMode(mode);
	}
	return mode < 3 ? 255 : (bm->gfrate > 255 ? 255 : bm->gfrate);
}

static void DrawTexturedQuad(BMSCR* bm, int texid, float x0, float y0, float x1, float y1,
	float u0, float v0, float u1, float v1, int alpha, int rgb)
{
	ChangeTex(texid);
	const unsigned int color = ColorWithAlpha(rgb, alpha);
	const HGIO_DX11_VERTEX v[] = {
		{ x0, y0, 0.0f, color, u0, v0 },
		{ x1, y0, 0.0f, color, u1, v0 },
		{ x1, y1, 0.0f, color, u1, v1 },
		{ x0, y1, 0.0f, color, u0, v1 },
	};
	DrawQuad(v, true);
}

/*------------------------------------------------------------*/
/*
		Universal Draw Service
*/
/*------------------------------------------------------------*/

void hgio_line( BMSCR *bm, float x, float y )
{
	//		ライン描画
	//		(bm!=NULL の場合、ライン描画開始)
	//		(bm==NULL の場合、ライン描画完了)
	//		(ラインの座標は必要な数だけhgio_line2を呼び出す)
	//
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex(-1); SetAlphaMode(0);
	lineBaseX = x; lineBaseY = y; lineColor = bm->color;
}


void hgio_line2( float x, float y )
{
	//		ライン描画
	//		(hgio_lineで開始後に必要な回数呼ぶ、hgio_line(NULL)で終了すること)
	//
	DrawLine(lineBaseX, lineBaseY, x, y, ColorWithAlpha(lineColor, 255));
	lineBaseX = x; lineBaseY = y;
}


void hgio_boxfAlpha(BMSCR *bm, float x0, float y0, float x1, float y1, int alphamode)
{
	//		矩形描画
	//
	unsigned int color;

	if (bm == NULL) return;
	if (bm->type != HSPWND_TYPE_MAIN) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex(-1);
	if (alphamode) {
		int alpha = alphamode ? CopyAlpha(bm, false) : 255;
		color = ColorWithAlpha(bm->color, alpha);
	}
	else {
		SetAlphaMode(0);
		color = ColorWithAlpha(bm->color, 255);
	}

	// Opaque blue rectangle.
	const HGIO_DX11_VERTEX Rect[] = {
		{ x0, y0, 0.0f, color, 0.0f, 0.0f },
		{ x1, y0, 0.0f, color, 0.0f, 0.0f },
		{ x1, y1, 0.0f, color, 0.0f, 0.0f },
		{ x0, y1, 0.0f, color, 0.0f, 0.0f },
	};
	DrawQuad(Rect, false);
}


void hgio_boxf( BMSCR *bm, float x1, float y1, float x2, float y2 )
{
	hgio_boxfAlpha(bm, x1, y1, x2, y2, 0);
}


void hgio_circle(BMSCR* bm, float x0, float y0, float x1, float y1, int mode)
{
	//		円描画
	//
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	const float cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f, rx = fabsf(x1 - x0) * 0.5f, ry = fabsf(y1 - y0) * 0.5f;
	const float step = 6.283185307179586f / CIRCLE_DIV;
	if (mode == 0) {
		for (int i = 0; i <= CIRCLE_DIV; ++i) {
			const float x = cx + cosf(i * step) * rx, y = cy + sinf(i * step) * ry;
			if (i) DrawLine(lineBaseX, lineBaseY, x, y, ColorWithAlpha(bm->color, 255));
			lineBaseX = x; lineBaseY = y;
		}
		return;
	}
	HGIO_DX11_VERTEX v[CIRCLE_DIV + 2];
	for (int i = 0; i <= CIRCLE_DIV; ++i) v[i] = { cx + cosf(i * step) * rx,cy + sinf(i * step) * ry,0,ColorWithAlpha(bm->color,255),0,0 };
	render->Draw(HGIO_TOPOLOGY_TRIANGLE_FAN, v, CIRCLE_DIV + 1, false);
}


void hgio_fillrot( BMSCR *bm, float x, float y, float sx, float sy, float ang )
{
	//		矩形(回転)描画
	//
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex(-1);
	const float sinAngle = sinf(ang);
	const float cosAngle = cosf(ang);
	const float halfX = sx * 0.5f;
	const float halfY = sy * 0.5f;
	const float axisXx = cosAngle * halfX;
	const float axisXy = sinAngle * halfX;
	const float axisYx = -sinAngle * halfY;
	const float axisYy = cosAngle * halfY;
	const unsigned int color = ColorWithAlpha(bm->color, CopyAlpha(bm, false));

	const HGIO_DX11_VERTEX v[] = {
		{ x - axisXx - axisYx, y - axisXy - axisYy, 0.0f, color, 0.0f, 0.0f },
		{ x + axisXx - axisYx, y + axisXy - axisYy, 0.0f, color, 0.0f, 0.0f },
		{ x + axisXx + axisYx, y + axisXy + axisYy, 0.0f, color, 0.0f, 0.0f },
		{ x - axisXx + axisYx, y - axisXy + axisYy, 0.0f, color, 0.0f, 0.0f },
	};
	DrawQuad(v, false);
}


void hgio_copy(BMSCR* bm, short xx, short yy, short sx, short sy, BMSCR* bmsrc, float scaleX, float scaleY)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	TEXINF *tex;
	int texid;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) {
		//Alertf( "type%d #%d",bm->type,bm->wid );
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	if (drawflag == 0) hgio_render_start();

	texid = bmsrc->texid;
	tex = GetTex(texid); if (!tex) return;

	const float u0 = xx * tex->ratex + tex->ratehx, v0 = yy * tex->ratey + tex->ratehy;
	const float u1 = (xx + sx) * tex->ratex, v1 = (yy + sy) * tex->ratey;
	float fx = ((float)bm->cx) + 0.5f;
	float fy = ((float)bm->cy) + 0.5f;
	DrawTexturedQuad(bm, bmsrc->texid, fx, fy, fx + scaleX, fy + scaleY,
		scaleX < 0 ? u1 : u0, scaleY < 0 ? v1 : v0, scaleX < 0 ? u0 : u1, scaleY < 0 ? v0 : v1,
		CopyAlpha(bm, false), bm->mulcolor);

}


void hgio_fontcopy(BMSCR *bm, int x, int y, int psx, int psy, int texid, int basex, int basey)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	if (bm == NULL) return;
	if (bm->type != HSPWND_TYPE_MAIN) {
		//Alertf( "type%d #%d",bm->type,bm->wid );
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	if (drawflag == 0) hgio_render_start();

	TEXINF* tex = GetTex(texid);
	if (!tex) return;
	ChangeTex(texid);

	HGIO_DX11_VERTEX vertices[4];
	HGIO_DX11_VERTEX* v;
	float x1, y1, x2, y2, sx, sy;
	float tx0, ty0, tx1, ty1;

	tx0 = ((float)(basex));
	tx1 = ((float)(psx));
	ty0 = ((float)(basey));
	ty1 = ((float)(psy));

	x1 = ((float)x)+0.5f;
	y1 = ((float)y)+0.5f;
	x2 = x1 + psx;
	y2 = y1 + psy;

	sx = tex->ratex;
	sy = tex->ratey;

	tx0 *= sx;
	tx1 *= sx;
	ty0 *= sy;
	ty1 *= sy;
	tx0 += tex->ratehx;
	ty0 += tex->ratehy;

	int col;
	if (GetSysReq(SYSREQ_FIXMESALPHA)) {
		SetAlphaMode(2);
		col = ColorWithAlpha(bm->color, 255);
	}
	else {
		const int alpha = CopyAlpha(bm, true);
		col = ColorWithAlpha(bm->color, alpha);
	}
	v = &vertices[0];
	v[0].color = v[1].color = v[2].color = v[3].color = col;

	v[0].x = x1;
	v[0].y = y1;
	v[0].z = 0.0f;
	v[0].u = tx0;
	v[0].v = ty0;

	v[1].x = x2;
	v[1].y = y1;
	v[1].z = 0.0f;
	v[1].u = tx1;
	v[1].v = ty0;

	v[2].x = x2;
	v[2].y = y2;
	v[2].z = 0.0f;
	v[2].u = tx1;
	v[2].v = ty1;

	v[3].x = x1;
	v[3].y = y2;
	v[3].z = 0.0f;
	v[3].u = tx0;
	v[3].v = ty1;

	DrawQuad(vertices, true);
}


void hgio_copyrot(BMSCR* bm, short xx, short yy, short srcsx, short srcsy, float s_ofsx, float s_ofsy, BMSCR* bmsrc, float psx, float psy, float ang)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	if (bm == NULL) return;
	if (bm->type != HSPWND_TYPE_MAIN) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	TEXINF* tex = GetTex(bmsrc->texid);
	if (!tex) return;
	ChangeTex(bmsrc->texid);

	HGIO_DX11_VERTEX vertices[4];
	HGIO_DX11_VERTEX *v;
	int texpx, texpy, texid;
	float x, y, x0, y0, x1, y1, ofsx, ofsy, mx0, mx1, my0, my1;
	float tx0, ty0, tx1, ty1, sx, sy;

	mx0 = -(float)sin(ang);
	my0 = (float)cos(ang);
	mx1 = -my0;
	my1 = mx0;

	ofsx = -s_ofsx;
	ofsy = -s_ofsy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	//		基点の算出
	x = ((float)bm->cx - (-x0 + x1));
	y = ((float)bm->cy - (-y0 + y1));

	/*-------------------------------*/

	//		回転座標の算出
	ofsx = -psx;
	ofsy = -psy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	/*-------------------------------*/

	texid = bmsrc->texid;
	ChangeTex(texid);
	tex = GetTex(texid);
	sx = tex->ratex;
	sy = tex->ratey;
	texpx = xx + srcsx;
	texpy = yy + srcsy;

	tx0 = ((float)xx) * sx;
	ty0 = ((float)yy) * sy;
	tx1 = ((float)(texpx)) * sx;
	ty1 = ((float)(texpy)) * sy;
	tx0 += tex->ratehx;
	ty0 += tex->ratehy;

	v = &vertices[0];
	const int alpha = CopyAlpha(bm, false);
	v[0].color = v[1].color = v[2].color = v[3].color = ColorWithAlpha(bm->mulcolor, alpha);

	/*-------------------------------*/

	v->x = (x);
	v->y = (y);
	v->z = 0.0f;
	v->u = tx0;
	v->v = ty0;
	v++;

	/*-------------------------------*/

	v->x = ((x1)+x);
	v->y = ((y1)+y);
	v->z = 0.0f;
	v->u = tx1;
	v->v = ty0;
	v++;

	/*-------------------------------*/

	v->x = ((-x0 + x1) + x);
	v->y = ((-y0 + y1) + y);
	v->z = 0.0f;
	v->u = tx1;
	v->v = ty1;
	v++;

	/*-------------------------------*/

	v->x = ((-x0) + x);
	v->y = ((-y0) + y);
	v->z = 0.0f;
	v->u = tx0;
	v->v = ty1;
	v++;

	/*-------------------------------*/

	DrawQuad(vertices, true);
}


void hgio_setfilter( int type, int opt )
{
	switch( type ) {
	case HGIO_FILTER_TYPE_LINEAR:
		render->SetLinearFilter(true);
		break;
	case HGIO_FILTER_TYPE_LINEAR2:
		render->SetLinearFilter(true);
		break;
	default:
		render->SetLinearFilter(false);
		break;
	}
}



void hgio_square_tex( BMSCR *bm, int *posx, int *posy, BMSCR *bmsrc, int *uvx, int *uvy )
{
	//		四角形(square)テクスチャ描画
	//
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	TEXINF* tex = GetTex(bmsrc->texid);
	if (!tex) return;

	ChangeTex(bmsrc->texid);
	const unsigned int color = ColorWithAlpha(0x00ffffff, CopyAlpha(bm, false));
	HGIO_DX11_VERTEX vertices[4];
	// Preserve the winding used by the original primitive fan.
	for (int i = 0; i < 4; ++i) {
		vertices[i] = { static_cast<float>(posx[i]), static_cast<float>(posy[i]), 0.0f, color,
			uvx[i] * tex->ratex, uvy[i] * tex->ratey };
	}
	DrawQuad(vertices, true);
}


void hgio_square( BMSCR *bm, int *posx, int *posy, int *colors )
{
	//		四角形(square)単色描画
	//
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex(-1);
	const int alpha = CopyAlpha(bm, false);
	HGIO_DX11_VERTEX vertices[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i] = { static_cast<float>(posx[i]), static_cast<float>(posy[i]), 0.0f,
			ColorWithAlpha(colors[i], alpha), 0.0f, 0.0f };
	}
	DrawQuad(vertices, false);
}


int hgio_gettick( void )
{
	return timeGetTime();
}


HSPREAL hgio_getinfo( int type )
{
	int i;
	i = type - GINFO_EXINFO_BASE;
	if (( i >= 0 )&&( i < GINFO_EXINFO_MAX)) {
		return infoval[i];
	}
	return 0.0;
}

void hgio_setinfo( int type, HSPREAL val )
{
	int i;
	i = type - GINFO_EXINFO_BASE;
	if (( i >= 0 )&&( i < GINFO_EXINFO_MAX)) {
		infoval[i] = val;
	}
}

HWND hgio_gethwnd( void )
{
	return master_wnd;
}


int hgio_celputmulti( BMSCR *bm, int *xpos, int *ypos, int *cel, int count, BMSCR *bmsrc )
{
	//		マルチ画像コピー
	//		int配列内のX,Y,CelIDを元に等倍コピーを行なう(count=個数)
	//		カレントポジション、描画モードはBMSCRから取得
	//
	int psx,psy;
	float f_psx,f_psy;
	int i;
	int id;
	int *p_xpos;
	int *p_ypos;
	int *p_cel;
	int xx,yy;
	int total;

	if ( bm == NULL ) return 0;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	total =0;

	p_xpos = xpos;
	p_ypos = ypos;
	p_cel = cel;

	psx = bmsrc->divsx;
	psy = bmsrc->divsy;
	f_psx = (float)psx;
	f_psy = (float)psy;

	for(i=0;i<count;i++) {

		id = *p_cel;

		if ( id >= 0 ) {

			xx = ( id % bmsrc->divx ) * psx;
			yy = ( id / bmsrc->divx ) * psy;

			bm->cx = *p_xpos;
			bm->cy = *p_ypos;

			hgio_copy( bm, xx, yy, psx, psy, bmsrc, f_psx, f_psy );

			total++;
		}

		p_xpos++;
		p_ypos++;
		p_cel++;

	}

	return total;
}


void hgio_setview(BMSCR* bm)
{
	// vp_flagに応じたビューポートの設定を行う
	//
	if (!bm || bm->vp_flag == BMSCR_VPFLAG_NOUSE) return;

	MATRIX tmp;
	switch (bm->vp_flag) {
	case BMSCR_VPFLAG_2D:
		UnitMatrix(); RotZ(bm->vp_viewrotate[2]); GetCurrentMatrix(&tmp);
		OrthoMatrix(-bm->vp_viewtrans[0], -bm->vp_viewtrans[1],
			nDestWidth / bm->vp_viewscale[0], nDestHeight / bm->vp_viewscale[1], 0.0f, 1.0f);
		MulMatrix(&tmp); GetCurrentMatrix(&mat_proj); break;
	case BMSCR_VPFLAG_3D:
		UnitMatrix(); RotZ(bm->vp_viewrotate[2]); RotY(bm->vp_viewrotate[1]); RotX(bm->vp_viewrotate[0]);
		Scale(bm->vp_viewscale[0], bm->vp_viewscale[1], bm->vp_viewscale[2]);
		Trans(bm->vp_viewtrans[0], bm->vp_viewtrans[1], bm->vp_viewtrans[2]); GetCurrentMatrix(&tmp);
		PerspectiveFOV(bm->vp_view3dprm[0], bm->vp_view3dprm[1], bm->vp_view3dprm[2], 0, 0,
			nDestWidth / 10.0f, nDestHeight / 10.0f); MulMatrix(&tmp); GetCurrentMatrix(&mat_proj); break;
	case BMSCR_VPFLAG_MATRIX:
		memcpy(&mat_proj, bm->vp_viewtrans, sizeof(mat_proj)); break;
	default: return;
	}

	render->SetViewMatrix(&mat_proj);

	//	投影マトリクスの逆行列を設定する
	SetCurrentMatrix(&mat_proj);
	InverseMatrix(&mat_unproj);

}


void hgio_cnvview(BMSCR* bm, int* xaxis, int* yaxis)
{
	//	ビュー変換後の座標 -> 元の座標に変換する
	//	(タッチ位置再現のため)
	//
	VECTOR v1,v2;
	if (bm->vp_flag == 0) return;
	v1.x = (float)*xaxis;
	v1.y = (float)(nDestHeight-*yaxis);
	v1.z = 1.0f;
	v1.w = 0.0f;

	v1.x -= nDestWidth/2;
	v1.y -= nDestHeight/2;
	v1.x *= 2.0f / float(nDestWidth);
	v1.y *= 2.0f / float(nDestHeight);

//	*xaxis = (int)(v1.x);
//	*yaxis = (int)(v1.y);

//	D3DXVECTOR3 a1,a2;
//	D3DXVec3TransformCoord(&a2, &D3DXVECTOR3(v1.x, v1.y, v1.z), &InvViewport);
//	*xaxis = (int)a2.x;
//	*yaxis = (int)a2.y;

	ApplyMatrix(&mat_unproj, &v2, &v1);
	*xaxis = (int)v2.x;
	*yaxis = (int)v2.y;
}


int hgio_mestex(BMSCR *bm, texmesPos *tpos)
{
	//		TEXMESPOSによる文字表示
	//
	int mode, x, y, sx, sy;
	int orgx, orgy;
	int tx, ty;
	int xsize, ysize;
	int esx, esy;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return -1;
	if (drawflag == 0) hgio_render_start();

	// print per line
	orgx = bm->cx;
	orgy = bm->cy;
	mode = tpos->mode;

	sx = tpos->sx;
	if (sx <= 0) {
		sx = bm->sx - orgx;
		if (sx <= 0) return -1;
	}
	sy = tpos->sy;
	if (sy <= 0) {
		sy = bm->sy - orgy;
		if (sy <= 0) return -1;
	}

	int id = tpos->texid;
	if (id < 0) {
		char *str = tpos->getString();
		id = tmes.texmesRegist(str, tpos);
		if (id < 0) {
			ysize = tmes._fontsize;
			x = orgx; y = orgy;
			if (mode & TEXMES_MODE_CENTERX) {
				int px = sx / 2;
				x += px;
			}
			if (mode & TEXMES_MODE_CENTERY) {
				int py = (sy - ysize) / 2;
				if (py < 0) { py = 0; }
				y += py;
			}
			tpos->lastcx = x;
			tpos->lastcy = y;
			tpos->printysize = ysize;
			return -1;
		}
		tpos->texid = id;
	}

	texmes* tex;
	tex = tmes.texmesUpdateLife(id);
	if (tex == NULL) return -1;

	xsize = tex->sx;
	ysize = tex->sy;
	tpos->printysize = ysize;

	x = orgx; y = orgy;
	tx = 0; ty = 0;

	esx = x + sx;
	esy = y + sy;

	if (mode & TEXMES_MODE_CENTERX) {
		int px = (sx - xsize) / 2;
		if (px < 0) { px = 0; }
		x += px;
	}
	if (mode & TEXMES_MODE_CENTERY) {
		int py = (sy - ysize) / 2;
		if (py < 0) { py = 0; }
		y += py;
	}
	tpos->lastcx = x;
	tpos->lastcy = y;

	if ( tpos->attribute == NULL ) {
		if ((x + xsize) >= esx) {
			xsize = esx - x;
			if (xsize <= 0) return -1;
		}
		if ((y + ysize) >= esy) {
			ysize = esy - y;
			if (ysize <= 0) return -1;
		}
		hgio_fontcopy(bm, x, y, xsize, ysize, tex->_texture, tx, ty);
	}

	bm->cy += ysize;
	bm->printsizex = xsize;
	bm->printsizey = ysize;
	return 0;
}


int hgio_mes(BMSCR* bm, char* msg)
{
	//		mes,print 文字表示
	//
	int xsize, ysize;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return -1;
	if (drawflag == 0) hgio_render_start();

	// print per line
	if (bm->vp_flag == BMSCR_VPFLAG_NOUSE) {
		if (bm->cy >= bm->sy) return -1;
	}

	if (*msg == 0) {
		ysize = tmes._fontsize;
		bm->printsizey += ysize;
		bm->cy += ysize;
		return 0;
	}

	int id;
	texmes* tex;
	id = tmes.texmesRegist(msg);
	if (id < 0) return -1;
	tex = tmes.texmesGet(id);
	if (tex == NULL) return -1;

	xsize = tex->sx;
	ysize = tex->sy;

	if (bm->printoffsetx > 0) {			// センタリングを行う(X)
		int offset = (bm->printoffsetx - xsize) / 2;
		if (offset > 0) {
			bm->cx += offset;
		}
		bm->printoffsetx = 0;
	}
	if (bm->printoffsety > 0) {			// センタリングを行う(Y)
		int offset = (bm->printoffsety - ysize) / 2;
		if (offset > 0) {
			bm->cy += offset;
		}
		bm->printoffsety = 0;
	}

	hgio_fontcopy(bm, bm->cx, bm->cy, xsize, ysize, tex->_texture, 0, 0);

	//xsize = game->drawFont(bm->cx, bm->cy, str1, (gameplay::Vector4*)bm->colorvalue, &ysize);
	if (xsize > bm->printsizex) bm->printsizex = xsize;
	bm->printsizey += ysize;
	bm->cy += ysize;
	return 0;
}


int hgio_font(char *fontname, int size, int style)
{
	//		文字フォント指定
	//
	//Alertf("[%s]%d,%d", fontname, size, style);
	tmes.setFont(fontname, size, style);
	return 0;
}


void hgio_fontsystem_delete(int id)
{
	DeleteTex(id);
}


int hgio_fontsystem_setup(int sx, int sy, void *buffer)
{
	int id = RegistTexEmpty(sx, sy, 1);
	int res = UpdateTex32(id, (char*)buffer, 0);
	if ( res < 0) return -1;
	return id;
}


void hgio_editputclip(BMSCR* bm, char *str)
{
	//		クリップボードコピー
	//
	HGLOBAL hg;
	char *strMem;
	if (!OpenClipboard(master_wnd)) return;
	EmptyClipboard();

	int len = (int)strlen(str)+1;
	hg = GlobalAlloc(GHND | GMEM_SHARE, len);
	strMem = (char *)GlobalLock(hg);
	strcpy(strMem, str);
	GlobalUnlock(hg);

	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
}


char *hgio_editgetclip(BMSCR* bm)
{
	//		クリップボードペースト文字列取得
	//
	HGLOBAL hg;
	char *strClip;
	if (OpenClipboard(master_wnd) && (hg = GetClipboardData(CF_TEXT))) {
		char* p = code_stmp((int)GlobalSize(hg));
		strClip = (char *)GlobalLock(hg);
		strcpy(p, strClip);
		GlobalUnlock(hg);
		CloseClipboard();
		return p;
	}
	return NULL;
}


