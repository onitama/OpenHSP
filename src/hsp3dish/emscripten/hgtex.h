//
//	hgtex.cpp header
//
#ifndef __hgtex_h
#define __hgtex_h

#ifdef __cplusplus
extern "C" {
#endif

#define TEXINF_MAX 256
#define TEXMES_CACHE_DEFAULT 8		// キャッシュのデフォルト生存フレーム
#define TEXMES_NAME_BUFFER 32		// テキストハッシュネーム用バッファサイズ

// テクスチャ情報
typedef struct
{
short mode;			// mode
short opt;			// option
short sx;			// x-size
short sy;			// y-size
short width;		// real x-size
short height;		// real y-size
int texid;			// OpenGL TexID(GLuint)
float ratex;		// 1/sx
float ratey;		// 1/sy

short hash;			// Text Hashcode (short)
short life;			// Text Cache life
char *text;			// Text Message
int font_size;		// Text font size
int font_style;		// Text font style
char buf[TEXMES_NAME_BUFFER];		// Text Data Buffer

} TEXINF;

enum {
TEXMODE_NONE = 0,
TEXMODE_NORMAL,
TEXMODE_MES8,
TEXMODE_MES4,
TEXMODE_MAX,
};


TEXINF *GetTex( int id );
void TexInit( void );
void TexReset( void );
void TexTerm( void );
void ChangeTex( int id );
void DeleteTex( int id );
void DeleteTexInf( TEXINF *t );

void TexProc( void );

int RegistTexMem( unsigned char *ptr, int size );
int RegistTex( char *fname );
int MakeEmptyTex( int width, int height );

int GetCacheMesTextureID( char *msg, int font_size, int font_style );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



