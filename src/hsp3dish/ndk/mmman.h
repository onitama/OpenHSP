
//
//	mmman.cpp functions (iOS)
//
#ifndef __mmman_h
#define __mmman_h

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//	flags
//
enum
{
MMDATA_NONE = 0,
MMDATA_INTWAVE,
MMDATA_MAX,
};

struct MMM;

//  class
class MMMan {
public:
	MMMan();
	~MMMan();

	void Reset( void *hwnd );
	void ClearAllBank( void );
	void DeleteBank( int bank );
	int AllocBank( void );
	int SearchBank( int num );
	MMM *SetBank( int num, int flag, int opt, void *mempt, char *fname );

	int Load( char *fname, int bank, int opt );
	int Play( int bank );
	void StopNum( int num );
	void Pause( void );
	void Resume( void );
	void Stop( void );
	void StopBank( int bank=-1 );
	void Notify( void );

	void PlayBank( MMM *mmm );
	void StopBank( MMM *mmm );
	void PauseBank( MMM *mmm );
	void ResumeBank( MMM *mmm );
	void SeekBank( MMM *mmm, int pos, SLuint32 seekMode );
	int BankLoad( MMM *mmm, char *fname );
	void SetLoopBank( MMM *mmm, int flag );

	void GetInfo( int bank, char **fname, int *num, int *flag, int *opt );
	int GetBusy( void );
	void SetWindow( void *hwnd, int x, int y, int sx, int sy );
	int GetBankMax( void ) { return mm_cur;  };

	void SetVol( int bank, int vol );
	void SetPan( int bank, int pan );
	int GetStatus( int bank, int infoid );

private:
	int mm_cur;
	MMM *mem_snd;
	int curmus;			// current playing MIDI (-1=none)
	char res[256];
	void *hwm;
	void *avi_wnd;
	int	avi_x, avi_y,avi_sx,avi_sy;
	char avi_wh[64];

	int engine_flag;
    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject;

	SLuint32 GetState( MMM *mmm );
	void SetState( MMM *mmm, SLuint32 state );

};


#endif
