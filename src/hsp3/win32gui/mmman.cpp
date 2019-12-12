
//
//		MMMAN : Multimedia manager source
//				for Windows95/NT 32bit enviroment
//					onion software/onitama 1997/8
//

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

/*
	rev 43
	mingw : error : 不明な関数 tolower
	に対処
*/
#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "../supio.h"
#include "../dpmread.h"
#include "../strbuf.h"
#include "mmman.h"

/*
	rev 43
	mingw : warning : #pragma comment を無視する
	に対処
*/
#if defined( _MSC_VER )
#pragma comment(lib,"winmm.lib")
#endif

#ifdef HSPUTF8
#pragma execution_character_set("utf-8")
#endif


#define sndbank(a) (char *)(mem_snd[a].mempt)

void MMMan::SendMCIT(TCHAR *ss)
{
	char *ss8;
	apichartohspchar(ss,&ss8);
	SendMCI( ss8 );
	freehc(&ss8);
}

MMMan::MMMan()
{
	//		initalize MM manager
	//
	mem_snd = NULL;
	mm_cur = 0;
}


MMMan::~MMMan()
{
	//		terminate MM manager
	//
	ClearAllBank();
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


void MMMan::Reset( HWND hwnd )
{
	ClearAllBank();
	hwm = hwnd;
	avi_wnd = hwnd;
	curmus=-1;
}


int MMMan::SendMCI( char *mci_commands )
{
	int a;
	HSPAPICHAR *hactmp1 = 0;
	HSPAPICHAR *hactmp2 = 0;
	a=mciSendString( chartoapichar(mci_commands,&hactmp1),chartoapichar(res,&hactmp2),256,hwm );
	freehac(&hactmp1);
	freehac(&hactmp2);
	if (a) return -1;
	return atoi(res);
}


char *MMMan::GetMCIResult( void )
{
	return res;
}


void MMMan::SetWindow( HWND hwnd, int x, int y, int sx, int sy )
{
	avi_wnd = hwnd;
	avi_x = x; avi_y = y;
	avi_sx = sx; avi_sy = sy;
}


void MMMan::Stop( void )
{
	//		stop playing sound
	//
	sndPlaySound(NULL, 0);				// stop PCM sound
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
	int a = 1,getlen;
//	char fext[8];
	char a1,a2,a3;
	char *pt;
	int flag;
	MMM *mmm;
	HSPAPICHAR *hactmp1 = 0;
	HSPAPICHAR wfext[9];

	flag = MMDATA_MCIVOICE;
	pt = NULL;

	a1=tolower(fname[0]);
	a2=tolower(fname[1]);
	a3=fname[2];
	if ((a1=='c')&&(a2=='d')&&(a3==':')) {	// when "CD"
		flag = MMDATA_CDAUDIO;
		a = atoi( fname+3 );if ( a<1 ) a=1;
	}

	getpathW(chartoapichar(fname,&hactmp1),wfext,16+2);				// 拡張子を小文字で取り出す

	if (!_tcscmp(wfext,TEXT(".avi"))) {				// when "AVI"
		flag = MMDATA_MCIVIDEO;
	}

	if (!_tcscmp(wfext,TEXT(".wmv"))) {				// when "WMV"
		flag = MMDATA_MCIVIDEO;
	}

	if (!_tcscmp(wfext,TEXT(".mpg"))) {				// when "MPG"
		flag = MMDATA_MPEGVIDEO;
	}

	if (!_tcscmp(wfext,TEXT(".wav"))) {				// when "WAV"
		getlen = dpm_exist( fname );
		if (getlen == -1) {
			freehac(&hactmp1);
			return 1;
		}
		if ( getlen < 2000000 ) {			// 2MB以上はMCIから再生
			pt = (char *)malloc( getlen+16 );
			dpm_read( fname, pt, getlen, 0 );
			flag = MMDATA_INTWAVE;
		}
	}
	freehac(&hactmp1);

	mmm = SetBank( num, flag, opt, pt, fname );

	if ( flag == MMDATA_CDAUDIO ) {
		mmm->track = a;
		mmm->lasttrk = SendMCI( "status cdaudio number of tracks" );
	}
	return 0;
}


int MMMan::Play( int num )
{
	//		Play sound
	//
	int a,i,j,flg;
	int prm;
	int bank;
	TCHAR ss[1024];
	TCHAR fpath[MAX_PATH];
	MMM *mmm;
	HSPAPICHAR *hactmp1 = 0;

	bank = SearchBank( num );
	if ( bank < 0 ) return 1;

	mmm=&mem_snd[bank];
	a=mmm->opt;
	flg=mmm->flag;
	if ((flg>1)&&(curmus!=-1)) Stop();
	switch(flg) {

		case MMDATA_INTWAVE:							// when "WAV"

			prm=SND_MEMORY | SND_NODEFAULT;
			if (a==0) prm|=SND_ASYNC;
			if (a==1) prm|=SND_LOOP | SND_ASYNC;
			if (a==2) prm|=SND_SYNC;
#ifdef HSPUTF8
			sndPlaySound( (LPCTSTR)mmm->mempt,prm );
#else
			sndPlaySound( (LPCSTR)mmm->mempt,prm);
#endif
			return 0;

		case MMDATA_MCIVOICE:							// when "MID" file
		case MMDATA_MCIVIDEO:							// when "AVI" file
		case MMDATA_MPEGVIDEO:							// when "MPG" file

			if ( GetShortPathName( chartoapichar(mmm->fname,&hactmp1), fpath, MAX_PATH ) == 0 ) {
				freehac(&hactmp1);
				return 1;
			}
			freehac(&hactmp1);
			if ( flg!=MMDATA_MPEGVIDEO ) {
				_stprintf( ss,TEXT("open %s alias myid"),fpath );
			} else {
				_stprintf( ss,TEXT("open %s type MPEGVIDEO alias myid"),fpath );
			}
			SendMCIT( ss );
			if (flg!=MMDATA_MCIVOICE) {
				if ( SendMCI( "where myid source" )==0 ) strcpy( avi_wh,res+4 );
				if ( a&16 ) {
					sprintf( avi_wh,"%d %d",avi_sx,avi_sy );
					avi_x=0;avi_y=0;
				}
				_stprintf( ss,TEXT("window myid handle %u"), (unsigned)HandleToUlong(avi_wnd) );
				SendMCIT( ss );
				_stprintf( ss,TEXT("put myid destination at %d %d %s"),avi_x,avi_y,chartoapichar(avi_wh,&hactmp1) );
				freehac(&hactmp1);
				SendMCIT( ss );
			}
			_tcscpy( ss,TEXT("play myid from 0") );
			break;

		case MMDATA_CDAUDIO:							// when "CD audio"

			SendMCI( "open cdaudio alias myid" );
			SendMCI( "set myid time format tmsf" );

			i=mmm->track;j=mmm->lasttrk;
			if ((i==j)||(a==3)) {
				_stprintf( ss,TEXT("play myid from %d"),i );
			} else {
				_stprintf( ss,TEXT("play myid from %d to %d"),i,i+1 );
			}
			break;
	}

	a&=15;
	if (a==1) _tcscat( ss,TEXT(" notify") );
	if (a==2) _tcscat( ss,TEXT(" wait") );
	SendMCIT(ss);
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
		//
		break;
	case MMDATA_MCIVOICE:							// when "MID" file
	case MMDATA_MCIVIDEO:							// when "AVI" file
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
		//
		break;
	}
}


int MMMan::GetStatus( int num, int infoid )
{
	MMM *mmm;
	int bank,flg;
	int res;
	bank = SearchBank( num );
	if ( bank < 0 ) return 0;

	mmm=&mem_snd[bank];
	flg=mmm->flag;
	res = 0;
	switch( infoid ) {
	case 0:
		res = mmm->opt;
		break;
	case 1:
		res = mmm->vol;
		break;
	case 2:
		res = mmm->pan;
		break;
	case 16:
		if (( flg == MMDATA_MCIVOICE )||( flg == MMDATA_MCIVIDEO )||( flg == MMDATA_MPEGVIDEO )) {
			if (curmus!=-1) res = 1;
		}
		break;
	}
	return res;
}


void MMMan::StopBank(int num)
{
	//		stop playing sound
	//
	MMM* mmm;
	int bank, flg;

	if (num < 0) {
		Stop();
		return;
	}

	bank = SearchBank(num);
	if (bank < 0) return;
	mmm = &mem_snd[bank];
	flg = mmm->flag;
	switch (flg) {
	case MMDATA_INTWAVE:							// when "WAV"
		sndPlaySound(NULL, 0);						// stop PCM sound
		break;

	case MMDATA_MCIVOICE:							// when "MID" file
	case MMDATA_MCIVIDEO:							// when "AVI" file
	case MMDATA_MPEGVIDEO:							// when "MPG" file
		if (curmus != -1) {
			SendMCI("stop myid");
			SendMCI("close myid");
			curmus = -1;
		}
		break;
	}
}


