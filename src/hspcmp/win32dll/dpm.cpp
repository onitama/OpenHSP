
//
//	DPM : Datafile Package Manager
//			1996/6 for DOS onitama
//			1997/9 for Win32 onitama
//			2000/7 for HSP2.6 onitama
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EMSCRIPTEN
#ifndef _MAX_PATH
#define _MAX_PATH       256
#endif
#ifndef _MAX_DIR
#define _MAX_DIR        256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT        256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME      256
#endif
#else
#include <windows.h>
#endif
#include "../membuf.h"
#include "../supio.h"

/*----------------------------------------------------------*/

static CMemBuf *errbuf = NULL;
static void prtini( CMemBuf *msg )
{
	errbuf = msg;
}

static void prt( char *msg )
{
	if ( errbuf!=NULL ) errbuf->PutStr( msg );
}

/*----------------------------------------------------------*/

#define FILEMAX 4096

static	int a,b,c,i,j;
static	unsigned char a1,a2,a3;
static	char fname[256];
static	char aname[256];
static	char bname[256];
static	char sname[256];
static	char s1[256],s2[256],s3[256],tmp[256];
static	unsigned char buf[0x80];
static	char lname[0x80];
static	long fent[FILEMAX],fsiz[FILEMAX],fnam[FILEMAX],fenc[FILEMAX];
static	long optr,dent,fptr,fs,dnam,dpm_enc;
static	int seed1,seed2,deckey;
static	int defseed1, defseed2;

static	unsigned char *src;
static	unsigned int srcbuf;

#define	p_optr 0x04			// 実データ開始ポインタ
#define p_dent 0x08			// ディレクトリエントリ数
#define p_dnam 0x0c			// ネームスペース確保サイズ(ver2.6)

#define d_farc 0x08			// 圧縮flag(先頭が'\0'の時のみ有効)
#define d_fasz 0x0c			// 圧縮後size(先頭が'\0'の時のみ有効)
#define d_fnpt 0x10			// 名前格納ポインタ(先頭が'\0'の時のみ有効)
#define d_fenc 0x14			// 暗号化flag(先頭が'\0'の時のみ有効)
#define d_fent 0x18			// ファイル格納ポインタ
#define d_fsiz 0x1c			// ファイルsize

/*----------------------------------------------------------*/

static void putdw( int ofs, unsigned long val )
{
	//	put DWORD to buffer

	buf[ofs]=(char)val;
	buf[ofs+1]=(char)(val>>8);
	buf[ofs+2]=(char)(val>>16);
	buf[ofs+3]=(char)(val>>24);
}

static unsigned long getdw( int ofs )
{
	//	get DWORD from buffer

	unsigned long c1,c2,c3;
	c1=(unsigned long)(buf[ofs+1]);
	c2=(unsigned long)(buf[ofs+2]);
	c3=(unsigned long)(buf[ofs+3]);
	return (unsigned long)buf[ofs]+(c1<<8)+(c2<<16)+(c3<<24);
}

/*----------------------------------------------------------*/

static void puthdr( void )
{
	strcpy( (char *)buf,"DPMX" );
	putdw( p_optr,optr );
	putdw( p_dent,dent );
	putdw( p_dnam,dnam );
}

static void gethdr( void )
{
	optr=getdw( p_optr );
	dent=getdw( p_dent );
	dnam=getdw( p_dnam );
}

static void getdir( FILE *fp )
{
	//	get pack file directory

	//int a1;
	fread( buf,32,1,fp );
	//a1=14;while(1) {
	//	if (buf[a1]!=0) break;
	//	buf[a1--]=32;
	//}
	fptr=getdw( d_fent );
	fs=getdw( d_fsiz );
	dpm_enc=getdw( d_fenc );
}

static void bufinit( void )
{
	int a;
	for(a=0;a<0x40;a++) { buf[a]=0; }
}

static long chkfile( char *filename )
{
	//	check file exist and size

	FILE *ff;
	long filesize;
	ff=fopen( filename,"rb" );
	if (ff==NULL) return -1;
	filesize=0;
	while(1) {
		if (fgetc(ff)<0) break;
		filesize++;
	}
	fclose(ff);
	return filesize;
}

static char *gettvfolder( char *name )
{
	//	get HSPTV resource folder path

#ifdef HSPWIN
	static char p[_MAX_PATH];
	char ifname[_MAX_PATH];
	GetModuleFileName( NULL,ifname,_MAX_PATH );
	getpath( ifname, p, 32 );
	CutLastChr( p, '\\' );
	strcat( p, "\\hsptv\\" );
	strcat( p, name );
	return p;
#endif
	return NULL;
}


static void cpyfile( FILE *ff, char *filename, int encode )
{
	//	copy file to pack
	//
	int a;
	FILE *ff2;

	ff2 = fopen( filename, "rb" );
	if (ff2==NULL) {
		char *name2;
		name2 = gettvfolder( filename );
		if ( name2 == NULL ) return;
		ff2 = fopen( name2, "rb" );
		if ( ff2 == NULL ) return;
	}

	while(1) {
		a=fgetc(ff2);if (a<0) break;
		fputc(a,ff);
	}

	fclose(ff2);
}


static int getfile( void )
{
	//	take file from pack

	FILE *fp,*fp2;
	long la;
	int a1,a2;

	fp=fopen(fname,"rb");
	if (fp==NULL) {
		sprintf(tmp,"#No pack file [%s].\r\n",fname);
		prt(tmp);
		return -1;
	}
	sprintf(tmp,"Searching pack file [%s].\r\n",fname);
	prt(tmp);
	fread( buf,16,1,fp );gethdr();

	a2=-1;cutlast(aname);
	for(a1=0;a1<dent;a1++) {
		getdir(fp);strcpy(s1,(char *)buf);cutlast(s1);
		if (strcmp(s1,aname)==0) { a2=a1;break; }
	}
	fclose(fp);

	if (a2==-1) {
		sprintf( tmp,"#No such file.[%s]\r\n",aname );
		prt(tmp);
		return -1;
	}

	fp=fopen(fname,"rb");
	fseek(fp,fptr+optr,0);
		fp2=fopen( aname,"wb" );
		for(la=0;la<fs;la++) {
			a2=fgetc(fp);if (a2<0) break;
			fputc(a2,fp2);
		}
		fclose(fp2);
	fclose(fp);

	sprintf( tmp,"Take [%s](%ld) from pack.\r\n",aname,fs );
	prt(tmp);
	return 0;
}


static int viewfile( void )
{
	//	view pack file
	//
	FILE *fp;
	int a1;
	int ee;

	fp=fopen(fname,"rb");
	if (fp==NULL) {
		sprintf( tmp,"#No pack file [%s].\r\n",fname );
		prt(tmp);
		return -1;
	}

	//sprintf(tmp,"Look up pack file [%s].\n",fname);
	//prt(tmp);

	fread( buf,16,1,fp );gethdr();

	//sprintf(tmp,"name=%s,dir=%d,ptr=%d,name=%d\r\n",fname,dent,optr,dnam);
	//prt(tmp);
	
	for(a1=0;a1<dent;a1++) {
		getdir(fp);
		ee=0;if ( dpm_enc ) ee=1;
		sprintf( tmp,"%s,%x,%d,%d\r\n",buf,fptr,fs,ee );
		prt(tmp);
		//sprintf( tmp,"%2d/%s (%6ld) ",a1,buf,fs );
		//prt(tmp);
		//sprintf( tmp,"[$%08lx]\n",fptr );
		//prt(tmp);
	}
	fclose(fp);

	//sprintf( tmp,"Total %d files ",dent );
	//prt(tmp);
	//sprintf( tmp,"( object size %ld ).\n",optr+fptr+fs );
	//prt(tmp);
	return 0;
}


static int newfile( int mode )
{
	//	make new dpm file
	//		mode(1=ForDPM/0=ForExecutable)
	//
	FILE *fp;
	FILE *fp2;
	long res;
	int a1,efl,encode;
	char *mem_nam;
	int idx_nam;
	int total;
	unsigned char *uc;

	deckey = 0;
	uc = (unsigned char *)&deckey;

	//	initalize buffer

	bufinit();
	optr=0x10;dent=0;

	//	open packfile list

	fp2=fopen(aname,"rb");
	if (fp2==NULL) {
		sprintf(tmp,"#Listing file [%s] not found.\r\n",aname);
		prt(tmp);
		return -1;
	}
	sprintf(tmp,"Listing file [%s] analysis.\r\n",aname);
	prt(tmp);

	//	make dir buffer

	mem_nam = (char *)malloc( FILEMAX*32 );
	idx_nam=0;

	//	check file list

	fptr=0;efl=0;total=0;
	while(1) {
		if (fgets(s1,255,fp2)==NULL) break;
		cutlast(s1);if ((s1[0]!=';')&&(s1[0]!=0)) {
			encode = 0;
			if ( s1[0]=='+' ) {
				strcpy( s1,s1+1 );
			}

			res=chkfile(s1);
			if (res<0) {
				char *name2;
				name2 = gettvfolder( s1 );
				if ( name2 != NULL ) {
					res = chkfile( name2 );
					if ( res < 0 ) name2 = NULL;
				}
				if ( name2 == NULL ) {
					sprintf(tmp,"#No File [%s]\r\n",s1);
					prt(tmp);efl++;
					break;
				}
			}

			//		longname bufferに書き込み
			//{
			//int sz;
			//cutlast2(s1);
			//sz=strlen(s1)+1;
			//strcpy( mem_nam+idx_nam, s1 ); idx_nam += sz;
			//fnam[dent]=idx_nam;
			//}

			//		拡張フィールド
			fenc[dent]= encode;
			fnam[dent]= -1;

			fent[dent]=fptr;
			fsiz[dent]=res;
			total+=res;
			fptr+=res;dent++;
		}
	}
	fclose(fp2);
	if (efl) { free( mem_nam );return -1; }

	//		解読キーを生成する
	//
	seed1 = defseed1; seed2 = defseed2;		// data.dpm用のSEED

	//	write header

	fp=fopen(fname,"wb");
	if (fp==NULL) {
		free( mem_nam );
		prt("#File write error.\r\n");
		return -1;
	}
	optr += dent*0x20;

	idx_nam = ( idx_nam + 15 )>>4;		// name buffer add
	optr += idx_nam<<4;
	
	puthdr();
	fwrite( buf,16,1,fp );

	//	write directories

	a1=0;
	fp2=fopen(aname,"rb");
	while(1) {
		if (fgets(s1,255,fp2)==NULL) break;
		cutlast2(s1);
		if ( s1[0]=='+' ) strcpy( s1,s1+1 );
		if ((s1[0]!=';')&&(s1[0]!=0)) {
			bufinit();
			if ( fnam[a1]==-1 ) {
				strcpy( (char *)buf,s1 );
			}
			putdw( d_fnpt,fnam[a1] );
			putdw( d_fent,fent[a1] );
			putdw( d_fsiz,fsiz[a1] );
			putdw( d_fenc,fenc[a1] );
			fwrite( buf,32,1,fp );
			a1++;
		}
	}
	fclose(fp2);

	//	write name image

	if ( idx_nam>0 ) {
		fwrite( mem_nam,idx_nam,1,fp );
	}
	free( mem_nam );

	//	write file image

	a1=0;
	fp2=fopen(aname,"rb");
	while(1) {
		if (fgets(s1,255,fp2)==NULL) break;
		cutlast(s1);
		if ( s1[0]=='+' ) strcpy( s1,s1+1 );
		if ((s1[0]!=';')&&(s1[0]!=0)) {
			cpyfile( fp, s1, fenc[a1] );
			a1++;
		}
	}
	fclose(fp2);

	//	all ends

	fclose(fp);
	return 0;
}


static int makexe( int mode, char *hspexe, int opt1, int opt2, int opt3 )
{
	//	make custom EXE file
	//		mode : 0=normal
	//		       1=full screen
	//		       2=ssaver
	//		opt1,opt2 : x,y axis
	//		opt3      : disp_sw
	//
	//
	FILE *fp,*fp2,*fp3;
	long sidx,sidx2,x0,x1;
	int a1;
	unsigned char s4[64];
	char c;
	int *ip;
	int chksum, sum, sumseed, sumsize;

	char hrtfile[_MAX_PATH];
	char p_drive[_MAX_PATH];
	char p_dir[_MAX_DIR];
	char p_fname[_MAX_FNAME];
	char p_ext[_MAX_EXT];


	//		HSPヘッダーを検索
	//
	strcpy( hrtfile, hspexe );
	_splitpath( hrtfile, p_drive, p_dir, p_fname, p_ext );
	fp=fopen( hrtfile, "rb" );
	if (fp==NULL) {
		sprintf( hrtfile,"%s%sruntime\\%s%s", p_drive, p_dir, p_fname, p_ext ); 
		fp=fopen( hrtfile, "rb" );
		//
		if (fp==NULL) {
			sprintf( hrtfile,"%s%s", p_fname, p_ext ); 
			fp=fopen( hrtfile, "rb" );
			if (fp==NULL) {
				sprintf( tmp,"#No file [%s].\r\n",hspexe );
				prt(tmp);
				return -1;
			}
		}
	}
	strcpy( (char *)s4,"HSPHED~~" );
	b=0;x1=0;sidx=0;sidx2=0;
	while(1) {
		a1=fgetc(fp);if (a1<0) break;
		if ( a1!=s4[b] ) b=0; else b++;
		if (b==8) sidx=x1-7;
		x1++;
	}
	fclose(fp);

	if (sidx==0) {
		prt("#Not found hsp index.\r\n");
		return -1;
	}
	sprintf( tmp,"#Found hsp index in $%05lx/$%05lx.\r\n",sidx,x1 );
	prt(tmp);
	strcpy(sname,bname);

	//		作成される実行ファイル名
	//
	if (mode==2) strcat(sname,".scr");
			else strcat(sname,".exe");


	//		DPMのチェックサムを作成
	//
	fp=fopen(fname,"rb");
	if (fp==NULL) {
		sprintf(tmp,"#No file [%s].\r\n",fname);
		prt(tmp);
		return -1;
	}
	sum = 0; sumsize = 0;
	sumseed = ((deckey>>24)&0xff)/7;
	while(1) {
		a1=fgetc(fp);if (a1<0) break;
		sum+=a1+sumseed;sumsize++;
	}
	fclose(fp);
	chksum = sum & 0xffff;				// lower 16bit sum
	//AlertV( "DECKEY",deckey );
	//AlertV( "SUM",sum );
	//AlertV( "SUMSEED",sumseed );


	//		ヘッダ情報を書き込み
	//
	sidx2=sidx+36;
	for(a1=0;a1<32;a1++) { s4[a1]=0; }
	//strcpy(s4,bname);
	sprintf( (char *)s4+9,"%5ld",x1-0x10000);
	c=0;
	if (mode==1) c='f';
	if (mode==2) c='s';
	s4[17]=c;
	s4[19]='x';s4[20]=opt1&0xff;s4[21]=(opt1>>8)&0xff;
	s4[22]='y';s4[23]=opt2&0xff;s4[24]=(opt2>>8)&0xff;
	s4[25]='d';s4[26]=opt3&0xff;s4[27]=(opt3>>8)&0xff;
	s4[28]='s';s4[29]=chksum&0xff;s4[30]=(chksum>>8)&0xff;
	s4[31]='k'; ip = (int *)(s4+32); *ip = deckey;

	fp2=fopen(fname,"rb");
	fp=fopen( hrtfile, "rb");
	if (fp==NULL) {
		sprintf(tmp,"#No file [%s].\r\n",hspexe );
		prt(tmp);
		return -1;
	}
	fp3=fopen(sname,"wb");
	if (fp3==NULL) {
		sprintf(tmp,"#Write error [%s].\r\n",sname );
		prt(tmp);
		return -1;
	}

	x0=0;
	while(1) {
		if (x0<x1) {
			a1=fgetc(fp);
			if (x0>=sidx) if (x0<sidx2) a1=s4[x0-sidx];
		} else {
			a1=fgetc(fp2);
		}
		if (a1<0) break;
		fputc(a1,fp3);
		x0++;
	}

	fclose(fp3);fclose(fp2);fclose(fp);

	sprintf(tmp,"Make custom execute file [%s](%ld).\r\n",sname,x0);
	prt(tmp);
	return 0;
}


/*----------------------------------------------------------*/

void dpmc_ini( CMemBuf *mesbuf, char *infile )
{
	prtini(mesbuf);
	strcpy(fname,infile);
	strcpy(bname,fname);
	addext(fname,"dpm");
	strcpy(aname,"packfile");
	prt("Datafile Pack Manager ver.3.0 / onion software 1997-2012\r\n");
	defseed1 = 0xaa; defseed2 = 0x55;			// data.dpm用のデフォルトSEED
}

int dpmc_view( void )
{
	if ( viewfile() ) return 1;
	return 0;
}

int dpmc_pack( int mode )
{
	if ( newfile( mode ) ) return 1;
	return 0;
}

int dpmc_mkexe( int mode, char *hspexe, int opt1, int opt2, int opt3 )
{
	if ( makexe(mode,hspexe,opt1,opt2,opt3) ) return 1;
	return 0;
}

int dpmc_get( char *outfile )
{
	strcpy(aname,outfile);
	if ( getfile() ) return 1;
	return 0;
}

void dpmc_dpmkey( int key )
{
	int deckey;
	unsigned char *dec;

	deckey = key;
	dec = (unsigned char *)&deckey;
	defseed1 = ( dec[0] * dec[2] / 3 );
	defseed2 = ( dec[1] * dec[3] / 5 );
	defseed1 = defseed1 & 0xff;
	defseed2 = (defseed2 & 0xff)^0xaa;
}

