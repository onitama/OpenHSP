#ifndef texmes_H_
#define texmes_H_

#ifdef HSPDISHGP
#include "gameplay.h"
#else
#include <string>
#endif

#ifdef HSPDISHGP
using namespace gameplay;
#endif

/**
 * Texture cache class.
 */

#define TEXMESINF_MAX 256			// テクスチャキャッシュの最大数
#define TEXMES_CACHE_DEFAULT 8		// キャッシュのデフォルト生存フレーム
#define TEXMES_NAME_BUFFER 64		// テキストハッシュネーム用バッファサイズ
#define TEXMES_BUFFER_MAX 0x40000	// 初期テクスチャバッファバッファサイズ

// テクスチャキャッシュ情報
class texmes {
public:
	texmes();
	~texmes();
	void reset(int sizex, int sizey, int p_texsx, int p_texsy, void *data);
	void clear(void);
	void terminate(void);
	int registText(char* msg);

	int entry;			// entry ID
	int flag;			// flag (0=none/1=enable)
	int sx;				// x-size (real)
	int sy;				// y-size (real)
	int texsx;			// texture x-size
	int texsy;			// texture y-size
	float ratex;		// 1/sx
	float ratey;		// 1/sy

	int hash;			// Text Hashcode
	int life;			// Text Cache life
	char* text;			// Long Text Message
	int textsize;		// Long Text Message size
	int font_size;		// Text font size
	int font_style;		// Text font style
	char buf[TEXMES_NAME_BUFFER];		// Text Data Buffer

#ifdef HSPDISHGP
	Texture::Sampler *_texture;
#else
	int _texture;
#endif
};

class texmesPos;

// メッセージマネージャー
class texmesManager {
public:
	texmesManager();
	~texmesManager();

	void texmesInit(int maxmes);
	void texmesTerm(void);
	void texmesProc(void);

	void setFont(char *fontname, int size, int style);
	int texmesRegist(char* msg, texmesPos *info=NULL);
	texmes* texmesGet(int id);
	texmes* texmesUpdateLife(int id);

	int _area_px, _area_py;
	int _fontsize, _fontstyle;
	std::string	_fontname;

protected:
	int Get2N(int val);
	texmes* addTexmes(void);
	int str2hash(char* msg, int* out_len);
	int texmesGetCache(char* msg, short mycache);
	unsigned char* texmesGetFont(char* msg, int* out_sx, int *out_sy, int* out_tsx, int* out_tsy, texmesPos *info);
	unsigned char *texmesBuffer(int size);

	// gptexmes
	int _maxtexmes;
	texmes* _texmes;
	int _texmesbuf_max;
	unsigned char* _texmesbuf;

};


#endif
