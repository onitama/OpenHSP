//
//		MMMAN : Multimedia manager source
//				for SDL enviroment
//				onion software/onitama 2012/6
//				zakki 2014/7
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "../../hsp3/hsp3config.h"
#include "../../hsp3/dpmread.h"
#include "../../hsp3/strbuf.h"
#include "../supio.h"

#include "mmman.h"

#define sndbank(a) (char *)(mem_snd[a].mempt)

#define MIX_MAX_CHANNEL 32

//---------------------------------------------------------------------------

//	MMDATA structure
//
typedef struct MMM
{
	//	Multimedia Data structure
	//
	int		flag;			//	bank mode (0=none/1=wav/2=mid/3=cd/4=avi)
	int		opt;			//	option (0=none/1=loop/2=wait/16=fullscr)
	int		num;			//	request number
	short	track;			//	CD track No.
	short	lasttrk;		//	CD last track No.
	void	*mempt;			//	pointer to sound data
	char	*fname;			//	sound filename (sbstr)
	// int		vol;
	// int		pan;
	int		start;
	int		end;
	
	Mix_Chunk	*chunk;
	int		channel;
	// int	pause_flag;

} MMM;


//---------------------------------------------------------------------------

MMMan::MMMan()
{
	mm_cur = 0;

	mem_snd = NULL;
	engine_flag = false;

	Mix_Init(MIX_INIT_OGG);
	// Mix_ReserveChannels(16);

	int ret = Mix_OpenAudio(0, 0, 0, 0);
	//assert(ret == 0);

	engine_flag = ret == 0;
}


MMMan::~MMMan()
{
	ClearAllBank();

	while(Mix_Init(0))
		Mix_Quit();
	Mix_CloseAudio();
}

void MMMan::DeleteBank( int bank )
{
	MMM *m;
	char *lpSnd;

	m = &(mem_snd[bank]);
	if ( m->flag == MMDATA_INTWAVE ) {
		StopBank( m );
		Mix_FreeChunk( m->chunk );
	}
	lpSnd = sndbank( bank );
	if ( lpSnd != NULL ) {
		free( lpSnd );
	}
	mem_snd[bank].mempt=NULL;
}


int MMMan::AllocBank( void )
{
	if ( !engine_flag ) return -1;

	int ids = mm_cur++;
	int sz = mm_cur * sizeof(MMM);
	if ( mem_snd == NULL ) {
		mem_snd = (MMM *)sbAlloc( sz );
	} else {
		mem_snd = (MMM *)sbExpand( (char *)mem_snd, sz );
	}
	mem_snd[ids].flag = MMDATA_NONE;
	mem_snd[ids].num = -1;
	mem_snd[ids].channel = -1;
	return ids;
}


int MMMan::SearchBank( int num )
{
	for(int a=0;a<mm_cur;a++) {
		if ( mem_snd[a].num == num ) return a;
	}
	return -1;
}


MMM *MMMan::SetBank( int num, int flag, int opt, void *mempt, char *fname, int start, int end )
{
	int bank;
	MMM *m;

	bank = SearchBank( num );
	if ( bank < 0 ) {
		bank = AllocBank();
	} else {
		DeleteBank( bank );
	}

	if ( bank < 0 ) return NULL;

	m = &(mem_snd[bank]);
	m->flag = flag;
	m->opt = opt;
	m->num = num;
	m->mempt = mempt;
	m->fname = NULL;
	// m->pause_flag = 0;
	// m->vol = 0;
	// m->pan = 0;
	m->start = start;
	m->end = end;
	return m;
}


void MMMan::ClearAllBank( void )
{
	int a;
	if ( mem_snd != NULL ) {
		Stop();
		for(a=0;a<mm_cur;a++) {
			DeleteBank( a );
		}
		sbFree( mem_snd );
		mem_snd = NULL;
		mm_cur = 0;
	}
}


void MMMan::Reset( void *hwnd )
{
	ClearAllBank();
}


void MMMan::SetWindow( void *hwnd, int x, int y, int sx, int sy )
{
}


void MMMan::Pause( void )
{
	//		pause all playing sounds
	//
	Mix_Pause( -1 );
}


void MMMan::Resume( void )
{
	//		resume all playing sounds
	//
	Mix_Resume( -1 );
}


void MMMan::Stop( void )
{
	//		stop all playing sounds
	//
	Mix_HaltChannel( -1 );
}


void MMMan::StopBank( MMM *mmm )
{
}


void MMMan::PauseBank( MMM *mmm )
{
}


void MMMan::ResumeBank( MMM *mmm )
{
}


void MMMan::PlayBank( MMM *mmm )
{
}


int MMMan::BankLoad( MMM *mmm, char *fname )
{
	if ( mmm == NULL ) return -9;
	mmm->chunk = Mix_LoadWAV( fname );
	mmm->channel = mmm->num;
	return 0;
}


int MMMan::Load( char *fname, int num, int opt, int start, int end )
{
	//		Load sound to bank
	//			opt : 0=normal/1=loop/2=wait/3=continuous
	//
	if ( num < 0 || num >= MIX_MAX_CHANNEL ) return -1;
	int flag,res;
	MMM *mmm;

	flag = MMDATA_INTWAVE;
	mmm = SetBank( num, flag, opt, NULL, fname, start, end );

	if ( mmm != NULL ) {
		res = BankLoad( mmm, fname );
		if ( res ) {
			mmm->flag = MMDATA_NONE;
			Alertf( "[MMMan] Failed %s on bank #%d (%d)",fname,num,res );
			return -1;
		}
		//if ( opt == 1 ) SetLoopBank( mmm, opt );
	}
	Alertf( "[MMMan] Loaded %s on bank #%d",fname,num );
	return 0;
}


int MMMan::Play( int num, int ch )
{
	//		Play sound
	//
	int bank;
	MMM *m;
	bank = SearchBank(num);
	if ( bank < 0 ) return 1;
	m = &(mem_snd[bank]);
	if ( m->flag == MMDATA_INTWAVE && m != NULL ) {
		if ( ch < 0 ) ch = m->channel;
		if ( ch >= 0 && ch < MIX_MAX_CHANNEL ) {
			bool loop = m->opt & 1;
			Mix_HaltChannel( ch );
#ifdef HSPLINUX
			Mix_PlayChannel( ch, m->chunk, loop ? -1 : 0 );
#else
			Mix_PlayChannel( ch, m->chunk, loop ? -1 : 0 , m->start, m->end );
#endif
		}
	}
	return 0;
}


void MMMan::Notify( void )
{
}


void MMMan::GetInfo( int bank, char **fname, int *num, int *flag, int *opt )
{
	//		Get MMM info
	//
	MMM *mmm;
	mmm=&mem_snd[bank];
	*fname = mmm->fname;
	*opt=mmm->opt;
	*flag=mmm->flag;
	*num=mmm->num;
}


void MMMan::SetVol( int num, int vol )
{
	if ( num < 0 || num >= MIX_MAX_CHANNEL ) return;
	if ( vol >     0 ) vol =     0;
	if ( vol < -1000 ) vol = -1000;
	Mix_Volume( num, (int)(MIX_MAX_VOLUME * ((float)(vol + 1000) / 1000.0f)) );
}



void MMMan::SetPan( int num, int pan )
{
	if ( num < 0 || num >= MIX_MAX_CHANNEL ) return;
	if ( pan >  1000 ) pan =  1000;
	if ( pan < -1000 ) pan = -1000;
	int l, r;
	int max = 255;
	if ( pan > 0 ) {
		l = (1000 - pan) * max;
		r = 1000 * max;
	} else {
		l = 1000 * max;
		r = (1000 + pan) * max;
	}
	Mix_SetPanning( num, l / 1000, r / 1000 );
}


int MMMan::GetStatus( int num, int infoid )
{
	if ( num >= MIX_MAX_CHANNEL ) return 0;
	int res = 0;
	switch( infoid ) {
	case 16:
		if ( num < 0 ) {
			res = Mix_Playing( -1 );
		} else {
			res = Mix_Playing( num );
		}
		break;
	}
	return res;
}


void MMMan::StopBank( int num )
{
	//		stop playing sound
	//
	if ( num < 0 ) {
		Stop();
	} else if ( num < MIX_MAX_CHANNEL ) {
		Mix_HaltChannel( num );
	}
	return;
}
