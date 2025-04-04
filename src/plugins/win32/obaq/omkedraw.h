
//
//	omkedraw.cpp functions
//
#include "hsp3plugin.h"

void OmkeDrawLineStart( BMSCR *bm, int col );
void OmkeDrawLineEnd( BMSCR *bm );
void OmkeDrawLine( BMSCR *bm, int cx, int cy, int xx,int yy );
void OmkeDrawCircle( BMSCR *bm, int x,int y,int range, int col );
void OmkeDrawSetGMode( int gmode, int grate, int ofsx, int ofsy );
void OmkeDrawSprite( BMSCR *bm, BMSCR *bm2, int celid, int putx, int puty, float rot, float zx, float zy );
void OmkeDrawSprite2( BMSCR *bm, BMSCR *bm2, int celid, int putx, int puty, float rot, float zx, float zy );

