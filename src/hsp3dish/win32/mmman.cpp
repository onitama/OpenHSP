
//
//		MMMAN : Multimedia manager source
//				for Windows95/NT 32bit enviroment
//					onion software/onitama 1997/8
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../../hsp3/hsp3config.h"

#ifdef HSPWIN
#include <windows.h>
#include <windowsx.h>
#include <direct.h>
#include <shlobj.h>

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "dxguid.lib")
#endif

/*
	rev 43
	mingw : error : 不明な関数 tolower
	に対処
*/
#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "../supio.h"
#include "../../hsp3/dpmread.h"
#include "../../hsp3/strbuf.h"
#include "mmman.h"

#ifdef MMMAN_USE_DXSND
#include "dxsnd.h"
#endif


/*
	rev 43
	mingw : warning : #pragma comment を無視する
	に対処
*/
#if defined( _MSC_VER )
#pragma comment(lib,"winmm.lib")
#endif

#define sndbank(a) (char *)(mem_snd[a].mempt)


MMMan::MMMan()
{
	//		initalize MM manager
	//
	mem_snd = NULL;
	mm_cur = 0;
	hwm = NULL;
}


MMMan::~MMMan()
{
	//		terminate MM manager
	//
	ClearAllBank();
#ifdef MMMAN_USE_DXSND
	SndTerm();
#endif
}


void MMMan::Reset( void *hwnd )
{
	ClearAllBank();
	curmus=-1;

#ifdef MMMAN_USE_DXSND
	if ( hwm == NULL ) {
		SndInit( (HWND)hwnd );
	}
#endif
	hwm = hwnd;
	avi_wnd = hwnd;

#ifdef MMMAN_USE_DXSND
	SndReset();
#endif
}


void MMMan::DeleteBank( int bank )
{
	char *lpSnd;
	lpSnd = sndbank( bank );
	if ( lpSnd != NULL ) {
		free( lpSnd );
	}
	mem_snd[bank].mempt=NULL;
	if ( mem_snd[bank].fname != NULL ) {
		free( mem_snd[bank].fname );
		mem_snd[bank].fname = NULL;
	}
}


int MMMan::SearchBank( int num )
{
	int a;
	for(a=0;a<mm_cur;a++) {
		if ( mem_snd[a].num == num ) return a;
	}
	return -1;
}


MMM *MMMan::SetBank( int num, int flag, int opt, void *mempt, char *fname )
{
	int bank;
	MMM *m;

	bank = SearchBank( num );
	if ( bank < 0 ) {
		bank = AllocBank();
	} else {
		DeleteBank( bank );
	}

	m = &(mem_snd[bank]);
	m->flag = flag;
	m->opt = opt;
	m->num = num;
	m->vol = 0;
	m->pan = 0;
	m->mempt = mempt;
	m->fname = NULL;

	if ( fname != NULL ) {
		m->fname = (char *)malloc( strlen( fname )+1 );
		strcpy( m->fname, fname );
	}
	return m;
}


int MMMan::AllocBank( void )
{
	int id,sz;
	id = mm_cur++;
	sz = mm_cur * sizeof(MMM);
	if ( mem_snd == NULL ) {
		mem_snd = (MMM *)sbAlloc( sz );
	} else {
		mem_snd = (MMM *)sbExpand( (char *)mem_snd, sz );
	}
	mem_snd[id].flag = MMDATA_NONE;
	mem_snd[id].num = -1;
	return id;
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


int MMMan::SendMCI( char *mci_commands )
{
	int a;
	a=mciSendString( mci_commands,res,256,(HWND)hwm );
	if (a) return -1;
	return atoi(res);
}


char *MMMan::GetMCIResult( void )
{
	return res;
}


void MMMan::SetWindow( void *hwnd, int x, int y, int sx, int sy )
{
	avi_wnd = hwnd;
	avi_x = x; avi_y = y;
	avi_sx = sx; avi_sy = sy;
}


void MMMan::Stop( void )
{
	//		stop playing sound
	//
#ifndef MMMAN_USE_DXSND
	sndPlaySound(NULL, 0);				// stop PCM sound
#endif
	if (curmus!=-1) {
		SendMCI("stop myid");
		SendMCI("close myid");
		curmus=-1;
	}
}


/*
	rev 43
	mingw : warning : a は代入前に使われる
	に対処。
	実際にはおこりえない。
*/
int MMMan::Load( char *fname, int num, int opt )
{
	//		Load sound to bank
	//			opt : 0=normal/1=loop/2=wait/3=continuous
	//
	char fext[8];
	char *pt;
	int flag;
	int track;
	int lasttrack;
	MMM *mmm;

	flag = MMDATA_MPEGVIDEO;
	pt = NULL;
	track = -1;
	lasttrack = -1;


#if 0
	char a1,a2,a3;
	a1=tolower(fname[0]);
	a2=tolower(fname[1]);
	a3=fname[2];
	if ((a1=='c')&&(a2=='d')&&(a3==':')) {	// when "CD"
		int a;
		flag = MMDATA_CDAUDIO;
		a = atoi( fname+3 );if ( a<1 ) a=1;
		track = a;
		lasttrk = SendMCI( "status cdaudio number of tracks" );
	}
#endif

	getpath(fname,fext,16+2);				// 拡張子を小文字で取り出す

#if 0
	if (!strcmp(fext,".avi")) {				// when "AVI"
		flag = MMDATA_MCIVIDEO;
	}

	if (!strcmp(fext,".wmv")) {				// when "WMV"
		flag = MMDATA_MCIVIDEO;
	}

	if (!strcmp(fext,".mpg")) {				// when "MPG"
		flag = MMDATA_MPEGVIDEO;
	}
#endif

	if (!strcmp(fext,".wav")) {				// when "WAV"
#ifdef MMMAN_USE_DXSND
		char *mp;
		mp = dpm_readalloc( fname );		// HSPリソースを含めて検索する
		if ( mp == NULL ) { return 1; }
		track = SndRegistWav( -1, mp, 0);
		if ( opt == 1 ) { SndSetLoop( track, 0 ); }
		mem_bye( mp );
		flag = MMDATA_INTWAVE;
#else
		getlen = dpm_exist( fname );
		if ( getlen==-1 ) return 1;
		if ( getlen < 2000000 ) {			// 2MB以上はMCIから再生
			pt = (char *)malloc( getlen+16 );
			dpm_read( fname, pt, getlen, 0 );
			flag = MMDATA_INTWAVE;
		}
#endif
	}

	mmm = SetBank( num, flag, opt, pt, fname );
	mmm->track = (short)track;
	mmm->lasttrk = (short)lasttrack;
	return 0;
}


int MMMan::Play( int num )
{
	//		Play sound
	//
	int a,flg;
	int bank;
	char ss[1024];
	char fpath[MAX_PATH];
	MMM *mmm;

	bank = SearchBank( num );
	if ( bank < 0 ) return 1;

	mmm=&mem_snd[bank];
	a=mmm->opt;
	flg=mmm->flag;
	if ((flg>1)&&(curmus!=-1)) {
		Stop();
	}
	switch(flg) {

		case MMDATA_INTWAVE:							// when "WAV"

#ifdef MMMAN_USE_DXSND
			SndPlay( mmm->track );
#else	
			{
			int prm;
			prm=SND_MEMORY | SND_NODEFAULT;
			if (a==0) prm|=SND_ASYNC;
			if (a==1) prm|=SND_LOOP | SND_ASYNC;
			if (a==2) prm|=SND_SYNC;
			sndPlaySound( (LPCSTR)mmm->mempt,prm);
			}
#endif
			return 0;

		case MMDATA_MCIVOICE:							// when "MID" file
		case MMDATA_MCIVIDEO:							// when "AVI" file
		case MMDATA_MPEGVIDEO:							// when "MPG" file

			if ( GetShortPathName( mmm->fname, fpath, MAX_PATH ) == 0 ) {
				return 1;
			}
			if ( flg!=MMDATA_MPEGVIDEO ) {
				sprintf( ss,"open %s alias myid",fpath );
			} else {
				sprintf( ss,"open %s type MPEGVIDEO alias myid",fpath );
			}
			SendMCI( ss );
/*
			if (flg!=MMDATA_MCIVOICE) {
				if ( SendMCI( "where myid source" )==0 ) strcpy( avi_wh,res+4 );
				if ( a&16 ) {
					sprintf( avi_wh,"%d %d",avi_sx,avi_sy );
					avi_x=0;avi_y=0;
				}
				sprintf( ss,"window myid handle %d", (int)avi_wnd );
				SendMCI( ss );
				sprintf( ss,"put myid destination at %d %d %s",avi_x,avi_y,avi_wh );
				SendMCI( ss );
			}
*/
			strcpy( ss,"play myid from 0" );
			break;

/*
		case MMDATA_CDAUDIO:							// when "CD audio"
			{
			int i,j;
			SendMCI( "open cdaudio alias myid" );
			SendMCI( "set myid time format tmsf" );

			i=mmm->track;j=mmm->lasttrk;
			if ((i==j)||(a==3)) {
				sprintf( ss,"play myid from %d",i );
			} else {
				sprintf( ss,"play myid from %d to %d",i,i+1 );
			}
			break;
			}
*/
	}

	a&=15;
	if (a==1) strcat( ss," notify" );
	if (a==2) strcat( ss," wait" );
	SendMCI( ss );
	curmus = num;

	if ( mmm->vol != 0 ) { SetVol( num, mmm->vol ); }

	return 0;
}


void MMMan::Notify( void )
{
	//		callback from windows message
	//			"MM_MCINOTIFY"
	//
	int a;
	a=curmus;
	if (curmus!=-1) {
		SendMCI("stop myid");
		SendMCI("close myid");
		curmus=-1;
	}
	Play( a );
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

/*
int MMMan::GetBusy( void )
{
	//		wavが再生中かを調べる
	//
	if ( sndPlaySound( (LPCSTR)"",SND_NOSTOP|SND_NODEFAULT ) == FALSE ) {
		return 1;
	}
	return 0;
}
*/


void MMMan::SetVol( int num, int vol )
{
	MMM *mmm;
	int bank,flg;
	char ss[1024];
	bank = SearchBank( num );
	if ( bank < 0 ) return;

	mmm=&mem_snd[bank];
	mmm->vol = vol;
	if ( mmm->vol > 0 ) mmm->vol = 0;
	if ( mmm->vol < -1000 ) mmm->vol = -1000;

	flg=mmm->flag;
	switch(flg) {
	case MMDATA_INTWAVE:							// when "WAV"
#ifdef MMMAN_USE_DXSND
		{
		// DirectSoundのボリューム値に変換する
		double myvol;
		double maxvol;
		if ( mmm->vol == 0 ) {
			SndSetVolume( mmm->track, 0 );
			break;
		}
		maxvol = 1000.0;
		myvol = (double)(mmm->vol + 1000);
		if ( myvol <= 0.005 ) {
			myvol = -10000.0;
		} else {
			myvol = ( 20.0 * log10( myvol / maxvol )) * 100.0;
		}
		SndSetVolume( mmm->track, (int)myvol );
		}
#endif
		break;
	case MMDATA_MPEGVIDEO:							// when "MPG" file
		if ( curmus != -1 ) {
			int mcivol;
			mcivol = mmm->vol + 1000;
			sprintf( ss,"setaudio myid volume to %d",mcivol );
			SendMCI( ss );
		}
		break;
	}
}


void MMMan::SetPan( int num, int pan )
{
	MMM *mmm;
	int bank,flg;
	bank = SearchBank( num );
	if ( bank < 0 ) return;

	mmm=&mem_snd[bank];
	flg=mmm->flag;
	switch(flg) {
	case MMDATA_INTWAVE:							// when "WAV"
#ifdef MMMAN_USE_DXSND
		{
		// DirectSoundの値に変換する
		int fixpan;
		fixpan = pan;
		mmm->pan = fixpan;
		if ( fixpan > 1000 ) fixpan = 1000;
		if ( fixpan < -1000 ) fixpan = -1000;
		SndSetPan( mmm->track, fixpan * 10 );
		}
#endif
		break;
	}
}


int MMMan::GetStatus( int num, int infoid )
{
	MMM *mmm;
	int bank,flg;
	int resv;
	bank = SearchBank( num );
	if ( bank < 0 ) return 0;

	mmm=&mem_snd[bank];
	flg=mmm->flag;
	resv = 0;
	switch( infoid ) {
	case 0:
		resv = mmm->opt;
		break;
	case 1:
		resv = mmm->vol;
		break;
	case 2:
		resv = mmm->pan;
		break;
	case 16:
#ifdef MMMAN_USE_DXSND
		if ( flg == MMDATA_INTWAVE ) {
			resv = SndGetStatus( mmm->track, 16 );
			break;
		}
#endif
		SendMCI("status myid mode");
		if ( strcmp(res,"playing") == 0 ) resv = 1;

		break;
	}
	return resv;
}


void MMMan::StopBank( int num )
{
	//		stop playing sound
	//
	MMM *mmm;
	int bank,flg;

	if ( num < 0 ) {
#ifdef MMMAN_USE_DXSND
	SndStopAll();
#endif
		Stop();
		return;
	}

	bank = SearchBank( num );
	if ( bank < 0 ) return;
	mmm=&mem_snd[bank];
	flg=mmm->flag;
	switch(flg) {
	case MMDATA_INTWAVE:							// when "WAV"
#ifdef MMMAN_USE_DXSND
		SndStop( mmm->track );
#else
		sndPlaySound(NULL, 0);						// stop PCM sound
#endif
		break;

	case MMDATA_MCIVOICE:							// when "MID" file
	case MMDATA_MCIVIDEO:							// when "AVI" file
	case MMDATA_MPEGVIDEO:							// when "MPG" file
		if (curmus!=-1) {
			SendMCI("stop myid");
			SendMCI("close myid");
			curmus=-1;
		}
		break;
	}
}


