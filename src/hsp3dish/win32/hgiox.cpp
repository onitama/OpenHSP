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

#include "../texmes.h"
#include "../emscripten/fontsystem.h"
void hgio_fontsystem_win32_init(HWND wnd);

#define RELEASE(x) 	if(x){x->Release();x=NULL;}

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")

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
	fsize = dpm_exist( fname );		// ファイルのサイズを取得
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
		DirectX8 Service
*/
/*------------------------------------------------------------*/

//		Settings
//
static		char *lpDest;		// 描画画面のバッファ
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ

static		HWND master_wnd;	// 表示対象Window
static		int drawflag;		// レンダー開始フラグ

static		texmesManager tmes;	// テキストメッセージマネージャー

static		BMSCR *mainbm;		// メインスクリーンのBMSCR
static		char m_tfont[256];	// テキスト使用フォント
static		int m_tsize;		// テキスト使用フォントのサイズ
static		int m_tstyle;		// テキスト使用フォントのスタイル指定
static		float center_x, center_y;

static		BMSCR *backbm;		// 背景消去用のBMSCR(null=NC)

static		HSPREAL infoval[GINFO_EXINFO_MAX];

static		MATRIX mat_proj;	// プロジェクションマトリクス
static		MATRIX mat_unproj;	// プロジェクション逆変換マトリクス

static		HCURSOR cursor_arrow;	// 通常カーソル
static		HCURSOR cursor_ibeam;	// テキストエリア用カーソル

//		DirectX objects
//
static		D3DDISPLAYMODE target_disp;
static		LPDIRECT3D8 d3d;
static		LPDIRECT3DDEVICE8 d3ddev;
static		D3DDEVTYPE	d3ddevtype;
static		D3DPRESENT_PARAMETERS d3dapp;

#define CIRCLE_DIV 16
#define DEFAULT_FONT_NAME ""
#define DEFAULT_FONT_SIZE 18
#define DEFAULT_FONT_STYLE 0

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
	float x, y, z;
	float rhw;
	DWORD color;
	DWORD specular;
}D3DTLVERTEXC;

typedef struct _D3DEXVERTEX_ {
	float x, y, z;
	DWORD color;
	//	DWORD specular;
	float tu0, tv0;
}D3DEXVERTEX;

typedef struct _D3DEXVERTEXC_ {
	float x, y, z;
	DWORD color;
	DWORD specular;
}D3DEXVERTEXC;

//	頂点フォーマット
//
#define D3DFVF_TLVERTEX 		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define D3DFVF_TLVERTEXC 		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR )

#define D3DFVF_EXVERTEXC 		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR )
#define D3DFVF_EXVERTEX 		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define vertex2D_max 5

static D3DTLVERTEX vertex2D[ vertex2D_max ];
static D3DTLVERTEXC vertex2DC[ vertex2D_max ];
static D3DEXVERTEX vertex2DEX[vertex2D_max];
static D3DEXVERTEXC vertex2DEXC[vertex2D_max];
static float linebasex, linebasey;

static void InitDraw( void )
{
    d3ddev->SetRenderState( D3DRS_ZENABLE, FALSE);			// T&L使用時
//	d3ddev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
	d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
//	d3ddev->SetRenderState( D3DRS_AMBIENT, 0xff808080 );
//    d3ddev->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	//d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	d3ddev->SetRenderState(D3DRS_LIGHTING,FALSE);           //ライティングしない
	//d3ddev->SetRenderState(D3DRS_COLORVERTEX,TRUE);
	d3ddev->SetRenderState( D3DRS_COLORVERTEX, FALSE );

//	d3ddev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
//	d3ddev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	d3ddev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT  );
	d3ddev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT  );

	//	DiffuseA x TextureA をAlpha値とする
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAOP,D3DTOP_MODULATE );
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	d3ddev->SetTextureStageState( 0,D3DTSS_ALPHAARG2, D3DTA_CURRENT );

	//  αテスト
	d3ddev -> SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	d3ddev -> SetRenderState( D3DRS_ALPHAREF, 0 );
	d3ddev -> SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
}


static int Init3DDevicesW( HWND hwnd )
{
	//
	//	Direct3Dデバイス初期化
	//
	int vmode;
	D3DDISPLAYMODE dmode;
	unsigned long VertexShaderVersion;

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
		if ( GetSysReq( SYSREQ_DXVSYNC ) ) {
			d3dapp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		} else {
			d3dapp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
	}

	// Z バッファの自動作成(T&L使用時)
	d3dapp.EnableAutoDepthStencil = TRUE;
	d3dapp.AutoDepthStencilFormat = D3DFMT_D16;

	target_disp = dmode;
	vmode = dmode.Format;
	SetSysReq( SYSREQ_RESVMODE, vmode );


	//==============================================================================
	// シェーダーバージョン取得
	//==============================================================================
	D3DCAPS8 caps;
	d3d->GetDeviceCaps( 0, D3DDEVTYPE_HAL, &caps );
	VertexShaderVersion = caps.VertexShaderVersion;

	//==============================================================================
	// デバイスの生成
	//==============================================================================

	int devset;
	devset = 0;
	if ( GetSysReq( SYSREQ_FPUPRESERVE ) ) {
		devset |= D3DCREATE_FPU_PRESERVE;
	}

	// 頂点シェーダーの1.1はあるのんか？(なぜか1.1だと動かない機種が出るのでなし)
	//	if ( VertexShaderVersion >= D3DVS_VERSION(1,1) )
	d3ddevtype = D3DDEVTYPE_HAL;
	if ( VertexShaderVersion >= D3DVS_VERSION(1,0) )
	{
		// HARDWARE T&L
		if FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | devset, &d3dapp, &d3ddev ) )
		{
			// SOFTWARE T&L
			if FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | devset, &d3dapp, &d3ddev ) )
			{
				// REFERENCE RASTERIZE
				if FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | devset, &d3dapp, &d3ddev ) )
				{				
					// だめそうです
					MessageBox( NULL,"Direct3Dデバイスの生成に失敗しました", "Error" , MB_OK | MB_ICONSTOP );
					SetSysReq( SYSREQ_RESULT, 3 );
					return false;
				} else {
					d3ddevtype = D3DDEVTYPE_REF;				// 奇跡的にREFがあった場合
					vmode|=0x1000;
				}
			}
		}
	}
	else
	{
			// SOFTWARE T&L
			if FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | devset, &d3dapp, &d3ddev ) )
			{
				// REFERENCE RASTERIZE
				if FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | devset, &d3dapp, &d3ddev ) )
				{				
					// だめそうです
					MessageBox( NULL,"Direct3Dデバイスの生成に失敗しました", "Error" , MB_OK | MB_ICONSTOP );
					SetSysReq( SYSREQ_RESULT, 3 );
					return false;
				} else {
					d3ddevtype = D3DDEVTYPE_REF;				// 奇跡的にREFがあった場合
					vmode|=0x1000;
				}
			}
	}

	//D3DXCreateTextureFromFile(pD3DDevice,"chr.bmp",&pTexture);

	//バックバッファを得る
	//pD3DDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

	SetSysReq( SYSREQ_RESVMODE, vmode );
	SetSysReq( SYSREQ_PTRD3D, (int)d3d );
	SetSysReq( SYSREQ_PTRD3DDEV, (int)d3ddev );

	InitDraw();

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


static void InitVertexTemp( void )
{
	int i;
	D3DTLVERTEX *v;
	D3DTLVERTEXC* vc;
	D3DEXVERTEX* ve;
	D3DEXVERTEXC* vec;
	for( i=0;i<vertex2D_max;i++ ) {
		v = &vertex2D[ i ];
		v->z = 0.0f;
		v->rhw = 1.0f;
		v->color = 0;
		v->specular = 0;

		vc = &vertex2DC[ i ];
		vc->z = 0.0f;
		vc->rhw = 1.0f;
		vc->color = 0;
		vc->specular = 0;

		ve = &vertex2DEX[i];
		ve->z = 0.0f;
		ve->color = 0;

		vec = &vertex2DEXC[i];
		vec->z = 0.0f;
		vec->color = 0;
		vec->specular = 0;
	}
}


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
	HRESULT hr;
	IDirect3DSurface8 *pBackBuffer;
	IDirect3DSurface8 *pTmp;
	RECT tmparea;
	D3DLOCKED_RECT rect;
	BYTE *base;
	BYTE *hspvram;
	int *data;
	int i,j,k,ofsx,xand;
	int sx_fix, sy_fix;

	//バックバッファを得る

	hr = d3ddev->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	if ( FAILED(hr) ) return -1;

	sx_fix = sx;
	sy_fix = sy;
	if (sx_fix > hgio_getWidth()) { sx_fix = hgio_getWidth(); }
	if (sy_fix > hgio_getHeight()) { sy_fix = hgio_getHeight(); }

	// ARGB用フォーマットのサーフェースをつくる
	int main_sx = Get2N(hgio_getWidth());
	int main_sy = Get2N(hgio_getHeight());
	hr = d3ddev->CreateImageSurface(main_sx, main_sy, D3DFMT_A8R8G8B8, &pTmp);
	if (FAILED(hr)) {
		if (main_sx < main_sy) {
			main_sx = main_sy;
		}
		else {
			if (main_sx > main_sy) main_sy = main_sx;
		}
		hr = d3ddev->CreateImageSurface(main_sx, main_sy, D3DFMT_A8R8G8B8, &pTmp);
		if (FAILED(hr)) return -5;
	}

	tmparea.left = 0;
	tmparea.top = 0;
	tmparea.right = sx - 1;
	tmparea.bottom = sy - 1;

	hr = D3DXLoadSurfaceFromSurface( pTmp, NULL, &tmparea, pBackBuffer, NULL, NULL, D3DX_FILTER_NONE, 0 );
	if ( FAILED(hr) ) return -4;

	hr = pTmp->LockRect( &rect, NULL, 0 );
	if ( FAILED(hr) ) return -3;

	data = (int *)res;
	xand = ( px - 1 )<<2;
	switch( mode ) {
	case 0:
		//	HSP互換(RGBA)
		for (j = 0; j < sy_fix; j += py) {
			hspvram = ((BYTE *)res) + ((sx * 4) * j);
			base = ((BYTE *)rect.pBits) + rect.Pitch * j;
			for (i = 0; i < sx_fix; i += px) {
				*hspvram++ = base[2];
				*hspvram++ = base[1];
				*hspvram++ = base[0];
				*hspvram++ = 0xff;
				base += 4;
			}
		}
		break;
	case 1:
		//	OpenGL互換(BGRA)
		for (j = 0; j < sy_fix; j += py) {
			hspvram = ((BYTE *)res) + ((sx * 4) * j);
			base = ((BYTE *)rect.pBits) + rect.Pitch * j;
			for (i = 0; i < sx_fix; i += px) {
				*hspvram++ = base[0];
				*hspvram++ = base[1];
				*hspvram++ = base[2];
				*hspvram++ = 0xff;
				base += 4;
			}
		}
		break;

	default:
		for(j=0;j<sy;j+=py) {
			base = ((BYTE *)rect.pBits ) + rect.Pitch * j;
			for(i=0;i<sx;i+=px) {
				for(k=0;k<4;k++) {
					ofsx = (i+k)<<2;
					//*data++  = 0x33 * k;
					*data++  = (base[ofsx+2]<<16)|(base[ofsx+1]<<8)|base[ofsx];
					//*data++  = (base[2]<<16)|(base[1]<<8)|base[0];
					//base+=4;
				}
			}
		}
		break;
	}

	pTmp->UnlockRect();

	pTmp->Release();
	pBackBuffer->Release();
	return 0;
}


static void ClearDest( int mode, int color, int tex )
{
	switch ( mode ) {
	case CLSMODE_NONE:
		//消去(Zバッファのみ)
		d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		break;
	case CLSMODE_SOLID:
		//塗りつぶして消去
		d3ddev->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,color,1.0f,0);	// T&L使用時
		break;
	case CLSMODE_TEXTURE:
		{
		//テクスチャで消去
		int i;
		D3DTLVERTEX v[4];
		TEXINF *texinf;
		d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		ChangeTex( tex );
		texinf = GetTex( tex );
		d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		v[3].x = (float)-0.5f; v[3].y = (float)-0.5f;
		v[2].x = (float)(nDestWidth)-0.5f; v[2].y = v[3].y;
		v[1].x = v[2].x; v[1].y = (float)(nDestHeight)-0.5f;
		v[0].x = v[3].x; v[0].y = v[1].y;
	
		for(i=0;i<4;i++) {
			v[i].z = 0.0f;
			v[i].rhw = 1.0f;
			v[i].color = 0xffffffff;
			v[i].specular = 0;
			if ( v[i].x < 0.0f ) v[i].tu0 = 0.0f; else v[i].tu0 = texinf->ratex * texinf->width;
			if ( v[i].y < 0.0f ) v[i].tv0 = 0.0f; else v[i].tv0 = texinf->ratey * texinf->height;
		}
		//デバイスに使用する頂点フォーマットをセットする
		d3ddev->SetVertexShader(D3DFVF_TLVERTEX);
		// とりあえず直接描画(四角形)
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,v,sizeof(D3DTLVERTEX));
		//　Zテストを戻す
		d3ddev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		break;
		}
	case CLSMODE_BLUR:
		{
		//blur clear
		int i;
		int col;
		D3DTLVERTEX v[4];
		d3ddev->Clear(0,NULL,D3DCLEAR_ZBUFFER,color,1.0f,0);
		d3ddev->SetTexture( 0, NULL );
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
			v[i].specular = 0; v[i].tu0 = 0.0f; v[i].tv0 = 0.0f;
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
}


static void InitTexture(void)
{
	//		テクスチャ情報初期化
	//
	SetSysReq(SYSREQ_CLSMODE, CLSMODE_NONE);
	TexSetD3DParam(d3d, d3ddev, target_disp);
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

	hgio_fontsystem_win32_init(master_wnd);

	//		バッファ初期化
	//
	lpDest = NULL;

	//	Direct3Dオブジェクトの取得
	//
	d3d = Direct3DCreate8(D3D_SDK_VERSION);
	if( d3d == NULL ) {
		SetSysReq( SYSREQ_RESULT, 1 );
		return;
	}

	//		デバイス初期化
	//
	Init3DDevicesW( (HWND)hwnd );
	InitVertexTemp();
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


int hgio_device_restore( void )
{
	//	デバイスの修復
	//		(0=OK/1=NG)
	//
	HRESULT hr;
	if( FAILED( hr = d3ddev->TestCooperativeLevel() ) ) {

		if( D3DERR_DEVICELOST == hr ) return -1;
		if( D3DERR_DEVICENOTRESET == hr ) {

			//		デバイス再初期化
			//
//			VertexShaderTerm();
			hr = d3ddev->Reset(&d3dapp);
			if ( FAILED(hr) ) {
				return -2;
			}
			Init3DDevicesW( master_wnd );
			InitVertexTemp();
//			VertexShaderInit();
			TexSetD3DParam( d3d, d3ddev, target_disp );
		}
	}
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

	//テクスチャステージのリセット
	d3ddev->SetTexture( 0, NULL );
	//シーンレンダー終了
	d3ddev->EndScene();
	hr =  d3ddev->Present(NULL,NULL,NULL,NULL);
	if( FAILED(hr) ) res = -1;
	SetSysReq( SYSREQ_DEVLOST, res );
	drawflag = 0;

	tmes.texmesProc();

	return res;
}


static void Mat2D3DMAT(D3DXMATRIX* dst, MATRIX* src)
{
	dst->_11 = src->m00;
	dst->_12 = src->m01;
	dst->_13 = src->m02;
	dst->_14 = src->m03;

	dst->_21 = src->m10;
	dst->_22 = src->m11;
	dst->_23 = src->m12;
	dst->_24 = src->m13;

	dst->_31 = src->m20;
	dst->_32 = src->m21;
	dst->_33 = src->m22;
	dst->_34 = src->m23;

	dst->_41 = src->m30;
	dst->_42 = src->m31;
	dst->_43 = src->m32;
	dst->_44 = src->m33;
}


int hgio_render_start( void )
{
	//static D3DXMATRIX InvViewport;
	if ( drawflag ) {
		hgio_render_end();
	}

	//	デバイスロスト時の対応
	//
	if ( GetSysReq( SYSREQ_DEVLOST ) ) {
		if ( hgio_device_restore() == 0 ) {
			SetSysReq( SYSREQ_DEVLOST, 0 );			// デバイス戻った
		} else {
			return -1;
		}
	}

	//	マトリクスを設定
	D3DXMATRIX matrixIdentitiy;
	ZeroMemory(&matrixIdentitiy, sizeof(matrixIdentitiy));
	matrixIdentitiy._11 = 1.0f;
	matrixIdentitiy._22 = 1.0f;
	matrixIdentitiy._33 = 1.0f;
	matrixIdentitiy._44 = 1.0f;
	d3ddev->SetTransform(D3DTS_WORLD, &matrixIdentitiy);
	d3ddev->SetTransform(D3DTS_VIEW, &matrixIdentitiy);

	//D3DXMATRIX matrixView;
	//Mat2D3DMAT(&matrixView, GetCurrentMatrixPtr());
	//d3ddev->SetTransform(D3DTS_VIEW, &matrixView);

	//	標準の投影マトリクスを設定する
	D3DXMATRIX matrixProj;
	MATRIX *mymat = &mat_proj;
	UnitMatrix();
	OrthoMatrix(0.0f, 0.0f, (float)nDestWidth, (float)nDestHeight, 0.0f, 1.0f);
	GetCurrentMatrix(mymat);
	Mat2D3DMAT(&matrixProj, mymat);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matrixProj);

	//	画面クリア
	int bgtex = -1;
	if (backbm != NULL) { bgtex = backbm->texid; }
	ClearDest(GetSysReq(SYSREQ_CLSMODE), GetSysReq(SYSREQ_CLSCOLOR), bgtex);

	//	ユーザー設定の投影マトリクスを設定する
	hgio_setview(mainbm);

	//	デバイス初期化
	InitDraw();

	//シーンレンダー開始
	d3ddev->BeginScene();
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
	Term3DDevices();
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
	SetClassLong(hwnd, -12, (LONG)hc);

	return 0;
}


int hgio_dialog( int mode, char *str1, char *str2 )
{
	//		dialog表示
	//
	int i,res;
	i = 0;
	if (mode&1) i|=MB_ICONEXCLAMATION; else i|=MB_ICONINFORMATION;
	if (mode&2) i|=MB_YESNO; else i|=MB_OK;
	res = MessageBox( master_wnd, str1, str2, i );
	return res;
}


int hgio_title( char *str1 )
{
	//		title変更
	//
	SetWindowText( master_wnd, str1 );
	return 0;
}


int hgio_texload( BMSCR *bm, char *fname )
{
	//		テクスチャ読み込み
	//
	int i,fsize;
	TEXINF *tex;

	fsize = OpenMemFilePtr( fname );				// HSPリソースを含めて検索する
	i = RegistTex( GetMemFilePtr(), fsize, 0, -1, -1, -1 );
	CloseMemFilePtr();
	if ( i < 0 ) return i;

	tex = GetTex( i );
	bm->sx = tex->width;
	bm->sy = tex->height;
	bm->texid = i;

	return 0;
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

static void SetAlphaMode( int p_alpha )
{
	//		アルファブレンドオペレーション処理
	//
	if ( p_alpha < 2 ) {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	} else {
		d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	}
	switch( p_alpha ) {
	default:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case 5:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;
	case 6:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;
	case 7:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	}
}


static int GetCopyTexAlpha(BMSCR *bm)
{
	//		ブレント設定
	int alpha;

	SetAlphaMode(bm->gmode);
	if (bm->gmode < 3) {
		alpha = 0xff000000;
	}
	else {
		alpha = (bm->gfrate & 0xff) << 24;
	}
	return alpha;
}


static int GetCopyTexAlphaMes(BMSCR *bm)
{
	//		Alphaを含むテクスチャ用のブレント設定
	int alpha;
	int gmode;
	gmode = bm->gmode;
	if (gmode < 3) gmode = 3;
	SetAlphaMode(gmode);
	if (bm->gmode < 3) {
		alpha = 0xff000000;
	}
	else {
		alpha = (bm->gfrate & 0xff) << 24;
	}
	return alpha;
}


static int SetAlphaModeDG( int p_alpha )
{
	//		アルファブレンドオペレーション処理(DG用)
	//
	int alpha, alphaop;
	alphaop = p_alpha >> 8;
	alpha = (p_alpha & 255)<<24;
	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	switch( alphaop ) {
	case 0:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case 1:
	case 5:
		alpha = 0xff000000;
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case 2:
	case 6:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;
	case 3:
	case 7:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	case 4:
	case 8:
		d3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		d3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;
	}
	return alpha;
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
	int color;
	D3DEXVERTEXC *v;
	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex( -1 );
	SetAlphaMode( 0 );

	color = bm->color;
	linebasex = x;
	linebasey = y;

	v = vertex2DEXC;
	v[0].color = v[1].color = color;

	d3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEXC);
}


void hgio_line2( float x, float y )
{
	//		ライン描画
	//		(hgio_lineで開始後に必要な回数呼ぶ、hgio_line(NULL)で終了すること)
	//
	D3DEXVERTEXC *v;
	v = vertex2DEXC;
	v[0].x = (float)linebasex;
	v[0].y = (float)linebasey;
	v[1].x = (float)x;
	v[1].y = (float)y;

	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_LINELIST,1,vertex2DEXC,sizeof(D3DEXVERTEXC));

	linebasex = x;
	linebasey = y;
}


void hgio_boxfAlpha(BMSCR *bm, float x1, float y1, float x2, float y2, int alphamode)
{
	//		矩形描画
	//
	int col;
	D3DEXVERTEXC *v;

	if (bm == NULL) return;
	if (bm->type != HSPWND_TYPE_MAIN) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex(-1);

	if (alphamode == 0) {
		col = bm->color;
		SetAlphaMode(0);
	}
	else {
		col = GetCopyTexAlpha(bm) | (bm->color & 0xffffff);
	}

	v = vertex2DEXC;
	v[0].color = v[1].color = v[2].color = v[3].color = col;

	v[3].x = (float)x1;
	v[3].y = (float)y1;
	v[2].x = (float)x2;
	v[2].y = (float)y1;
	v[1].x = (float)x2;
	v[1].y = (float)y2;
	v[0].x = (float)x1;
	v[0].y = (float)y2;

	d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEXC);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex2DEXC, sizeof(D3DEXVERTEXC));
}


void hgio_boxf( BMSCR *bm, float x1, float y1, float x2, float y2 )
{
	hgio_boxfAlpha(bm, x1, y1, x2, y2, 0);
}


void hgio_circle( BMSCR *bm, float x1, float y1, float x2, float y2, int mode )
{
	//		円描画
	//
	D3DEXVERTEXC *v;
	D3DEXVERTEXC arScreen[CIRCLE_DIV + 2];
	int col;
	float x,y,rx,ry,sx,sy,rate;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	rate = D3DX_PI * 2.0f / (float)CIRCLE_DIV;
	sx = (float)fabs(x2-x1); sy = (float)fabs(y2-y1);
	rx = sx * 0.5f;
	ry = sy * 0.5f;
	x = x1 + rx;
	y = y1 + ry;

	ChangeTex( -1 );
	SetAlphaMode( 0 );
	col = bm->color;

	v = arScreen;
	for(int i = 1; i<=CIRCLE_DIV + 1; i ++) {
		v->x = x + (float)cos((float)i * rate)*rx;
		v->y = y + (float)sin((float)i * rate)*ry;
		v->z = 0.0f;
		v->color = col;
		v++;
	}

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEXC);
	// とりあえず直接描画(四角形)
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,CIRCLE_DIV,arScreen,sizeof(D3DEXVERTEXC));
	d3ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}


void hgio_fillrot( BMSCR *bm, float x, float y, float sx, float sy, float ang )
{
	//		矩形(回転)描画
	//
	D3DEXVERTEXC *v;
	float x0,y0,x1,y1,ofsx,ofsy;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ofsx = sx;
	ofsy = sy;
	x0 = -(float)sin( ang );
	y0 = (float)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

	ChangeTex( -1 );

	v = vertex2DEXC;
	v[0].color = v[1].color = v[2].color = v[3].color =  GetCopyTexAlpha( bm ) | (bm->color & 0xffffff);


	v->x = (-x0+x1) + x;
	v->y = (-y0+y1) + y;
	v++;

	v->x = (x0+x1) + x;
	v->y = (y0+y1) + y;
	v++;

	v->x = (x0-x1) + x;
	v->y = (y0-y1) + y;
	v++;

	v->x = (-x0-x1) + x;
	v->y = (-y0-y1) + y;
	v++;

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEXC);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2DEXC,sizeof(D3DEXVERTEXC));
}


void hgio_copy( BMSCR *bm, short xx, short yy, short srcsx, short srcsy, BMSCR *bmsrc, float s_psx, float s_psy )
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	D3DEXVERTEX *v;
	TEXINF *tex;
	int texid;
	float psx,psy;
	float x1,y1,x2,y2,sx,sy;
	float tx0,ty0,tx1,ty1;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) {
		//Alertf( "type%d #%d",bm->type,bm->wid );
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	if (drawflag == 0) hgio_render_start();

	if ( s_psx < 0.0 ) {
		psx = -s_psx;
		tx1 = ((float)xx);
		tx0 = ((float)(xx + srcsx));
	} else {
		psx = s_psx;
		tx0 = ((float)xx);
		tx1 = ((float)(xx + srcsx));
	}
	if ( s_psy < 0.0 ) {
		psy = -s_psy;
		ty1 = ((float)yy);
		ty0 = ((float)(yy + srcsy));
	} else {
		psy = s_psy;
		ty0 = ((float)yy);
		ty1 = ((float)(yy + srcsy));
	}

	x1 = ((float)bm->cx) - 0.5f;
	y1 = ((float)bm->cy) - 0.5f;
	x2 = x1 + psx;
	y2 = y1 + psy;

	texid = bmsrc->texid;
	ChangeTex( texid );
	tex = GetTex( texid );
	sx = tex->ratex;
	sy = tex->ratey;

	tx0 *= sx;
	tx1 *= sx;
	ty0 *= sy;
	ty1 *= sy;

	v = vertex2DEX;
	v[0].color = v[1].color = v[2].color = v[3].color = GetCopyTexAlpha( bm ) | ( bm->mulcolor );

	v[3].x = x1;
	v[3].y = y1;
	v[3].tu0 = tx0;
	v[3].tv0 = ty0;
	v[2].x = x2;
	v[2].y = y1;
	v[2].tu0 = tx1;
	v[2].tv0 = ty0;
	v[1].x = x2;
	v[1].y = y2;
	v[1].tu0 = tx1;
	v[1].tv0 = ty1;
	v[0].x = x1;
	v[0].y = y2;
	v[0].tu0 = tx0;
	v[0].tv0 = ty1;

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2DEX,sizeof(D3DEXVERTEX));
}


void hgio_fontcopy(BMSCR *bm, int x, int y, int psx, int psy, int texid, int basex, int basey)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	D3DEXVERTEX *v;
	TEXINF *tex;
	float x1, y1, x2, y2, sx, sy;
	float tx0, ty0, tx1, ty1;

	if (bm == NULL) return;
	if (bm->type != HSPWND_TYPE_MAIN) {
		//Alertf( "type%d #%d",bm->type,bm->wid );
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	if (drawflag == 0) hgio_render_start();

	tx0 = ((float)(basex));
	tx1 = ((float)(psx));
	ty0 = ((float)(basey));
	ty1 = ((float)(psy));

	x1 = ((float)x) - 0.5f;
	y1 = ((float)y) - 0.5f;
	x2 = x1 + psx;
	y2 = y1 + psy;

	ChangeTex(texid);
	tex = GetTex(texid);
	sx = tex->ratex;
	sy = tex->ratey;

	tx0 *= sx;
	tx1 *= sx;
	ty0 *= sy;
	ty1 *= sy;

	int col;
	if (GetSysReq(SYSREQ_FIXMESALPHA)) {
		col = bm->color;
		SetAlphaMode(2);
	}
	else {
		col = GetCopyTexAlphaMes(bm) | (bm->color & 0xffffff);
	}

	v = vertex2DEX;
	v[0].color = v[1].color = v[2].color = v[3].color = col;

	v[3].x = x1;
	v[3].y = y1;
	v[3].tu0 = tx0;
	v[3].tv0 = ty0;
	v[2].x = x2;
	v[2].y = y1;
	v[2].tu0 = tx1;
	v[2].tv0 = ty0;
	v[1].x = x2;
	v[1].y = y2;
	v[1].tu0 = tx1;
	v[1].tv0 = ty1;
	v[0].x = x1;
	v[0].y = y2;
	v[0].tu0 = tx0;
	v[0].tv0 = ty1;

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex2DEX, sizeof(D3DEXVERTEX));
}


void hgio_copyrot( BMSCR *bm, short xx, short yy, short srcsx, short srcsy, float s_ofsx, float s_ofsy, BMSCR *bmsrc, float psx, float psy, float ang )
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	D3DEXVERTEX *v;
	TEXINF *tex;
	int texpx,texpy,texid;
	float x,y,x0,y0,x1,y1,ofsx,ofsy,mx0,mx1,my0,my1;
	float tx0,ty0,tx1,ty1,sx,sy;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	mx0=-(float)sin( ang );
	my0=(float)cos( ang );
	mx1 = -my0;
	my1 = mx0;

	ofsx = -s_ofsx;
	ofsy = -s_ofsy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	//		基点の算出
	x = ( (float)bm->cx - (-x0+x1) );
	y = ( (float)bm->cy - (-y0+y1) );

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
	ChangeTex( texid );
	tex = GetTex( texid );
	sx = tex->ratex;
	sy = tex->ratey;
	texpx = xx + srcsx;
	texpy = yy + srcsy;

	tx0 = ((float)xx) * sx;
	ty0 = ((float)yy) * sy;
	tx1 = ((float)(texpx)) * sx;
	ty1 = ((float)(texpy)) * sy;

	v = vertex2DEX;
	v[0].color = v[1].color = v[2].color = v[3].color = GetCopyTexAlpha( bm ) |  ( bm->mulcolor );

	/*-------------------------------*/

	v->x = ((-x0+x1) + x);
	v->y = ((-y0+y1) + y);
	v->tu0 = tx1;
	v->tv0 = ty1;
	v++;

	/*-------------------------------*/

	v->x = ((x1) + x);
	v->y = ((y1) + y);
	v->tu0 = tx1;
	v->tv0 = ty0;
	v++;

	/*-------------------------------*/

	v->x = (x);
	v->y = (y);
	v->tu0 = tx0;
	v->tv0 = ty0;
	v++;

	/*-------------------------------*/

	v->x = ((-x0) + x);
	v->y = ((-y0) + y);
	v->tu0 = tx0;
	v->tv0 = ty1;
	v++;

	/*-------------------------------*/

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2DEX,sizeof(D3DEXVERTEX));
}


void hgio_setfilter( int type, int opt )
{
	int ft;
	switch( type ) {
	case HGIO_FILTER_TYPE_LINEAR:
		ft = D3DTEXF_LINEAR;
		break;
	case HGIO_FILTER_TYPE_LINEAR2:
		ft = D3DTEXF_FLATCUBIC;
		break;
	default:
		ft = D3DTEXF_POINT;
		break;
	}
	d3ddev->SetTextureStageState( 0, D3DTSS_MAGFILTER, ft  );
	d3ddev->SetTextureStageState( 0, D3DTSS_MINFILTER, ft  );
}



void hgio_square_tex( BMSCR *bm, int *posx, int *posy, BMSCR *bmsrc, int *uvx, int *uvy )
{
	//		四角形(square)テクスチャ描画
	//
	D3DEXVERTEX *v;
	TEXINF *tex;
	int texid;
	float sx,sy;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	texid = bmsrc->texid;
	ChangeTex( texid );
	tex = GetTex( texid );
	sx = tex->ratex;
	sy = tex->ratey;

	v = vertex2DEX;
	v[0].color = v[1].color = v[2].color = v[3].color = GetCopyTexAlpha( bm ) | 0xffffff;

	v->x = (float)posx[2];
	v->y = (float)posy[2];
	v->tu0 = ((float)uvx[2]) * sx;
	v->tv0 = ((float)uvy[2]) * sy;
	v++;
	v->x = (float)posx[1];
	v->y = (float)posy[1];
	v->tu0 = ((float)uvx[1]) * sx;
	v->tv0 = ((float)uvy[1]) * sy;
	v++;
	v->x = (float)posx[0];
	v->y = (float)posy[0];
	v->tu0 = ((float)uvx[0]) * sx;
	v->tv0 = ((float)uvy[0]) * sy;
	v++;
	v->x = (float)posx[3];
	v->y = (float)posy[3];
	v->tu0 = ((float)uvx[3]) * sx;
	v->tv0 = ((float)uvy[3]) * sy;

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEX);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2DEX,sizeof(D3DEXVERTEX));
}


void hgio_square( BMSCR *bm, int *posx, int *posy, int *color )
{
	//		四角形(square)単色描画
	//
	D3DEXVERTEXC *v;
	int basecolor;

	if ( bm == NULL ) return;
	if ( bm->type != HSPWND_TYPE_MAIN ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if (drawflag == 0) hgio_render_start();

	ChangeTex( -1 );

	v = vertex2DEXC;
	basecolor = GetCopyTexAlpha( bm );

	v[2].color = basecolor | ( color[0] & 0xffffff );
	v[1].color = basecolor | ( color[1] & 0xffffff );
	v[0].color = basecolor | ( color[2] & 0xffffff );
	v[3].color = basecolor | ( color[3] & 0xffffff );

	v->x = (float)posx[2];
	v->y = (float)posy[2];
	v++;
	v->x = (float)posx[1];
	v->y = (float)posy[1];
	v++;
	v->x = (float)posx[0];
	v->y = (float)posy[0];
	v++;
	v->x = (float)posx[3];
	v->y = (float)posy[3];

	//デバイスに使用する頂点フォーマットをセットする
	d3ddev->SetVertexShader(D3DFVF_EXVERTEXC);
	// とりあえず直接描画(四角形)
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertex2DEXC,sizeof(D3DEXVERTEXC));
}


int hgio_gettick( void )
{
	return timeGetTime();
}


int hgio_exec( char *stmp, char *option, int mode )
{
	int i,j;
	j=SW_SHOWDEFAULT;if (mode&2) j=SW_SHOWMINIMIZED;

	if ( *option != 0 ) {
		SHELLEXECUTEINFO exinfo;
		memset( &exinfo, 0, sizeof(SHELLEXECUTEINFO) );
		exinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		exinfo.fMask = SEE_MASK_INVOKEIDLIST;
		exinfo.hwnd = master_wnd;
		exinfo.lpVerb = option;
		exinfo.lpFile = stmp;
		exinfo.nShow = SW_SHOWNORMAL;
		if ( ShellExecuteEx( &exinfo ) == false ) throw HSPERR_EXTERNAL_EXECUTE;
		return 0;
	}
		
	if ( mode&16 ) {
		i = (int)ShellExecute( NULL,NULL,stmp,"","",j );
	}
	else if ( mode&32 ) {
		i = (int)ShellExecute( NULL,"print",stmp,"","",j );
	}
	else {
		i=WinExec( stmp,j );
	}
	if (i<32) throw HSPERR_EXTERNAL_EXECUTE;
	return 0;
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
	int i;
	MATRIX *vmat;
	MATRIX tmpmat;
	float* vp;
	vmat = &mat_proj;
	vp = (float*)vmat;
	float* mat = (float*)GetCurrentMatrixPtr();

	switch (bm->vp_flag) {
	case BMSCR_VPFLAG_2D:
		//	2D projection mode
		UnitMatrix();
		RotZ(bm->vp_viewrotate[2]);
		GetCurrentMatrix(&tmpmat);
		OrthoMatrix(-bm->vp_viewtrans[0], -bm->vp_viewtrans[1], (float)nDestWidth / bm->vp_viewscale[0], (float)nDestHeight / bm->vp_viewscale[1], 0.0f, 1.0f);
		MulMatrix(&tmpmat);
		break;
	case BMSCR_VPFLAG_3D:
		//	3D projection mode
		UnitMatrix();
		RotZ(bm->vp_viewrotate[2]);
		RotY(bm->vp_viewrotate[1]);
		RotX(bm->vp_viewrotate[0]);
		Scale(bm->vp_viewscale[0], bm->vp_viewscale[1], bm->vp_viewscale[2]);
		Trans(bm->vp_viewtrans[0], bm->vp_viewtrans[1], bm->vp_viewtrans[2]);
		GetCurrentMatrix(&tmpmat);
		PerspectiveFOV(bm->vp_view3dprm[0], bm->vp_view3dprm[1], bm->vp_view3dprm[2], 0.0f, 0.0f, (float)nDestWidth / 10, (float)nDestHeight / 10);
		//PerspectiveFOV(45.0f, 1.0f, 500.0f, 0.0f, 0.0f, (float)nDestWidth / 10, (float)nDestHeight / 10);
		//PerspectiveWithZBuffer(10.0f, 0.0f, 60.0f, 1.0f, 0.0f);
		MulMatrix(&tmpmat);
		break;
	case BMSCR_VPFLAG_MATRIX:
		//	user matrix mode
		mat = &bm->vp_viewtrans[0];
		break;
	case BMSCR_VPFLAG_NOUSE:
	default:
		return;
	}

	//	mat_projに設定する
	for (i = 0; i < 16; i++) {
		*vp++ = *mat++;
	}

	D3DXMATRIX matrixProj;
	Mat2D3DMAT(&matrixProj, vmat);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matrixProj);

	//	投影マトリクスの逆行列を設定する
	//D3DXMatrixInverse(&InvViewport, NULL, &matrixProj);
	SetCurrentMatrix(vmat);
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
	if (bm->cy >= bm->sy) return -1;

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
	if (UpdateTex32(id, (char*)buffer, 0) < 0) return -1;
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

	int len = strlen(str)+1;
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
		char *p = code_stmp(GlobalSize(hg));
		strClip = (char *)GlobalLock(hg);
		strcpy(p, strClip);
		GlobalUnlock(hg);
		CloseClipboard();
		return p;
	}
	return NULL;
}


