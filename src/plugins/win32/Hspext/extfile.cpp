
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998-2002
//

#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <winbase.h>
#include <winuser.h>
#include <shlobj.h>

#include "../hpi3sample/hsp3plugin.h"

/*------------------------------------------------------------*/
/*
		Extra file access routines
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI fxcopy ( char *p1, char *p2, int p3, int p4 )
{
	//
	//		FileCopy srcfile,"distdir" (type5)
	//
	char a1;
	a1=p2[strlen(p2)-1];
	if (a1!='\\') strcat(p2,"\\");
	strcat(p2,p1);

	if (p3==0) {
		if ( CopyFile(p1,p2,FALSE)==0 ) return -1;
	}
	else {
		if ( MoveFile(p1,p2)==0 ) return -1;
	}
	return 0;
}

EXPORT BOOL WINAPI fxren ( char *p1, char *p2, int p3, int p4 )
{
	//
	//		FileCopy srcfile,"distdir" (type5)
	//
	if ( rename(p1,p2) ) return -1;
	return 0;
}


//
//  ドライブ空き容量取得(FAT32対応)
//
static int GetDiskFree( char *path, DWORD *lword, DWORD *hword )
{
	{
        ULARGE_INTEGER avaliable,totalbytes,freebytes;
		int fl=0;
        HINSTANCE inst = LoadLibrary("KERNEL32.DLL");
        FARPROC proc = GetProcAddress(inst,"GetDiskFreeSpaceExA");
		if ( proc!=NULL ) {
	        fl = ((BOOL (WINAPI *)(LPCTSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER))proc)(path,&avaliable,&totalbytes,&freebytes);
		}
        FreeLibrary(inst);
        if (fl) {
			*hword = freebytes.HighPart;
            *lword = freebytes.LowPart;
			return -1;
        }
    }

	{
        DWORD sector_per_cluster,bytes_per_sector;
        DWORD number_of_freeclustors,total_number_of_crustors;
        if (GetDiskFreeSpace( path, &sector_per_cluster, &bytes_per_sector, &number_of_freeclustors,&total_number_of_crustors )) {
            DWORDLONG size = (DWORDLONG)number_of_freeclustors * (DWORDLONG)sector_per_cluster * (DWORDLONG)bytes_per_sector;
            *hword = (DWORD)((size >> 32) & 0xffffffff);
            *lword = (DWORD)(size & 0xffffffff);
			return (int)size;
		}
    }
    return 0;
}


static void GetDiskFreeStr( char *path, char *result )
{
	ULARGE_INTEGER size;
	GetDiskFree( path, &size.LowPart, &size.HighPart );
	sprintf( result, "%I64d", size );
}


EXPORT BOOL WINAPI fxinfo ( int *p1, int p2, int p3, int p4 )
{
	//
	//		Get Drive info  val, drive , type (type1)
	//

	//#define DRIVE_UNKNOWN     0
	//#define DRIVE_NO_ROOT_DIR 1
	//#define DRIVE_REMOVABLE   2
	//#define DRIVE_FIXED       3
	//#define DRIVE_REMOTE      4
	//#define DRIVE_CDROM       5
	//#define DRIVE_RAMDISK     6

	//LPDWORD lpVolumeSerialNumber, 
	//		address of volume serial number 
	//LPDWORD lpMaximumComponentLength, 
	//		address of system’s maximum filename length 
	//LPDWORD lpFileSystemFlags, 
	//		address of file system flags 
 
	char *pp;
	char pdrv[8];
	char vol[64];
	char fsys[64];

	int b;
	BOOL a;
	DWORD fs[5];

	strcpy( pdrv,"@:\\" );
	pp=pdrv;
	if (p2==0) pp=NULL; else pdrv[0]+=p2;

	if (p3&8) {
		*p1=(int)GetDriveType(pp);
		return 0;
	}
	if (p3&16) {
		a=GetVolumeInformation( pp,vol,64,&fs[2],&fs[3],&fs[4],fsys,64 );
		if (a==0) return -1;
		b=p3&15;
		if (b==0) strcpy( (char *)p1,vol );
		if (b==1) strcpy( (char *)p1,fsys );
		if (b>1) *p1=(int)fs[b];
		return 0;
	}

	if (p3&32) {
		GetDiskFreeStr( pp, (char *)p1 );
		return 0;
	}

	*p1 = GetDiskFree( pp,&fs[0],&fs[1] );
	//a=GetDiskFreeSpace( pp,&fs[1],&fs[2],&fs[3],&fs[4] );
	//if (a==0) return -1;
	//fs[0]=fs[1]*fs[2]*fs[3];
	//*p1=(int)fs[p3];
	return 0;
}


EXPORT BOOL WINAPI fxaset ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		SetAttribute "file",type  (type6)
	//
	//#define FILE_ATTRIBUTE_READONLY         0x00000001  
	//#define FILE_ATTRIBUTE_HIDDEN           0x00000002  
	//#define FILE_ATTRIBUTE_SYSTEM           0x00000004  
	//#define FILE_ATTRIBUTE_DIRECTORY        0x00000010  
	//#define FILE_ATTRIBUTE_ARCHIVE          0x00000020  
	//#define FILE_ATTRIBUTE_NORMAL           0x00000080  
	//#define FILE_ATTRIBUTE_TEMPORARY        0x00000100  
	//#define FILE_ATTRIBUTE_COMPRESSED       0x00000800  
	//#define FILE_ATTRIBUTE_OFFLINE          0x00001000  
	BOOL a;
	a=SetFileAttributes(p1,p2);
	if ( a==0 ) return -1;
	return 0;
}


EXPORT BOOL WINAPI fxaget ( int *p1, char *p2, int p3, int p4 )
{
	//
	//		GetAttribute val,"file" (type5)
	//
	int att;
	att=(int)GetFileAttributes(p2);
	if (att==-1) return -1;
	*p1=att;
	return 0;
}


EXPORT BOOL WINAPI fxtget ( int *p1, char *p2, int p3, int p4 )
{
	//
	//		GetFileTime  val,"file"  (type5)
	//
	int i,j,p;
	BOOL a;
	unsigned short *pp;
	HANDLE hf;
	FILETIME ft[3];
	FILETIME ft2[3];
	SYSTEMTIME syst;

    hf = CreateFile( p2,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hf == INVALID_HANDLE_VALUE) return -2;
	a=GetFileTime( hf,&ft[0],&ft[2],&ft[1] );
	CloseHandle(hf);
	if (a==0) return -1;

	pp=(unsigned short *)&syst;
	p=0;
	for(i=0;i<3;i++) {
		FileTimeToLocalFileTime(&ft[i],&ft2[i]);
		a=FileTimeToSystemTime(&ft2[i],&syst);
		if (a==0) return -1;
		for(j=0;j<8;j++) { p1[p++]=pp[j]; }
	}
	return 0;
}


EXPORT BOOL WINAPI fxtset ( int *p1, char *p2, int p3, int p4 )
{
	//
	//		SetFileTime  "file",type  (type5)
	//
	int i,j,p;
	BOOL a;
	unsigned short *pp;
	HANDLE hf;
	FILETIME ft[3];
	FILETIME ft2[3];
	SYSTEMTIME syst;

	pp=(unsigned short *)&syst;
	p=0;
	for(i=0;i<3;i++) {
		for(j=0;j<8;j++) { pp[j]=p1[p++]; }
		a=SystemTimeToFileTime( &syst,&ft2[i] );
		if (a==0) return -1;
		LocalFileTimeToFileTime( &ft2[i],&ft[i] );
	}

    hf = CreateFile( p2,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hf == INVALID_HANDLE_VALUE) return -2;
	a=SetFileTime( hf,&ft[0],&ft[2],&ft[1] );
	CloseHandle(hf);
	if (a==0) return -1;
	return 0;
}


EXPORT BOOL WINAPI fxlong ( char *p1, char *p2, int p3, int p4 )
{
	//
	//		Get long file name
	//			fxlong p1,"file"  (type5)
	//

	//int a;
	//a=GetLongPathName( p2,p1,MAX_PATH );
	//return (a==0);
	return 0;
}


EXPORT BOOL WINAPI fxshort ( char *p1, char *p2, int p3, int p4 )
{
	//
	//		Get short file name
	//			fxshort p1,"file"  (type5)
	//
	int a;
	a=GetShortPathName( p2,p1,MAX_PATH );
	return (a==0);
}


/*
#define CSIDL_DESKTOP                   0x0000
#define CSIDL_INTERNET                  0x0001
#define CSIDL_PROGRAMS                  0x0002
#define CSIDL_CONTROLS                  0x0003
#define CSIDL_PRINTERS                  0x0004
#define CSIDL_PERSONAL                  0x0005
#define CSIDL_FAVORITES                 0x0006
#define CSIDL_STARTUP                   0x0007
#define CSIDL_RECENT                    0x0008
#define CSIDL_SENDTO                    0x0009
#define CSIDL_BITBUCKET                 0x000a
#define CSIDL_STARTMENU                 0x000b
#define CSIDL_DESKTOPDIRECTORY          0x0010
#define CSIDL_DRIVES                    0x0011
#define CSIDL_NETWORK                   0x0012
#define CSIDL_NETHOOD                   0x0013
#define CSIDL_FONTS                     0x0014
#define CSIDL_TEMPLATES                 0x0015
#define CSIDL_COMMON_STARTMENU          0x0016
#define CSIDL_COMMON_PROGRAMS           0X0017
#define CSIDL_COMMON_STARTUP            0x0018
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019
#define CSIDL_APPDATA                   0x001a
#define CSIDL_PRINTHOOD                 0x001b
#define CSIDL_ALTSTARTUP                0x001d         // DBCS
#define CSIDL_COMMON_ALTSTARTUP         0x001e         // DBCS
#define CSIDL_COMMON_FAVORITES          0x001f
#define CSIDL_INTERNET_CACHE            0x0020
#define CSIDL_COOKIES                   0x0021
#define CSIDL_HISTORY                   0x0022
*/

//*********************************************************
// 関数SHGetSpecialFolderPath() が使用できない環境で特殊フォルダのパスを取得する。
// 引数は SHGetSpecialFolderPath() と同じ。
//*********************************************************
static BOOL GetSpecialFolderPath( HWND hWnd, int nFolder, char *Path )
{
	IMalloc    *pMalloc;
	ITEMIDLIST *pidl;
	
	if ( NOERROR == SHGetMalloc( &pMalloc ) )
	{
		if ( NOERROR == SHGetSpecialFolderLocation( hWnd, nFolder, &pidl ) )
		{
			if ( SHGetPathFromIDList( pidl, Path ) )
			{
				pMalloc->Free( pidl );
				pMalloc->Release();
				return TRUE;
			}
			pMalloc->Free( pidl );
		}
		pMalloc->Release();
	}
	return FALSE;
}//GetSpecialFolder


EXPORT BOOL WINAPI fxdir( char *p1, int p2, int p3, int p4 )
{
	//		get special folder path (type1)
	//			getdir a,type
	//
	int ftype;
	char st[MAX_PATH];

	ftype=p2;
	if (ftype<0) {
		if (ftype==-2) {
			GetTempPath( MAX_PATH,p1 );
		}
		else {
			GetSystemDirectory( p1,MAX_PATH );
		}
		return 0;
	}
	if ( GetSpecialFolderPath( NULL, ftype, st ) == TRUE ) {
		strcpy(p1,st);
		return 0;
	}
	return -1;
}


/*------------------------------------------------------------*/
/*
		additional dialogs
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI selfolder( char *p1, char *p2, int p3, char *p4 )
{
	//
	//		フォルダ選択ダイアログ
	//			(type$15)
	//
	BROWSEINFO BrowsingInfo;
	char DirPath[MAX_PATH];
	char FolderName[MAX_PATH];
	LPITEMIDLIST ItemID;
	memset( &BrowsingInfo, 0, sizeof(BROWSEINFO) );
	memset( DirPath, 0, MAX_PATH );
	memset( FolderName, 0, MAX_PATH );
	BrowsingInfo.hwndOwner      = NULL;
	BrowsingInfo.pszDisplayName = FolderName;

	if (*p2==0) {
		BrowsingInfo.lpszTitle      = "フォルダを選択してください";
	}
	else {
		BrowsingInfo.lpszTitle      = p2;
	}

	BrowsingInfo.ulFlags        = BIF_RETURNONLYFSDIRS;
	ItemID = SHBrowseForFolder( &BrowsingInfo );
	if (ItemID==NULL) return -1;

	SHGetPathFromIDList(ItemID, DirPath );
	LocalFree( ItemID );						//ITEMIDLISTの解放
	strcpy( p1,DirPath );
	strcpy( p4,FolderName );
	return 0;
}


/*------------------------------------------------------------*/
/*
		advanced directory detect
*/
/*------------------------------------------------------------*/

static char *dirlist_ptr;
static int dirlist_split;
static int dirlist_cur;
static int dirlist_size;
static int *dirlist_result;
static char dirlist_spchr[4];
static char *dirlist_header = NULL;
static char dirlist_header_buffer[MAX_PATH];
static int dirlist_info_mode = 0xffff;
static int dirlist_info_split = ',';
static char *dirlist_info_opt[4];
static int dirlist_filetotal;

#define DIRLIST_INFO_FULLNAME 1
#define DIRLIST_INFO_NAME 2
#define DIRLIST_INFO_SIZE 4
#define DIRLIST_INFO_DATE 8
#define DIRLIST_INFO_TIME 16
#define DIRLIST_INFO_OPT1 32
#define DIRLIST_INFO_OPT2 64
#define DIRLIST_INFO_PATH 128


static void dirlist2add( char *tmp )
{
	char *ptr_bak;
	strcpy( &dirlist_ptr[dirlist_cur], tmp );
	dirlist_cur += strlen( tmp );
	if (( dirlist_cur + 0x1000 ) >= dirlist_size ) {
		ptr_bak = dirlist_ptr;
		dirlist_size += 0x10000;
		dirlist_ptr = (char *)malloc( dirlist_size );
		memcpy( dirlist_ptr, ptr_bak, dirlist_cur+1 );
		free( ptr_bak );
	}
}


static void dirlist2adds( char *tmp )
{
	char *p;
	char a1;
	int fl;
	p = tmp;
	fl = 0;
	while(1) {
		a1 = *p++; if ( a1==0 ) break;
		if ( a1==0x22 ) { fl=2; break; }
		if ( a1==',' ) fl=1;
	}
	if ( fl ) {											// "～" add
		dirlist_ptr[dirlist_cur++]=0x22;
		if ( fl==2 ) dirlist_ptr[dirlist_cur++]=0x22;
	}
	dirlist2add( tmp );
	if ( fl ) {											// "～" add
		dirlist_ptr[dirlist_cur++]=0x22;
		if ( fl==2 ) dirlist_ptr[dirlist_cur++]=0x22;
	}
}

	
static void dirlist2ini( int split )
{
	dirlist_cur = 0;
	dirlist_size = 0x1000;
	dirlist_ptr = (char *)malloc( dirlist_size );
	dirlist_split = split;
	if ( split == 0 ) dirlist_split = '\\';
	dirlist_spchr[0] = dirlist_split;
	dirlist_spchr[1] = 0;
	*dirlist_ptr = 0;
	dirlist_filetotal = 0;
}

	
static int dirlist2sub( char *filemask, int flag, char *prefix ) 
{
	char tmp[MAX_PATH];
	char tmp2[MAX_PATH];
	char pretmp[MAX_PATH];
	char info_spchr[4];
	char info[256];
	HANDLE sh;
	WIN32_FIND_DATA fd;
	DWORD fmask;
	BOOL ff;
	int fnum,fflag,fadd,p3;

	fnum=0;
	fmask=0;
	p3=flag;
	if (p3&2) fmask|=FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

	if (p3&1) {
		//		再帰検索
		//fmask|=FILE_ATTRIBUTE_DIRECTORY;
		sh=FindFirstFile( "*.*", &fd );
		if (sh==INVALID_HANDLE_VALUE) return 0;
		while(1) {
			ff = 1;
			if ( fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) {
				if ( fmask == 0 ) ff = 0;
			}
			//ff=( fd.dwFileAttributes & fmask )>0;
			//if ((p3&4)==0) ff=!ff;
			if (ff) {
				fflag=1;
				strcpy( tmp,fd.cFileName );
				if ( tmp[0]==0 ) fflag=0;
				if ( tmp[0]=='.' ) {
					if ( tmp[1]==0 ) fflag=0;
					if (( tmp[1]=='.' )&&( tmp[2]==0 )) fflag=0;
				}
				if (( fflag )&&( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
					_chdir( tmp );
					strcpy( pretmp, prefix );
					strcat( pretmp, tmp );
					strcat( pretmp, dirlist_spchr );
					fnum += dirlist2sub( filemask, flag, pretmp );
					_chdir( ".." );
					fflag = 0;
				}
			}
			if ( !FindNextFile(sh,&fd) ) break;
		}
		FindClose(sh);
	}

	sh=FindFirstFile( filemask, &fd );
	if (sh==INVALID_HANDLE_VALUE) return 0;

	info_spchr[0] = dirlist_info_split;
	info_spchr[1] = 0;

	while(1) {
			ff = 1;
			if ( fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) {
				if ( fmask == 0 ) ff = 0;
			}
			if (ff) {
				fflag=1;
				strcpy( tmp,fd.cFileName );
				if ( tmp[0]==0 ) fflag=0;
				if ( tmp[0]=='.' ) {
					if ( tmp[1]==0 ) fflag=0;
					if (( tmp[1]=='.' )&&( tmp[2]==0 )) fflag=0;
				}
/*
				if (( fflag )&&( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
					_chdir( tmp );
					strcpy( pretmp, prefix );
					strcat( pretmp, tmp );
					strcat( pretmp, dirlist_spchr );
					fnum += dirlist2sub( filemask, flag, pretmp );
					_chdir( ".." );
					fflag = 0;
				}
*/
				if ( fflag ) {
					fnum++;
					fadd = 0;
					dirlist_filetotal++;
					if (  dirlist_info_mode & DIRLIST_INFO_FULLNAME ) {
						//		add full name
						strcpy( tmp2, prefix );
						strcat( tmp2, tmp );
						dirlist2adds( tmp2 );
						fadd++;
					}
					if (  dirlist_info_mode & DIRLIST_INFO_NAME ) {
						//		add name
						if ( fadd ) dirlist2add( info_spchr );
						dirlist2adds( tmp );
						fadd++;
					}
					if (  dirlist_info_mode & DIRLIST_INFO_SIZE ) {
						//		add size
						long lsize;
						if ( fadd ) dirlist2add( info_spchr );
						lsize = (long)fd.nFileSizeHigh;
						lsize <<= 32;
						lsize += fd.nFileSizeLow;
						sprintf( info,"%ld",lsize );
						dirlist2add( info );
						fadd++;
					}
					if (  dirlist_info_mode & DIRLIST_INFO_DATE ) {
						//		add date
						FILETIME *ft;
						FILETIME lft;
						SYSTEMTIME syst;
						if ( fadd ) dirlist2add( info_spchr );
						ft = &fd.ftLastWriteTime;
						FileTimeToLocalFileTime( ft, &lft );
						FileTimeToSystemTime( &lft, &syst );
						sprintf( info,"%02d/%02d/%02d", syst.wYear, syst.wMonth, syst.wDay );
						dirlist2add( info );
						fadd++;
					}
					if (  dirlist_info_mode & DIRLIST_INFO_TIME ) {
						//		add time
						FILETIME *ft;
						FILETIME lft;
						SYSTEMTIME syst;
						if ( fadd ) dirlist2add( info_spchr );
						ft = &fd.ftLastWriteTime;
						FileTimeToLocalFileTime( ft, &lft );
						FileTimeToSystemTime( &lft, &syst );
						sprintf( info,"%02d:%02d:%02d", syst.wHour, syst.wMinute, syst.wSecond );
						dirlist2add( info );
						fadd++;
					}
/*
					if (  dirlist_info_mode & DIRLIST_INFO_OPT1 ) {
						//		add opt1
						if ( fadd ) dirlist2add( info_spchr );
						dirlist2adds( dirlist_info_opt[ 0 ] );
						fadd++;
					}
					if (  dirlist_info_mode & DIRLIST_INFO_OPT2 ) {
						//		add opt2
						if ( fadd ) dirlist2add( info_spchr );
						dirlist2adds( dirlist_info_opt[ 1 ] );
						fadd++;
					}
*/
					if (  dirlist_info_mode & DIRLIST_INFO_PATH ) {
						//		add path
						if ( fadd ) dirlist2add( info_spchr );
						if ( prefix[0] != 0 ) dirlist2adds( prefix );
						fadd++;
					}

					//		add CR/LF
					//
					dirlist2add( "\r\n" );
				}
			}
			if ( !FindNextFile(sh,&fd) ) break;
	}
	FindClose(sh);

	return fnum;
}


EXPORT BOOL WINAPI dirlist2( int *p1, char *p2, int p3, int p4 )
{
	//
	//		フォルダ内容取得  size取得変数, "filemask", mode, split
	//			(type$5)
	//
	int ff,rsize;
	char *prefix;
	dirlist_header = NULL;
	prefix = dirlist_header_buffer;
	dirlist_result = p1;
	dirlist2ini( p4 );
	if ( dirlist_header == NULL ) {
		prefix[0] = 0;
	}
	ff = dirlist2sub( p2, p3, prefix );
	rsize = dirlist_cur + 1;
	if ( rsize<64 ) rsize=64;
	*p1 = rsize;
	return -dirlist_filetotal;
}


EXPORT BOOL WINAPI dirlist2h( int p1, int p2, int p3, int p4 )
{
	//
	//		ヘッダ文字列設定  mode, split
	//			(type$6)
	//
/*
	if ( p2[0] == 0 ) dirlist_header = NULL;
	else {
		dirlist_header = dirlist_header_buffer;
		strcpy( dirlist_header_buffer, p2 );
	}
*/
	dirlist_info_mode = p1;
	if ( dirlist_info_mode == 0 ) dirlist_info_mode = 0xffff;
	dirlist_info_split = p2;
	if ( dirlist_info_split == 0 ) dirlist_info_split = ',';
	return 0;
}

/*
EXPORT BOOL WINAPI dirlist2opt( char *p1, int p2, int p3, int p4 )
{
	//
	//		option文字列設定  val, opt#
	//			(type$1)
	//
	dirlist_info_opt[ p2 ] = p1;
	return 0;
}
*/

EXPORT BOOL WINAPI dirlist2r( char *p1, int p2, int p3, int p4 )
{
	//
	//		フォルダ内容取得  取得変数
	//			(type$1)
	//
	*p1=0;
	strcpy( p1, dirlist_ptr );
	free( dirlist_ptr );	
	return 0;
}

