
#include <stdio.h>
#include <string.h>
#include "gamehsp.h"

#include "../../hsp3/hsp3config.h"
#include "../supio.h"
#include "../sysreq.h"

int hgio_fontsystem_exec(char* msg, unsigned char* buffer, int pitch, int* out_sx, int* out_sy);

/*------------------------------------------------------------*/
/*
		gameplay Node Obj
*/
/*------------------------------------------------------------*/

gptexmes::gptexmes()
{
	// コンストラクタ
	flag = 0;
	text = NULL;
	_texture = NULL;
}

gptexmes::~gptexmes()
{
}

void gptexmes::clear(void)
{
	if (_texture) {
		delete _texture;
		_texture = NULL;
	}
	if (text) {
		free(text);		// 拡張されたネーム用バッファがあれば解放する
		text = NULL;
	}
	flag = 0;
}


void gptexmes::reset(int width, int height, int p_texsx, int p_texsy, void *data)
{
	clear();

	flag = 1;
	sx = width;
	sy = height;
	texsx = p_texsx;
	texsy = p_texsy;

	Texture* texture = Texture::create(Texture::Format::RGBA, texsx, texsy, NULL, false, Texture::TEXTURE_2D);
	texture->setData((const unsigned char*)data);

	// Bind the texture to the material as a sampler
	_texture = Texture::Sampler::create(texture); // +ref texture
	_texture->setFilterMode(Texture::Filter::NEAREST, Texture::Filter::NEAREST);

	ratex = ( 1.0f / (float)texsx) * (sx-1);
	ratey = ( 1.0f / (float)texsy) * (sy-1);
	hash = 0;
	life = TEXMES_CACHE_DEFAULT;
	font_size = 0;
	font_style = 0;
	text = NULL;
	buf[0] = 0;
}

int gptexmes::registText(char* msg)
{
	//		文字列を設定する
	//
	int mylen = strlen(msg);
	if (mylen >= (TEXMES_NAME_BUFFER - 1)) {
		text = (char*)malloc(mylen + 1);		// テキストハッシュネーム用バッファを作成する
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

void gamehsp::texmesInit(void)
{
	_maxtexmes = TEXMESINF_MAX;
	if (_maxtexmes <= GetSysReq(SYSREQ_MESCACHE_MAX)) _maxtexmes = GetSysReq(SYSREQ_MESCACHE_MAX)-1;

	_gptexmes = new gptexmes[_maxtexmes];

	_fontsize = 0;
	_fontstyle = 0;
	setFont("",18,0);

	int i;
	for (i = 0; i < _maxtexmes; i++) {
		_gptexmes[i].entry = i;
	}

	_texmesbuf_max = 0;
	texmesBuffer(TEXMES_BUFFER_MAX);

	int sx = 32;
	int sy = 32;

	Texture* texture;
	texture = Texture::create(Texture::Format::RGBA, sx, sy, NULL, false, Texture::TEXTURE_2D);
	//texture = Texture::create(id, sx, sy, Texture::Format::RGBA);
	//texture = Texture::create("chr.png");
	if (texture == NULL) {
		Alertf("ERR");
		return;
	}
	//texture->setData( (const unsigned char *)data );
	_fontMaterial = makeMaterialTex2D(texture, GPOBJ_MATOPT_NOMIPMAP);
	if (_fontMaterial == NULL) {
		Alertf("ERR");
		return;
	}
	SAFE_RELEASE(texture);

	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
		VertexFormat::Element(VertexFormat::COLOR, 4)
	};

	unsigned int elementCount = sizeof(elements) / sizeof(VertexFormat::Element);
	_meshBatch_font = MeshBatch::create(VertexFormat(elements, elementCount), Mesh::TRIANGLE_STRIP, _fontMaterial, true, 16, 256);
}


void gamehsp::texmesTerm(void)
{
	if (_gptexmes) {
		int i;
		for (i = 0; i < _maxtexmes; i++) {
			_gptexmes[i].clear();
		}
		delete[] _gptexmes;
		_gptexmes = NULL;
	}
	if (_texmesbuf) {
		free(_texmesbuf);
		_texmesbuf = NULL;
	}

}


unsigned char*gamehsp::texmesBuffer(int size)
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


gptexmes* gamehsp::addTexmes(void)
{
	//	空のgptexmesを生成する
	//
	int i;
	for (i = 0; i < _maxtexmes; i++) {
		if (_gptexmes[i].flag == 0) return &_gptexmes[i];
	}
	if (_meshBatch_font) {
		delete _meshBatch_font;
		_meshBatch_font = NULL;
	}

	return NULL;
}


void gamehsp::texmesProc(void)
{
	//		フレーム単位でのキャッシュリフレッシュ
	//		(キャッシュサポート時は、毎フレームごとに呼び出すこと)
	//
	int i;
	gptexmes* t;
	t = _gptexmes;
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


int gamehsp::texmesGetCache(char* msg, short mycache)
{
	//		キャッシュ済みの文字列があればidを返す
	//		(存在しない場合は-1)
	//
	int i;
	gptexmes* t;
	t = _gptexmes;
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


gptexmes* gamehsp::texmesGet(int id)
{
	if ((id<0)||(id>=_maxtexmes)) {
		return NULL;
	}
	return &_gptexmes[id];
}


int gamehsp::str2hash(char* msg, int* out_len)
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


int gamehsp::Get2N(int val)
{
	int res = 1;
	while (1) {
		if (res >= val) break;
		res <<= 1;
	}
	return res;
}


int gamehsp::texmesRegist(char* msg)
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
	gptexmes* tex;

	mycache = str2hash(msg, &mylen);			// キャッシュを取得
	if (mylen <= 0) return -1;

	texid = texmesGetCache(msg, mycache);
	if (texid >= 0) {
		tex = texmesGet(texid);
		_area_px = tex->sx;
		_area_py = tex->sy;
		return texid;							// キャッシュがあった
	}

	//		キャッシュが存在しないので作成
	tex = addTexmes();
	if (tex == NULL) return -1;

	//		ビットマップを作成
	pImg = texmesGetFont(msg, &sx, &sy, &tsx, &tsy);
	tex->reset(sx,sy,tsx,tsy,pImg);
	_area_px = sx;
	_area_py = sy;

	tex->registText(msg);
	tex->hash = mycache;
	tex->font_size = _fontsize;
	tex->font_style = _fontstyle;

	return tex->entry;
}



void gamehsp::texmesDraw(int x, int y, char* msg, Vector4* p_color)
{
	//		フォントメッセージを表示する
	//
	int id;
	gptexmes* tex;
	float psx, psy;
	float x1, y1, x2, y2, sx, sy;
	float tx0, ty0, tx1, ty1;

	float a_val = 1.0f;

	id = texmesRegist(msg);
	if (id < 0) return;
	tex = texmesGet(id);
	if (tex==NULL) return;

	//		meshのTextureを差し替える
	Uniform* samplerUniform = NULL;
	for (unsigned int i = 0, count = _spriteEffect->getUniformCount(); i < count; ++i)
	{
		Uniform* uniform = _spriteEffect->getUniform(i);
		if (uniform && uniform->getType() == GL_SAMPLER_2D)
		{
			samplerUniform = uniform;
			break;
		}
	}

	_fontMaterial->getParameter(samplerUniform->getName())->setValue(tex->_texture);

	//		描画する
	tx0 = 0.0f;
	ty0 = 0.0f;
	tx1 = tex->ratex;
	ty1 = tex->ratey;

	x1 = (float)x;
	y1 = (float)y;
	x2 = x1 + tex->sx;
	y2 = y1 + tex->sy;

	float* v = _bufPolyTex;

	*v++ = x1; *v++ = y2; v++;
	*v++ = tx0; *v++ = ty1;
	*v++ = p_color->x; *v++ = p_color->y; *v++ = p_color->z; *v++ = p_color->w;
	*v++ = x1; *v++ = y1; v++;
	*v++ = tx0; *v++ = ty0;
	*v++ = p_color->x; *v++ = p_color->y; *v++ = p_color->z; *v++ = p_color->w;
	*v++ = x2; *v++ = y2; v++;
	*v++ = tx1; *v++ = ty1;
	*v++ = p_color->x; *v++ = p_color->y; *v++ = p_color->z; *v++ = p_color->w;
	*v++ = x2; *v++ = y1; v++;
	*v++ = tx1; *v++ = ty0;
	*v++ = p_color->x; *v++ = p_color->y; *v++ = p_color->z; *v++ = p_color->w;

	static unsigned short indices[] = { 0, 1, 2, 3 };

	_meshBatch_font->start();
	_meshBatch_font->add(_bufPolyTex, 4, indices, 4);
	_meshBatch_font->finish();
	_meshBatch_font->draw();
}


unsigned char* gamehsp::texmesGetFont(char* msg, int* out_sx, int* out_sy, int *out_tsx, int *out_tsy)
{
	int sx, sy, size;
	int pitch,tsx,tsy;

	hgio_fontsystem_exec(msg, NULL, 0, &sx, &sy);

	tsx = Get2N(sx);
	tsy = Get2N(sy);
	pitch = tsx;
	*out_tsx = tsx;
	*out_tsy = tsy;

	//Alertf("%d x %d (%d,%d)", sx, sy, tsx, tsy);

	size = pitch * tsy * sizeof(int);
	unsigned char* buf = texmesBuffer(size);

	hgio_fontsystem_exec(msg, buf, pitch, &sx, &sy);

	*out_sx = sx;
	*out_sy = sy;

	//int* data = (int*)buf;
	//for (int i = 0; i < (pitch * tsy); i++) { *data++ = 0xffffffff; }

	return buf;
}


void gamehsp::setFont(char* fontname, int size, int style)
{
	// フォント設定
	_fontsize = size;
	_fontstyle = style;
	_fontname = fontname;
}


int gamehsp::drawFont(int x, int y, char* text, Vector4* p_color, int* out_ysize)
{
	// フォントで描画
	int xsize, ysize;

#ifdef USE_GPBFONT
	if (mFont == NULL) return 0;
	mFont->start();
	xsize = mFont->drawText(text, x, y, *p_color, size);
	mFont->finish();
	ysize = size;
#else
	texmesDraw(x, y, text, p_color);
	xsize = _area_px;
	ysize = _area_py;
#endif

	* out_ysize = ysize;
	return xsize;
}


