//
//		Polygon draw lib (directX8)
//			onion software/onitama 2001/6
//
#include <stdio.h>
#define STRICT
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <math.h>

#include "../hgimg/hginc.h"
#include "hgiox.h"
#include "../hgimg/sysreq.h"

#define RELEASE(x) 	if(x){x->Release();x=NULL;}


//
//	function prototypes
//
void setDest( void *dest, int sx, int sy );
void releaseDest( void );

void ClearDest( int mode, int color, int tex );
void DrawLineF4( POLY4 *poly );
void DrawLineF4G( POLY4G *poly );
void DrawPolygonF4( POLY4 *lpPolyData );
void DrawPolygonTex( POLY4 *lpPolyData );
void DrawPolygonF4G( POLY4G *lpPolyData );
void DrawPolygonTexG( POLY4G *lpPolyData );
void DrawSpriteTex( P_SPRITE *spr );
void DrawBGSpriteTex( P_BGSPR *bg );
void DrawFontSprite( int x, int y, int cx, int cy, int px, char *mes, int attr, int tex );
void DrawPolygonTLG( POLY4GX *poly );
void DrawLineTLG( POLY4GX *poly );
void SetViewMatrix( P_MAT *pmat );

void TexInit( void );
void TexTerm( void );
int RegistTex( char *data, int sx, int sy, int width, int height, int sw );
int GetNextTexID( void );
void DeleteTex( int id );
void ChangeTex( int id );
TEXINF *GetTex( int id );
void SetSrcTex( void *src, int sx, int sy );

static void TLUpdate( void );



//		Data
//
static		TEXINF texinf[TEXINF_MAX];

//		Settings
//
static		char *lpDest;		// 描画画面のバッファ
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ
static		int DestSize;		// 描画画面バッファのサイズ

static		HWND target_wnd;	// 表示対象Window
static		D3DDISPLAYMODE target_disp;

static		IAXIS sprclip1;		// 2D表示用クリップ領域(左上)
static		IAXIS sprclip2;		// 2D表示用クリップ領域(右下)

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID
static		int nTexWidth;		// テクスチャ幅
static		int nTexHeight;		// テクスチャ高さ

static		char ck1;			// カラーキー1
static		char ck2;			// カラーキー2
static		char ck3;			// カラーキー3

//		DirectX objects
//
static		LPDIRECT3D8 d3d;
static		LPDIRECT3DDEVICE8 d3ddev;
static		D3DPRESENT_PARAMETERS d3dapp;
static		D3DMATERIAL8 d3dmat;

//LPDIRECT3DTEXTURE8 pTexture = NULL;


/*------------------------------------------------------------*/
/*
		Direct3D interface
*/
/*------------------------------------------------------------*/

typedef struct _D3DTLVERTEX_ {
	float x,y,z;
	float rhw;
	DWORD color;
	DWORD specular;
	float tu0,tv0;
}D3DTLVERTEX;

typedef struct _D3DTLVERTEXC_ {
	float x,y,z;
	float rhw;
	DWORD color;
	DWORD specular;
}D3DTLVERTEXC;

typedef struct _D3DEXVERTEX_ {
	float x,y,z;
	DWORD color;
//	DWORD specular;
	float tu0,tv0;
}D3DEXVERTEX;

typedef struct _D3DLNVERTEX_ {
	float x,y,z;
	float nx,ny,nz;
	float tu0,tv0;
}D3DLNVERTEX;

typedef struct _D3DEXVERTEXC_ {
	float x,y,z;
	DWORD color;
	DWORD specular;
}D3DEXVERTEXC;

//	頂点フォーマット
//
#define D3DFVF_TLVERTEX 		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define D3DFVF_TLVERTEXC 		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR )

#define D3DFVF_LNVERTEX 		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_EXVERTEX 		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
//#define D3DFVF_EXVERTEX 		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define D3DFVF_EXVERTEXC 		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR )



static int Init3DDevicesW( HWND hwnd )
{
	//
	//	Direct3Dデバイス初期化
	//
	int vmode;
	D3DDISPLAYMODE dmode;

	SetSysReq( SYSREQ_RESULT, 0 );
	SetSysReq( SYSREQ_RESVMODE, 0 );

	//Direct3Dオブジェクトの取得
	d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if( d3d == NULL ) {
		SetSysReq( SYSREQ_RESULT, 1 );
		return FALSE;
	}

	//現在のディスプレイモードを得る
	if(FAILED( d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dmode)))	//DEFAULT指定でプライマリアダプタを選択
//	if(FAILED( d3d->GetDisplayMode(&dmode)))	//DEFAULT指定でプライマリアダプタを選択
	{
		SetSysReq( SYSREQ_RESULT, 2 );
		return FALSE;
	}

	//バックサーフェースのフォーマットをコピーして使用する
	ZeroMemory( &d3dapp, sizeof(d3dapp) );
	d3dapp.Windowed = TRUE;							//ウィンドウモード
	d3dapp.SwapEffect = D3DSWAPEFFECT_DISCARD;		//垂直同期でフリップ
	d3dapp.BackBufferFormat = dmode.Format;
	d3dapp.BackBufferCount = 1;
	d3dapp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dapp.hDeviceWindow = hwnd;

	if ( GetSysReq( SYSREQ_DXMODE ) ) {
		d3dapp.Windowed = FALSE;
		d3dapp.BackBufferWidth = GetSysReq( SYSREQ_DXWIDTH );
		d3dapp.BackBufferHeight = GetSysReq( SYSREQ_DXHEIGHT );
	}

	// Z バッファの自動作成(T&L使用時)
	d3dapp.EnableAutoDepthStencil = TRUE;
	d3dapp.AutoDepthStencilFormat = D3DFMT_D16;

	target_disp = dmode;
	vmode = dmode.Format;
	SetSysReq( SYSREQ_RESVMODE, vmode );

	//デバイスの作成
	if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dapp,&d3ddev))){
		if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dapp,&d3ddev))){
			if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dapp,&d3ddev))){
				SetSysReq( SYSREQ_RESULT, 3 );
				return FALSE;
			}
			else { vmode|=0x1000; }
		}
	}

	//D3DXCreateTextureFromFile(pD3DDevice,"chr.bmp",&pTexture);

	//バックバッファを得る
	//pD3DDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

	SetSysReq( SYSREQ_RESVMODE, vmode );

    d3ddev->SetRenderState( D3DRS_ZENABLE, TRUE);			// T&L使用時
    d3ddev->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
	//d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	d3ddev->SetRenderState(D3DRS_LIGHTING,FALSE);           //ライティングしない
	d3ddev->SetRenderState(D3DRS_COLORVERTEX,TRUE);

	d3dmat.Diffuse.r = 1.0f;
	d3dmat.Diffuse.g = 1.0f;
	d3dmat.Diffuse.b = 1.0f;
	d3dmat.Diffuse.a = 1.0f;
	//d3ddev->SetMaterial( &d3dmat );

	//	DiffuseA x TextureA をAlpha値とする
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAOP,D3DTOP_MODULATE );
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAARG2, D3DTA_CURRENT );

	//  αテスト
	d3ddev -> SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	d3ddev -> SetRenderState( D3DRS_ALPHAREF, 0 );
	d3ddev -> SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );

	return TRUE;
}


static void Term3DDevices( void )
{
	//
	//	Direct3Dデバイス解放
	//
	d3ddev->Reset(&d3dapp);
	RELEASE( d3ddev );
	delete d3ddev;
	RELEASE( d3d );
	delete d3d;
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgiox_init( HGProc *p, int mode, int sx, int sy, HWND hwnd )
{
	//		関数ポインタ設定
	//
	p->setDest = setDest;
	p->releaseDest = releaseDest;
	p->ClearDest = ClearDest;
	p->DrawLineF4 = DrawLineF4;
	p->DrawLineF4G = DrawLineF4G;
	p->DrawPolygonF4 = DrawPolygonF4;
	p->DrawPolygonTex = DrawPolygonTex;
	p->DrawPolygonF4G = DrawPolygonF4G;
	p->DrawPolygonTexG = DrawPolygonTexG;
	p->DrawSpriteTex = DrawSpriteTex;
	p->DrawBGSpriteTex = DrawBGSpriteTex;
	p->DrawFontSprite = DrawFontSprite;
	p->DrawPolygonTLG = DrawPolygonTLG;
	p->DrawLineTLG = DrawLineTLG;
	p->SetViewMatrix = SetViewMatrix;

	p->TexInit = TexInit;
	p->TexTerm = TexTerm;
	p->RegistTex = RegistTex;
	p->GetNextTexID = GetNextTexID;
	p->DeleteTex = DeleteTex;
	p->ChangeTex = ChangeTex;
	p->GetTex = GetTex;
	p->SetSrcTex = SetSrcTex;

	//		デバイス初期化
	//
	Init3DDevicesW( hwnd );

	//		バッファ初期化
	//
	lpDest = NULL;
	lpTex = NULL;

	//		テクスチャ情報初期化
	//
	TexInit();
}


void hgiox_term( void )
{
	releaseDest();
	d3ddev->SetTexture( 0, NULL );
	TexTerm();
	Term3DDevices();
}


void hgiox_render_start( void )
{
	//シーンレンダー開始
	d3ddev->BeginScene();
	TLUpdate();
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
}


void hgiox_render_end( void )
{
	//テクスチャステージのリセット
	d3ddev->SetTexture( 0, NULL );
	//シーンレンダー終了
	d3ddev->EndScene();
	d3ddev->Present(NULL,NULL,NULL,NULL);
}


static void setDest( void *dest, int sx, int sy )
{
	releaseDest();
	lpDest = (char *)dest;
	nDestWidth = sx;
	nDestHeight = sy;
	DestSize = sx*sy;
	setIAXIS( sprclip1, 0, 0 );
	setIAXIS( sprclip2, nDestWidth, nDestHeight );
}


static void releaseDest( void )
{
	if ( lpDest == NULL ) return;
	lpDest = NULL;
}


static void ClearDest( int mode, int color, int tex )
{
	switch ( mode ) {
	case 0:
		//消去(Zバッファのみ)
		d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		break;
	case 1:
		//塗りつぶして消去
//		d3ddev->Clear(0,NULL,D3DCLEAR_TARGET,color,1.0f,0);
		d3ddev->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,color,1.0f,0);	// T&L使用時
		break;
//	if ( mode == 2 ) {
//	}
	case 3:
		//blur clear
		int i;
		int col;
		D3DTLVERTEX v[4];
		d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		d3ddev->SetTexture( 0, NULL );
		curtex = -1;
//	 	d3ddev->BeginScene();

//		d3ddev->SetRenderState( D3DRS_ZENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
//		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
//		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		col = color & 0xff;

		
		v[3].x = (float)0.0f; v[3].y = (float)0.0f;
		v[2].x = (float)(nDestWidth); v[2].y = (float)0.0f;
		v[1].x = (float)(nDestWidth); v[1].y = (float)(nDestHeight);
		v[0].x = (float)0.0f; v[0].y = (float)(nDestHeight);
	
		for(i=0;i<4;i++) {
			v[i].z = 0.0f;
			v[i].rhw = 1.0f;
			v[i].color = (col<<24);
			v[i].specular = 0; v[i].tu0 = 0.0f; v[0].tv0 = 0.0f;
		}
		//デバイスに使用する頂点フォーマットをセットする
		d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
		// とりあえず直接描画(四角形)
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
		//　Zテストを戻す
		d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//		d3ddev->SetRenderState( D3DRS_ZENABLE, TRUE );
//		d3ddev->EndScene();
		break;
	}
}


/*------------------------------------------------------------*/
/*
		Polygon Draw Routines
*/
/*------------------------------------------------------------*/


static void DrawLineF4( POLY4 *poly )
{
	D3DTLVERTEX v[5];
	IAXIS2 *ia;
	int x0,x1,x2,y0,y1,y2;
	int i;
	int color;
	int alphaop,alpha;

	//		裏表判定
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	if ( poly->tex != -1 ) {
		d3ddev->SetTexture( 0, NULL );
		curtex = -1;
	}
	if ( poly->alpha!=0x100 ) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}

	color = poly->color | alpha;
	for(i=0;i<5;i++) {
		if (i<4) ia = &poly->v[i]; else ia = &poly->v[0];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1;
		v[i].rhw = 1;
		v[i].color = color;
		v[i].specular = 0;
//		v[i].tu0 = 0.0f;	v[0].tv0 = 0.0f;
	}

	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);

	if (( v[2].x==v[3].x )&&( v[2].y==v[3].y )) {
		v[3].x=v[0].x; v[3].y=v[0].y;
		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, v, sizeof(D3DTLVERTEX) );
	} else {
		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, v, sizeof(D3DTLVERTEX) );
	}
}


static void DrawLineF4G( POLY4G *poly )
{
	D3DTLVERTEX v[5];
	IAXIS2 *ia;
	int x0,x1,x2,y0,y1,y2;
	int i;
	int alphaop,alpha;

	//		裏表判定
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	if ( poly->tex != -1 ) {
		d3ddev->SetTexture( 0, NULL );
		curtex = -1;
	}

	if (( poly->attr & NODE_ATTR_COLKEY )||( poly->alpha!=0x100 )) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}

	for(i=0;i<5;i++) {
		if (i<4) ia = &poly->v[i]; else ia = &poly->v[0];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1;
		v[i].rhw = 1;
		v[i].color = poly->color[i] | alpha;
		v[i].specular = 0;
//		v[i].tu0 = 0.0f;	v[0].tv0 = 0.0f;
	}
	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画
	if (( v[2].x==v[3].x )&&( v[2].y==v[3].y )) {
		v[3].x=v[0].x; v[3].y=v[0].y; v[3].color=v[0].color;
		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, v, sizeof(D3DTLVERTEX) );
	} else {
		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, v, sizeof(D3DTLVERTEX) );
	}
}


static void DrawPolygonF4( POLY4 *poly )
{
	D3DTLVERTEX v[4];
	IAXIS2 *ia;
	int x0,x1,x2,y0,y1,y2;
	int i;
	int color;
	int alphaop,alpha;

	//		裏表判定
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	if ( poly->tex != -1 ) {
		d3ddev->SetTexture( 0, NULL );
		curtex = -1;
	}
	if ( poly->alpha!=0x100 ) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0xff000000;
	}
	//d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	color = poly->color | alpha;
	for(i=0;i<4;i++) {
		ia = &poly->v[i];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1;
		v[i].rhw = 1;
		v[i].color = color;
		v[i].specular = 0;
//		v[i].tu0 = 0.0f;	v[0].tv0 = 0.0f;
	}

	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
}


static void DrawPolygonTex( POLY4 *poly )
{
	D3DTLVERTEX v[4];
	IAXIS2 *ia;
//	int x0,x1,x2,y0,y1,y2;
	int i;
	int color;
	int alphaop,alpha;

	//		裏表判定
/*
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;
*/
	//		Texture select
	if ( poly->tex != curtex ) {
		//テクスチャを選択
		ChangeTex( poly->tex );
		d3ddev->SetTexture( 0, (IDirect3DBaseTexture8 *)lpTex );
	}
	if (( poly->attr & NODE_ATTR_COLKEY )||( poly->alpha!=0x100 )) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}

/*
	if ( poly->attr & NODE_ATTR_COLKEY ) {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
*/
	color = poly->color | alpha;
	for(i=0;i<4;i++) {
		ia = &poly->v[i];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1.0f;
		v[i].rhw = 1.0f;
		v[i].color = color;
		v[i].specular = 0;
		v[i].tu0 = (float)ia->tx / (float)nTexWidth;
		v[i].tv0 = (float)ia->ty / (float)nTexHeight;
	}

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
}


static void DrawPolygonF4G( POLY4G *poly )
{
	D3DTLVERTEX v[4];
	IAXIS2 *ia;
	int x0,x1,x2,y0,y1,y2;
	int i;
	int alphaop,alpha;

	//		裏表判定
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	if ( poly->tex != -1 ) {
		d3ddev->SetTexture( 0, NULL );
		curtex = -1;
	}

	if (( poly->attr & NODE_ATTR_COLKEY )||( poly->alpha!=0x100 )) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}

	for(i=0;i<4;i++) {
		ia = &poly->v[i];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1;
		v[i].rhw = 1;
		v[i].color = poly->color[i] | alpha;
		v[i].specular = 0;
//		v[i].tu0 = 0.0f;	v[0].tv0 = 0.0f;
	}

//	d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//	d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
}


static void DrawPolygonTexG( POLY4G *poly )
{
	D3DTLVERTEX v[4];
	IAXIS2 *ia;
	int x0,x1,x2,y0,y1,y2;
	int i;
	int alphaop,alpha;

	//		裏表判定
	x0 = poly->v[0].x;
	x1 = poly->v[1].x;
	x2 = poly->v[3].x;
	y0 = poly->v[0].y;
	y1 = poly->v[1].y;
	y2 = poly->v[3].y;
	if ( (x0 - x1) * (y1 - y2) - (x1 - x2) * (y0 - y1) >= 0 ) return;

	//		Texture select
	if ( poly->tex != curtex ) {
		//テクスチャを選択
		ChangeTex( poly->tex );
		d3ddev->SetTexture( 0, (IDirect3DBaseTexture8 *)lpTex );
	}

	if (( poly->attr & NODE_ATTR_COLKEY )||( poly->alpha!=0x100 )) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}

	for(i=0;i<4;i++) {
		ia = &poly->v[i];
		v[i].x = (float)ia->x;
		v[i].y = (float)ia->y;
		v[i].z = 1;
		v[i].rhw = 1;
		v[i].color = poly->color[i] | alpha;
		v[i].specular = 0;
		v[i].tu0 = (float)ia->tx / (float)nTexWidth;
		v[i].tv0 = (float)ia->ty / (float)nTexHeight;
	}

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
}


/*------------------------------------------------------------*/
/*
		Sprite Draw Routines
*/
/*------------------------------------------------------------*/

static void DrawSpriteTex( P_SPRITE *spr )
{
	D3DTLVERTEX v[4];
	int i,x,y;
	int tx,ty,tsx,tsy;
	int x0,y0,tx0,ty0;
	int clipx0,clipy0;
	int clipx1,clipy1;
	int alphaop,alpha,color;
	float sx,sy;

	clipx0 = sprclip1.x;
	clipy0 = sprclip1.y;
	clipx1 = sprclip2.x;
	clipy1 = sprclip2.y;

	x = spr->v[0].x;
	y = spr->v[0].y;
	tx = spr->v[0].tx;
	ty = spr->v[0].ty;
	if ( x >= clipx1 ) return;
	if ( y >= clipy1 ) return;
/*
	if ( x < clipx0 ) {
		tx += clipx0-x; x = clipx0;
	}
	if ( y < clipy0 ) {
		ty += clipy0-y; y = clipy0;
	}
*/
	x0 = spr->v[1].x;
	y0 = spr->v[1].y;
	tx0 = spr->v[1].tx;
	ty0 = spr->v[1].ty;
	if ( x0 < clipx0 ) return;
	if ( y0 < clipy0 ) return;
/*
	if ( x0 >= clipx1 ) {
		tx0 -= x0-clipx1+1; x0 = clipx1-1;
	}
	if ( y0 >= clipy1 ) {
		ty0 -= y0-clipy1+1; y0 = clipy1-1;
	}
*/
	tsx = tx0-tx;
	tsy = ty0-ty;

	//		Texture select
	if ( spr->tex != curtex ) {
		//テクスチャを選択
		ChangeTex( spr->tex );
		d3ddev->SetTexture( 0, (IDirect3DBaseTexture8 *)lpTex );
	}

	if (( spr->attr & NODE_ATTR_COLKEY )||( spr->alpha!=0x100 )) {
		alphaop = spr->alpha>>8;
		alpha = (spr->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		alpha = 0;
	}
	color = 0xffffff | alpha;
	for(i=0;i<4;i++) {
		v[i].z = 0.0f;
		v[i].rhw = 1.0f;
		v[i].color = color;
		v[i].specular = 0;
	}
	sx = (float)nTexWidth; sy = (float)nTexHeight;
	v[3].x = (float)x; v[3].y = (float)y;
	v[3].tu0 = (float)(tx) / sx;
	v[3].tv0 = (float)(ty) / sy;
	v[2].x = (float)(x+tsx); v[2].y = (float)y;
	v[2].tu0 = (float)(tx+tsx) / sx;
	v[2].tv0 = (float)(ty) / sy;
	v[1].x = (float)(x+tsx); v[1].y = (float)(y+tsy);
	v[1].tu0 = (float)(tx+tsx) / sx;
	v[1].tv0 = (float)(ty+tsy) / sy;
	v[0].x = (float)x; v[0].y = (float)(y+tsy);
	v[0].tu0 = (float)(tx) / sx;
	v[0].tv0 = (float)(ty+tsy) / sy;
	
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
}


static void DrawBGSpriteTex( P_BGSPR *bg )
{
	P_SPRITE spr;
	short *vram;
	short val;
	char *p;
	int x,y,wx,wy,vx,vy,cx,cy,sx,sy,adjx,adjy;
	int i,j,a;

	vx = bg->vx / bg->cx;
	vy = bg->vy / bg->cy;
	adjx = bg->vx % bg->cx;
	adjy = bg->vy % bg->cy;
	x = bg->xx;
	y = bg->yy;
	sx = bg->sx << 1;
	sy = bg->sy;
	cx = bg->cx;
	cy = bg->cy;
	wx = bg->wx;if ( adjx ) { wx+=1; x-=adjx; }
	wy = bg->wy;if ( adjy ) { wy+=1; y-=adjy; }
	p = bg->vram + ( vy * sx ) + (vx<<1);

	spr.v[0].y = y;
	spr.v[1].y = y + cy - 1;

	//		Set Clip
	//
	setIAXIS( sprclip1, bg->xx, bg->yy );
	setIAXIS( sprclip2, bg->xx + ( bg->wx * cx ), bg->yy + ( bg->wy * cy ) );

	spr.tex = bg->tex;
	spr.attr = bg->attr;
	spr.alpha = bg->alpha;
	for(j=0;j<wy;j++) {
		spr.v[0].x = x;
		spr.v[1].x = x + cx - 1;
		vram = (short *)p;
		for(i=0;i<wx;i++) {
			val = *vram++;
			a = (val & 15) * cx;
			spr.v[0].tx = a;
			spr.v[1].tx = a + cx -1;
			a = (val >> 4) * cy;
			spr.v[0].ty = a;
			spr.v[1].ty = a + cy -1;

			DrawSpriteTex( &spr );
		
			spr.v[0].x += cx;
			spr.v[1].x += cx;
		}
		p += sx;
		spr.v[0].y += cy;
		spr.v[1].y += cy;
	}

	//		Reset Clip
	//
	setIAXIS( sprclip1, 0, 0 );
	setIAXIS( sprclip2, nDestWidth, nDestHeight );
}


static void DrawFontSprite( int x, int y, int cx, int cy, int px, char *mes, int attr, int tex )
{
	unsigned char *p;
	int a,val;
	P_SPRITE spr;
	spr.attr = (short)attr;
	if ( attr & NODE_ATTR_USEALPHA ) {
		spr.alpha = attr & 0x3ff;
	} else {
		spr.alpha = 0x100;
	}
	spr.tex = tex;
	spr.v[0].y = y;
	spr.v[1].y = y + cy - 1;
	spr.v[0].x = x;
	spr.v[1].x = x + cx - 1;
	p = (unsigned char *)mes;

	while(1) {
		val = (int)*p++;
		if ( val == 0 ) break;
		if ( val != 32 ) {			// spaceは無視
			a = (val & 15) * cx;
			spr.v[0].tx = a;
			spr.v[1].tx = a + cx;
			a = (val >> 4) * cy;
			spr.v[0].ty = a;
			spr.v[1].ty = a + cy;
			DrawSpriteTex( &spr );
		}
		spr.v[0].x += px;
		spr.v[1].x += px;
	}
}


/*------------------------------------------------------------*/
/*
		texture process
*/
/*------------------------------------------------------------*/

static void TexInit( void )
{
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		texinf[i].flag = 0;
	}
}


static void TexTerm( void )
{
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		DeleteTex( i );
	}
}


static int GetNextTexID( void )
{
	int i,sel;
	sel = -1;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( texinf[i].flag == 0 ) { sel=i;break; }
	}
	return sel;
}

	
static void TexCopySub32( char *dst, char *src, int size )
{
	//		(R,G,B) を (A8,R8,G8,B8) 形式にしてコピー
	//
	char *p;
	char *sp;
	char a1,a2,a3,a4;
	int i;
	i = size;
	p = dst; sp = src;
	while( i>0 ) {
		a1=*sp++;a2=*sp++;a3=*sp++;
		a4=(char)0xff;
		if (( a1==ck1 )&&( a2==ck2 )&&( a3==ck3 )) a4=0;
		*p++ = a1;
		*p++ = a2;
		*p++ = a3;
		*p++ = a4;
		i--;
	}
}


static void TexCopySub16( char *dst, char *src, int size )
{
	//		(R,G,B) を (A1,R5,G5,B5) 形式にしてコピー
	//
	char *p;
	char *sp;
	char a1,a2,a3,a4;
	int i;
	int val;
	i = size;
	p = dst; sp = src;
	while( i>0 ) {
		a1=*sp++;a2=*sp++;a3=*sp++;
		a4=(char)0xff;
		if (( a1==ck1 )&&( a2==ck2 )&&( a3==ck3 )) a4=0;
		a1 = ( a1 >> 3 )&31;
		a2 = ( a2 >> 3 )&31;
		a3 = ( a3 >> 3 )&31;
		a4 = a4 & 1;
		val = ( (int)a4<<15 ) | ( (int)a1) | ( (int)a2<<5 ) | ( (int)a3<<10 );
		*(short *)p = (short)val;
		p+=2;
		i--;
	}
}


static D3DFORMAT GetAvailableFormat( int sw )
{
	//		テクスチャの使用可能なフォーマットを調べる
	//				sw: 0=full color/1=palette 
	D3DFORMAT ok;
	HRESULT hr;
	int bitn;
	int vmode = GetSysReq( SYSREQ_RESVMODE );
	D3DDEVTYPE dtype;

	if ( vmode & 0x1000 ) dtype = D3DDEVTYPE_REF; else dtype = D3DDEVTYPE_HAL;
	if ( sw == 0 ) {
		bitn=32;
		if ((target_disp.Format==D3DFMT_X1R5G5B5)||
		    (target_disp.Format==D3DFMT_A1R5G5B5)) {
			bitn=16;
		}
		if ( bitn==32 ) {
			ok = D3DFMT_A8R8G8B8;
			hr = d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, dtype,
					target_disp.Format, 0, D3DRTYPE_TEXTURE, ok );
			if ( hr==D3D_OK ) return ok;
			ok = D3DFMT_X8R8G8B8;
			hr = d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, dtype,
					target_disp.Format, 0, D3DRTYPE_TEXTURE, ok );
			if ( hr==D3D_OK ) return ok;
		}
		ok = D3DFMT_A1R5G5B5;
		hr = d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, dtype,
				target_disp.Format, 0, D3DRTYPE_TEXTURE, ok );
		if ( hr==D3D_OK ) return ok;
		ok = D3DFMT_X1R5G5B5;
		hr = d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, dtype,
				target_disp.Format, 0, D3DRTYPE_TEXTURE, ok );
		if ( hr==D3D_OK ) return ok;
		return D3DFMT_UNKNOWN;
	}
	ok = D3DFMT_P8;
	hr = d3d->CheckDeviceFormat( D3DADAPTER_DEFAULT, dtype,
			target_disp.Format, 0, D3DRTYPE_TEXTURE, ok );
	if ( hr==D3D_OK ) return ok;
	return D3DFMT_UNKNOWN;
}
	

static int RegistTex( char *data, int sx, int sy, int width, int height, int sw )
{
	int sel,y,i;
	TEXINF *t;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;
	HRESULT hr;
	char *p;
	char *src;
	D3DFORMAT textype;

	//		カラーキー更新
	//
	i = GetSysReq( SYSREQ_COLORKEY );
	ck1 = i & 0xff;
	ck2 = (i>>8) & 0xff;
	ck3 = (i>>16) & 0xff;

	//		フォーマット変換
	//
	textype = GetAvailableFormat( 0 );
	if ( textype==D3DFMT_UNKNOWN ) return -2;
 
	hr = d3ddev->CreateTexture( sx,sy,1,0, textype, D3DPOOL_MANAGED, &pTex );
	if ( hr!=D3D_OK ) return -1;
	pTex->LockRect( 0,&lock,NULL,0 );
	p = (char *)lock.pBits;
	if (( textype==D3DFMT_A1R5G5B5 )||( textype==D3DFMT_X1R5G5B5 )) {
		//	16bit texture
		if ( (sw&1) == 0 ) {
			for(y=0;y<sy;y++) {
				src = data + (sx*3*(sy-1-y));
				TexCopySub16( p, src, sx );
				p+=sx*2;
			}
		}
		else {
			TexCopySub16( p, data, sx*sy );
		}
	}
	else {
		//	32bit texture
		if ( (sw&1) == 0 ) {
			for(y=0;y<sy;y++) {
				src = data + (sx*3*(sy-1-y));
				TexCopySub32( p, src, sx );
				p+=sx*4;
			}
		}
		else {
			TexCopySub32( p, data, sx*sy );
		}
	}
	pTex->UnlockRect( 0 );

	sel = GetNextTexID();
	t = GetTex( sel );
	t->flag = 1;
	t->mode = sw;
	t->sx = sx;
	t->sy = sy;
	t->width = width;
	t->height = height;
	t->data = (char *)pTex;
	return sel;
}


int hgiox_RegistTexIndex( char *data, char *palette, int sx, int sy, int width, int height, int sw, int pals )
{
	int sel,y;
	TEXINF *t;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;
	HRESULT hr;
	char *p;
	char *src;
	D3DFORMAT textype;

	textype = GetAvailableFormat( 1 );
	if ( textype==D3DFMT_UNKNOWN ) return -2;
	hr = d3ddev->CreateTexture( sx,sy,1,0, textype, D3DPOOL_MANAGED, &pTex );
	if ( hr!=D3D_OK ) return -1;
	d3ddev->SetPaletteEntries( 0, (PALETTEENTRY*)palette );
	pTex->LockRect( 0,&lock,NULL,0 );
	p = (char *)lock.pBits;
	if ( (sw&1) == 0 ) {
		for(y=0;y<sy;y++) {
			src = data + (sx*(sy-1-y));
			memcpy( p, src, sx );
			p+=sx;
		}
	}
	else {
		memcpy( p, data, sx*sy );
	}
	pTex->UnlockRect( 0 );

	sel = GetNextTexID();
	t = GetTex( sel );
	t->flag = 1;
	t->mode = sw;
	t->sx = sx;
	t->sy = sy;
	t->width = width;
	t->height = height;
	t->data = (char *)pTex;

	return sel;
}


static void DeleteTex( int id )
{
	TEXINF *t;
	LPDIRECT3DTEXTURE8 pTex;
	t = GetTex( id );
	if ( t->flag == 0 ) return;
	t->flag = 0;
	pTex = (LPDIRECT3DTEXTURE8)t->data;
	RELEASE( pTex );
	//pTex->Relase();
	delete pTex;
}


static void ChangeTex( int id )
{
	TEXINF *t;
	if ( id < 0 ) { curtex=-1; lpTex=NULL; return; }
	//if ( curtex == id ) return;
	t = GetTex( id );
	if ( t->flag == 0 ) return;
	curtex = id;
	lpTex = t->data;			// テクスチャのバッファ
	nTexWidth = t->sx;			// テクスチャ幅
	nTexHeight = t->sy;			// テクスチャ高さ
}


static TEXINF *GetTex( int id )
{
	return &texinf[id];
}


static void SetSrcTex( void *src, int sx, int sy )
{
	lpTex = (char *)src;
	nTexWidth = sx;
	nTexHeight = sy;
}


/*------------------------------------------------------------*/
/*
		Polygon Draw Routines ( for internal T&L )
*/
/*------------------------------------------------------------*/

//		DirectX objects
//
static		D3DXMATRIX pers;
static		float projscale, projfov,projfar;

void SetTLParam( float fov, float scale, float pfar )
{
	projscale = scale;
	projfov = fov;
	projfar = pfar * 5.0f;
}

static void TLUpdate( void )
{
//	D3DXMatrixPerspectiveFovRH( &pers, D3DXToRadian(45.0f), 1.0f, 50, 1000 );
//	D3DXMatrixPerspectiveFovRH( &pers, D3DXToRadian(45.0f) + projfov, (float)nDestWidth / (float)nDestHeight, 50, 1500 );
	D3DXMatrixPerspectiveFovRH( &pers, D3DXToRadian(45.0f) + projfov, (float)nDestWidth / (float)nDestHeight, 5, projfar );
	{
	D3DXMATRIX scale;
	D3DXMatrixScaling( &scale, 1.0f * projscale, -1.0f * projscale, 1.0f * projscale );
	pers *= scale;
	}

	d3ddev->SetTransform( D3DTS_PROJECTION, &pers );
//	{
//		D3DXMATRIX view;
//		D3DXMatrixLookAtRH( &view, &D3DXVECTOR3(0,0,200), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0) );
//		d3ddev->SetTransform( D3DTS_VIEW, &view );
//	}
}


static void SetViewMatrix( P_MAT *pmat )
{
	D3DXMATRIX *view = (D3DXMATRIX *)&pmat->mat;
//	{
//	D3DXMATRIX scale;
//	D3DXMatrixScaling( &scale, 1.0f, -1.0f, 1.0f );
//	pers *= scale;
//	}
	d3ddev->SetTransform( D3DTS_VIEW, view );
}

	
static void DrawPolygonTLG( POLY4GX *poly )
{
	D3DEXVERTEX *v;
	int alphaop,alpha;
	float sx,sy;
	v = (D3DEXVERTEX *)&poly->v[0];

	//		Texture select
	if ( poly->tex != curtex ) {
		//	テクスチャを選択
		ChangeTex( poly->tex );
		d3ddev->SetTexture( 0, (IDirect3DBaseTexture8 *)lpTex );
	}
	if ( curtex>=0 ) {
		//	UVを再設定(困ったなぁ…)
		sx = (float)nTexWidth; sy = (float)nTexHeight;
		v[0].tu0 = v[0].tu0 / sx;
		v[0].tv0 = v[0].tv0 / sy;
		v[1].tu0 = v[1].tu0 / sx;
		v[1].tv0 = v[1].tv0 / sy;
		v[2].tu0 = v[2].tu0 / sx;
		v[2].tv0 = v[2].tv0 / sy;
		v[3].tu0 = v[3].tu0 / sx;
		v[3].tv0 = v[3].tv0 / sy;
	}
	if ( poly->alpha!=0x100 ) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		//d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	    d3ddev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE  );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
		v[0].color|=alpha; v[1].color|=alpha; v[2].color|=alpha; v[3].color|=alpha;

		//デバイスに使用する頂点フォーマットをセットする
		d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
		// とりあえず直接描画(四角形)
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DEXVERTEX));
		//d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	    d3ddev->SetRenderState( D3DRS_ZENABLE, TRUE );
		return;
	}
	if ( poly->attr & NODE_ATTR_COLKEY ) {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		alpha = 0xff000000;
		v[0].color|=alpha; v[1].color|=alpha; v[2].color|=alpha; v[3].color|=alpha;
		//デバイスに使用する頂点フォーマットをセットする
		d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
		// とりあえず直接描画(四角形)
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DEXVERTEX));
		return;
	}

	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DEXVERTEX));
}


static void DrawLineTLG( POLY4GX *poly )
{
	D3DEXVERTEX *v;
	int alphaop,alpha;
	v = (D3DEXVERTEX *)&poly->v[0];

	//		Texture select
	if ( curtex != -1 ) {
		//	テクスチャを選択
		ChangeTex( -1 );
		d3ddev->SetTexture( 0, (IDirect3DBaseTexture8 *)lpTex );
	}
	if (( poly->attr & NODE_ATTR_COLKEY )||( poly->alpha!=0x100 )) {
		alphaop = poly->alpha>>8;
		alpha = (poly->alpha & 255)<<24;
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		switch( alphaop ) {
		case 0:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 1:
			alpha = 0xff000000;
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		case 2:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case 3:
			d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			break;
		}
		v[0].color|=alpha; v[1].color|=alpha; v[2].color|=alpha; v[3].color|=alpha;
	}
	else {
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(lines)
//	if (( v[2].x==v[3].x )&&( v[2].y==v[3].y )&&( v[2].z==v[3].z )) {
		v[3].x=v[0].x; v[3].y=v[0].y; v[3].z=v[0].z;
		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, v, sizeof(AXP4GX) );
//	} else {
//		d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, v, sizeof(D3DFVF_EXVERTEX) );
//	}
}


//-----------------------------------------------------------------
//		ゴミコード
//-----------------------------------------------------------------

/*
	D3DXIMAGE_INFO info;
	HRESULT hr;
//	hr = D3DXCreateTextureFromFile( d3ddev, data, pTex );

//	hr = D3DXCreateTextureFromFileEx( d3ddev, data,
//		D3DX_DEFAULT, D3DX_DEFAULT, 1,
//		0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
//		D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000,
//		&info, NULL,
//		&pTex );

	if ( hr!=D3D_OK ) {
		char st[128];
		sprintf( st,"[%s]\nTexture load failed.\n", data );
		MessageBox( NULL, st, "Error",MB_ICONINFORMATION | MB_OK );
		return -1;
	}
*/
