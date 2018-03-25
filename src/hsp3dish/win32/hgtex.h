#ifndef	__HGTEX_H__
#define	__HGTEX_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

	// Font options
#define TEXFONT_OPT_NORMAL 0
#define TEXFONT_OPT_BOLD 1
#define TEXFONT_OPT_BOLD2 2
#define TEXFONT_OPT_BOLD3 3
#define TEXFONT_OPT_ITALIC 4
#define TEXFONT_OPT_NOANTI 0x1000			// アンチエイリアスなし
#define TEXFONT_OPT_NOCOLOR 0x2000			// アルファ変更のみ
#define TEXFONT_OPT_NOLOCK 0x10000			// vramのlockなし(CalcTextDrawSize用)

#define TEXFONT_ROT_NORMAL 0
#define TEXFONT_ROT_ROTATE 1

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
int GetNextTexID( void );
void DeleteTex( int id );
void ChangeTex( int id );
TEXINF *GetTex( int id );
void SetSrcTex( void *src, int sx, int sy );
void ClearTex( int id, int color );
void SendMesTex( int id, char *vram );

void DrawTexString( int xx, int yy, char *Buffer );
void DrawTexColor( DWORD TextColor );
void DrawTexSpacing( int offset );
void DrawTexLineSpacing( int offset );
void DrawTexRotate( int rotopt );
void DrawTexOpen( HWND hwnd, int id, char *fontname, int size, int option );
void DrawTexRect( int xx, int yy, int sx, int sy, DWORD color );
int DrawTestY( void );
void DrawTexClose( void );

int TexGetDrawSizeX( void );
int TexGetDrawSizeY( void );

/*
void DrawTexString( int id, int xx, int yy, DWORD TextColor, char *Buffer );
void DrawTexOpen( HWND hWnd, char *fontname, int size );
void DrawTexClose( void );
*/

int CreateTexture2( int w, int h, D3DFORMAT Format, LPDIRECT3DTEXTURE8 &pTexture );
void TexDivideSize( int id, int new_divsx, int new_divsy, int new_ofsx, int new_ofsy );

/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
