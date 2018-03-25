#ifndef	__DXSND_H__
#define	__DXSND_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

#define SNDINF_MAX 512

#define SNDFLAG_NONE 0
#define SNDFLAG_READY_WAV 1
#define SNDFLAG_READY_OGG 2
#define SNDFLAG_LOOP 0x1000

int SndInit( HWND hWnd );
void SndTerm( void );
void SndReset( void );
void SndDelete( int id );
void SndSetVolume( int id, int vol );
void SndSetPan( int id, int pan );
void SndSetLoop( int id, int loop );
int SndRegistWav( int newid, char *mem, int option );
int SndRegistOgg( int newid, char *fname, int option );
void SndStopAll( void );

void SndPlay( int id );
void SndPlayPos( int id, int pos );
void SndStop( int id );
int SndGetStatus( int id, int option );
double SndGetTime( int id, int option );

/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
