//
//		OpenGL Texture lib (iOS/android/opengl/ndk)
//			onion software/onitama 2011/11
//


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "stb_image.h"

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../../hsp3/hsp3config.h"
#else
#include "../hsp3config.h"
#endif

#ifdef HSPNDK
#define USE_JAVA_FONT
#define FONT_TEX_SX 512
#define FONT_TEX_SY 128
#include "../../appengine.h"
#include "../../javafunc.h"
#include "font_data.h"
#endif

#ifdef HSPIOS
//#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <CoreFoundation/CoreFoundation.h>
#include "iOSBridge.h"
#include "appengine.h"
#endif

#ifdef HSPEMSCRIPTEN
#define USE_JAVA_FONT
#define FONT_TEX_SX 512
#define FONT_TEX_SY 128
#include "appengine.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <emscripten.h>
#endif

#ifdef HSPLINUX

#include "appengine.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#endif

#include "../supio.h"
#include "../sysreq.h"
#include "../hgio.h"

/*-------------------------------------------------------------------------------*/

static TEXINF texinf[TEXINF_MAX];
static int curtex;				// 現在選択されているテクスチャID
static int curmestex;			// メッセージ用にキャッシュされたテクスチャ数


/*-------------------------------------------------------------------------------*/
/*
		Texture Manage Routines
*/
/*-------------------------------------------------------------------------------*/

TEXINF *GetTex( int id )
{
	//	TEXINF idから構造体を取得
	//
	return &texinf[id];
}


void DeleteTexInf( TEXINF *t )
{
	//	TEXINFのテクスチャを破棄
	//
	if ( t->mode == TEXMODE_NONE ) return;
	glDeleteTextures( 1, (GLuint *)&t->texid );
	if ( t->text ) {
		free( t->text );		// 拡張されたネーム用バッファがあれば解放する
		t->text = NULL;
	}

	t->mode = TEXMODE_NONE;
}


void DeleteTex( int id )
{
	//	TEXINF idのテクスチャを破棄
	//
	DeleteTexInf( GetTex( id ) );
}


void TexReset( void )
{
	//	リセット
	//
	curtex = -1;
}


void TexInit( void )
{
	//	初期化
	//
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		texinf[i].mode = TEXMODE_NONE;
	}
	curmestex = 0;
	TexReset();
}


void TexTerm( void )
{
	//	終了処理
	//
	int i;
	for(i=0;i<TEXINF_MAX;i++) {
		DeleteTex( i );
	}
}


void ChangeTex( int id )
{
	//	テクスチャ設定
	//	TexIDではなくOpenGLのIDを渡すこと
	if ( id < 0 ) {
		curtex = -1;
	    glBindTexture(GL_TEXTURE_2D,0);
#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
		glDisable(GL_TEXTURE_2D);
#endif
		return;
	}
	curtex = id;
    glBindTexture( GL_TEXTURE_2D, id );
#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
	glEnable(GL_TEXTURE_2D);
#endif
}


static int GetNextTex( void )
{
	//		新規のTEXINF idを作成する
	//
	int i,sel;
	sel = -1;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( texinf[i].mode == TEXMODE_NONE ) { sel=i;break; }
	}
	return sel;
}


static int SetTex( int sel, short mode, short opt, short sx, short sy, short width, short height, GLuint texid )
{
	//		TEXINFを設定する
	//
	TEXINF *t;
	int myid;
	myid = sel;
	if ( sel >= 0 ) {
		t = GetTex( sel );
	} else {
		myid = GetNextTex();
		if ( myid < 0 ) return myid;
		t = GetTex( myid );
	}
	t->mode = mode;
	t->opt = opt;
	t->sx = sx;
	t->sy = sy;
	t->width = width;
	t->height = height;
	t->ratex = 1.0f / (float)sx;
	t->ratey = 1.0f / (float)sy;
	t->texid = (int)texid;
	t->hash = 0;
	t->life = TEXMES_CACHE_DEFAULT;
	t->text = NULL;
	return myid;
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


int RegistTexMem( unsigned char *ptr, int size )
{
	//		メモリ上の画像ファイルデータからテクスチャ読み込み
	//		(TEXINFのidを返す)
	//
	GLuint id;
	int texid, tsx,tsy,comp;
	int sx,sy;
	unsigned char *pImg;
	unsigned char *pImg2;

	pImg = stbi_load_from_memory( ptr, size, &tsx, &tsy, &comp, 4 );

	id = -1;
	if ( pImg != NULL ) {
		sx = Get2N( tsx );
		sy = Get2N( tsy );
		if (( sx != tsx )||( sy != tsy )) {
			//	Exchange to 2N bitmap
			char *p;
			char *p2;
			int x,y;
			pImg2 = (unsigned char *)mem_ini( sx * sy * 4 );
			p = (char *)pImg;
			p2 = (char *)pImg2;
			for(y=0;y<tsy;y++) {
#if 0
 				p2 = (char *)pImg2 + (sx*y*4);
 				for(x=0;x<tsx;x++) {
					p2[0] = p[0];
					p2[1] = p[1];
					p2[2] = p[2];
					p2[3] = p[3];
					p+=4; p2+=4;
				}
#else
				memcpy( p2, p, tsx*4 );
				p+=tsx*4;
				p2+=sx*4;
#endif
			}
			mem_bye(pImg);
			pImg = pImg2;
		}
		glGenTextures( 1, &id );
		glBindTexture( GL_TEXTURE_2D, id );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, sx, sy, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImg );
		mem_bye(pImg);
		texid = SetTex( -1, TEXMODE_NORMAL, 0, sx, sy, tsx, tsy, id );
		Alertf( "Tex:ID%d (%d,%d)(%dx%d)",texid,sx,sy,tsx,tsy );
		return texid;
	}
	Alertf( "Tex:failed" );
	return -1;
}


int RegistTex( char *fname )
{
	//		画像ファイルからテクスチャ読み込み
	//		(TEXINFのidを返す)
	//
	char *ptr;
	int len;
	int id;

	len = dpm_exist( fname );
	//Alertf( "Tex:read(%s)(%d)", fname, len );
	if ( len < 0 ) return -1;
	ptr = mem_ini( len );
	dpm_read( fname, ptr, len, 0 );
	id = RegistTexMem( (unsigned char *)ptr, len );
	mem_bye( ptr );
	return id;
}


int MakeEmptyTex( int width, int height )
{
	//		メッセージ用の空テクスチャを作成する
	//
	GLuint id;
	int texid;
	int sx,sy;
	unsigned char *pImg;

	sx = Get2N( width );
	sy = Get2N( height );

	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_2D, id );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, sx, sy, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL );
	texid = SetTex( -1, TEXMODE_MES8, 0, sx, sy, width, height, id );
	Alertf( "Tex:ID%d (%d,%d) Clear",texid,sx,sy );
	return texid;
}


/*--------------------------------------------------------------------------------*/

static short str2hash( char *msg, int *out_len )
{
	//		文字列の簡易ハッシュ(short)を得る
	//		同時にout_lenに文字列長を返す
	//
	int len;
	short cache;
	unsigned char a1;
	unsigned char *p;
	p = (unsigned char *)msg;
	len = 0;
	a1 = *p;
	cache = ((short)a1)<<8;		// 先頭の文字コードを上位8bitにする
	while(1) {
		if ( a1 == 0 ) break;
		a1 = *p++;
		len++;
	}
	*out_len = len;
	if ( len > 0 ) {			// 終端の文字コードを下位8bitにする
		p--;
		cache += (short)*p;
	}
	return cache;
}


static int getCache( char *msg, short mycache, int font_size, int font_style )
{
	//		キャッシュ済みの文字列があればTEXINFを返す
	//		(存在しない場合はNULL)
	//
	int i;
	TEXINF *t;
	t = texinf;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( t->mode == TEXMODE_MES8 ) {		// メッセージテクスチャだった時
			if ( t->hash == mycache ) {			// まずハッシュを比べる
				if ( t->font_size == font_size && t->font_style == font_style ) {	// サイズ・スタイルを比べる
					if ( t->text ) {
						if ( strcmp( msg, t->text ) == 0 ) {
							t->life = TEXMES_CACHE_DEFAULT;			// キャッシュを保持
							return i;
						}
					} else {
						if ( strcmp( msg, t->buf ) == 0 ) {
							t->life = TEXMES_CACHE_DEFAULT;			// キャッシュを保持
							return i;
						}
					}
				}
			}
		}
		t++;
	}
	return -1;
}


void TexProc( void )
{
	//		フレーム単位でのキャッシュリフレッシュ
	//		(キャッシュサポート時は、毎フレームごとに呼び出すこと)
	//
	int i;
	TEXINF *t;
	t = texinf;
	curmestex = 0;
	for(i=0;i<TEXINF_MAX;i++) {
		if ( t->mode == TEXMODE_MES8 ) {		// メッセージテクスチャだった時
			if ( t->life > 0 ) {
				t->life--;						// キャッシュのライフを減らす
				curmestex++;
			} else {
				DeleteTexInf( t );				// テクスチャのエントリを破棄する
			}
		}
		t++;
	}
}


int GetCacheMesTextureID( char *msg, int font_size, int font_style )
{
	//		キャッシュ済みのテクスチャIDを返す(OpenGLテクスチャIDを返す)
	//		(作成されていないメッセージテクスチャは自動的に作成する)
	//		(作成の必要がない場合は-1を返す)
	//
#ifdef HSPNDK
	GLuint id;
	int texid;
	int tsx,tsy;
	unsigned char *pImg;

	TEXINF *t;
	int mylen;
	short mycache;
	
	mycache = str2hash( msg, &mylen );			// キャッシュを取得
	if ( mylen <= 0 ) return -1;

	texid = getCache( msg, mycache, font_size, font_style );
	if ( texid >= 0 ) {
		return texid;							// キャッシュがあった
	}

	//		キャッシュが存在しないので作成
	pImg = (unsigned char *)j_callFontBitmap( msg, font_size, font_style, &tsx, &tsy );
	texid = MakeEmptyTex( tsx, tsy );
	if ( texid < 0 ) return -1;

	t = GetTex( texid );
	t->hash = mycache;
	t->font_size = font_size;
	t->font_style = font_style;

	if ( curmestex >= GetSysReq(SYSREQ_MESCACHE_MAX) ) {	// エントリ数がオーバーしているものは次のフレームで破棄
		t->life = 0;
		t->buf[0] = 0;
	} else {
		//		キャッシュの登録
		if ( mylen >= ( TEXMES_NAME_BUFFER - 1 ) ) {
			t->text = (char *)malloc( mylen+1 );		// テキストハッシュネーム用バッファを作成する
			strcpy( t->text, msg );
		} else {
			strcpy( t->buf, msg );						// 標準バッファにコピーする
		}
	}

	id = (GLuint)t->texid;

	glBindTexture( GL_TEXTURE_2D, id );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		(GLint)0,
		(GLint)0,
		(GLsizei)tsx,
		(GLsizei)tsy,
		GL_ALPHA,
		GL_UNSIGNED_BYTE,
		pImg
	);

	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	free(pImg);

	return texid;
#endif

#ifdef HSPEMSCRIPTEN
	GLuint id;
	int texid;
	int tsx,tsy;
	unsigned char *pImg;

	TEXINF *t;
	int mylen;
	short mycache;

	mycache = str2hash( msg, &mylen );			// キャッシュを取得
	if ( mylen <= 0 ) return -1;

	texid = getCache( msg, mycache, font_size, font_style );
	if ( texid >= 0 ) {
		return texid;							// キャッシュがあった
	}

	EM_ASM_({
		var d = document.getElementById('hsp3dishFontDiv');
		if (!d) {
			d = document.createElement("div");
			d.id = 'hsp3dishFontDiv';
			d.style.setProperty("width", "auto");
			d.style.setProperty("height", "auto");
			d.style.setProperty("position", "absolute");
			d.style.setProperty("visibility", "hidden");
		}
		d.style.setProperty("font", $1 + "px 'sans-serif'");
		document.body.appendChild(d);

		var t = document.createTextNode(Pointer_stringify($0));
		if (d.hasChildNodes())
			d.removeChild(d.firstChild);
		d.appendChild(t);
		}, msg, font_size);
	tsx = EM_ASM_INT_V({
		var d = document.getElementById('hsp3dishFontDiv');
		return d.clientWidth;
	});
	tsy = EM_ASM_INT_V({
		var d = document.getElementById('hsp3dishFontDiv');
		return d.clientHeight;
	});
	//printf("texsize %dx%d '%s'\n", tsx, tsy, msg);

	//		キャッシュが存在しないので作成
	//pImg = (unsigned char *)j_callFontBitmap( msg, font_size, font_style, &tsx, &tsy );

	texid = MakeEmptyTex( tsx, tsy );
	if ( texid < 0 ) return -1;

	t = GetTex( texid );
	t->hash = mycache;
	t->font_size = font_size;
	t->font_style = font_style;

	if ( curmestex >= GetSysReq(SYSREQ_MESCACHE_MAX) ) {	// エントリ数がオーバーしているものは次のフレームで破棄
		t->life = 0;
		t->buf[0] = 0;
	} else {
		//		キャッシュの登録
		if ( mylen >= ( TEXMES_NAME_BUFFER - 1 ) ) {
			t->text = (char *)malloc( mylen+1 );		// テキストハッシュネーム用バッファを作成する
			strcpy( t->text, msg );
		} else {
			strcpy( t->buf, msg );						// 標準バッファにコピーする
		}
	}

	id = (GLuint)t->texid;

	glBindTexture( GL_TEXTURE_2D, id );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

	EM_ASM_({
		var canvas = document.getElementById('hsp3dishFontCanvas');
		if (canvas) {
			document.body.removeChild(canvas);
		}
		canvas = document.createElement("canvas");
		canvas.id = 'hsp3dishFontCanvas';
		//canvas.style.setProperty("position", "absolute");
		canvas.style.setProperty("visibility", "hidden");
		canvas.width = $2;
		canvas.height = $3;
		document.body.appendChild(canvas);

		var context = canvas.getContext("2d");
		context.font = $1 + "px 'sans-serif'";

		var msg = Pointer_stringify($0);
		context.clearRect ( 0 , 0 , $2 , $3);
		//context.fillStyle = 'rgba(0, 0, 255, 255)';
		context.fillStyle = 'rgba(255, 255, 255, 255)';
		//context.strokeText(msg, 150, 150);
		context.fillText(msg, 0, $1);
		console.log(msg);

		//GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.ALPHA, GLctx.ALPHA, GLctx.UNSIGNED_BYTE, canvas);
		GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, canvas);
		}, msg, font_size, t->sx, t->sy);

	glBindTexture(GL_TEXTURE_2D, 0);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return texid;
#endif

#if defined(HSPLINUX)
	return -1;
#endif
}
