//
//		Texture manager (directX8)
//			onion software/onitama 2001/6
//
#include <stdio.h>
#define STRICT
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <math.h>

#include "hgtex.h"

#include "../sysreq.h"
#include "../supio.h"

#define RELEASE(x) 	if(x){x->Release();x=NULL;}


//		Data
//
static		TEXINF texinf[TEXINF_MAX];
static		char ck1;			// カラーキー1
static		char ck2;			// カラーキー2
static		char ck3;			// カラーキー3

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID

static		D3DDISPLAYMODE target_disp;
static		LPDIRECT3D8 d3d;
static		LPDIRECT3DDEVICE8 d3ddev;

static		DWORD AlphaTbl[34];
static		int FixedFontWidth;
static		HFONT htexfont;		// TEXTURE用のフォント
static		HFONT htexfont_old;	// TEXTURE用のフォント(保存用)
static		LPBYTE lpFont;
static		HDC htexdc;
static		HWND htexwnd;
static		int htexsize;
static		int lasttex;
static		D3DLOCKED_RECT MemTexRect;	// テクスチャ作業バッファ
static		int drawsx, drawsy;	// 描画サイズ
static		int spacing;		// 文字の間隔(dot)
static		int linespace;		// 行の間隔(dot)
static		int fontoption;		// フォント描画オプション
static		int fontrot;		// 回転オプション
static		TEXTMETRIC tm;


/*------------------------------------------------------------*/
/*
		texture process
*/
/*------------------------------------------------------------*/

void TexInit( void )
{
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		texinf[i].flag = TEXMODE_NONE;
	}

	TexReset();
	lpFont = (LPBYTE)malloc( 0x10000 );			// フォント取得用のワーク
}


void TexSetD3DParam( LPDIRECT3D8 p1, LPDIRECT3DDEVICE8 p2, D3DDISPLAYMODE p3 )
{
	d3d = p1;
	d3ddev = p2;
	target_disp = p3;
}


void TexTerm( void )
{
	int i;
	d3ddev->SetTexture( 0, NULL );
	if ( lpFont != NULL ) { free( lpFont ); lpFont = NULL; }
	for(i=0;i<TEXINF_MAX;i++) {
		DeleteTex( i );
	}
}


void TexReset( void )
{
	curtex = -1;
	d3ddev->SetTexture( 0, NULL );
	FixedFontWidth = 0;
	htexfont = NULL;
	htexsize = 0;
	//lpFont = NULL;
}


int GetNextTexID( void )
{
	int i,sel;
	sel = -1;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( texinf[i].flag == TEXMODE_NONE ) { sel=i;break; }
	}
	return sel;
}


void SetTex( int sel, int flag, int sw, int sx, int sy, int width, int height, void *pTex )
{
	TEXINF *t;
	t = GetTex( sel );
	t->flag = flag;
	t->mode = sw;
	t->sx = sx;
	t->sy = sy;
	t->width = width;
	t->height = height;
	t->data = (char *)pTex;
	t->ratex = 1.0f / (float)sx;
	t->ratey = 1.0f / (float)sy;
	TexDivideSize( sel, 0, 0, 0, 0 );
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
	int vmode = 0;	//GetSysReq( SYSREQ_RESVMODE );
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
	

static int Get2N( int val )
{
	int res = 1;
	while(1) {
		if ( res >= val ) break;
		res<<=1;
	}
	return res;
}


int CreateTexture2( int w, int h, D3DFORMAT Format, LPDIRECT3DTEXTURE8 &pTexture )
{
	int sx,sy;
	HRESULT hr;
	sx = Get2N( w );
	sy = Get2N( h );
	hr = d3ddev->CreateTexture( sx, sy, 1, 0,Format, D3DPOOL_MANAGED, &pTexture );
	if SUCCEEDED( hr ) return 0;
	if ( sx < sy ) sx=sy;
	hr = d3ddev->CreateTexture( sx, sx, 1, 0,Format, D3DPOOL_MANAGED, &pTexture );
	if SUCCEEDED( hr ) return 0;
	return -1;
}


int RegistTexFromMemFile( char *data, int size, int orgx, int orgy )
{
	int sel,sx,sy,px,py;
	HRESULT hr;
	LPDIRECT3DTEXTURE8 t;
	D3DSURFACE_DESC desc;
	D3DFORMAT format;
	D3DXIMAGE_INFO info;

	format = D3DFMT_A8R8G8B8;
	//format = D3DFMT_A4R4G4B4;
	//GetSysReq(SYSREQ_NOMIPMAP) = 0
	hr = D3DXCreateTextureFromFileInMemoryEx( d3ddev, data, size, 0, 0, 0, 0, format,
								D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE,
								0, &info, NULL, &t );
//	hr = D3DXCreateTextureFromFileInMemoryEx( d3ddev, data, size, 0, 0, 0, 0, format,
//								D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
//								0, &info, NULL, &t );
	if ( FAILED(hr) ) return -1;

	t->GetLevelDesc( 0, &desc );
	sx = desc.Width;
	sy = desc.Height;
	//Alertf( "Lev[%d][%d][%d][%d][%d]", t->GetLevelCount(),sx,sy,orgx,orgy );
	px = orgx; if ( px <= 0 ) px = sx;
	py = orgy; if ( py <= 0 ) py = sy;
	//px = orgx; if ( px <= 0 ) px = info.Width;
	//py = orgy; if ( py <= 0 ) py = info.Height;

	sel = GetNextTexID();
	SetTex( sel, TEXMODE_NORMAL, 0, px, py, info.Width, info.Height, t );
	return sel;
}


int RegistTex( char *data, int sx, int sy, int width, int height, int sw )
{
	int sel,y,i;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;
	char *p;
	char *src;
	D3DFORMAT textype;

	if ( sw < 0 ) {
		return RegistTexFromMemFile( data, sx, width, height );
	}

	//		カラーキー更新
	//
	i = 0;
	//i = GetSysReq( SYSREQ_COLORKEY );
	ck1 = i & 0xff;
	ck2 = (i>>8) & 0xff;
	ck3 = (i>>16) & 0xff;

	//		フォーマット変換
	//
	textype = GetAvailableFormat( 0 );
	if ( textype==D3DFMT_UNKNOWN ) return -2;

	//hr = d3ddev->CreateTexture( sx,sy,1,0, textype, D3DPOOL_MANAGED, &pTex );
	i = CreateTexture2( sx, sy, textype, pTex );
	if ( i ) return -1;

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
	SetTex( sel, TEXMODE_NORMAL, sw, sx, sy, width, height, pTex );
	return sel;
}


int RegistTexIndex( char *data, char *palette, int sx, int sy, int width, int height, int sw, int pals )
{
	int sel,y,i;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;
	char *p;
	char *src;
	D3DFORMAT textype;

	textype = GetAvailableFormat( 1 );
	if ( textype==D3DFMT_UNKNOWN ) return -2;

	i = CreateTexture2( sx, sy, textype, pTex );
	if ( i ) return -1;

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
	SetTex( sel, TEXMODE_NORMAL, sw, sx, sy, width, height, pTex );

	return sel;
}


int UpdateTex( int texid, char *data, int sw )
{
	int y,i;
	int sx,sy,width,height;
	TEXINF *texinf;
	HRESULT hr;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;
	char *p;
	char *src;
	D3DFORMAT textype;

	//		カラーキー更新
	//
	//i = GetSysReq( SYSREQ_COLORKEY );
	i = 0;
	ck1 = i & 0xff;
	ck2 = (i>>8) & 0xff;
	ck3 = (i>>16) & 0xff;

	texinf = GetTex( texid );
	sx = texinf->sx;
	sy = texinf->sy;
	width = texinf->width;
	height = texinf->height;
	pTex = (LPDIRECT3DTEXTURE8)texinf->data;


	//		フォーマット変換
	//
	textype = GetAvailableFormat( 0 );
	if ( textype==D3DFMT_UNKNOWN ) return -2;

	hr = pTex->LockRect( 0,&lock,NULL,0 );
	if ( FAILED( hr ) ) return -1;
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
	return 0;
}


void DeleteTex( int id )
{
	TEXINF *t;
	LPDIRECT3DTEXTURE8 pTex;
	t = GetTex( id );
	if ( t->flag == TEXMODE_NONE ) return;
	t->flag = TEXMODE_NONE;
	pTex = (LPDIRECT3DTEXTURE8)t->data;
	RELEASE( pTex );
}


void ChangeTex( int id )
{
	TEXINF *t;
	if ( id < 0 ) {
		curtex=-1;
		d3ddev->SetTexture( 0, NULL );
		return;
	}
	if ( curtex == id ) return;
	t = GetTex( id );
	if ( t->flag == TEXMODE_NONE ) {
		curtex=-1;
		d3ddev->SetTexture( 0, NULL );
		return;
	}
	curtex = id;
	d3ddev->SetTexture( 0, (LPDIRECT3DTEXTURE8)t->data );
}


TEXINF *GetTex( int id )
{
	return &texinf[id];
}


void SetSrcTex( void *src, int sx, int sy )
{
	lpTex = (char *)src;
}


//======================================================================
//
//		テクスチャ・メッセージ関連
//
//======================================================================

int RegistTexEmpty( int w, int h, int tmode )
{
	// メッセージエリア用のテクスチャ作成
	int sel,sx,sy,flag;
	D3DSURFACE_DESC desc;
	LPDIRECT3DTEXTURE8 lpTex;

	if ( tmode==0 ) {
		if ( CreateTexture2( w,h, D3DFMT_A4R4G4B4, lpTex ) ) {
			if ( CreateTexture2( w,h, D3DFMT_A8R8G8B8, lpTex ) ) {
				RELEASE( lpTex );
				return -1;
			}
		}
		
	} else {
		if ( CreateTexture2( w,h, D3DFMT_A8R8G8B8, lpTex ) ) {
			if ( CreateTexture2( w,h, D3DFMT_A4R4G4B4, lpTex ) ) {
				RELEASE( lpTex );
				return -1;
			}
		}
	}

	lpTex->GetLevelDesc( 0, &desc );
	// データ
	sx = desc.Width;
	sy = desc.Height;
	flag = TEXMODE_MES8;
	if ( desc.Format == D3DFMT_A4R4G4B4 ) flag = TEXMODE_MES4;

	sel = GetNextTexID();
	SetTex( sel, flag, 0, sx, sy, sx, sy, lpTex );
	return sel;
}



void ClearTex( int id, int color )
{
	int sx,sy,i,j;
	TEXINF *texinf;
	DWORD pitch;
	D3DLOCKED_RECT TexRect;
	LPDIRECT3DTEXTURE8 lpTex;
	HRESULT hr;

	texinf = GetTex( id );
	if ( texinf->flag == TEXMODE_NONE ) return;
	lpTex = (LPDIRECT3DTEXTURE8)texinf->data;
	sx = texinf->sx;
	sy = texinf->sy;

	hr = lpTex->LockRect( 0, &TexRect, NULL, 0 );
	if FAILED( hr ) { lpTex->UnlockRect( 0 ); return; }

	if ( color == 0 ) {
		memset( TexRect.pBits, 0x00, TexRect.Pitch * sy );
	} else {

		switch ( texinf->flag ) {
		case TEXMODE_MES8:
		{
			LPDWORD p1 = (LPDWORD)TexRect.pBits;
			pitch = TexRect.Pitch / 4;
			for ( i = 0; i < sy ; i++ ) {
				for ( j = 0; j < sx; j++ ) {
					p1[j] = color;
				}
				p1 += pitch;
			}
			break;
		}
		case TEXMODE_MES4:
		{
			LPWORD p1 = (LPWORD)TexRect.pBits;
			WORD a = (WORD)((color>>24)&0xf0);
			WORD r = (WORD)((color>>16)&0xf0);
			WORD g = (WORD)((color>> 8)&0xf0);
			WORD b = (WORD)((color    )&0xf0);
			a>>=4; r>>=4; g>>=4; b>>=4;
			WORD c = (WORD)( (a<<12) | (r<<8) | (g<<4) | b );
			pitch = TexRect.Pitch / 2;
			for ( i = 0; i < sy ; i++ ) {
				for ( j = 0; j < sx; j++ ) {
					p1[j] = c;
				}
				p1 += pitch;
			}
			break;
		}
		}
	}
	lpTex->UnlockRect( 0 );
}


void SendMesTex( int id, char *vram )
{
	TEXINF *texinf;
	D3DLOCKED_RECT TexRect;
	LPDIRECT3DTEXTURE8 lpTex;
	HRESULT hr;
	LPBYTE src;
	LPBYTE dst;
	int i,j,sx,sy,size;
	int pitch;

	texinf = GetTex( id );
	if ( texinf->flag == TEXMODE_NONE ) return;
	lpTex = (LPDIRECT3DTEXTURE8)texinf->data;
	sx = texinf->width;
	sy = texinf->height;

	hr = lpTex->LockRect( 0, &TexRect, NULL, 0 );
	if FAILED( hr ) { lpTex->UnlockRect( 0 ); return; }

	src = (LPBYTE)vram;
	size = sx * sy;
	dst = (LPBYTE)TexRect.pBits;
	pitch = TexRect.Pitch;
	dst += pitch * ( sy - 1 );

	switch ( texinf->flag ) {
	case TEXMODE_MES8:
		{
			for ( i = 0; i < sy ; i++ ) {
				for ( j = 0; j < sx; j++ ) {
					*dst++ = *src++;
					*dst++ = *src++;
					*dst++ = *src++;
					*dst++ = 0;
				}
				dst -= pitch + (sx*4);
			}
			break;
		}
		case TEXMODE_MES4:
		{
			WORD a,r,g,b;
			a = 0;
			for ( i = 0; i < sy ; i++ ) {
				for ( j = 0; j < sx; j++ ) {
					b = (WORD)*src++;
					g = (WORD)*src++;
					r = (WORD)*src++;
					r>>=4; g>>=4; b>>=4;
					*dst++ = (BYTE)( (g<<4) | b );
					*dst++ = (BYTE)( (a<<4) | (r) );
				}
				dst -= pitch + (sx*2);
			}
			break;
		}
	}
	lpTex->UnlockRect( 0 );
}


static long GetCharCode( unsigned char *str )
{
	//		キャラコード取得
	long uiCharacter =
			( (*(long *)str ) & 0x000000ff ) << 8 |
			( (*(long *)str ) & 0x0000ff00 ) >> 8;

	return uiCharacter;
}


static bool IsKanji( unsigned char c )
{
	//		全角チェック
	if ( c < 0x81 ) return false;
	if ( c < 0xA0 ) return true;
	if ( c < 0xE0 ) return false;
	if ( c < 0xFF ) return true;
	return false;
}


static DWORD GetFontBuffer( HDC hdc, long code, GLYPHMETRICS *pgm, LPBYTE lpData )
{
	//		フォントバッファ取得
	MAT2 mat;
	DWORD Size;
	ZeroMemory( pgm, sizeof(GLYPHMETRICS) );

	long m11 = (long)(1.0 * 65536.0);	long m12 = (long)(0.0 * 65536.0);
    long m21 = (long)(0.0 * 65536.0);	long m22 = (long)(1.0 * 65536.0);
	mat.eM11 = *( (FIXED *)&m11 );	mat.eM12 = *( (FIXED *)&m12 );
	mat.eM21 = *( (FIXED *)&m21 );	mat.eM22 = *( (FIXED *)&m22 );

	
	// バッファサイズ受信
	Size = GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, pgm, 0, NULL, &mat );
	// バッファ取得
	GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, pgm, Size, lpData, &mat );

	return Size;
}


static DWORD GetFontBufferBW( HDC hdc, long code, GLYPHMETRICS *pgm, LPBYTE lpData )
{
	//		フォントバッファ取得(アンチなし)
	MAT2 mat;
	DWORD Size;
	ZeroMemory( pgm, sizeof(GLYPHMETRICS) );

	long m11 = (long)(1.0 * 65536.0);	long m12 = (long)(0.0 * 65536.0);
    long m21 = (long)(0.0 * 65536.0);	long m22 = (long)(1.0 * 65536.0);
	mat.eM11 = *( (FIXED *)&m11 );	mat.eM12 = *( (FIXED *)&m12 );
	mat.eM21 = *( (FIXED *)&m21 );	mat.eM22 = *( (FIXED *)&m22 );

	// バッファサイズ受信
	Size = GetGlyphOutline( hdc, code, GGO_BITMAP, pgm, 0, NULL, &mat );
	// バッファ取得
	GetGlyphOutline( hdc, code, GGO_BITMAP, pgm, Size, lpData, &mat );

	return Size;
}


static void SendTextBuffer( TEXINF *tex, char *src, int sx, int sy )
{
	//		テキスト描画
	DWORD x,y,yy,Width;
	BYTE *p;
	BYTE *s;
	BYTE *stmp;
	BYTE *dst;
	HRESULT hr;
	D3DLOCKED_RECT TexRect;
	LPDIRECT3DTEXTURE8 lpTex;
	int pitch;

	lpTex = (LPDIRECT3DTEXTURE8)tex->data;
	hr = lpTex->LockRect( 0, &TexRect, NULL, 0 );
	if FAILED( hr ) return;

	dst = (BYTE *)TexRect.pBits;

	Width = tex->sx;
	yy = tex->sy;
	if ( yy > (DWORD)sy ) yy=sy;

	pitch = TexRect.Pitch;
	switch ( tex->flag ) {
	case TEXMODE_MES4:
		BYTE a1,a2,a3;
		stmp = (BYTE *)src;
		for(y=0;y<yy;y++) {
			s = stmp + ((yy-1-y)*sx*3); p = dst;
			for(x=0;x<Width;x++) {
				a1 = *s++; a2 = *s++; a3 = *s++;
				*p++ = (a2&0xf0)|(a1>>4);
				*p++ = (a3>>4)|0xf0;
			}
			dst+=pitch;
		}
		break;
	case TEXMODE_MES8:
		stmp = (BYTE *)src;
		for(y=0;y<yy;y++) {
			s = stmp + ((yy-1-y)*sx*3); p = dst;
			for(x=0;x<Width;x++) {
				*p++ = *s++;
				*p++ = *s++;
				*p++ = *s++;
				*p++ = 0xff;
			}
			dst+=pitch;
		}
		break;
	}
	lpTex->UnlockRect( 0 );
}


void DrawTexOpen( HWND hwnd, int id, char *fontname, int size, int option )
{
	TEXINF *tex;
	HRESULT hr;
	LPDIRECT3DTEXTURE8 lpTex;
	int fw;

	htexwnd = hwnd;
	//DrawTexClose();
	htexdc = GetDC( htexwnd );
	htexsize = size;
	spacing = 0;
	linespace = 0;
	fontoption = option;
	fontrot = 0;

	if ( size >= 0 ) {
		switch( option & 3 ) {
		case 1:
			fw = FW_BOLD;
			break;
		case 2:
			fw = FW_ULTRABOLD;
			break;
		case 3:
			fw = FW_HEAVY;
			break;
		default:
			fw = FW_REGULAR;
			break;
		}

		htexfont = CreateFont(
				size,						// フォント高さ
				0,							// 文字幅
				0,							// テキストの角度	
				0,							// ベースラインとｘ軸との角度
				fw,							// フォントの重さ（太さ）
				((option&TEXFONT_OPT_ITALIC)!=0),
											// イタリック体
				FALSE,						// アンダーライン
				FALSE,						// 打ち消し線
				DEFAULT_CHARSET,			// 文字セット
				OUT_TT_PRECIS,				// 出力精度
				CLIP_DEFAULT_PRECIS,		// クリッピング精度
				PROOF_QUALITY,				// 出力品質
				DEFAULT_PITCH | FF_MODERN,	// ピッチとファミリー
				fontname					// 書体名
			);
	} else {
		htexfont = (HFONT)fontname;
	}
	lasttex = id;
	htexfont_old = (HFONT)SelectObject( htexdc, htexfont );

	//		テクスチャをロックする
	tex = GetTex( id );
	if ( tex->flag == TEXMODE_NONE ) { lasttex = -1; return; }
	lpTex = (LPDIRECT3DTEXTURE8)tex->data;

	if ( option & 0x10000 ) {
		lasttex = -1;
		return;
	}
	hr = lpTex->LockRect( 0, &MemTexRect, NULL, 0 );
	if FAILED( hr ) { lasttex = -1; return; }
}


void DrawTexClose( void )
{
	TEXINF *tex;
	LPDIRECT3DTEXTURE8 lpTex;
	if ( lasttex != -1 ) {
		tex = GetTex( lasttex );
		lpTex = (LPDIRECT3DTEXTURE8)tex->data;
		lpTex->UnlockRect( 0 );
	}
	//
	if ( htexfont == NULL ) return;
	SelectObject( htexdc, htexfont_old );
	if ( htexsize >= 0 ) DeleteObject( htexfont );
	ReleaseDC( htexwnd, htexdc );
	htexfont = NULL;
}


static void MakeAlphaTable( int format, int TextColor )
{
	//	アンチエイリアスカラーテーブルを作成
	DWORD rval,gval,bval,iv;
	rval = (DWORD)((TextColor>>16)&0xff);
	gval = (DWORD)((TextColor>> 8)&0xff);
	bval = (DWORD)((TextColor    )&0xff);

	if (( TextColor & 0xff000000 ) == 0 ) {
		for ( DWORD ia = 0; ia <= 32; ia++ ) {
			AlphaTbl[ia] = 0;
		}
		return;
	}

	switch( format ) {
	case TEXMODE_MES8:
		for ( DWORD ia = 0; ia <= 32; ia++ )
		{
			iv = (ia); if ( iv>15 ) iv=15;
			AlphaTbl[ia] = (DWORD)( (iv<<28)|
	                           ((rval)<<16)|
	                           ((gval)<<8)|
	                           (bval));
		}
		break;
	case TEXMODE_MES4:
		rval>>=4; gval>>=4; bval>>=4;
		for ( DWORD ia = 0; ia <= 32; ia++ )
		{
			iv = (ia); if ( iv>15 ) iv=15;
			AlphaTbl[ia] = (DWORD)( (iv<<12)|
	                           ((rval)<<8)|
	                           ((gval)<<4)|
	                           (bval));
		}
		break;
	}
}


void DrawTexColor( DWORD TextColor )
{
	TEXINF *tex;
	if ( lasttex < 0 ) return;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return;
	MakeAlphaTable( tex->flag, TextColor );
}


void DrawTexSpacing( int offset )
{
	TEXINF *tex;
	if ( lasttex < 0 ) return;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return;
	spacing = offset;
}


void DrawTexLineSpacing( int offset )
{
	TEXINF *tex;
	if ( lasttex < 0 ) return;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return;
	linespace = offset;
}


void DrawTexRotate( int rotopt )
{
	fontrot = rotopt;
}


void DrawTexRect( int s_xx, int s_yy, int s_sx, int s_sy, DWORD color )
{
	//		単色の矩形を描画する(テクスチャ)
	//
	TEXINF *tex;
	int Format, x, y;
	int xx,yy,sx,sy;
	DWORD rval,gval,bval,aval;

	if ( lasttex < 0 ) return;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return;

	xx = s_xx;
	yy = s_yy;
	sx = s_sx;
	sy = s_sy;
	//		クリッピング
	if ( xx < 0 ) { sx+=xx; xx=0; }
	if ( yy < 0 ) { sy+=yy; yy=0; }
	if ( xx >= tex->sx ) return;
	if ( yy >= tex->sy ) return;
	if (( sx <= 0 )||( sy <= 0 )) return;
	if ( (xx+sx) > tex->sx ) { sx = tex->sx - xx; }
	if ( (yy+sy) > tex->sy ) { sy = tex->sy - yy; }

	Format = tex->flag;
	aval = (DWORD)((color>>24)&0xff);
	rval = (DWORD)((color>>16)&0xff);
	gval = (DWORD)((color>> 8)&0xff);
	bval = (DWORD)((color    )&0xff);

	switch ( Format ) {
	case TEXMODE_MES4:
		{
		// 転送先のサーフェイスの始点(16bit)
		LPWORD p1 = (LPWORD)MemTexRect.pBits;
		DWORD pitch = MemTexRect.Pitch / 2;
		WORD cval;
		rval>>=4; gval>>=4; bval>>=4;aval>>=4;
		cval = (WORD)( (aval<<12)|(rval<<8)|(gval<<4)|bval );
		p1 += (xx) + ((tex->sy-1-yy) * pitch);
		for(y=0;y<sy;y++) {
			for(x=0;x<sx;x++) {
				p1[x] = cval;
			}
			p1-=pitch;
		}
		break;
		}
	case TEXMODE_MES8:
		{
		// 転送先のサーフェイスの始点(32bit)
		LPDWORD p1 = (LPDWORD)MemTexRect.pBits;
		DWORD pitch = MemTexRect.Pitch / 4;
		p1 += (xx) + ((tex->sy-1-yy) * pitch);
		for(y=0;y<sy;y++) {
			for(x=0;x<sx;x++) {
				p1[x] = color;
			}
			p1-=pitch;
		}
		break;
		}
	}
}


int DrawTexStringSub( HDC hdc, TEXINF *tex, int x, int y, long code )
{
	int Format;
	int px, ybase;
	int width,height;
	int tmpy;
	GLYPHMETRICS gm;

	//tmpx = x + tm.tmMaxCharWidth;
	tmpy = y + tm.tmHeight;
	if (( x >= tex->sx )||( tmpy < 0 )||( y >= tex->sy )) return 0;		// はみ出しチェック

	Format = tex->flag;
	if ( fontoption & TEXFONT_OPT_NOANTI ) {
		// アンチなし
		// フォントバッファ取得
		DWORD DataSize = GetFontBufferBW( hdc, code, &gm, lpFont );

		LPBYTE p2 = lpFont;
		// フォントピッチ
		DWORD fontPitch = (DataSize / gm.gmBlackBoxY) & ~0x03;

		// サイズ取得
		width = (int)gm.gmBlackBoxX;
		height = (int)gm.gmBlackBoxY;

		// 描画位置を進める量
		if ( FixedFontWidth == 0 ) {
			px = gm.gmCellIncX + spacing;
			//px = gm.gmBlackBoxX + gm.gmptGlyphOrigin.x + spacing;
		} else {
			px = FixedFontWidth + spacing;
		}
		int sy = tm.tmAscent - gm.gmptGlyphOrigin.y;
		ybase = y+sy;
		if (  (x+px) >= tex->sx ) return -1;
		if (( x<0 )||( y<0 )||( (y+sy+height)>tex->sy )) return px;
		if ( ybase >= tex->sy ) return px;

		if ( fontoption & TEXFONT_OPT_NOCOLOR ) {
			// αのみ更新
			switch ( Format )
			{
			case TEXMODE_MES4:
			{
				LPWORD p1 = (LPWORD)MemTexRect.pBits;
				DWORD pitch = MemTexRect.Pitch / 2;
				WORD pix;
				LPBYTE pp;
				int bmask;
				// 転送先のサーフェイスの始点
				p1 += (x + gm.gmptGlyphOrigin.x) + (ybase * pitch);
				pix = (WORD)AlphaTbl[31];
				pix = (pix&15)<<12;
				for ( int y = 0; y < height ; y++ )
				{
					bmask = 0x80; pp = p2;
					for ( int x = 0; x < width; x++ )
					{
						if ( bmask == 0 ) { bmask = 0x80; pp++; }
						if ( *pp & bmask ) { p1[x] = ( p1[x] & 0xfff ) | pix; }
						bmask>>=1;
					}
					p1 += pitch;
					p2 += fontPitch;
				}
			}
			break;
			case TEXMODE_MES8:
			{
				LPDWORD p1 = (LPDWORD)MemTexRect.pBits;
				DWORD pitch = MemTexRect.Pitch / 4;
				DWORD pix;
				LPBYTE pp;
				int bmask;
				// 転送先のサーフェイスの始点
				p1 += (x + gm.gmptGlyphOrigin.x) + ( ybase * pitch);
				pix = AlphaTbl[31];
				pix = (pix&255)<<24;
				for ( int y = 0; y < height ; y++ )
				{
					bmask = 0x80; pp = p2;
					for ( int x = 0; x < width; x++ )
					{
						if ( bmask == 0 ) { bmask = 0x80; pp++; }
						if ( *pp & bmask ) { p1[x] = ( p1[x] & 0xffffff ) | pix ; }
						bmask>>=1;
					}
					p1 += pitch;
					p2 += fontPitch;
				}
			}
			break;
			}
			return px;
		}

		switch ( Format )
		{
		case TEXMODE_MES4:
			{
				LPWORD p1 = (LPWORD)MemTexRect.pBits;
				DWORD pitch = MemTexRect.Pitch / 2;
				WORD pix;
				LPBYTE pp;
				int bmask;

				// 転送先のサーフェイスの始点
				p1 += (x + gm.gmptGlyphOrigin.x) + ( ybase * pitch );
				pix = (WORD)AlphaTbl[31];

				for ( int y = 0; y < height ; y++ )
				{
					bmask = 0x80; pp = p2;
					for ( int x = 0; x < width; x++ )
					{
						if ( bmask == 0 ) { bmask = 0x80; pp++; }
						if ( *pp & bmask ) { p1[x] = pix; }
						bmask>>=1;
					}
					p1 += pitch;
					p2 += fontPitch;
				}
			}
			break;
		case TEXMODE_MES8:
			{
				LPDWORD p1 = (LPDWORD)MemTexRect.pBits;
				DWORD pitch = MemTexRect.Pitch / 4;
				DWORD pix;
				LPBYTE pp;
				int bmask;

				// 転送先のサーフェイスの始点
				p1 += (x + gm.gmptGlyphOrigin.x) + ( ybase * pitch );
				pix = AlphaTbl[31];

				for ( int y = 0; y < height ; y++ )
				{
					bmask = 0x80; pp = p2;
					for ( int x = 0; x < width; x++ )
					{
						if ( bmask == 0 ) { bmask = 0x80; pp++; }
						if ( *pp & bmask ) { p1[x] = pix; }
						bmask>>=1;
					}
					p1 += pitch;
					p2 += fontPitch;
				}
			}
			break;
		}

		return px;
	}

	// アンチあり
	// フォントバッファ取得
	DWORD DataSize = GetFontBuffer( hdc, code, &gm, lpFont );

	LPBYTE p2 = lpFont;
	// フォントピッチ
	DWORD fontPitch = (DataSize / gm.gmBlackBoxY) & ~0x03;

	// サイズ取得
	width = (int)gm.gmBlackBoxX;
	height = (int)gm.gmBlackBoxY;

	// 描画位置を進める量
	if ( FixedFontWidth == 0 ) {
		px = gm.gmCellIncX + spacing;
	} else {
		px = FixedFontWidth + spacing;
	}
	int sy = tm.tmAscent - gm.gmptGlyphOrigin.y;
	ybase = y+sy;

	if (  (x+px) >= tex->sx ) return -1;
	if (( x<0 )||( y<0 )||( (y+sy+height)>tex->sy )) return px;
	if ( ybase >= tex->sy ) return px;

	switch ( Format )
	{
	case TEXMODE_MES4:
		{
			LPWORD p1 = (LPWORD)MemTexRect.pBits;
			DWORD pitch = MemTexRect.Pitch / 2;

			// 転送先のサーフェイスの始点
			p1 += (x + gm.gmptGlyphOrigin.x) + (ybase * pitch);
			for ( int y = 0; y < height ; y++ )
			{
				for ( int x = 0; x < width; x++ )
				{
					if ( p2[x] > 0 )
					{
						p1[x] = (WORD)AlphaTbl[p2[x]];
					}
				}
				p1 += pitch;
				p2 += fontPitch;
			}
		}
		break;
	case TEXMODE_MES8:
		{
			LPDWORD p1 = (LPDWORD)MemTexRect.pBits;
			DWORD pitch = MemTexRect.Pitch / 4;

			// 転送先のサーフェイスの始点
			p1 += (x + gm.gmptGlyphOrigin.x) + ( ybase * pitch);
			for ( int y = 0; y < height ; y++ )
			{
				for ( int x = 0; x < width; x++ )
				{
					if ( p2[x] > 0 )
					{
						p1[x] = AlphaTbl[p2[x]];
					}
				}
				p1 += pitch;
				p2 += fontPitch;
			}
		}
		break;
	}

	return px;
}


void DrawTexString( int xx, int yy, char *Buffer )
{
	//		テキスト描画
	//ABC abc;
	HDC hdc;
	TEXINF *tex;
	int wsize;
	int res, x, y, xleft, tmpx;

	if ( lasttex < 0 ) return;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return;
	hdc = htexdc;
	GetTextMetrics( hdc, &tm );
	x = xx; y = yy;
	drawsx = drawsy = 0;
	xleft = xx; if ( xleft < 0 ) xleft = 0;

	// 文字長
	DWORD len = (DWORD)strlen( Buffer );

	// 文字列を処理する
	DWORD i = 0;
	while ( i < len )
	{
		long code = ((long)Buffer[i])&0xff;

		// 全角
		if ( IsKanji( Buffer[i] ) )
		{
			// Shift-JISコード
			code = GetCharCode( (unsigned char *)&Buffer[i] );
			i++;
		}
		i++;

		switch ( code )
		{
		case 10:
			x = xx;
			y += tm.tmHeight + linespace;
			break;
		case 13:
			break;
		case ' ':
		case '　':
			GetCharWidth( hdc, code, code, &wsize );
			x += wsize;
			//GetCharABCWidths( hdc, code, code, &abc );
			//x += abc.abcA + abc.abcB + abc.abcC;
			tmpx = x - xleft;
			if ( tmpx > drawsx ) drawsx = tmpx;
			break;
		default:
			res = DrawTexStringSub( hdc, tex, x, y, code );
			if ( res < 0 ) {
				tmpx = x - xleft;
				if ( tmpx > drawsx ) drawsx = tmpx;
				x = xx;
				y += tm.tmHeight + linespace;
				res = DrawTexStringSub( hdc, tex, x, y, code );
			}
			//GetCharWidth( hdc, code, code, &wsize );
			//x += wsize;
			x += res;
			tmpx = x - xleft;
			if ( tmpx > drawsx ) drawsx = tmpx;
			break;
		}

	}
	drawsy = y - yy;
}


int TexGetDrawSizeX( void )
{
	return drawsx;
}


int TexGetDrawSizeY( void )
{
	return drawsy;
}


int CalcTextDrawSize( char *Buffer )
{
	//		テキスト描画サイズを取得(横幅のみ)
	HDC hdc;
	int wsize;
	int x;

	// 文字長
	DWORD len = (DWORD)strlen( Buffer );

	hdc = htexdc;
	x = 0;

	// 文字列を処理する
	DWORD i = 0;
	while ( i < len )
	{
		long code = ((long)Buffer[i])&0xff;

		// 全角
		if ( IsKanji( Buffer[i] ) )
		{
			// Shift-JISコード
			code = GetCharCode( (unsigned char *)&Buffer[i] );
			i++;
		}
		i++;

		GetCharWidth( hdc, code, code, &wsize );
		x += wsize;
	}
	return x;
}


int DrawTestY( void )
{
	HDC hdc;
	TEXINF *tex;

	if ( lasttex < 0 ) return 0;
	tex = GetTex( lasttex );
	if ( tex->flag == TEXMODE_NONE ) return 0;
	hdc = htexdc;
	GetTextMetrics( hdc, &tm );
	return tm.tmHeight + linespace;
}


void TexDivideSize( int id, int new_divsx, int new_divsy, int new_ofsx, int new_ofsy )
{
	//		セル分割サイズを設定
	//
	TEXINF *t;
	t = GetTex( id );
	if ( t->flag == TEXMODE_NONE ) return;
		
	if ( new_divsx > 0 ) t->divsx = new_divsx; else t->divsx = t->sx;
	if ( new_divsy > 0 ) t->divsy = new_divsy; else t->divsy = t->sy;
	t->divx = t->sx / t->divsx;
	t->divy = t->sy / t->divsy;
	t->celofsx = new_ofsx;
	t->celofsy = new_ofsy;
}


