
//
//	mmman.cpp functions (iOS)
//
#ifndef __mmman_h
#define __mmman_h

//	flags
//
enum
{
MMDATA_NONE = 0,
MMDATA_INTWAVE,
MMDATA_MCIVOICE,
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
	void Stop( void );
	void Notify( void );

	void GetInfo( int bank, char **fname, int *num, int *flag, int *opt );
	int GetBusy( void );
	void SetWindow( void *hwnd, int x, int y, int sx, int sy );
	int GetBankMax( void ) { return mm_cur;  };

	void StopBank( int bank=-1 );
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
};

//---------------------------------------------------------------------------
//	dxsnd service
//---------------------------------------------------------------------------

#define SNDINF_MAX 512

#define SNDFLAG_NONE 0
#define SNDFLAG_READY_WAV 1
#define SNDFLAG_READY_OGG 2
#define SNDFLAG_LOOP 0x1000

int SndInit( void *hWnd );
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



#endif
