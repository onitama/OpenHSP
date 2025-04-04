//
//	functions for ddraw.cpp
//
int dd_ini( HWND hwnd, int wx, int wy, int bpp, char winmode, char sysflag);
int dd_dwindow( HINSTANCE hInst,HWND ghwnd);
void dd_bye( void );
int dd_buffer( int wid, HDC shdc, int sx, int sy, int xpitch, char *ctbl, int sysmem, int trans,unsigned char *srcbuf,unsigned char pmflag);
void dd_release( int wid );
void dd_palinit( void );
void dd_palarea( int cst, int clen );
void dd_palfade( int val );
int dd_getwid( void );
int dd_flip( HDC hdc,int times, int vsync );
int dd_getcaps( int id, int emu );

int dd_copy( LPRECT dst, int wid, LPRECT src );
int dd_copyn( LPRECT dst, int wid, LPRECT src );
void dd_fcopy( int x, int y, int wid, LPRECT src );
void dd_fcopyn( int x, int y, int wid, LPRECT src );

void dd_boxf( LPRECT lprect, COLORREF col );
void dd_boxf2( LPRECT lprect, int col );
void dd_print( int x, int y, char *mes, int id );
void dd_fprint( int x, int y, char *mes, HFONT fon, COLORREF col, int id );

int dd_getbuf( int wid, HDC dhdc, int sx, int sy, int xpitch );
int dd_bufcopy( int tarid, LPRECT dst, int srcid, LPRECT src, int opt );
void dd_palset( int pal, int rval, int gval, int bval );
int dd_getdbg( void );

void dd_palsethsp(char *ctbl);
int dd_islost(int begin,int end);
void dd_saver(void);
