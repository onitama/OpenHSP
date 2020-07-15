//
//		Texture manager (directX8+TEXMES)
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

#define USE_STAR_FIELD

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
		Star Field
*/
/*------------------------------------------------------------*/

#ifdef USE_STAR_FIELD

#define STAR_RNG_PERIOD     ((1 << 17) - 1)
#define RGB_MAXIMUM         224
#define STAR_SX         256
#define STAR_SY         256

static unsigned char m_stars[STAR_RNG_PERIOD];
static int m_stars_enabled = 0;
static int m_stars_count;
static int m_star_rng_origin;
static unsigned int m_star_color[64];

static void star_init(void)
{
	//	星(StarField)の初期化
	m_stars_count = 0;
	m_stars_enabled = 1;

	//	テーブル作成
	unsigned int shiftreg;
	int i;

	shiftreg = 0;
	for (i = 0; i < STAR_RNG_PERIOD; i++)
	{
		int enabled = ((shiftreg & 0x1fe01) == 0x1fe00);
		int color = (~shiftreg & 0x1f8) >> 3;
		m_stars[i] = color | (enabled << 7);
		// LFSRによる乱数生成
		shiftreg = (shiftreg >> 1) | ((((shiftreg >> 12) ^ ~shiftreg) & 1) << 16);
	}

	unsigned int minval = RGB_MAXIMUM * 130 / 150;
	unsigned int midval = RGB_MAXIMUM * 130 / 100;
	unsigned int maxval = RGB_MAXIMUM * 130 / 60;

	unsigned int starmap[4]{
			0,
			minval,
			minval + (255 - minval) * (midval - minval) / (maxval - minval),
			255 };

	for (i = 0; i < 64; i++)
	{
		int bit0, bit1;

		bit0 = (i>>5)&1;
		bit1 = (i>>4)&1;
		int r = starmap[(bit1 << 1) | bit0];
		bit0 = (i >> 3) & 1;
		bit1 = (i >> 2) & 1;
		int g = starmap[(bit1 << 1) | bit0];
		bit0 = (i >> 1) & 1;
		bit1 = i & 1;
		int b = starmap[(bit1 << 1) | bit0];
		m_star_color[i] = 0xff000000+(r<<16)+(g<<8)+(b);
	}

}

static void star_draw_y(unsigned char *dest, int y, int maxx, int offset)
{
	//	星(StarField)の描画(1line)
	int x;
	int ofs;
	unsigned int *ptr;
	unsigned char star;

	ofs = offset %= STAR_RNG_PERIOD;
	ptr = (unsigned int*)dest;

	/* iterate over the specified number of 6MHz pixels */
	for (x = 0; x < maxx; x++)
	{
		int enable_star = (y ^ (x >> 3)) & 1;
		star = m_stars[ofs++];
		if (ofs >= STAR_RNG_PERIOD) ofs = 0;
		if (enable_star && (star & 0x80) != 0 && (star & 0xff) != 0) {
			*ptr++ = m_star_color[star & 63];
		}
		else {
			*ptr++ = 0;
		}
	}

}

static void star_draw(char *dest, int sx, int sy, int mode)
{
	//	星(StarField)の描画
	if (m_stars_enabled == 0) return;

	int y;
	unsigned char *ptr = (unsigned char*)dest;

	for (y = 0; y < 224; y++)
	{
		int star_offs = (m_star_rng_origin>>1) + y * 512;
		star_draw_y(ptr, y, 256, star_offs);
		ptr += sx;
	}

	m_star_rng_origin += (mode & 3);
	if (mode & 4) {
		if ((m_stars_count&63)==63) {
			m_star_rng_origin = rand()+ rand();
		}
	}
	m_stars_count++;
}


#endif

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

#ifdef USE_STAR_FIELD
	star_init();
#endif
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


int UpdateTexStar(int texid, int mode)
{
	int sx, sy, width, height, Format;
	TEXINF* texinf;
	HRESULT hr;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;

	texinf = GetTex(texid);
	if (texinf->flag == TEXMODE_NONE) { return -1; }

	Format = texinf->flag;
	sx = texinf->sx;
	sy = texinf->sy;
	width = texinf->width;
	height = texinf->height;
	pTex = (LPDIRECT3DTEXTURE8)texinf->data;

	hr = pTex->LockRect(0, &lock, NULL, 0);
	if (FAILED(hr)) return -1;

	switch (Format) {
	case TEXMODE_MES4:
		break;
	case TEXMODE_MES8:
		// 転送先のサーフェイスの始点(32bit)
#ifdef USE_STAR_FIELD
		char* p;
		p = (char*)lock.pBits;
		star_draw(p, lock.Pitch, sy, mode);
#endif
		break;
	}
	pTex->UnlockRect(0);
	return 0;
}

int UpdateTex32(int texid, char * srcptr, int mode)
{
	int sx, sy, width, height, Format;
	TEXINF* texinf;
	HRESULT hr;
	LPDIRECT3DTEXTURE8 pTex;
	D3DLOCKED_RECT lock;

	texinf = GetTex(texid);
	if (texinf->flag == TEXMODE_NONE) { return -1; }

	Format = texinf->flag;
	sx = texinf->sx;
	sy = texinf->sy;
	width = texinf->width;
	height = texinf->height;
	pTex = (LPDIRECT3DTEXTURE8)texinf->data;

	hr = pTex->LockRect(0, &lock, NULL, 0);
	if (FAILED(hr)) return -1;

	switch (Format) {
	case TEXMODE_MES4:
		break;
	case TEXMODE_MES8:
		// 転送先のサーフェイスの始点(32bit)
		char* p;
		p = (char*)lock.pBits;
		if (mode & 1) {
			memcpy(p, srcptr, sx*sy * 4);
		}
		else {
			int i;
			char *src = srcptr;
			char a1,a2,a3,a4;
			for (i = 0; i < sx*sy; i++) {
				a1 = *src++;
				a2 = *src++;
				a3 = *src++;
				a4 = *src++;
				*p++ = a3;
				*p++ = a2;
				*p++ = a1;
				*p++ = a4;
			}
		}
		break;
	}
	pTex->UnlockRect(0);
	return 0;
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


void TexDivideSize(int id, int new_divsx, int new_divsy, int new_ofsx, int new_ofsy)
{
	//		セル分割サイズを設定
	//
	TEXINF *t;
	t = GetTex(id);
	if (t->flag == TEXMODE_NONE) return;

	if (new_divsx > 0) t->divsx = new_divsx; else t->divsx = t->sx;
	if (new_divsy > 0) t->divsy = new_divsy; else t->divsy = t->sy;
	t->divx = t->sx / t->divsx;
	t->divy = t->sy / t->divsy;
	t->celofsx = new_ofsx;
	t->celofsy = new_ofsy;
}

int RegistTexEmpty(int w, int h, int tmode)
{
	// メッセージエリア用のテクスチャ作成
	int sel, sx, sy, flag;
	D3DSURFACE_DESC desc;
	LPDIRECT3DTEXTURE8 lpTex;

	if (tmode == 0) {
		if (CreateTexture2(w, h, D3DFMT_A4R4G4B4, lpTex)) {
			if (CreateTexture2(w, h, D3DFMT_A8R8G8B8, lpTex)) {
				RELEASE(lpTex);
				return -1;
			}
		}

	}
	else {
		if (CreateTexture2(w, h, D3DFMT_A8R8G8B8, lpTex)) {
			if (CreateTexture2(w, h, D3DFMT_A4R4G4B4, lpTex)) {
				RELEASE(lpTex);
				return -1;
			}
		}
	}

	lpTex->GetLevelDesc(0, &desc);
	// データ
	sx = desc.Width;
	sy = desc.Height;
	flag = TEXMODE_MES8;
	if (desc.Format == D3DFMT_A4R4G4B4) flag = TEXMODE_MES4;

	sel = GetNextTexID();
	SetTex(sel, flag, 0, sx, sy, sx, sy, lpTex);
	return sel;
}


