
//
//			CSV parse
//			onion software/onitama 2002/3
//
#include <stdio.h>
#include <mbstring.h>
#include "windows.h"
#include "ccsv.h"

/*
static void DebugMes( char *mes )
{
	MessageBox( NULL, mes, "error",MB_ICONINFORMATION | MB_OK );
}

*/

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

ccsv::ccsv( void )
{
	SetBuffer( NULL );
	SetResultBuffer( NULL, 0 );
	spchr = ',';
	SetOption( 0 );
}


ccsv::ccsv( char *ptr )
{
	SetBuffer( ptr );
	SetResultBuffer( NULL, 0 );
	spchr = ',';
	SetOption( 0 );
}


void ccsv::SetBuffer( char *ptr )
{
	int i;
	buf = ptr;
	for(i=0;i<FLAGMAX;i++) {
		flag[i] = CCSV_FLAG_ENABLE;
	}
}


void ccsv::SetResultBuffer( char *ptr, int size )
{
	resbuf = ptr;
	resp = ptr;
	if ( ptr != NULL ) *ptr=0;
	ressize = size;
	resmax = resp + ressize - 1;
}


void ccsv::SetSeparate( char chr )
{
	spchr = chr;
}


void ccsv::SetFlag( int id, int val )
{
	flag[id]=val;
}


void ccsv::AddResult( char *str )
{
	char *src;
	char a1;
	if ( resbuf == NULL ) return;
	src = str;
	while(1) {
		if ( resp >= resmax ) break;
		a1 = *src++;if ( a1==0 ) break;
		*resp++ = a1;
	}
	*resp = 0;
}


void ccsv::SetOption( int val )
{
	opt = val;
}


ccsv::~ccsv( void )
{
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void ccsv::StrCnvLC( char *src )
{
	unsigned char *p;
	unsigned char a1;
	int i;
	p = (unsigned char *)src;
	while(1) {
		a1 = *p;if ( a1==0 ) break;
		i = 0;
		if (a1>=129) {						// 全角文字チェック
			if (a1<=159) { *p++=a1;p++;i++;	}
			else if (a1>=224) { *p++=a1;p++;i++; }
		}
		if ( i==0 ) *p++ = tolower(a1);
	}
}


void ccsv::StrCnvZ2H( char *src, char *result )
{
	unsigned char a1;
	unsigned char *p;
	unsigned char *s;
	unsigned int az;
	s = (unsigned char *)src;
	p = (unsigned char *)result;
	while(1) {
		a1 = *s;
		if (a1==0) break;
		if (a1>=129) {						// 全角文字チェック
			if (a1<=159) {
				s++; az = (a1<<8)|(*s);a1 = _mbctombb( az );
			} else if (a1>=224) {
				s++; az = (a1<<8)|(*s);a1 = _mbctombb( az );
			}
		}
		*p++ = a1;s++;
	}
	*p=0;
}


void ccsv::CsvCopy( char *target, char *src )
{
	int i,flg;
	unsigned char *p;
	unsigned char *s;
	unsigned char a1;
	unsigned char spc;

	spc = (unsigned char)spchr;
	p=(unsigned char *)target;
	s=(unsigned char *)src;
	i = 0; flg = 1;
	if ( flag[i]==CCSV_FLAG_NONE ) flg=0;

	while(1) {
		a1=*s++;if (a1==0) break;
		if (a1==spc) {
			i++;flg = 1;					// csv flag check
			if ( flag[i]==CCSV_FLAG_NONE ) flg=0;
		}
		else {
			if (flg) *p++=a1;
			if (a1>=129) {					// 全角文字チェック
				if ((a1<=159)||(a1>=224)) {
					a1=*s++;if (a1==0) break;
					if (flg) *p++=a1;
				}
			}
		}
	}
	*p=0;
}


char *ccsv::StrStrZH( char *target, char *src )
{
	//		strstr関数の全角対応版
	//
	unsigned char *p;
	unsigned char *s;
	unsigned char *p2;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	p=(unsigned char *)target;
	if (( *src==0 )||( *target==0 )) return NULL;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		p2 = p;
		s=(unsigned char *)src;
		while(1) {
			a2=*s++;if (a2==0) return (char *)p2;
			a3=*p2++;if (a3==0) break;
			if (a2!=a3) break;
		}
		p++;							// 検索位置を移動
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
	}
	return NULL;
}


int ccsv::SearchSub( char *search )
{
	char *p;
	char *vs;
	char a1;
	char s1[512];
	int a,i,andcheck;

	strcpy( orgstr, chkstr );
	if ( flagsel ) CsvCopy( chkstr, orgstr );

	if ( opt & CCSV_OPT_ZENKAKU ) {
		StrCnvZ2H( search, search );
		StrCnvZ2H( orgstr, chkstr );
	}
	if ( opt & CCSV_OPT_NOCASE ) {
		StrCnvLC( search );
		StrCnvLC( chkstr );
	}

	andcheck=1; a=0; i=0;
	if ((opt & CCSV_OPT_ANDMATCH)==0) andcheck=0;
	vs = search;
	while(1) {
		a1=*vs++;
		if ( a1 == 0 ) break;
		if ( a1 == 13 ) {
			if ( *vs==10 ) vs++;
			i=1;
		}
		if ( a1 == 10 ) { i=1; }
		if ( i ) {
			if ( a>0 ) {
				s1[a]=0;
				p = StrStrZH( chkstr, s1 );
				if (( p != NULL )&&( (opt & CCSV_OPT_ANDMATCH)==0 )) andcheck=1;
				if (( p == NULL )&&( opt & CCSV_OPT_ANDMATCH )) andcheck=0;
			}
			i = 0; a = 0;
			continue;
		}
		s1[a++]=a1;
	}
	if ( a>0 ) {
		s1[a]=0;
		p = StrStrZH( chkstr, s1 );
		if (( p != NULL )&&( (opt & CCSV_OPT_ANDMATCH)==0 )) andcheck=1;
		if (( p == NULL )&&( opt & CCSV_OPT_ANDMATCH )) andcheck=0;
	}

	if ( andcheck==0 ) return 0;

	//		Match
	if ( cres < startid ) { cres++; return 1; }
	cres++;
	if ( maxres>0 ) {
		if ( rescnt >= maxres ) return 1;
	}
	rescnt++;

	if ( opt & CCSV_OPT_ADDLINE ) {
		sprintf( chkstr,"#%05d:",cline );
		AddResult( chkstr );
	}
	AddResult( orgstr );
	AddResult( "\r\n" );
	return 1;
}


int ccsv::Search( char *search, int max, int start )
{
	unsigned char *vs;
	unsigned char a1;
	int a;
	int res;
	if ( resbuf == NULL ) return 0;

	startid = start;
	maxres = max;
	cres = 0;
	rescnt = 0;
	cline = 0;
	res = 0;
	flagsel = 0;

	for(a=0;a<FLAGMAX;a++) {
		if ( flag[a]==CCSV_FLAG_NONE ) flagsel++;
	}

	vs = (unsigned char *)buf;
	a = 0;
	while(1) {
		a1=*vs;
		if ( a1 == 0 ) break;
		vs++;
		if ( a1 == 13 ) {
			if ( *vs == 10 ) vs++;
			if ( a>0 ) {
				chkstr[a++]=0;
				res += SearchSub( search );
			}
			cline++; a=0; continue;
		}
		if ( a1 == 10 ) {
			if ( a>0 ) {
				chkstr[a++]=0;
				res += SearchSub( search );
			}
			cline++; a=0; continue;
		}
		chkstr[a++]=a1;
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				chkstr[a++] = *vs++;
			}
		}
	}
	if ( a>0 ) {
		chkstr[a++]=0;
		res += SearchSub( search );
	}
	return res;
}

