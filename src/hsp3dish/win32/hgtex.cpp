//
//		Texture manager (directX8+TEXMES)
//			onion software/onitama 2001/6
//
#include <stdio.h>
#define STRICT
#include <windows.h>
#include <wincodec.h>
#include <math.h>
#include <vector>
#include <dxgiformat.h>

#include "hgtex.h"
#include "dx11_renderer.h"

#include "../sysreq.h"
#include "../supio.h"

#define RELEASE(x) 	if(x){x->Release();x=NULL;}

#define USE_STAR_FIELD

//		Data
//
static		std::vector<TEXINF> texinf;
static		char ck1;			// カラーキー1
static		char ck2;			// カラーキー2
static		char ck3;			// カラーキー3

static		char *lpTex;		// テクスチャのバッファ
static		int curtex;			// current texID

static		DWORD AlphaTbl[34];
static		int FixedFontWidth;
static		HFONT htexfont;		// TEXTURE用のフォント
static		HFONT htexfont_old;	// TEXTURE用のフォント(保存用)
static		LPBYTE lpFont;
static		HDC htexdc;
static		HWND htexwnd;
static		int htexsize;
static		int lasttex;
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
		texture service
*/
/*------------------------------------------------------------*/

static		HGIO_DX11_RENDERER* render;


HGIO_DX11_TEXTURE* Texture(TEXINF* info) { return info ? reinterpret_cast<HGIO_DX11_TEXTURE*>(info->data) : NULL; }

static void SetTexture(int id, int flag, int mode, int sx, int sy, int width, int height, HGIO_DX11_TEXTURE* texture)
{
	TEXINF* t = GetTex(id);
	t->flag = flag; t->mode = mode; t->sx = sx; t->sy = sy; t->width = width; t->height = height;
	t->data = reinterpret_cast<char*>(texture);
	t->ratex = 1.0f / sx; t->ratey = 1.0f / sy;
	t->ratehx = 0.5f / sx; t->ratehy = 0.5f / sy;
	TexDivideSize(id, 0, 0, 0, 0);
}

static bool DecodeBGRA(const void* data, size_t size, std::vector<unsigned char>& pixels, UINT& width, UINT& height)
{
	IWICImagingFactory* factory = NULL; IWICStream* stream = NULL; IWICBitmapDecoder* decoder = NULL;
	IWICBitmapFrameDecode* frame = NULL; IWICFormatConverter* converter = NULL; bool result = false;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) goto done;
	if (FAILED(factory->CreateStream(&stream)) || FAILED(stream->InitializeFromMemory((WICInProcPointer)data, (DWORD)size)) ||
		FAILED(factory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder)) ||
		FAILED(decoder->GetFrame(0, &frame)) || FAILED(frame->GetSize(&width, &height)) ||
		FAILED(factory->CreateFormatConverter(&converter)) ||
		FAILED(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom))) goto done;
	pixels.resize((size_t)width * height * 4);
	result = SUCCEEDED(converter->CopyPixels(NULL, width * 4, (UINT)pixels.size(), pixels.data()));
done:
	if (converter) converter->Release(); if (frame) frame->Release(); if (decoder) decoder->Release();
	if (stream) stream->Release(); if (factory) factory->Release();
	return result;
}

static void Convert24(const char* source, int sx, int sy, int sw, std::vector<unsigned char>& pixels)
{
	pixels.resize((size_t)sx * sy * 4);
	for (int y = 0; y < sy; ++y) {
		const char* src = source + (sw & 1 ? y : sy - 1 - y) * sx * 3;
		unsigned char* dst = pixels.data() + (size_t)y * sx * 4;
		for (int x = 0; x < sx; ++x) {
			const unsigned char r = (unsigned char)src[x * 3];
			const unsigned char g = (unsigned char)src[x * 3 + 1];
			const unsigned char b = (unsigned char)src[x * 3 + 2];
			dst[x * 4] = r; dst[x * 4 + 1] = g; dst[x * 4 + 2] = b; dst[x * 4 + 3] = 255;
		}
	}
}

/*------------------------------------------------------------*/
/*
		texture process
*/
/*------------------------------------------------------------*/

void TexInit( void )
{
	texinf.clear();

	TexReset();
	lpFont = (LPBYTE)malloc( 0x10000 );			// フォント取得用のワーク

#ifdef USE_STAR_FIELD
	star_init();
#endif
}


void TexSetParam(void* p_render)
{
	//		renderパラメータ設定
	//
	render = (HGIO_DX11_RENDERER *)p_render;
}

void TexTerm( void )
{
	render->SetTexture(NULL);
	if ( lpFont != NULL ) { free( lpFont ); lpFont = NULL; }

	if (!texinf.empty()) {
		for (size_t i = 0; i < texinf.size(); i++) {
			DeleteTex(i);
		}
		texinf.clear();
	}
}


void TexReset( void )
{
	curtex = -1;
	render->SetTexture(NULL);
	FixedFontWidth = 0;
	htexfont = NULL;
	htexsize = 0;
	//lpFont = NULL;
}


int GetNextTexID( void )
{
	int sel;
	sel = (int)texinf.size();

	if (!texinf.empty()) {
		for (int i = 0; i < sel; i++) {
			if (texinf[i].flag == TEXMODE_NONE) return i;
		}
	}
	TEXINF tinfo = { TEXMODE_NONE,0,0,0,0,0,NULL,0,0, 0,0, 0,0, 0,0 };
	texinf.push_back(tinfo);
	return sel;
}


void SetTex( int sel, int flag, int sw, int sx, int sy, int width, int height, void *pTex )
{
	TEXINF *t;
	t = GetTex( sel );
	if (t == NULL) return;
	t->flag = flag;
	t->mode = sw;
	t->sx = sx;
	t->sy = sy;
	t->width = width;
	t->height = height;
	t->data = (char *)pTex;
	t->ratex = 1.0f / (float)sx;
	t->ratey = 1.0f / (float)sy;
	t->ratehx = 0.5f / (float)sx;
	t->ratehy = 0.5f / (float)sy;
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

static int Get2N( int val )
{
	int res = 1;
	while(1) {
		if ( res >= val ) break;
		res<<=1;
	}
	return res;
}


int RegistTex( char *data, int size )
{
	HGIO_DX11_TEXTURE* dxTexture = new HGIO_DX11_TEXTURE{};
	if (!render->CreateTextureFromMemory(dxTexture, data, (size_t)size)) {
		delete dxTexture;
		return -1;
	}
	int sel = GetNextTexID();
	SetTex(sel, TEXMODE_NORMAL, 0, dxTexture->width, dxTexture->height, dxTexture->width, dxTexture->height, dxTexture);
	return sel;
}


int UpdateTex32(int texid, char * srcptr, int mode)
{
	TEXINF *dxInfo = GetTex(texid);
	if (dxInfo == NULL || dxInfo->flag == TEXMODE_NONE) return -1;
	HGIO_DX11_TEXTURE *dxTexture = (HGIO_DX11_TEXTURE *)dxInfo->data;
	if (dxTexture == NULL) return -1;
	if (mode & 1) return render->UpdateTexture(dxTexture, srcptr, dxInfo->sx * 4) ? 0 : -1;
	std::vector<unsigned char> pixels((size_t)dxInfo->sx * dxInfo->sy * 4);
	for (int i = 0; i < dxInfo->sx * dxInfo->sy; ++i) {
		pixels[i * 4] = srcptr[i * 4 + 2];
		pixels[i * 4 + 1] = srcptr[i * 4 + 1];
		pixels[i * 4 + 2] = srcptr[i * 4];
		pixels[i * 4 + 3] = srcptr[i * 4 + 3];
	}
	return render->UpdateTexture(dxTexture, pixels.data(), dxInfo->sx * 4) ? 0 : -1;
}

int UpdateTexStar(int texid, int mode)
{
#ifdef USE_STAR_FIELD
	TEXINF* dxInfo = GetTex(texid);
	if (dxInfo == NULL || dxInfo->flag == TEXMODE_NONE) return -1;
	HGIO_DX11_TEXTURE* dxTexture = (HGIO_DX11_TEXTURE*)dxInfo->data;
	if (dxTexture == NULL) return -1;
	std::vector<unsigned char> pixels((size_t)dxInfo->sx * dxInfo->sy * 4);
	char* m_stars_pixel = (char*)pixels.data();
	star_draw(m_stars_pixel, STAR_SX * sizeof(int), STAR_SY, mode);
	return UpdateTex32(texid, m_stars_pixel, mode);
#else
	return 0;
#endif
}

int UpdateTex( int texid, char *data, int sw )
{
	TEXINF *dxInfo = GetTex(texid);
	if (dxInfo == NULL || dxInfo->flag == TEXMODE_NONE) return -1;
	HGIO_DX11_TEXTURE *dxTexture = (HGIO_DX11_TEXTURE *)dxInfo->data;
	if (dxTexture == NULL) return -1;
	std::vector<unsigned char> pixels((size_t)dxInfo->sx * dxInfo->sy * 4);
	for (int y = 0; y < dxInfo->sy; ++y) {
		char *src = data + (sw & 1 ? dxInfo->sx * 3 * y : dxInfo->sx * 3 * (dxInfo->sy - 1 - y));
		TexCopySub32((char *)&pixels[(size_t)y * dxInfo->sx * 4], src, dxInfo->sx);
	}
	return render->UpdateTexture(dxTexture, pixels.data(), dxInfo->sx * 4) ? 0 : -1;
}


void DeleteTex( int id )
{
	TEXINF *dxInfo = GetTex(id);
	if (dxInfo == NULL || dxInfo->flag == TEXMODE_NONE) return;
	dxInfo->flag = TEXMODE_NONE;
	HGIO_DX11_TEXTURE *dxTexture = (HGIO_DX11_TEXTURE *)dxInfo->data;
	if (dxTexture) {
		render->DeleteTexture(dxTexture);
		delete dxTexture;
	}
	dxInfo->data = NULL;
	return;
}


void ChangeTex( int id )
{
	TEXINF *t;
	if ( id < 0 ) {
		curtex=-1;
		render->SetTexture(NULL);
		return;
	}
	if ( curtex == id ) return;
	t = GetTex( id );
	if (t == NULL) return;
	if ( t->flag == TEXMODE_NONE ) {
		curtex=-1;
		render->SetTexture(NULL);
		return;
	}
	curtex = id;
	HGIO_DX11_TEXTURE *texture = (HGIO_DX11_TEXTURE *)t->data;
	render->SetTexture(texture ? texture->srv : NULL);
}


TEXINF *GetTex( int id )
{
	if ((id<0)||(id >= (int)texinf.size())) return NULL;
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
	if (t == NULL) return;
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
	HGIO_DX11_TEXTURE *dxTexture = new HGIO_DX11_TEXTURE{};
	if (!render->CreateTexture(dxTexture, w, h,
		DXGI_FORMAT_B8G8R8A8_UNORM, NULL, 0)) {
		delete dxTexture;
		return -1;
	}
	int dxSel = GetNextTexID();
	SetTex(dxSel, TEXMODE_MES8, 0, w, h, w, h, dxTexture);
	return dxSel;
}


char* GetPixelMaskBuffer(char* fileptr, int size, int *xsize, int *ysize)
{
	//		画像のピクセルバッファを取得する
	//		(画像ファイルのポインタを渡すと、αチャンルを2値化したバッファを返す)
	//
	return NULL;
}
