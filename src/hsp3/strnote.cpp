
/*----------------------------------------------------------------*/
//		notepad object related routines
//		(CR/LFだけでなくLFにも対応した版)
/*----------------------------------------------------------------*/

#include <string.h>
#include "hsp3config.h"
#include "strnote.h"
#include "supio.h"

#if defined(HSPLINUX) || defined(HSPMAC) || defined(HSPIOS) || defined(HSPNDK) || defined(HSPEMSCRIPTEN)
// LFを改行として扱う
#define MATCH_LF
#define CRSTR "\n"
#else
// CR/LFを改行として扱う
#define CRSTR "\r\n"
#endif

//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CStrNote::CStrNote()
{
	base = NULL;
	nulltmp[0] = 0;
}


CStrNote::~CStrNote()
{
}


void CStrNote::Select( char *str )
{
	base = str;
}


int CStrNote::GetSize( void )
{
	return (int)strlen( base );
}


//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

int CStrNote::nnget( char *nbase, int line )
{
	//	指定した行の先頭ポインタを求める
	//		nn = 先頭ポインタ
	//		lastcr : CR/LFで終了している
	//		line   : line number(-1=最終行)
	//		result:0=ok/1=no line
	//
	int a,i;
	char a1;
	a=0;
	lastcr=0;
	nn=nbase;
	if (line<0) {
		i=(int)strlen(nbase);if (i==0) return 0;
		nn+=i;a1=*(nn-1);
		if ((a1==10)||(a1==13)) lastcr++;
		return 0;
	}
	if (line) {
		while(1) {
			a1=*nn;if (a1==0) return 1;
			nn++;
#ifdef MATCH_LF
			if (a1==10) {
				a++;if (a==line) break;
			}
#endif
			if (a1==13) {
				if (*nn==10) nn++;
				a++;if (a==line) break;
			}
		}
	}
	lastcr++;
	return 0;
}


int CStrNote::GetLine( char *nres, int line )
{
	//		Get specified line from note
	//				result:0=ok/1=no line
	//
	char a1;
	char *pp;
	pp=nres;
	if ( nnget( base,line ) ) return 1;
	if (*nn==0) return 1;
	while(1) {
		a1=*nn++;
		if ((a1==0)||(a1==13)) break;
#ifdef MATCH_LF
		if (a1==10) break;
#endif
		*pp++=a1;
	}
	*pp=0;
	return 0;
}


int CStrNote::GetLine( char *nres, int line, int max )
{
	//		Get specified line from note
	//				result:0=ok/1=no line
	//
	char a1;
	char *pp;
	int cnt;
	pp=nres;
	cnt = 0;
	if ( nnget( base,line ) ) return 1;
	if (*nn==0) return 1;
	while(1) {
		if ( cnt>=max ) break;
		a1=*nn++;
		if ((a1==0)||(a1==13)) break;
#ifdef MATCH_LF
		if (a1==10) break;
#endif
		*pp++=a1;
		cnt++;
	}
	*pp=0;
	return 0;
}


char *CStrNote::GetLineDirect( int line )
{
	//		Get specified line from note
	//
	char a1;
	if ( nnget( base,line ) ) nn = nulltmp;
	lastnn = nn;
	while(1) {
		a1=*lastnn;
		if ((a1==0)||(a1==13)) break;
#ifdef MATCH_LF
		if (a1==10) break;
#endif
		lastnn++;
	}
	lastcode = a1;
	*lastnn = 0;
	return nn;
}


void CStrNote::ResumeLineDirect( void )
{
	//		Resume last GetLineDirect function
	//
	*lastnn = lastcode;
}


int CStrNote::GetMaxLine( void )
{
	//		Get total lines
	//
	int a,b;
	char a1;
	a=1;b=0;
	nn=base;
	while(1) {
		a1=*nn++;if (a1==0) break;
#ifdef MATCH_LF
		if ((a1==13)||(a1==10)) {
			if (a1=13&&*nn==10) nn++;
#else
		if (a1==13) {
			if (*nn==10) nn++;
#endif
			a++;b=0;
		}
		else b++;
	}
	if (b==0) a--;
	return a;
}


int CStrNote::PutLine( char *nstr2, int line, int ovr )
{
	//		Pet specified line to note
	//				result:0=ok/1=no line
	//
	int a = 0,ln,la,lw;
	char a1;
	char *pp;
	char *p1;
	char *p2;
	char *nstr;
	if ( nnget( base,line ) ) return 1;
	if (lastcr==0) {
		if ( nn != base ) {
			strcat( base, CRSTR );nn+=2;
		}
	}
	nstr = nstr2;
	if ( nstr == NULL ) { nstr=""; }

	pp=nstr;
	if ( nstr2 != NULL ) strcat(nstr, CRSTR );
	ln=(int)strlen(nstr);			// base new str + cr/lf
	la=(int)strlen(base);
	lw=la-(int)(nn-base)+1;
	//
	if (ovr) {						// when overwrite mode
		p1=nn;a=0;
		while(1) {
			a1=*p1++;if (a1==0) break;
			a++;
#ifdef MATCH_LF
			if ((a1==13)||(a1==10)) {
			if (a1=13&&*p1==10) { p1++;a++; }
#else
			if (a1==13) {
				if (*p1==10) { p1++;a++; }
#endif
				break;
			}
		}
		ln=ln-a;
		lw=lw-a;if (lw<1) lw=1;
	}
	//
	if (ln>=0) {
		p1=base+la+ln; p2=base+la;
		for(a=0;a<lw;a++) { *p1--=*p2--; }
	}
	 else {
		p1=nn+a+ln;p2=nn+a;
		for(a=0;a<lw;a++) { *p1++=*p2++; }
	}
	//
	while(1) {
		a1=*pp++;if (a1==0) break;
		*nn++=a1;
	}
	return 0;
}


int CStrNote::FindLine( char *nstr, int mode )
{
	//		Search string from note
	//				nstr:search string
	//				mode:STRNOTE_FIND_*
	//				result:line number(-1:no match)
	//
	char a1;
	int curline, len, res;

	nn=base;
	curline = 0; len = 0;

	baseline = nn;			// 行の先頭ポインタ

	while(1) {
		a1=*nn;if (a1==0) break;
#ifdef MATCH_LF
		if (a1==10) {
			if ( len ) {
				lastcode = a1; *nn = 0;
				res = FindLineSub( nstr, mode );
				*nn = lastcode;
				if ( res ) return curline;
			}
			nn++;
			curline++;len=0;
			baseline = nn;
			continue;
		}
#endif
		if (a1==13) {
			if ( len ) {
				lastcode = a1; *nn = 0;
				res = FindLineSub( nstr, mode );
				*nn = lastcode;
				if ( res ) return curline;
			}
			nn++;
			curline++;len=0;
			if (*nn==10) nn++;		// LFをスキップ
			baseline = nn;
			continue;
		}
		nn++;
		len++;
	}

	//	最終行に文字列があればサーチ
	if ( len ) {
		if ( FindLineSub( nstr, mode ) ) return curline;
	}

	return -1;
}


int CStrNote::FindLineSub( char *nstr, int mode )
{
	//		全体サーチ用文字列比較
	//		mode : STRNOTE_FIND_MATCH = 完全一致
	//		       STRNOTE_FIND_FIRST = 前方一致
	//		       STRNOTE_FIND_INSTR = 部分一致
	//
	switch( mode ) {
	case STRNOTE_FIND_MATCH:	// 完全一致
		if ( strcmp( baseline, nstr ) == 0 ) return 1;
		break;
	case STRNOTE_FIND_FIRST:	// 前方一致
		{
		char *p = strstr2( baseline, nstr );
		if ( p != NULL ) {
			if ( p == baseline ) return 1;
		}
		break;
		}
	case STRNOTE_FIND_INSTR:	// 部分一致
		if ( strstr2( baseline, nstr ) != NULL ) return 1;
		break;
	default:
		break;
	}
	return 0;
}



