
//
//		MMMAN : Multimedia manager source
//				for iOS enviroment
//				onion software/onitama 2011/9
//
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>

#include <stdio.h>
#include "../../hsp3/hsp3config.h"
#include "../../hsp3/dpmread.h"
#include "../../hsp3/strbuf.h"
#include "../supio.h"

#include "mmman.h"

#define sndbank(a) (char *)(mem_snd[a].mempt)

//---------------------------------------------------------------------------

ALCdevice* g_alDevice = NULL;
ALCcontext* g_alContext = NULL;

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
    int     vol;
    int     pan;
	ALuint  buffer;
	ALuint  source;
} MMM;

static void SoundInit(void)
{
	g_alDevice = alcOpenDevice(NULL);
	g_alContext = alcCreateContext(g_alDevice, NULL);
	alcMakeContextCurrent(g_alContext);

	OSStatus result = AudioSessionInitialize(NULL, NULL, NULL, NULL);
	UInt32 category = kAudioSessionCategory_SoloAmbientSound;
	result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
}

static void SoundTerm(void)
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(g_alContext);
	g_alContext = NULL;
	alcCloseDevice(g_alDevice);
	g_alDevice = NULL;
}

static void SoundDelete( MMM *mmm )
{
	if ( mmm->flag != MMDATA_MCIVOICE ) return;
	alSourceStop(mmm->source);
	alSourcei( mmm->source, AL_BUFFER, AL_NONE);
	alDeleteSources(1, &mmm->source);
	alDeleteBuffers(1, &mmm->buffer);
}

static void SoundPlay( MMM *mmm )
{
	if ( mmm->flag != MMDATA_MCIVOICE ) return;
	if ( mmm->opt == 1 ) {
		alSourcei( mmm->source, AL_LOOPING, AL_TRUE );
	} else {
		alSourcei( mmm->source, AL_LOOPING, AL_FALSE );
	}
	alSourcePlay( mmm->source );
    //Alertf("Play.");
}

static void SoundPause( MMM *mmm )
{
	if ( mmm->flag != MMDATA_MCIVOICE ) return;
	alSourcePause( mmm->source );
}

static void SoundStop( MMM *mmm )
{
	if ( mmm->flag != MMDATA_MCIVOICE ) return;
	alSourceStop( mmm->source );
}

static void SoundParam( MMM *mmm, float pan, float gain, float pitch )
{
	if ( mmm->flag != MMDATA_MCIVOICE ) return;
	alSource3f( mmm->source, AL_POSITION, pan, 0.0f, 0.0f );
	alSourcef( mmm->source, AL_GAIN, gain );
	alSourcef( mmm->source, AL_PITCH, pitch );
}

static int SoundState( MMM *mmm )
{
	ALint state;
	if ( mmm->flag != MMDATA_MCIVOICE ) return 0;
	alGetSourcei( mmm->source, AL_SOURCE_STATE, &state );
	if(state == AL_PLAYING) {
		return 1;
	}
	return 0;
}

static void* GetOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize, ALenum* dataFormat, ALsizei *sampleRate)
{
	OSStatus    err;
	UInt32      size;
	
	// オーディオファイルを開く
	ExtAudioFileRef audioFile;
	err = ExtAudioFileOpenURL(fileURL, &audioFile);
	if (err) {
		if (audioFile) {
			ExtAudioFileDispose(audioFile);
		}
		return NULL;
	}
	
	// オーディオデータフォーマットを取得する
	AudioStreamBasicDescription fileFormat;
	size = sizeof(fileFormat);
	err = ExtAudioFileGetProperty(
								  audioFile, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat);
	if (err) {
		if (audioFile) {
			ExtAudioFileDispose(audioFile);
		}
		return NULL;
	}
	
	// アウトプットフォーマットを設定する
	AudioStreamBasicDescription outputFormat;
	outputFormat.mSampleRate = fileFormat.mSampleRate;
	outputFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
	outputFormat.mFormatID = kAudioFormatLinearPCM;
	outputFormat.mBytesPerPacket = 2 * outputFormat.mChannelsPerFrame;
	outputFormat.mFramesPerPacket = 1;
	outputFormat.mBytesPerFrame = 2 * outputFormat.mChannelsPerFrame;
	outputFormat.mBitsPerChannel = 16;
	outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
	err = ExtAudioFileSetProperty(
								  audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(outputFormat), &outputFormat);
	if (err) {
		if (audioFile) {
			ExtAudioFileDispose(audioFile);
		}
		return NULL;
	}
	
	// フレーム数を取得する
	SInt64  fileLengthFrames = 0;
	size = sizeof(fileLengthFrames);
	err = ExtAudioFileGetProperty(
								  audioFile, kExtAudioFileProperty_FileLengthFrames, &size, &fileLengthFrames);
	if (err) {
		goto Exit;
	}
	
	// バッファを用意する
	UInt32          bufferSize;
	void*           data;
	AudioBufferList dataBuffer;
	bufferSize = fileLengthFrames * outputFormat.mBytesPerFrame;;
	data = malloc(bufferSize);
	dataBuffer.mNumberBuffers = 1;
	dataBuffer.mBuffers[0].mDataByteSize = bufferSize;
	dataBuffer.mBuffers[0].mNumberChannels = outputFormat.mChannelsPerFrame;
	dataBuffer.mBuffers[0].mData = data;
	
	// バッファにデータを読み込む
	err = ExtAudioFileRead(audioFile, (UInt32*)&fileLengthFrames, &dataBuffer);
	if (err) {
		free(data);
		data = NULL;
		goto Exit;
	}
	
	// 出力値を設定する
	*dataSize = (ALsizei)bufferSize;
	*dataFormat = (outputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	*sampleRate = (ALsizei)outputFormat.mSampleRate;
	
Exit:
	// オーディオファイルを破棄する
	if (audioFile) {
		ExtAudioFileDispose(audioFile);
	}
	
	return data;
}

static int SoundLoad( MMM *mmm, char *fname )
{
    char *fext;
/*
    char fname1[256];
    char fname2[64];
    getpath( fname, fname1, 17 );       // ファイル名
    getpath( fname, fname2, 18 );       // 拡張子
    fext = "";//fname2+1;
	Alertf("#%s[%s] load",fname1,fname2);
*/    
    fext = "";
	NSString* tmpnsstr1 = [[NSString alloc] initWithUTF8String:fname];
	NSString* tmpnsstr2 = [[NSString alloc] initWithUTF8String:fext];
	NSString* soundFilePath = [[NSBundle mainBundle] pathForResource:tmpnsstr1 ofType:tmpnsstr2];
	[tmpnsstr1 release];
	[tmpnsstr2 release];

	NSURL *soundFileUrl = [NSURL fileURLWithPath:soundFilePath];  

	ALsizei dataSize;
	ALenum  dataFormat;
	ALsizei sampleRate;
	void*   audioData = GetOpenALAudioData((CFURLRef)soundFileUrl, &dataSize, &dataFormat, &sampleRate);
	if ( audioData == NULL ) return -1;

	alGenBuffers(1, &mmm->buffer);
	alGenSources(1, &mmm->source);
	alBufferData( mmm->buffer, dataFormat, audioData, dataSize, sampleRate);
	alSourcei( mmm->source, AL_BUFFER, mmm->buffer);
	free(audioData);
	mmm->flag = MMDATA_MCIVOICE;
	Alertf("#%s rate:%d size:%d ok.",fname,sampleRate,dataSize);
	return 0;
}

//---------------------------------------------------------------------------

MMMan::MMMan()
{
	//		initalize MM manager
	//
	mem_snd = NULL;
	mm_cur = 0;
    SoundInit();
}


MMMan::~MMMan()
{
	//		terminate MM manager
	//
	ClearAllBank();
    SoundTerm();
}

void MMMan::DeleteBank( int bank )
{
	MMM *m;
	char *lpSnd;

	m = &(mem_snd[bank]);
	if ( m->flag == MMDATA_MCIVOICE ) {
		SoundDelete( m );
	}

	lpSnd = sndbank( bank );
	if ( lpSnd != NULL ) {
		free( lpSnd );
	}
	mem_snd[bank].mempt=NULL;
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
	m->mempt = mempt;
	m->fname = NULL;
    m->vol = 0;
    m->pan = 0;

	return m;
}


int MMMan::AllocBank( void )
{
	int ids,sz;
	ids = mm_cur++;
	sz = mm_cur * sizeof(MMM);
	if ( mem_snd == NULL ) {
		mem_snd = (MMM *)sbAlloc( sz );
	} else {
		mem_snd = (MMM *)sbExpand( (char *)mem_snd, sz );
	}
	mem_snd[ids].flag = MMDATA_NONE;
	mem_snd[ids].num = -1;
	return ids;
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
	//hwm = hwnd;
	//avi_wnd = hwnd;
	//curmus=-1;
}


void MMMan::SetWindow( void *hwnd, int x, int y, int sx, int sy )
{
}


void MMMan::Stop( void )
{
	//		stop playing sound
	//
	MMM *m;
	int a;
	m = &(mem_snd[0]);
	for(a=0;a<mm_cur;a++) {
		SoundStop( m );
		m++;
	}
}


int MMMan::Load( char *fname, int num, int opt )
{
	//		Load sound to bank
	//			opt : 0=normal/1=loop/2=wait/3=continuous
	//
	int flag;
	MMM *mmm;

	flag = MMDATA_MCIVOICE;
	mmm = SetBank( num, flag, opt, NULL, fname );

	if ( SoundLoad( mmm, fname ) ) {
		mmm->flag = MMDATA_NONE;
		return -1;
	}
	return 0;
}


int MMMan::Play( int num )
{
	//		Play sound
	//
    int bank;
	MMM *m;
    bank = SearchBank(num);
    if ( bank < 0 ) return 1;
	m = &(mem_snd[bank]);
	SoundPlay( m );
	return 0;
}


void MMMan::Notify( void )
{
	//		callback from windows message
	//			"MM_MCINOTIFY"
	//
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


void MMMan::StopBank( int num )
{
    int bank;
	MMM *m;
    if ( num < 0 ) {
        Stop();
        return;
    }
    bank = SearchBank(num);
    if ( bank < 0 ) return;
	m = &(mem_snd[bank]);
	SoundStop( m );
}


void MMMan::SetVol( int num, int vol )
{
    int bank;
	MMM *mmm;
    int fixvol;
    float gain;
    bank = SearchBank(num);
    if ( bank < 0 ) return;
	mmm = &(mem_snd[bank]);
	if ( mmm->flag != MMDATA_MCIVOICE ) return;

    fixvol = vol + 1000;
    if ( fixvol > 1000 ) fixvol = 1000;
    if ( fixvol < 0 ) fixvol = 0;
    mmm->vol = fixvol - 1000;
    gain = ((float)fixvol) / 1000.0f;
	alSourcef( mmm->source, AL_GAIN, gain );
    
	//alSource3f( mmm->source, AL_POSITION, pan, 0.0f, 0.0f );
	//alSourcef( mmm->source, AL_PITCH, pitch );
}


void MMMan::SetPan( int num, int pan )
{
    int bank;
	MMM *mmm;
    int fixpan;
    float posx;
    bank = SearchBank(num);
    if ( bank < 0 ) return;
	mmm = &(mem_snd[bank]);
	if ( mmm->flag != MMDATA_MCIVOICE ) return;

    fixpan = pan;
    if ( fixpan < -1000 ) fixpan = -1000;
    if ( fixpan > 1000 ) fixpan = 1000;
    mmm->pan = fixpan;
    posx = ((float)fixpan) / 1000.0f;
	
    alSource3f( mmm->source, AL_POSITION, posx, 0.0f, 0.0f );
}


int MMMan::GetStatus( int num, int infoid )
{
	MMM *mmm;
    int res;
    int bank;
    bank = SearchBank(num);
    if ( bank < 0 ) return 0;
	mmm = &(mem_snd[bank]);
	if ( mmm->flag != MMDATA_MCIVOICE ) return 0;

    res = 0;
    switch( infoid ){
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
            res = SoundState(mmm);
            break;
    }
    return res;
}



//---------------------------------------------------------------------------
//	dxsnd service
//---------------------------------------------------------------------------

//		Struct
//
typedef struct
{
short flag;		// enable flag
short volume;		// volume(0～-10000)
short pan;		// pan(-10000～10000)
short speed;		// speed
int loopptr;		// loop pointer
char *fname;		// file name (optional)

} SNDINF;

//		Data
//
static		int sndflg;
static		SNDINF *sndinf;


/*------------------------------------------------------------*/
/*
        Subroutines
 */
/*------------------------------------------------------------*/

static int GetEmptyEntry( void )
{
	int i;
	for(i=0;i<SNDINF_MAX;i++) {
		if ( sndinf[i].flag == SNDFLAG_NONE ) return i;
	}
	return -1;
}


static void ResetSndInf( SNDINF *snd )
{
	//	各種デフォルト
	snd->volume = 0;
	snd->pan = 0;
	snd->speed = 0;
	snd->loopptr = -1;
}

/*------------------------------------------------------------*/
/*
		Main process
*/
/*------------------------------------------------------------*/

int SndInit( void *hWnd )
{
	sndflg = -1;

	sndinf = (SNDINF *)malloc( sizeof(SNDINF) * SNDINF_MAX );
	for(int i=0;i<SNDINF_MAX;i++) {
		sndinf[i].flag = SNDFLAG_NONE;
	}

	sndflg = 0;

	return 0;
}


void SndTerm( void )
{
	if ( sndflg == 0 ) {
		//	終了処理
		SndStopAll();
		SndReset();
		free( sndinf );
	}
}


void SndReset( void )
{
	int i;
	if ( sndflg ) return;
	for(i=0;i<SNDINF_MAX;i++) {
		SndStop( i );
		SndDelete( i );
	}
}


void SndDelete( int id )
{
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;
	snd = &sndinf[id];
//	if ( snd->flag & SNDFLAG_READY_WAV ) {
//		snd->lpSoundBuffer->Release();
//	}
//	if ( snd->flag & SNDFLAG_READY_OGG ) {
//		free( snd->fname );
//	}
	snd->flag = SNDFLAG_NONE;
}


void SndSetVolume( int id, int vol )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;
	snd = &sndinf[id];
	snd->volume = vol;
	type = snd->flag & 15;
	switch( type ) {
//	case SNDFLAG_READY_WAV:
//		snd->lpSoundBuffer->SetVolume( snd->volume );
//		break;
	}
}


void SndSetPan( int id, int pan )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;
	snd = &sndinf[id];
	snd->pan = pan;
	type = snd->flag & 15;
	switch( type ) {
//	case SNDFLAG_READY_WAV:
//		snd->lpSoundBuffer->SetPan( snd->pan );
//		break;
	}
}


void SndSetLoop( int id, int loop )
{
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;
	snd = &sndinf[id];
	if ( loop < 0 ) {
		snd->flag &= ~SNDFLAG_LOOP;
		snd->loopptr = -1;
		return;
	}
	snd->flag |= SNDFLAG_LOOP;
	snd->loopptr = loop;
}


int SndRegistWav( int newid, char *mem, int option )
{
	int id;
	SNDINF *snd;
	id = newid;
	if ( sndflg ) return -1;
	if ( id < 0 ) {
		id = GetEmptyEntry();
	} else {
		SndDelete( id );
	}
	if (( id < 0 )||( id >= SNDINF_MAX )) return -1;
	snd = &sndinf[id];

/*
	snd->lpSoundBuffer = lpBuffer;
	snd->lpSoundBuffer->SetCurrentPosition( 0 );
	snd->lpSoundBuffer->SetVolume( 0 );
	snd->lpSoundBuffer->SetPan( 0 );
	snd->flag = SNDFLAG_READY_WAV;
*/
	snd->flag |= option;
	ResetSndInf( snd );

	//Alertf( "DataSize %d (%d,%d)", DataSize, size1, size2 );
	//Alertf( "%dBit %.1lfKHz", wf.Format.wBitsPerSample, wf.Format.nSamplesPerSec/1000.0 );

	return id;
}


void SndPlay( int id )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;

	snd = &sndinf[id];
	type = snd->flag & 15;
/*
	switch( type ) {
	case SNDFLAG_READY_WAV:
		SndStop( id );
		snd->lpSoundBuffer->SetVolume( snd->volume );
		snd->lpSoundBuffer->SetPan( snd->pan );
		if ( snd->flag & SNDFLAG_LOOP ) {
			snd->lpSoundBuffer->Play( 0, 0, DSBPLAY_LOOPING );
		} else {
			snd->lpSoundBuffer->Play( 0, 0, 0 );
		}
		break;
	}
*/
}


void SndPlayPos( int id, int pos )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;

	snd = &sndinf[id];
	type = snd->flag & 15;
/*
	switch( type ) {
	case SNDFLAG_READY_WAV:
		SndStop( id );
		snd->lpSoundBuffer->SetVolume( snd->volume );
		snd->lpSoundBuffer->SetPan( snd->pan );
		if ( snd->flag & SNDFLAG_LOOP ) {
			snd->lpSoundBuffer->Play( 0, 0, DSBPLAY_LOOPING );
		} else {
			snd->lpSoundBuffer->Play( 0, 0, 0 );
		}
		snd->lpSoundBuffer->SetCurrentPosition( pos );
		break;
	}
*/
}


void SndStopAll( void )
{
	for(int i=0;i<SNDINF_MAX;i++) {
		SndStop( i );
	}
}


void SndStop( int id )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return;
	if (( id < 0 )||( id >= SNDINF_MAX )) return;
	snd = &sndinf[id];
	type = snd->flag & 15;
/*
	switch( type ) {
	case SNDFLAG_READY_WAV:
		snd->lpSoundBuffer->Stop();
		snd->lpSoundBuffer->SetCurrentPosition( 0 );
		break;
	}
*/
}


int SndGetStatus( int id, int option )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return 0;
	if (( id < 0 )||( id >= SNDINF_MAX )) return 0;
	snd = &sndinf[id];
	if ( option < 16 ) {
		short *p;
		p = (short *)snd;
		return (int)p[ option ];
	}
/*
	type = snd->flag & 15;
	switch( type ) {
	case SNDFLAG_READY_WAV:
		{
		DWORD status = 0;
		snd->lpSoundBuffer->GetStatus( &status );
		if (status == DSBSTATUS_PLAYING) return 1;
		break;
		}
	}
*/
	return 0;
}


double SndGetTime( int id, int option )
{
	int type;
	SNDINF *snd;
	if ( sndflg ) return (0.0);
	if (( id < 0 )||( id >= SNDINF_MAX )) return (0.0);
	snd = &sndinf[id];
	type = snd->flag & 15;

	return (0.0);
}


