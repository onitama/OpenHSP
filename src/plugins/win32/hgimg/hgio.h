
//
//	hgio.cpp structures
//
#ifndef __hgio_h
#define __hgio_h

//		command callback function
//

struct HGProc
{
	void (*setDest)( void *dest, int sx, int sy );
	void (*releaseDest)( void );

	void (*ClearDest)( int mode, int color, int tex );
	void (*DrawLineF4)( POLY4 *lpPolyData );
	void (*DrawLineF4G)( POLY4G *lpPolyData );
	void (*DrawPolygonF4)( POLY4 *lpPolyData );
	void (*DrawPolygonTex)( POLY4 *lpPolyData );
	void (*DrawPolygonF4G)( POLY4G *lpPolyData );
	void (*DrawPolygonTexG)( POLY4G *lpPolyData );
	void (*DrawSpriteTex)( P_SPRITE *spr );
	void (*DrawBGSpriteTex)( P_BGSPR *bg );
	void (*DrawFontSprite)( int x, int y, int cx, int cy, int px, char *mes, int attr, int tex );
	void (*DrawPolygonTLG)( POLY4GX *poly );
	void (*DrawLineTLG)( POLY4GX *poly );
	void (*SetViewMatrix)( P_MAT *pmat );

	void (*TexInit)( void );
	void (*TexTerm)( void );
	int (*RegistTex)( char *data, int sx, int sy, int width, int height, int sw );
	int (*GetNextTexID)( void );
	void (*DeleteTex)( int id );
	void (*ChangeTex)( int id );
	TEXINF *(*GetTex)( int id );
	void (*SetSrcTex)( void *src, int sx, int sy );
};

void hgio_init( HGProc *p );
void hgio_term( void );

#endif
