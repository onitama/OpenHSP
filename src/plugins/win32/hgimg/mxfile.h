//---------------------------------------------------------------------------
#ifndef __mxfile_h
#define __mxfile_h

void mxInit( hgdraw *hg, char *fileptr, int mode );
void mxTerm( void );
int mxGetMaxImage( void );
char *mxGetImage( int id );
int mxConvHG( hgdraw *hg );
int mxGetNumPoly( void );
int mxConvHGAnim( hgdraw *hg, char *fileptr );
int mxSaveHG( hgdraw *hg, char *sname, int modelid, char *texlist );

//---------------------------------------------------------------------------
#endif
