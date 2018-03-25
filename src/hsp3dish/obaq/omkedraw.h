
//
//	omkedraw.cpp functions
//
void OmkeDrawSetBMSCR( void *bmscr );

void OmkeDrawLineStart( int col );
void OmkeDrawLineEnd( void );
void OmkeDrawLineStrip( int cx, int cy );

void OmkeDrawLine( int cx, int cy, int xx,int yy );
void OmkeDrawCircle( int x,int y,int range, int col );
void OmkeDrawCelPut( int cx, int cy, void *bm_src, int id, float destx, float desty, float ang );
void OmkeDrawSetGMode( int gmode, int grate, int ofsx, int ofsy );

