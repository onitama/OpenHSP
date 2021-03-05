
#include <stdio.h>
#include <string.h>

#ifndef HSPDISHGP
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#endif

#include "../hsp3/hsp3config.h"

#include "supio.h"
#include "sysreq.h"
#include "hgio.h"
#include "texmes.h"

/*------------------------------------------------------------*/
/*
		gameplay Node Obj
*/
/*------------------------------------------------------------*/

texmes::texmes()
{
	// コンストラクタ
	flag = 0;
	text = NULL;
	textsize = 0;
#ifdef HSPDISHGP
	_texture = NULL;
#else
	_texture = -1;
#endif
}

texmes::~texmes()
{
	terminate();
}

void texmes::clear(void)
{
#ifdef HSPDISHGP
	if (_texture) {
		delete _texture;
		_texture = NULL;
	}
#else
	if (_texture>=0) {
		hgio_fontsystem_delete(_texture);
		_texture = -1;
	}
#endif
	flag = 0;
}


void texmes::terminate(void)
{
	clear();
	if (text) {
		free(text);		// 拡張されたネーム用バッファがあれば解放する
		text = NULL;
	}
}


void texmes::reset(int width, int height, int p_texsx, int p_texsy, void *data)
{
	clear();

	flag = 1;
	sx = width;
	sy = height;
	texsx = p_texsx;
	texsy = p_texsy;

#ifdef HSPDISHGP
#ifdef HSPEMSCRIPTEN
	TextureHandle texid;
	texid = hgio_fontsystem_setup( texsx, texsy, data);
	Texture* texture = Texture::create(texid,texsx, texsy, Texture::Format::RGBA);
#else
	Texture* texture = Texture::create(Texture::Format::RGBA, texsx, texsy, NULL, false, Texture::TEXTURE_2D);
	texture->setData((const unsigned char*)data);
#endif
	// Bind the texture to the material as a sampler
	_texture = Texture::Sampler::create(texture); // +ref texture
	_texture->setFilterMode(Texture::Filter::NEAREST, Texture::Filter::NEAREST);
	SAFE_RELEASE(texture);

#else
	_texture = hgio_fontsystem_setup( texsx, texsy, data);

#endif

	ratex = ( 1.0f / (float)texsx);
	ratey = ( 1.0f / (float)texsy);
	hash = 0;
	life = TEXMES_CACHE_DEFAULT;
	font_size = 0;
	font_style = 0;
	if (text!=NULL) text[0] = 0;
	buf[0] = 0;
}

int texmes::registText(char* msg)
{
	//		文字列を設定する
	//
	int mylen = strlen(msg);
	if (mylen >= TEXMES_NAME_BUFFER) {
		int size = mylen + 1;
		if ( size > textsize) {
			if (text != NULL) {
				free(text);
			}
			text = (char*)malloc(size);		// テキストハッシュネーム用バッファを作成する
			textsize = size;
		}
		strcpy(text, msg);
	}
	else {
		strcpy(buf, msg);						// 標準バッファにコピーする
	}
	return 0;
}


/*------------------------------------------------------------*/
/*
		Texture message manager
*/
/*------------------------------------------------------------*/

texmesManager::texmesManager()
{
	// コンストラクタ
	_texmes = NULL;
	_texmesbuf = NULL;
}

texmesManager::~texmesManager()
{
	texmesTerm();
}

void texmesManager::texmesInit(int maxmes)
{
	texmesTerm();

	_maxtexmes = maxmes;
	if (_maxtexmes< TEXMESINF_MAX) _maxtexmes = TEXMESINF_MAX;

	_texmes = new texmes[_maxtexmes];

	_fontsize = 0;
	_fontstyle = 0;
	setFont("",18,0);

	int i;
	for (i = 0; i < _maxtexmes; i++) {
		_texmes[i].entry = i;
	}

	_texmesbuf_max = 0;
	texmesBuffer(TEXMES_BUFFER_MAX);

}


void texmesManager::texmesTerm(void)
{
	if (_texmes) {
		int i;
		for (i = 0; i < _maxtexmes; i++) {
			_texmes[i].clear();
		}
		delete[] _texmes;
		_texmes = NULL;
	}
	if (_texmesbuf) {
		free(_texmesbuf);
		_texmesbuf = NULL;
	}

}


unsigned char*texmesManager::texmesBuffer(int size)
{
	//	空のテクスチャバッファを準備する
	//
	if (size > _texmesbuf_max) {
		free(_texmesbuf);
		_texmesbuf_max = size;
		_texmesbuf = (unsigned char*)malloc(_texmesbuf_max);
	}
	memset(_texmesbuf,0,size);
	return _texmesbuf;
}


texmes* texmesManager::addTexmes(void)
{
	//	空のtexmesを生成する
	//
	int i;
	for (i = 0; i < _maxtexmes; i++) {
		if (_texmes[i].flag == 0) return &_texmes[i];
	}
	return NULL;
}


void texmesManager::texmesProc(void)
{
	//		フレーム単位でのキャッシュリフレッシュ
	//		(キャッシュサポート時は、毎フレームごとに呼び出すこと)
	//
	int i;
	texmes* t;

	if (_texmes == NULL) return;

	t = _texmes;
	for (i = 0; i < _maxtexmes; i++) {
		if (t->flag) {							// メッセージテクスチャだった時
			if (t->life > 0) {
				t->life--;						// キャッシュのライフを減らす
			}
			else {
				t->clear();						// テクスチャのエントリを破棄する
			}
		}
		t++;
	}
}


int texmesManager::texmesGetCache(char* msg, short mycache)
{
	//		キャッシュ済みの文字列があればidを返す
	//		(存在しない場合は-1)
	//
	int i;
	texmes* t;
	t = _texmes;
	for (i = 0; i < _maxtexmes; i++) {
		if (t->flag) {							// 使用中だった時
			if (t->hash == mycache) {			// まずハッシュを比べる
				if (t->font_size == _fontsize && t->font_style == _fontstyle) {	// サイズ・スタイルを比べる
					if (t->text) {
						if (strcmp(msg, t->text) == 0) {
							t->life = TEXMES_CACHE_DEFAULT;			// キャッシュを保持
							return i;
						}
					}
					else {
						if (strcmp(msg, t->buf) == 0) {
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


texmes* texmesManager::texmesGet(int id)
{
	if ((id < 0) || (id >= _maxtexmes)) {
		return NULL;
	}
	return &_texmes[id];
}


texmes* texmesManager::texmesUpdateLife(int id)
{
	if ((id < 0) || (id >= _maxtexmes)) {
		return NULL;
	}
	_texmes[id].life = TEXMES_CACHE_DEFAULT;
	return &_texmes[id];
}


int texmesManager::str2hash(char* msg, int* out_len)
{
	//		文字列の簡易ハッシュを得る
	//		同時にout_lenに文字列長を返す
	//
	int len;
	short cache;
	unsigned char a1;
	unsigned char* p;
	p = (unsigned char*)msg;
	len = 0;
	a1 = *p;
	cache = ((short)a1) << 8;		// 先頭の文字コードを上位8bitにする
	while (1) {
		if (a1 == 0) break;
		a1 = *p++;
		len++;
	}
	*out_len = len;
	if (len > 0) {			// 終端の文字コードを下位8bitにする
		p--;
		cache += (short)*p;
	}
	return cache;
}


int texmesManager::Get2N(int val)
{
	int res = 1;
	while (1) {
		if (res >= val) break;
		res <<= 1;
	}
	return res;
}

int texmesManager::texmesRegist(char* msg, texmesPos *info)
{
	//		キャッシュ済みのテクスチャIDを返す(texmesIDを返す)
	//		(作成されていないメッセージテクスチャは自動的に作成する)
	//		(作成の必要がない場合は-1を返す)
	//
	int mylen;
	int mycache;
	int texid;
	unsigned char* pImg;
	int tsx, tsy, sx, sy;
	texmes* tex;

	mycache = str2hash(msg, &mylen);			// キャッシュを取得
	if (mylen <= 0) return -1;

	texid = texmesGetCache(msg, mycache);
	if (texid >= 0) {
		tex = texmesGet(texid);
		_area_px = tex->sx;
		_area_py = tex->sy;
		return texid;							// キャッシュがあった
	}

	//		ビットマップを作成
	pImg = texmesGetFont(msg, &sx, &sy, &tsx, &tsy, info);
	if (pImg == NULL) return -1;

	//		キャッシュが存在しないので作成
	tex = addTexmes();
	if (tex == NULL) return -1;

	tex->reset(sx,sy,tsx,tsy,pImg);
	_area_px = sx;
	_area_py = sy;

	tex->registText(msg);
	tex->hash = mycache;
	tex->font_size = _fontsize;
	tex->font_style = _fontstyle;

	return tex->entry;
}


unsigned char* texmesManager::texmesGetFont(char* msg, int* out_sx, int* out_sy, int *out_tsx, int *out_tsy, texmesPos *info)
{
	int sx, sy, size;
	int pitch,tsx,tsy;

	if (info) {
		info->length = 0;
	}

	hgio_fontsystem_exec(msg, NULL, 0, &sx, &sy, info);

	if ((sx == 0) || (sy == 0)) return NULL;

	tsx = Get2N(sx);
	tsy = Get2N(sy);
	pitch = tsx;
	*out_tsx = tsx;
	*out_tsy = tsy;

	//Alertf("%d x %d (%d,%d)", sx, sy, tsx, tsy);

	size = pitch * tsy * sizeof(int);
	unsigned char* buf = texmesBuffer(size);

	hgio_fontsystem_exec(msg, buf, pitch, &sx, &sy, NULL);

	*out_sx = sx;
	*out_sy = sy;

	//int* data = (int*)buf;
	//for (int i = 0; i < (pitch * tsy); i++) { *data++ = 0xffffffff; }

	return buf;
}


void texmesManager::setFont(char* fontname, int size, int style)
{
	// フォント設定
	//
	if ((_fontsize == size)&&(_fontstyle == style)) {
		if ( strcmp(_fontname.c_str(), fontname )==0 ) {
			return;
		}
	}
	_fontsize = size;
	_fontstyle = style;
	_fontname = fontname;
	hgio_fontsystem_init(fontname, size, style);
}


