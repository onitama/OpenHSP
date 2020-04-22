#ifndef	__HGTEX_H__
#define	__HGTEX_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#define TEXINF_MAX 512
// テクスチャ情報
typedef struct
{
short flag;			// enable flag
short mode;			// mode
short sx;			// x-size
short sy;			// y-size
short width;		// real x-size
short height;		// real y-size
char *data;			// real data
float ratex;		// 1/sx
float ratey;		// 1/sy

short	divx, divy;					// Divide value for CEL
short	divsx, divsy;				// CEL size
short	celofsx, celofsy;			// CEL center offset

} TEXINF;

enum {
	TEXMODE_NONE = 0,
	TEXMODE_NORMAL,
	TEXMODE_MES8,
	TEXMODE_MES4,
	TEXMODE_MAX,
};

void TexInit( void );
void TexTerm( void );
void TexReset( void );
void TexSetD3DParam( LPDIRECT3D8 p1, LPDIRECT3DDEVICE8 p2, D3DDISPLAYMODE p3 );

int RegistTex( char *data, int sx, int sy, int width, int height, int sw );
int RegistTexIndex( char *data, char *palette, int sx, int sy, int width, int height, int sw, int pals );
int RegistTexEmpty( int w, int h, int tmode );
int UpdateTex( int texid, char *data, int sw );
int UpdateTexStar(int texid, int mode);
int UpdateTex32(int texid, char* srcptr, int mode);

int GetNextTexID( void );
void DeleteTex( int id );
void ChangeTex( int id );
TEXINF *GetTex( int id );
void SetSrcTex( void *src, int sx, int sy );

int CreateTexture2( int w, int h, D3DFORMAT Format, LPDIRECT3DTEXTURE8 &pTexture );
void TexDivideSize( int id, int new_divsx, int new_divsy, int new_ofsx, int new_ofsy );

/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
