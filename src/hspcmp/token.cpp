
//
//		Token analysis class
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "../hsp3/hsp3config.h"
#include "supio.h"
#include "token.h"
#include "label.h"
#include "tagstack.h"
#include "membuf.h"
#include "../hsp3/strnote.h"
#include "ahtobj.h"

#define s3size 0x8000

#ifdef HSPWIN
#include <windows.h>
#include <shlobj.h>
#endif

static inline int issjisleadbyte(unsigned char c)
{
	return (c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC);
}

static inline int tstrcmp(const char* str1, const char* str2)
{
	if (strcmp(str1, str2) == 0) return -1;
	return 0;
}

//-------------------------------------------------------------
//		String Service
//-------------------------------------------------------------

void strcase2(char* str, char* str2)
{
	//	string case to lower and copy
	//
	unsigned char a1;
	unsigned char* ss;
	unsigned char* ss2;
	ss = (unsigned char*)str;
	ss2 = (unsigned char*)str2;
	while (1) {
		a1 = *ss;
		if (a1 == 0) break;
		if (a1 >= 0x80) {
			*ss++;
			*ss2++ = a1;
			a1 = *ss++;
			if (a1 == 0) break;
			*ss2++ = a1;
		}
		else {
			a1 = tolower(a1);
			*ss++ = a1;
			*ss2++ = a1;
		}
	}
	*ss2 = 0;
}

void strcpy2(char* dest, const char* src, size_t size)
{
	if (size == 0) {
		return;
	}
	char* d = dest;
	const char* s = src;
	size_t n = size;
	while (--n) {
		if ((*d++ = *s++) == '\0') {
			return;
		}
	}
	*d = '\0';
	return;
}


static int findext(char const* st)
{
	//	拡張子をさがす。
	//
	int r = -1, f = 0;
	for (int i = 0; st[i] != '\0'; ++i) {
		if (f) {
			f = 0;
		}
		else {
			if (st[i] == '.') {
				r = i;
			}
			else if (st[i] == '\\') {
				r = -1;
			}
			f = issjisleadbyte(st[i]);
		}
	}
	return r;
}


void addext(char* st, const char* exstr)
{
	//	add extension of filename
	int i = findext(st);
	if (i == -1) {
		strcat(st, ".");
		strcat(st, exstr);
	}
}


void cutext(char* st)
{
	//		拡張子を取り除く
	//
	int i = findext(st);
	if (i >= 0) st[i] = '\0';
}


//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void CToken::Mes( char *mes )
{
	//		メッセージ登録
	//
	errbuf->PutStr( mes );
	errbuf->PutStr( "\r\n" );
}


void CToken::Mesf( char *format, ...)
{
	//		メッセージ登録
	//		(フォーマット付き)
	//
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	errbuf->PutStr( textbf );
	errbuf->PutStr( "\r\n" );
}


void CToken::Error( char *mes )
{
	//		エラーメッセージ登録
	//
	char tmp[256];
	sprintf( tmp, "#Error:%s\r\n", mes );
	errbuf->PutStr( tmp );
}


void CToken::LineError( char *mes, int line, char *fname )
{
	//		エラーメッセージ登録(line/filename)
	//
	char tmp[256];
	sprintf( tmp, "#Error:%s in line %d [%s]\r\n", mes, line, fname );
	errbuf->PutStr( tmp );
}


void CToken::SetErrorBuf( CMemBuf *buf )
{
	//		エラーメッセージバッファ登録
	//
	errbuf = buf;
}


void CToken::SetPackfileOut( CMemBuf *pack )
{
	//		packfile出力バッファ登録
	//
	packbuf = pack;
	packbuf->PutStr( ";\r\n;\tsource generated packfile\r\n;\r\n" );
}


void CToken::SetError( char *mes )
{
	//		エラーメッセージ仮登録
	//
	strcpy( errtmp, mes );
}


int CToken::AddPackfile( char *name, int mode )
{
	//		packfile出力
	//			0=name/1=+name/2=other
	//
	CStrNote note;
	int i,max;
	char fname[HSP_MAX_PATH];
	char p_fdir[HSP_MAX_PATH];
	char p_fname[HSP_MAX_PATH];
	char packadd[1024];
	char tmp[1024];
	char *s;
	char* findptr;
	bool absolutePath = false;					// 絶対パスか?

	getpath(name, p_fdir, 32);
	getpath(name, p_fname, 8);

#ifdef HSPWIN
	strchr3(p_fdir, ':', 0, &findptr);			// ドライブ文字があった
	if (findptr != NULL) {
		absolutePath = true;
	}
	if (*p_fdir == '\\') {
		absolutePath = true;
	}
#endif
	if (*p_fdir == '/') {
		absolutePath = true;
	}

	if (absolutePath==false) {
		strcpy(fname, search_path);
		strcat(fname, p_fdir);
		strcpy(p_fdir, fname);
		strcat(fname, p_fname);
	}
	else {
		strcpy(fname, name);
	}

	strcpy( packadd, fname);

	if ( mode<2 ) {
#ifdef HSPWIN
		strcase(packadd);
#endif
		note.Select( packbuf->GetBuffer() );
		max = note.GetMaxLine();
		for( i=0;i<max;i++ ) {
			note.GetLine( tmp, i );
			s = tmp;if ( *s=='+' ) s++;
			if ( strcmp( s, packadd ) == 0 ) return -1;
		}
		if ( mode==1 ) packbuf->PutStr( "+" );
		packbuf->PutStr(">");
	}
	packbuf->PutStr( packadd );
	packbuf->PutStr( "\r\n" );
	return 0;
}


int CToken::AddPackfileOrig(char* name, int mode)
{
	//		packfile出力
	//			0=name/1=+name/2=other
	//
	CStrNote note;
	int i, max;
	char fname[HSP_MAX_PATH];
	char p_fdir[HSP_MAX_PATH];
	char p_fname[HSP_MAX_PATH];
	char packadd[1024];
	char tmp[1024];
	char* s;
	char* findptr;
	bool absolutePath = false;					// 絶対パスか?

	getpath(name, p_fdir, 32);
	getpath(name, p_fname, 8);

#ifdef HSPWIN
	strchr3(p_fdir, ':', 0, &findptr);			// ドライブ文字があった
	if (findptr != NULL) {
		absolutePath = true;
	}
	if (*p_fdir == '\\') {
		absolutePath = true;
	}
#endif
	if (*p_fdir == '/') {
		absolutePath = true;
	}

	if (absolutePath == false) {
		strcpy(fname, search_path);
		strcat(fname, p_fdir);
		strcpy(p_fdir, fname);
		strcat(fname, p_fname);
	}
	else {
		strcpy(fname, name);
	}

	strcpy(packadd, fname);
	if (mode < 2) {
#ifdef HSPWIN
		strcase(packadd);
#endif
		note.Select(packbuf->GetBuffer());
		max = note.GetMaxLine();
		for (i = 0; i < max; i++) {
			note.GetLine(tmp, i);
			s = tmp; if (*s == '+') s++;
			if (strcmp(s, packadd) == 0) return -1;
		}
		if (mode == 1) packbuf->PutStr("+");
	}
	packbuf->PutStr(packadd);
	packbuf->PutStr("\r\n");
	return 0;
}


//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CToken::CToken( void )
{
	s3 = (unsigned char *)malloc( s3size );
	lb = new CLabel;
	tmp_lb = NULL;
	hed_cmpmode = CMPMODE_OPTCODE | CMPMODE_OPTPRM | CMPMODE_SKIPJPSPC;
	tstack = new CTagStack;
	errbuf = NULL;
	packbuf = NULL;
	ahtmodel = NULL;
	ahtbuf = NULL;
	scnvbuf = NULL;
	labbuf = NULL;
	ResetCompiler();
}


CToken::CToken( char *buf )
{
	s3 = (unsigned char *)malloc( s3size );
	lb = new CLabel;
	tmp_lb = NULL;
	hed_cmpmode = CMPMODE_OPTCODE | CMPMODE_OPTPRM | CMPMODE_SKIPJPSPC;
	tstack = new CTagStack;
	errbuf = NULL;
	packbuf = NULL;
	ahtmodel = NULL;
	ahtbuf = NULL;
	scnvbuf = NULL;
	labbuf = NULL;
	ResetCompiler();
}


CToken::~CToken( void )
{
	if ( scnvbuf!=NULL ) InitSCNV(-1);

	if ( tstack!=NULL ) { delete tstack; tstack = NULL; }
	if ( lb!=NULL ) { delete lb; lb = NULL; }
	if ( s3 != NULL ) { free( s3 );s3 = NULL; }
	//	buffer = NULL;
}


void CToken::addCmpMode(int mode)
{
	hed_cmpmode |= mode;
}


void CToken::delCmpMode(int mode)
{
	hed_cmpmode &= ~mode;
}


CLabel *CToken::GetLabelInfo( void )
{
	//		ラベル情報取り出し
	//		(CLabel *を取得したらそちらで、deleteすること)
	//
	CLabel *res;
	res = lb;
	lb = NULL;
	return res;
}


void CToken::SetLabelInfo( CLabel *lbinfo )
{
	//		ラベル情報設定
	//
	tmp_lb = lbinfo;
}


void CToken::SetLabelListBuffer(CMemBuf *buf, int mode, char* match, int line, char *filename)
{
	labbuf = buf;
	cg_labout_mode = mode;
	cg_labout_match = match;
	cg_labout_line = line;
	if (filename == NULL) {
		*cg_labout_orgfile = 0;
	}
	else {
		strcpy(cg_labout_orgfile, filename);
	}

	static char* p[] = {
		"---",
		"fnc",
		"mac",
		"lab",
		"var",
		"exv",
		"cmd",
		"exc",
		NULL };
	cg_labout_header = p;
}


char* CToken::GetLabelListHeader(int flag)
{
	int i = flag & (LABBUF_FLAG_REFER-1);
	if ((i < 0) || (i > LABBUF_FLAG_EXCMD)) i = 0;
	return cg_labout_header[i];
}


void CToken::ResetCompiler( void )
{
	line = 1;
	fpbit = 256.0;
	incinf = 0;
	swsp = 0; swmode = 0; swlevel = 0;
	SetModuleName( "" ); modgc = 0;
	search_path[0] = 0;
	lb->Reset();
	fileadd = 0;
	pp_orgline = 0;
	pp_orgfile[0] = 0;
	pp_orgfilefull[0] = 0;

	//		reset header info
	hed_option = 0;
	hed_runtime[0] = 0;
	hed_autoopt_timer = 0;
	hed_autoopt_strexchange = 0;
	pp_utf8 = 0;

	//		reset labout
	cg_labout_mode = 0;
	cg_labout_line = 0;
	cg_labout_caseflag = 0;
	cg_labout_match = NULL;
	*cg_labout_modname = 0;
}


void CToken::SetAHT( AHTMODEL *aht )
{
	ahtmodel = aht;
}


void CToken::SetAHTBuffer( CMemBuf *aht )
{
	ahtbuf = aht;
}


void CToken::SetLook( char *buf )
{
	wp = (unsigned char *)buf;
}


char *CToken::GetLook( void )
{
	return (char *)wp;
}


char *CToken::GetLookResult( void )
{
	return (char *)s2;
}


int CToken::GetLookResultInt( void )
{
	return val;
}


void CToken::Pickstr( void )
{
	//		Strings pick sub
	//
	int a=0;
	unsigned char a1;
	int skip, i;
	while(1) {

pickag:
		a1=(unsigned char)*wp;
#if 0
		if (a1>=0x81) {
			if (a1<0xa0) {				// s-jis code
				s3[a++]=a1;wp++;
				s3[a++]=*wp;wp++;
				continue;
			}
			else if (a1>=0xe0) {		// s-jis code2
				s3[a++]=a1;wp++;
				s3[a++]=*wp;wp++;
				continue;
			}
		}
#endif

		if (a1==0x5c) {					// '\' extra control
			wp++;a1=tolower(*wp);
			switch(a1) {
				case 'n':
					s3[a++]=13;a1=10;
					break;
				case 't':
					a1=9;
					break;
				case 'r':
					s3[a++]=13;wp++;
					goto pickag;
				case 0x22:
					s3[a++]=a1;wp++;
					goto pickag;
			}
		}
		if (a1==0) { wp=NULL;break; }
#ifdef HSPLINUX
		if (a1==10) {
			wp++;
			line++;
			break;
		}
#endif
		if (a1==13) {
			wp++;if ( *wp==10 ) wp++;
			line++;
			break;
		}
		if (a1==0x22) {
			wp++;
			if ( *wp == 0 ) wp=NULL;
			break;
		}

#if 0
		s3[a++] = a1; wp++;
#else
		skip = SkipMultiByte(a1);	// 全角文字チェック
		for (i = 0; i <= skip; i++) {
			s3[a++] = a1; wp++;
			a1 = *wp;
		}	
#endif

	}
	s3[a]=0;
}


char *CToken::Pickstr2( char *str )
{
	//		Strings pick sub '～'
	//
	unsigned char *vs;
	unsigned char *pp;
	unsigned char a1;
	int skip,i;
	vs = (unsigned char *)str;
	pp = s3;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==0x27) { vs++; break; }
		if (a1==0x5c) {					// '\'チェック
			vs++;
			a1 = tolower( *vs );
			if ( a1 < 32 ) continue;
			switch( a1 ) {
			case 'n':
				*pp++ = 13;
				a1 = 10;
				break;
			case 't':
				a1 = 9;
				break;
			case 'r':
				a1 = 13;
				break;
			}
		}

		skip = SkipMultiByte( a1 );
		if ( skip ) {					// 全角文字チェック
			for(i=0;i<skip;i++) {
				*pp++ = a1;
				vs++;
				a1=*vs;
			}
		}
		vs++;
		*pp++ = a1;
	}
	*pp = 0;
	return (char *)vs;
}


int CToken::CheckModuleName( char *name )
{
	int a;
	unsigned char *p;
	unsigned char a1;

	a = 0;
	p = (unsigned char *)name;
	while(1) {								// normal object name
		a1=*p;
		if (a1==0) { return 0; }
		if (a1<0x30) break;
		if ((a1>=0x3a)&&(a1<=0x3f)) break;
		if ((a1>=0x5b)&&(a1<=0x5e)) break;
		if ((a1>=0x7b)&&(a1<=0x7f)) break;

		p++;
		p += SkipMultiByte( a1 );		// 全角文字チェック
	}
	return -1;
}


int CToken::GetToken( void )
{
	//
	//	get new word from wp ( result:s3 )
	//			result : word type
	//
	int rval;
	int a,b;
	int minmode;
	unsigned char a1,a2,an;
	int fpflag;
//	int *fpival;
	unsigned char *wp_bak;
	int ft_bak;

	if (wp==NULL) return TK_NONE;

	a = 0;
	minmode = 0;
	rval=TK_OBJ;

	while(1) {
		a1=*wp;

#ifdef HSPWIN
		if ( a1 == 0x81 ) {
			if ( wp[1] == 0x40 ) {	// 全角スペースは無視
				if ( hed_cmpmode & CMPMODE_SKIPJPSPC ) {
					wp+=2; continue;
				}
			}
		}
#endif

		if ((a1!=32)&&(a1!=9)) break;	// Skip Space & Tab
		wp++;
	}

	if (a1==0) { wp=NULL;return TK_NONE; }		// End of Source
	if (a1==13) {					// Line Break
		wp++;if (*wp==10) wp++;
		line++;
		return TK_NONE;
	}
	if (a1==10) {					// Unix Line Break
		wp++;
		line++;
		return TK_NONE;
	}

	//	Check Extra Character
	if (a1<0x30) rval=TK_NONE;
	if ((a1>=0x3a)&&(a1<=0x3f)) rval=TK_NONE;
	if ((a1>=0x5b)&&(a1<=0x5e)) rval=TK_NONE;
	if ((a1>=0x7b)&&(a1<=0x7f)) rval=TK_NONE;

	if (a1==':') {   // multi statement
		wp++;
		return TK_SEPARATE;
	}

	if (a1=='0') {
		a2=wp[1];
		if (a2=='x') { wp++;a1='$'; }		// when hex code (0x)
		if (a2=='b') { wp++;a1='%'; }		// when bin code (0b)
	}
	if (a1=='$') {							// when hex code ($)
		wp++;val=0;
		while(1) {
			a1=toupper(*wp);b=-1;
			if (a1==0) { wp=NULL;break; }
			if ((a1>=0x30)&&(a1<=0x39)) b=a1-0x30;
			if ((a1>=0x41)&&(a1<=0x46)) b=a1-55;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;val=(val<<4)+b; }
			wp++;
		}
		s3[a]=0;
		return TK_NUM;
	}

	if (a1=='%') {							// when bin code (%)
		wp++;val=0;
		while(1) {
			a1=*wp;b=-1;
			if (a1==0) { wp=NULL;break; }
			if ((a1>=0x30)&&(a1<=0x31)) b=a1-0x30;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;val=(val<<1)+b; }
			wp++;
		}
		s3[a]=0;
		return TK_NUM;
	}
/*
	if (a1=='-') {							// minus operator (-)
		wp++;an=*wp;
		if ((an<0x30)||(an>0x39)) {
			s3[0]=a1;s3[1]=0;
			return a1;
		}
		minmode++;
		a1=an;						// 次が数値ならばそのまま継続
	}
*/		
	if ((a1>=0x30)&&(a1<=0x39)) {			// when 0-9 numerical
		fpflag = 0;
		ft_bak = 0;
		while(1) {
			a1=*wp;
			if (a1==0) { wp=NULL;break; }
			if (a1=='.') {
				if ( fpflag ) {
					break;
				}
				a2=*(wp+1);
				if ((a2<0x30)||(a2>0x39)) break;
				wp_bak = wp;
				ft_bak = a;
				fpflag = 3;
				//fpflag = -1;
				s3[a++]=a1;wp++;
				continue;
			}
			if ((a1<0x30)||(a1>0x39)) break;
			s3[a++]=a1;
			wp++;
		}
		s3[a]=0;
		if ( wp != NULL ) {
			if ( *wp=='k' ) { fpflag=1;wp++; }
			if ( *wp=='f' ) { fpflag=2;wp++; }
			if ( *wp=='d' ) { fpflag=3;wp++; }
			if ( *wp=='e' ) { fpflag=4;wp++; }
		}

		if ( fpflag<0 ) {				// 小数値でない時は「.」までで終わり
			s3[ft_bak] = 0;
			wp = wp_bak;
			fpflag = 0;
		}

		switch( fpflag ) {
		case 0:					// 通常の整数
			val=atoi_allow_overflow((char *)s3);
			if ( minmode ) val=-val;
			break;
		case 1:					// int固定小数
			val_d = atof( (char *)s3 );
			val = (int)( val_d * fpbit );
			if ( minmode ) val=-val;
			break;
		case 2:					// int形式のfloat値を返す
#if 0
			val_f = (float)atof( (char *)s3 );
			if ( minmode ) val_f=-val_f;
			fpival = (int *)&val_f;
			val = *fpival;
			break;
#endif
			val_d = atof((char*)s3);
			if (minmode) val_d = -val_d;
			return TK_DNUM;
		case 4:					// double値(指数表記)
			s3[a++]='e';
			a1 = *wp;
			if (( a1=='-' )||( a1=='+' )) {
				s3[a++] = a1;
				wp++;
			}
			while(1) {
				a1=*wp;
				if ((a1<0x30)||(a1>0x39)) break;
				s3[a++]=a1;wp++;
			}
			s3[a]=0;
		case 3:					// double値
			val_d = atof( (char *)s3 );
			if ( minmode ) val_d=-val_d;
			return TK_DNUM;
		}
		return TK_NUM;
	}

	if (a1==0x22) {							// when "string"
		wp++;Pickstr();
		return TK_STRING;
	}

	if (a1==0x27) {							// when 'char'
		wp++;
		wp = (unsigned char *)Pickstr2( (char *)wp );
		val=*(unsigned char *)s3;
		return TK_NUM;
	}

	if (rval==TK_NONE) {					// token code
		wp++;an=*wp;
		if (a1=='!') {
			if (an=='=') wp++;
		}
/*
		else if (a1=='<') {
			if (an=='<') { wp++;a1=0x63; }	// '<<'
			if (an=='=') { wp++;a1=0x61; }	// '<='
		}
		else if (a1=='>') {
			if (an=='>') { wp++;a1=0x64; }	// '>>'
			if (an=='=') { wp++;a1=0x62; }	// '>='
		}
*/
		else if (a1=='=') {
			if (an=='=') { wp++; }			// '=='
		}
		else if (a1=='|') {
			if (an=='|') { wp++; }			// '||'
		}
		else if (a1=='&') {
			if (an=='&') { wp++; }			// '&&'
		}
		s3[0]=a1;s3[1]=0;
		return a1;
	}

	while(1) {								// normal object name
		int skip,i;
		a1=*wp;
		if (a1==0) { wp=NULL;break; }
		if (a1<0x30) break;
		if ((a1>=0x3a)&&(a1<=0x3f)) break;
		if ((a1>=0x5b)&&(a1<=0x5e)) break;
		if ((a1>=0x7b)&&(a1<=0x7f)) break;

		if ( a>=OBJNAME_MAX ) break;

		skip = SkipMultiByte( a1 );	// 全角文字チェック
		if ( skip ) {
#ifdef HSPWIN
			if ( hed_cmpmode & CMPMODE_SKIPJPSPC ) {
				if ( a1 == 0x81 ) {
					if ( wp[1] == 0x40 ) {	// 全角スペースは終端として処理
						break;
					}
				}
			}
#endif
			for(i=0;i<skip;i++) {
				s3[a++]=a1;wp++;a1=*wp;
			}
		}
		s3[a++]=a1;wp++;
	}
	s3[a]=0;
	return TK_OBJ;
}


int CToken::PeekToken( void )
{
	// 戻すのは wp のみ。
	// s3, val, val_f, val_d などは戻されない
	unsigned char *wp_bak = wp;
	int result = GetToken();
	wp = wp_bak;
	return result;
}

//-----------------------------------------------------------------------------

void CToken::Calc_token( void )
{
	lasttoken = (char *)wp;
	ttype = GetToken();
}

void CToken::Calc_factor( CALCVAR &v )
{
	CALCVAR v1;
	int id,type;
	char *ptr_dval;
	if ( ttype==TK_NUM ) {
		v=(CALCVAR)val;
		Calc_token();
		return;
	}
	if ( ttype==TK_DNUM ) {
		v=(CALCVAR)val_d;
		Calc_token();
		return;
	}
	if ( ttype==TK_OBJ ) {
		id = lb->Search( (char *)s3 );
		if ( id == -1 ) { ttype=TK_CALCERROR; return; }
		type = lb->GetType( id );
		if ( type != LAB_TYPE_PPVAL ) { ttype=TK_CALCERROR; return; }
			ptr_dval = lb->GetData2( id );
			if ( ptr_dval == NULL ) {
				v = (CALCVAR)lb->GetOpt( id );
			} else {
				v = *(CALCVAR *)ptr_dval;
			}
		Calc_token();
		return;
	}
	if( ttype!='(' ) { ttype=TK_ERROR; return; }
	Calc_token();
	Calc_start(v1); 
	if( ttype!=')' ) { ttype=TK_CALCERROR; return; }
	Calc_token();
	v=v1;
}

void CToken::Calc_unary( CALCVAR &v )
{
	CALCVAR v1;
	int op;
	if ( ttype=='-' ) {
		op=ttype; Calc_token();
		Calc_unary(v1);
		v1 = -v1;
	} else {
		Calc_factor(v1);
	}
	v=v1;
}

void CToken::Calc_muldiv( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op;
	Calc_unary(v1);
	while( (ttype=='*')||(ttype=='/')||(ttype==0x5c)) {
		op=ttype; Calc_token();
		Calc_unary(v2);
		if (op=='*') v1*=v2;
		else if (op=='/') {
			if ( v2==0.0 ) { ttype=TK_CALCERROR; return; }
			v1/=v2;
		} else if (op==0x5c) {
			if ( (int)v2==0 ) { ttype=TK_CALCERROR; return; }
			v1 = fmod( v1, v2 );
		}
	}
	v=v1;
}

void CToken::Calc_addsub( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op;
	Calc_muldiv(v1);
	while( (ttype=='+')||(ttype=='-')) {
		op=ttype; Calc_token();
		Calc_muldiv(v2);
		if (op=='+') v1+=v2;
		else if (op=='-') v1-=v2;
	}
	v=v1;
}


void CToken::Calc_compare( CALCVAR &v )
{
	CALCVAR v1,v2;
	int v1i,v2i,op;
	Calc_addsub(v1);
	while( (ttype=='<')||(ttype=='>')||(ttype=='=')) {
		op=ttype; 
		if (op=='=') {
			Calc_token();
			Calc_addsub(v2);
			v1i = v1==v2;
			v1=(CALCVAR)v1i; continue;
		}
		if (op=='<') {
			if ( *wp=='=' ) {
				wp++;
				Calc_token();Calc_addsub(v2);
				v1i=(v1<=v2); v1=(CALCVAR)v1i; continue;
			}
			if ( *wp=='<' ) {
				wp++;
				Calc_token();Calc_addsub(v2);
				v1i = (int)v1;
				v2i = (int)v2;
				v1i<<=v2i;
				v1=(CALCVAR)v1i; continue;
			}
			Calc_token();
			Calc_addsub(v2);
			v1i=(v1<v2);
			v1=(CALCVAR)v1i; continue;
		}
		if (op=='>') {
			if ( *wp=='=' ) {
				wp++;
				Calc_token();Calc_addsub(v2);
				v1i=(v1>=v2);
				v1=(CALCVAR)v1i; continue;
			}
			if ( *wp=='>' ) {
				wp++;
				Calc_token();Calc_addsub(v2);
				v1i = (int)v1;
				v2i = (int)v2;
				v1i>>=v2i;
				v1=(CALCVAR)v1i; continue;
			}
			Calc_token();
			Calc_addsub(v2);
			v1i=(v1>v2);
			v1=(CALCVAR)v1i; continue;
		}
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_bool2( CALCVAR &v )
{
	CALCVAR v1,v2;
	int v1i,v2i;
	Calc_compare(v1);
	while( ttype=='!') {
		Calc_token();
		Calc_compare(v2);
		v1i = (int)v1;
		v2i = (int)v2;
		v1i = v1i != v2i;
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_bool( CALCVAR &v )
{
	CALCVAR v1,v2;
	int op,v1i,v2i;
	Calc_bool2(v1);
	while( (ttype=='&')||(ttype=='|')||(ttype=='^')) {
		op=ttype; Calc_token();
		Calc_bool2(v2);
		v1i = (int)v1;
		v2i = (int)v2;
		if (op=='&') v1i&=v2i;
		else if (op=='|') v1i|=v2i;
		else if (op=='^') v1i^=v2i;
		v1=(CALCVAR)v1i;
	}
	v=v1;
}


void CToken::Calc_start( CALCVAR &v )
{
	//		entry point
	Calc_bool(v);
}

int CToken::Calc( CALCVAR &val )
{
	CALCVAR v;
	Calc_token();
	Calc_start( v );
	if ( ttype == TK_CALCERROR ) {
		SetError("abnormal calculation");
		return -1;
	}
	if (ttype != TK_NONE) {
		SetError("expression syntax error");
		return -1;
	}
	if ( wp==NULL ) { val = v; return 0; }
	if ( *wp==0 ) { val = v; return 0; }
	SetError("expression syntax error");
	return -1;
}

//-----------------------------------------------------------------------------

char *CToken::ExpandStr( char *str, int opt )
{
	//		指定文字列をmembufへ展開する
	//			opt:0=行末までスキップ/1="まで/2='まで
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	unsigned char sep;
	int skip,i;
	vs = (unsigned char *)str;
	a = 0;
	sep = 0;
	if (opt==1) sep=0x22;
	if (opt==2) sep=0x27;
	s3[a++]=sep;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==sep) { vs++;break; }
		if ((a1<32)&&(a1!=9)) break;
		s3[a++]=a1;vs++;
		if (a1==0x5c) {					// '\'チェック
			s3[a++] = *vs++;
		}

		skip = SkipMultiByte( a1 );	// 全角文字チェック
		if ( skip ) {
			for( i=0;i<skip;i++ ) {
				s3[a++] = *vs++;
			}
		}
	}
	s3[a++]=sep;
	s3[a]=0;
	if ( opt!=0 ) {
		if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	}
	return (char *)vs;
}


char *CToken::ExpandAhtStr( char *str )
{
	//		コメントを展開する
	//		( ;;に続くAHT指定文字列用 )
	//
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	while(1) {
		a1=*vs;
		if (a1==0) break;
		if ((a1<32)&&(a1!=9)) break;
		vs++;
	}
	return (char *)vs;
}


char *CToken::ExpandStrEx( char *str )
{
	//		指定文字列をmembufへ展開する
	//		( 複数行対応 {"～"} )
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	int skip,i;
	vs = (unsigned char *)str;
	a = 0;
	//s3[a++]=0x22;

	while(1) {
		a1=*vs;
		if (a1==0) {
			//s3[a++]=13; s3[a++]=10;
			break;
		}
		if (a1==13) {
			s3[a++]=0x5c; s3[a++]='n';
			vs++;
			if (*vs==10) { vs++; }
			continue;
		}
#ifdef HSPLINUX
		if (a1==10) {
			s3[a++]=0x5c; s3[a++]='n';
			vs++;
			continue;
		}
#endif
//		if ((a1<32)&&(a1!=9)) break;
		if (a1==0x22) {
			if (vs[1]=='}') {
				s3[a++]=0x22; s3[a++]='}';
				mulstr=LMODE_ON; vs+=2; break;
			}
			s3[a++]=0x5c; s3[a++]=0x22;
			vs++;
			continue;
		}
		s3[a++]=a1;vs++;
		if (a1==0x5c) {					// '\'チェック
			if (*vs>=32) { s3[a++] = *vs; vs++; }
		}

		skip = SkipMultiByte( a1 );	// 全角文字チェック
		if ( skip ) {
			for(i=0;i<skip;i++) {
				s3[a++] = *vs++;
			}
		}

	}
	//s3[a++]=0x22;
	s3[a]=0;
	if (wrtbuf!=NULL) { wrtbuf->PutData( s3, a ); }
	return (char *)vs;
}


char *CToken::ExpandStrComment( char *str, int opt )
{
	//		/*～*/ コメントを展開する
	//
	int a;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;
	a = 0;

	while(1) {
		a1=*vs;
		if (a1==0) {
			//s3[a++]=13; s3[a++]=10;
			break;
		}
		if (a1=='*') {
			if (vs[1]=='/') {
				mulstr=LMODE_ON; vs+=2; break;
			}
			vs++;
			continue;
		}
		vs++;
		vs+=SkipMultiByte( a1 );	// 全角文字チェック
	}
	s3[a]=0;
	if ( opt==0 ) if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	return (char *)vs;
}


char *CToken::ExpandHex( char *str, int *val )
{
	//		16進数文字列をmembufへ展開する
	//
	int a,b,num;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	s3[0]='$'; a=1; num=0;
	while(1) {
		a1=toupper(*vs);b=-1;
		if ((a1>=0x30)&&(a1<=0x39)) b=a1-0x30;
		if ((a1>=0x41)&&(a1<=0x46)) b=a1-55;
		if (a1=='_') b=-2;
		if (b==-1) break;
		if (b>=0) { s3[a++]=a1;num=(num<<4)+b; }
		vs++;
	}
	s3[a]=0;
	if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	*val = num;
	return (char *)vs;
}


char *CToken::ExpandBin( char *str, int *val )
{
	//		2進数文字列をmembufへ展開する
	//
	int a,b,num;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;

	s3[0]='%'; a=1; num=0;
	while(1) {
			a1=*vs;b=-1;
			if ((a1>=0x30)&&(a1<=0x31)) b=a1-0x30;
			if (a1=='_') b=-2;
			if (b==-1) break;
			if (b>=0) { s3[a++]=a1;num=(num<<1)+b; }
			vs++;
		}
	s3[a]=0;
	if (wrtbuf!=NULL) wrtbuf->PutData( s3, a );
	return (char *)vs;
}


char *CToken::ExpandToken( char *str, int *type, int ppmode )
{
	//		stringデータをmembufへ展開する
	//			ppmode : 0=通常、1=プリプロセッサ時
	//
	int a,chk,id,ltype,opt;
	int flcnt;
	unsigned char *vs;
	unsigned char *vs_bak;
	unsigned char a1;
	unsigned char a2;
	unsigned char *vs_modbrk;
	char cnvstr[80];
	char fixname[256];
	char *macptr;

	vs = (unsigned char *)str;
	if ( vs==NULL ) {
		*type = TK_EOF;return NULL;			// already end
	}

	a1=*vs;
	if (a1==0) {							// end
		*type = TK_EOF;
		return NULL;
	}
	if (a1==10) {							// Unix改行
		vs++;
		if (wrtbuf!=NULL) wrtbuf->PutStr( "\r\n" );
		*type = TK_EOL;
		return (char *)vs;
	}
	if (a1==13) {							// 改行
		vs++;if ( *vs==10 ) vs++;
		if (wrtbuf!=NULL) wrtbuf->PutStr( "\r\n" );
		*type = TK_EOL;
		return (char *)vs;
	}
	if (a1==';') {							// コメント
		*type = TK_VOID;
		*vs=0;
		vs++;
		if ( *vs == ';' ) {
			vs++;
			if ( ahtmodel != NULL ) {
				ahtkeyword = (char *)vs;
			}
		}
		return ExpandStr( (char *)vs, 0 );
	}
	if (a1=='/') {							// Cコメント
		if (vs[1]=='/') {
			*type = TK_VOID;
			*vs=0;
			return ExpandStr( (char *)vs+2, 0 );
		}
		if (vs[1]=='*') {
			mulstr = LMODE_COMMENT;
			*type = TK_VOID;
			*vs=0;
			return ExpandStrComment( (char *)vs+2, 0 );
		}
	}
	if (a1==0x22) {							// "～"
		*type = TK_STRING;
		return ExpandStr( (char *)vs+1, 1 );
	}
	if (a1==0x27) {							// '～'
		*type = TK_STRING;
		return ExpandStr( (char *)vs+1, 2 );
	}
	if (a1=='{') {							// {"～"}
		if (vs[1]==0x22) {
			if (wrtbuf!=NULL) wrtbuf->PutStr( "{\"" );
			mulstr = LMODE_STR;
			*type = TK_STRING;
			char *pp = ExpandStrEx( (char *)vs+2 );
			return pp;
		}
	}

	if (a1=='0') {
		a2=vs[1];
		if (a2=='x') { vs++;a1='$'; }		// when hex code (0x)
		if (a2=='b') { vs++;a1='%'; }		// when bin code (0b)
	}
	if (a1=='$') {							// when hex code ($)
		*type = TK_OBJ;
		return ExpandHex( (char *)vs+1, &a );
	}

	if (a1=='%') {							// when bin code (%)
		*type = TK_OBJ;
		return ExpandBin( (char *)vs+1, &a );
	}
	
	if (a1<0x30) {							// space,tab
		*type = TK_CODE;
		vs++;
		if (wrtbuf!=NULL) wrtbuf->Put( (char)a1 );
		return (char *)vs;
	}

#ifdef HSPWIN
	if ( hed_cmpmode & CMPMODE_SKIPJPSPC ) {
		if (pp_utf8) {
			if (a1 == 0xe3 && vs[1] == 0x80 && vs[2] == 0x80) {	// 全角スペースを半角スペースに変換する(UTF8)
				*type = TK_CODE;
				vs += 3;
				if (wrtbuf != NULL) {
					wrtbuf->Put((char)0x20);
				}
				return (char*)vs;
			}
		}
		else {
			if (a1 == 0x81 && vs[1] == 0x40) {	// 全角スペースを半角スペースに変換する(SJIS)
				*type = TK_CODE;
				vs += 2;
				if (wrtbuf != NULL) {
					wrtbuf->Put((char)0x20);
				}
				return (char*)vs;
			}
		}
	}
#endif

	chk=0;
	if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
	if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
	if ((a1>=0x7b)&&(a1<=0x7f)) chk++;

	if (chk) {
		vs++;
		if (wrtbuf!=NULL) wrtbuf->Put( (char)a1 );		// 記号
		*type = a1;
		return (char *)vs;
	}

	if ((a1>=0x30)&&(a1<=0x39)) {			// when 0-9 numerical
		a=0; flcnt=0;
		while(1) {
			a1=*vs;
			if ( a1 == '.' ) {
				flcnt++;if ( flcnt>1 ) break;
			} else {
				if ((a1<0x30)||(a1>0x39)) break;
			}
			s2[a++]=a1;vs++;
		}
		if (( a1=='k' )||( a1=='f' )||( a1=='d' )) { s2[a++]=a1; vs++; }
		if ( a1 == 'e' ) {
			s2[a++]=a1; vs++;
			a1=*vs;
			if (( a1=='-' )||( a1=='+' )) {
				s2[a++] = a1;
				vs++;
			}
			while(1) {
				a1=*vs;
				if ((a1<0x30)||(a1>0x39)) break;
				s2[a++]=a1;vs++;
			}
		}

		s2[a]=0;
		if (wrtbuf!=NULL) wrtbuf->PutData( s2, a );
		*type = TK_OBJ;
		return (char *)vs;
	}

	a=0;
	vs_modbrk = NULL;

/*
	if ( ppmode ) {					// プリプロセッサ時は#を含めてキーワードとする
		s2[a++]='#';
	}
*/

	//		半角スペースの検出
	//
#ifdef HSPWIN
	if (( hed_cmpmode & CMPMODE_SKIPJPSPC ) == 0 ) {
		if ( strncmp( (char *)s2,"　",2 )==0 ) {
			SetError("SJIS space code error");
			*type = TK_ERROR; return (char *)vs;
		}
	}
#endif


	//	 シンボル取り出し
	//
	while(1) {
		int skip,i;
		a1=*vs;
		//if ((a1>='A')&&(a1<='Z')) a1+=0x20;		// to lower case

		skip = SkipMultiByte( a1 );					// 全角文字チェック
		if ( skip ) {

#ifdef HSPWIN
			if ( hed_cmpmode & CMPMODE_SKIPJPSPC ) {
				if ( a1 == 0x81 && vs[1] == 0x40 ) {	// 全角スペースは終端と判断
					break;
				}
			}
#endif
			for(i=0;i<(skip+1);i++) {
				if ( a<OBJNAME_MAX ) {
					s2[a++]=a1;
					vs++;
					a1=*vs;
				} else {
					vs++;
				}
			}
			continue;

		}

		chk=0;
		if (a1<0x30) chk++;
		if ((a1>=0x3a)&&(a1<=0x3f)) chk++;
		if ((a1>=0x5b)&&(a1<=0x5e)) chk++;
		if ((a1>=0x7b)&&(a1<=0x7f)) chk++;
		if ( chk ) break;
		vs++;

//		if ( a1=='@' ) if ( *vs==0 ) {
//			vs_modbrk = s2+a;
//		}
		if ( a<OBJNAME_MAX ) s2[a++]=a1;

	}
	s2[a]=0;

	if ( *s2=='@' ) {
		if (wrtbuf!=NULL) wrtbuf->PutData( s2, a );
		*type = TK_CODE;
		return (char *)vs;
	}


	//		シンボル検索
	//
	strcase2( (char *)s2, fixname );

	
	//	if ( vs_modbrk != NULL ) *vs_modbrk = 0;
	FixModuleName( (char *)s2 );
	AddModuleName( fixname );

	id = lb->SearchLocal( (char *)s2, fixname );
	if ( id!=-1 ) {
		ltype = lb->GetType(id);
		switch( ltype ) {
		case LAB_TYPE_PPVAL:
			{
			//		constマクロ展開
			GenerateLabelListAndTagRefPP(fixname, LABBUF_FLAG_MACRO);
			char *ptr_dval;
			ptr_dval = lb->GetData2( id );
			if ( ptr_dval == NULL ) {
				sprintf(cnvstr, "%d", lb->GetOpt(id));
			} else {
				sprintf(cnvstr, "%.16f", *(CALCVAR*)ptr_dval);
			}
			chk = ReplaceLineBuf( str, (char *)vs, cnvstr, 0, NULL );
			break;
			}

		case LAB_TYPE_PPINTMAC:
			//		内部マクロ
			//

			if ( ppmode ) {			//	プリプロセッサ時はそのまま展開
				if (wrtbuf!=NULL) {
					FixModuleName( (char *)s2 );
					wrtbuf->PutStr( (char *)s2 );
				}
				*type = TK_OBJ;
				return (char *)vs;
			}

		case LAB_TYPE_PPMAC:
			//		マクロ展開
			//
			vs_bak = vs;
			while(1) {		// 直後のspace/tabを除去
				a1=*vs_bak;if ((a1!=32)&&(a1!=9)) break;
				vs_bak++;
			}
			opt = lb->GetOpt(id);
			if (( a1 == '=' )&&( opt & PRM_MASK ) ) {	// マクロに代入しようとした場合のエラー
				SetError("Reserved word syntax error");
				*type = TK_ERROR; return (char *)vs;
			}
			//	
			macptr = lb->GetData(id);
			if ( macptr == NULL ) { *cnvstr=0; macptr=cnvstr; }
			if (ltype == LAB_TYPE_PPMAC) {
				GenerateLabelListAndTagRefPP(fixname, LABBUF_FLAG_MACRO);
			}
			chk = ReplaceLineBuf( str, (char *)vs, macptr, opt, (MACDEF *)lb->GetData2(id) );
			break;
		case LAB_TYPE_PPDLLFUNC:
			//		モジュール名付き展開キーワード
			if (wrtbuf!=NULL) {
//				AddModuleName( (char *)s2 );
				if ( lb->GetEternal(id) ) {
					FixModuleName( (char *)s2 );
					wrtbuf->PutStr( (char *)s2 );
				} else {
					wrtbuf->PutStr( fixname );
				}
			}
			*type = TK_OBJ;
			if ( *modname == 0 ) {
				lb->AddReference( id );
			} else {
				int i;
				i = lb->Search( GetModuleName() );
				if ( lb->SearchRelation( id, i ) == 0 ) {
					lb->AddRelation( id, i );
				}
			}
			return (char *)vs;
			break;
		case LAB_TYPE_COMVAR:
			//		COMキーワードを展開
			if (wrtbuf!=NULL) {
				if ( lb->GetEternal(id) ) {
					FixModuleName( (char *)s2 );
					wrtbuf->PutStr( (char *)s2 );
				} else {
					wrtbuf->PutStr( fixname );
				}
			}
			*type = TK_OBJ;
			lb->AddReference( id );
			return (char *)vs;

		case LAB_TYPE_PPMODFUNC:
		default:
			//		通常キーワードはそのまま展開
			if (wrtbuf!=NULL) {
				if ( !lb->GetEternal(id) ) { // local func
					strcpy((char*)s2, lb->GetName(id));
				}
				FixModuleName( (char *)s2 );
				wrtbuf->PutStr( (char *)s2 );
			}
			*type = TK_OBJ;
			lb->AddReference( id );
			return (char *)vs;
		}
		if ( chk ) { *type = TK_ERROR; return str; }
		*type = TK_OBJ;
		return str;
	}

	//		登録されていないキーワードを展開
	//
	if (wrtbuf!=NULL) {
//		AddModuleName( (char *)s2 );
		if ( strcmp( (char *)s2, fixname ) ) {
			//	後ろで定義されている関数の呼び出しのために
			//	モジュール内で@をつけていない識別子の位置を記録する
			undefined_symbol_t sym;
			sym.pos = wrtbuf->GetSize();
			sym.len_include_modname = (int)strlen( fixname );
			sym.len = (int)strlen( (char *)s2 );
			undefined_symbols.push_back( sym );
		}
		wrtbuf->PutStr( fixname );
//		wrtbuf->Put( '?' );
	}
	*type = TK_OBJ;
	return (char *)vs;
}


char *CToken::SkipLine( char *str, int *pline )
{
	//		strから改行までをスキップする
	//		( 行末に「\」で次行を接続 )
	//
	unsigned char *vs;
	unsigned char a1;
	unsigned char a2;
	vs = (unsigned char *)str;
	a2=0;
	while(1) {
		a1=*vs;
		if (a1==0) break;
		if (a1==13) {
			pline[0]++;
			vs++;if ( *vs==10 ) vs++;
			if ( a2!=0x5c ) break;
			continue;
		}
		if (a1==10) {
			pline[0]++;
			vs++;
			if ( a2!=0x5c ) break;
			continue;
		}
		if ((a1<32)&&(a1!=9)) break;
		vs++;a2=a1;
	}
	return (char *)vs;
}


char *CToken::SendLineBuf( char *str )
{
	//		１行分のデータをlinebufに転送
	//
	char *p;
	char *w;
	char a1;
	p = str;
	w = linebuf;
	while(1) {
		a1 = *p;if ( a1==0 ) break;
		p++;
		if ( a1 == 10 ) break;
		if ( a1 == 13 ) {
			if ( *p==10 ) p++;
			break;
		}
		*w++=a1;
	}
	*w=0;
	return p;
}


char *CToken::SendLineBufPP( char *str, int *lines )
{
	//		１行分のデータをlinebufに転送
	//			(行末の'\'は継続 linesに行数を返す)
	//
	unsigned char *p;
	unsigned char *w;
	unsigned char a1,a2;
	int ln;
	int i,skip;
	p = (unsigned char *)str;
	w = (unsigned char *)linebuf;
	a2 = 0; ln =0;
	while(1) {
		a1 = *p;if ( a1==0 ) break;
		p++;
		if ( a1 == 10 ) {
			if ( a2==0x5c ) {
				ln++; w--; a2=0; continue;
			}
			break;
		}
		if ( a1 == 13 ) {
			if ( a2==0x5c ) {
				if ( *p==10 ) p++;
				ln++; w--; a2=0; continue;
			}
			if ( *p==10 ) p++;
			break;
		}
		*w++=a1; a2=a1;
		//	skip multibyte
		skip = SkipMultiByte(a1);
		if (skip > 0) {
			for (i = 0; i < skip; i++) {
				a1 = *p; if (a1 == 0) break;
				p++;
				*w++ = a1;
			}
			a2 = 0;
		}
	}
	*w=0;
	*lines = ln;
	return (char *)p;
}


char *CToken::ExpandStrComment2( char *str )
{
	//		"*/" で終端していない場合は NULL を返す
	//
	int mulstr_bak = mulstr;
	mulstr = LMODE_COMMENT;
	char *result = ExpandStrComment( str, 1 );
	if ( mulstr == LMODE_COMMENT ) {
		result = NULL;
	}
	mulstr = mulstr_bak;
	return result;
}


int CToken::ReplaceLineBuf( char *str1, char *str2, char *repl, int opt, MACDEF *macdef )
{
	//		linebufのキーワードを置き換え
	//		(linetmpを破壊します)
	//			str1 : 置き換え元キーワード先頭(linebuf内)
	//			str2 : 置き換え元キーワード次ptr(linebuf内)
	//			repl : 置き換えキーワード
	//			macopt : マクロ添字の数
	//
	//		return : 0=ok/1=error
	//
	char *w;
	char *w2;
	char *p;
	char *endp;
	char *prm[32];
	char *prme[32];
	char *last;
	char *macbuf;
	char *macbuf2;
	char a1;
	char dummy[4];
	char mactmp[128];
	int i,flg,type,cnvfnc, tagid, stklevel;
	int macopt, ctype, noprm, kakko;

	i = 0; flg = 1; cnvfnc = 0; ctype = 0; kakko = 0;
	macopt = opt & PRM_MASK;
	if ( opt & PRM_FLAG_CTYPE ) ctype=1;
	*dummy = 0;
	strcpy( linetmp, str2 );
	wp = (unsigned char *)linetmp;
	if (( macopt )||( ctype )) {
		p = (char *)wp; type = GetToken();
		if ( ctype ) {
			if ( type!='(' ) {
#ifdef JPNMSG
				SetError("ctypeマクロの直後には、丸括弧でくくられた引数リストが必要です");
#else
				SetError("C-Type macro syntax error");
#endif
				return 4;
			}
			p = (char *)wp; type = GetToken();
		}
		if ( type != TK_NONE ) {
			wp = (unsigned char *)p;
			prm[i]=p;
			while(1) {					// マクロパラメータを取り出す
				p = (char *)wp; type = GetToken();
				if ( type==';' ) type = TK_SEPARATE;
				if ( type=='}' ) type = TK_SEPARATE;
				if ( type=='/' ) {		// Cコメント??
					if (*wp=='/') { type = TK_SEPARATE; }
					if (*wp=='*') {
						char *start = (char *)wp-1;
						char *end = ExpandStrComment2( start+2 );
						if ( end == NULL ) {	// 範囲コメントが次の行まで続いている
							type = TK_SEPARATE;
						} else {
							wp = (unsigned char *)end;
						}
					}
				}
				if ( flg ) {
					flg=0;
					prm[i]=p;
					if ( type == TK_NONE ) {
						prme[i++]=p;
						break;
					}
				}
				if ( type==TK_SEPARATE ) {
					wp = (unsigned char *)p;
					prme[i++]=(char *)wp;
					break;
				}
				if ( wp==NULL ) {
					prme[i++]=NULL; break;
				}
				if ( type==',' ) {
					if ( kakko == 0 ) {	// カッコに囲まれている場合は無視する
						prme[i]=p;
						flg=1;i++;
					}
				}
				if ( ctype == 0 ) {		// 通常時のカッコ処理
					if ( type=='(' ) kakko++;
					if ( type==')' ) kakko--;
				} else {				// Cタイプ時のカッコ処理
					if ( type=='(' ) { kakko++; ctype++; }
					if ( type==')' ) {
						kakko--;
						if ( ctype==1 ) {
							wp = (unsigned char *)p;
							prme[i++]=(char *)wp;
							while(1) {
								if ((*wp!=32)&&(*wp!=9)) break;
								wp++;
							}
							*wp = 32;		// ')'をspaceに
							break;
						}
						ctype--;
					}
				}
			}
		}

		if ( i>macopt ) {
			noprm=1;
			if (( ctype )&&( i==1 )&&( macopt==0 )&&( prm[0] == prme[0] )) noprm=0;
			if ( noprm ) {
#ifdef JPNMSG
				SetError("マクロの引数が多すぎます");
#else
				SetError("too many macro parameters");
#endif
				return 3;
			}
		}
		while(1) {					// 省略パラメータを補完
			if ( i>=macopt ) break;
			prm[i]=dummy; prme[i]=dummy;
			i++;
		}
//		{ int a;for(a=0;a<i;a++) {
//			sprintf( errtmp,"[%d][%s]",a,prm[a] );Alert( errtmp );
//		} }
	}
	last = (char *)wp;

	tagid = 0x10000;
	w = str1;
	wp = (unsigned char *)repl;
	while(1) {					// マクロ置き換え
		if ( wp==NULL ) break;
		if ( w>=linetmp ) { SetError("macro buffer overflow"); return 4; }
		a1=*wp++;if (a1==0) break;
		if (a1=='%') {
			if (*wp=='%') { *w++=a1;wp++;continue; }
			type = GetToken();
			if ( type==TK_OBJ ) {			// 特殊コマンドラベル処理
				macbuf = mactmp;
				*mactmp=0; a1 = tolower( (int)*s3 );
				switch( a1 ) {
				case 't':					// %tタグ名
					tagid = tstack->GetTagID( (char *)(s3+1) );
					break;
				case 'i':
					tstack->GetTagUniqueName( tagid, mactmp );
					tstack->PushTag( tagid, mactmp );
					if ( s3[1]=='0' ) *mactmp=0;
					break;
				case 's':
					val = (int)(s3[1]-48);val--;
					if ( !( 0 <= val && val <= macopt - 1 ) ) {
						SetError("illegal macro parameter %s"); return 2;
					}
					w2 = mactmp;
					p = prm[val]; endp = prme[val];
					if ( p==endp ) {				// 値省略時
						macbuf2 = macdef->data + macdef->index[val];
						while(1) {
							a1=*macbuf2++;if (a1==0) break;
							*w2++ = a1;
						}
					} else {
						while(1) {						// %numマクロ展開
							if ( p==endp ) break;
							a1 = *p++;if ( a1==0 ) break;
							*w2++ = a1;
						}
					}
					*w2=0;
					tstack->PushTag( tagid, mactmp );
					*mactmp=0;
					break;
				case 'n':
					tstack->GetTagUniqueName( tagid, mactmp );
					break;
				case 'p':
					stklevel = (int)(s3[1]-48);
					if (( stklevel<0 )||( stklevel>9 )) stklevel=0;
					macbuf = tstack->LookupTag( tagid, stklevel );
					break;
				case 'o':
					if ( s3[1]!='0' ) {
						macbuf = tstack->PopTag( tagid );
					} else {
						tstack->PopTag( tagid );
					}
					break;
				case 'c':
					mactmp[0]=0x0d; mactmp[1]=0x0a; mactmp[2]=0;
					break;
				default:
					macbuf = NULL;
					break;
				}
				if ( macbuf == NULL ) {
					sprintf( mactmp, "macro syntax error [%s]",tstack->GetTagName( tagid ) );
					SetError( mactmp ); return 2;
				}
				while(1) {					//mactmp展開
					a1 = *macbuf++;if ( a1==0 ) break;
					*w++ = a1;
				}
				if ( wp!=NULL ) {
					a1=*wp;if (a1==' ') wp++;	// マクロ後のspace除去
				}
				continue;
			}
			if ( type!=TK_NUM ) { SetError("macro parameter invalid"); return 1; }
			val--;
			if ( !( 0 <= val && val <= macopt - 1 ) ) {
				SetError("illegal macro parameter"); return 2;
			}
			p = prm[val]; endp = prme[val];
			if ( p==endp ) {				// 値省略時
				macbuf = macdef->data + macdef->index[val];
				if ( *macbuf == 0 ) {
#ifdef JPNMSG
					SetError("デフォルトパラメータのないマクロの引数は省略できません");
#else
					SetError("no default parameter");
#endif
					return 5;
				}
				while(1) {
					a1=*macbuf++;if (a1==0) break;
					*w++ = a1;
				}
				continue;
			}
			while(1) {						// %numマクロ展開
				if ( p==endp ) break;
				a1 = *p++;if ( a1==0 ) break;
				*w++ = a1;
			}
			continue;
		}
		*w++ = a1;
	}
	*w = 0;
	if ( last!=NULL ) {
		if ( w + strlen(last) + 1 >= linetmp ) { SetError("macro buffer overflow"); return 4; }
		strcpy( w, last );
	}
	return 0;
}


ppresult_t CToken::PP_SwitchStart( int sw )
{
	if ( swsp==0 ) { swflag = 1; swlevel = LMODE_ON; }
	if ( swsp >= SWSTACK_MAX ) {
		SetError("#if nested too deeply");
		return PPRESULT_ERROR;
	}
	swstack[swsp] = swflag;				// 有効フラグ
	swstack2[swsp] = swmode;			// elseモード
	swstack3[swsp] = swlevel;			// ON/OFF
	swsp++;
	swmode = 0;
	if ( swflag == 0 ) return PPRESULT_SUCCESS;
	if ( sw==0 ) { swlevel = LMODE_OFF; }
			else { swlevel = LMODE_ON; }
	mulstr = swlevel;
	if ( mulstr == LMODE_OFF ) swflag=0;
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_SwitchEnd( void )
{
	if ( swsp == 0 ) {
		SetError("#endif without #if");
		return PPRESULT_ERROR;
	}
	swsp--;
	swflag = swstack[swsp];
	swmode = swstack2[swsp];
	swlevel = swstack3[swsp];
	if ( swflag ) mulstr = swlevel;
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_SwitchReverse( void )
{
	if ( swsp == 0 ) {
		SetError("#else without #if");
		return PPRESULT_ERROR;
	}
	if ( swmode != 0 ) {
		SetError("#else after #else");
		return PPRESULT_ERROR;
	}
	if ( swstack[swsp-1] == 0 ) return PPRESULT_SUCCESS;	// 上のスタックが無効なら無視
	swmode = 1;
	if ( swlevel == LMODE_ON ) { swlevel = LMODE_OFF; } else { swlevel = LMODE_ON; }
	mulstr = swlevel;
	swflag ^= 1;
	return PPRESULT_SUCCESS;
}



ppresult_t CToken::PP_IncludeSub(char* word, int is_addition)
{
	char tmp_spath[HSP_MAX_PATH];
	int add_bak;

	incinf++;
	if (incinf > 32) {
		SetError("too many include level");
		return PPRESULT_ERROR;
	}
	strcpy(tmp_spath, search_path);
	if (is_addition) add_bak = SetAdditionMode(1);
	int res = ExpandFile(wrtbuf, word, word);
	if (is_addition) SetAdditionMode(add_bak);
	strcpy(search_path, tmp_spath);
	incinf--;
	if (res) {
		if (is_addition && res == -1) return PPRESULT_SUCCESS;
		return PPRESULT_ERROR;
	}
	return PPRESULT_INCLUDED;
}


ppresult_t CToken::PP_Include( int is_addition )
{
	char* word = (char*)s3;
	int type = GetToken();
	switch (type) {
	case TK_STRING:
		return PP_IncludeSub(word, is_addition);
	case TK_OBJ:
		strcat(word,".as");
		return PP_IncludeSub(word, is_addition);
	default:
		break;
	}

	if (is_addition) {
		SetError("invalid addition suffix");
	}
	else {
		SetError("invalid include suffix");
	}
	return PPRESULT_ERROR;
}


ppresult_t CToken::PP_use(void)
{
	char strtmp[1024];
	char* word = (char*)s3;
	int i;
	ppresult_t myres = PPRESULT_SUCCESS;
	CMemBuf inclist;
	CMemBuf *bak_wrtbuf;
	CStrNote note;

	while (1) {
		i = GetToken();
		switch (i) {
		case TK_OBJ:
			inclist.PutStr(word);
			inclist.PutCR();
			break;
		default:
			sprintf(strtmp, "invalid use suffix [%s]", word);
			SetError(strtmp);
			return PPRESULT_ERROR;
		}
		if (wp == NULL) break;

		i = GetToken();
		if (i == TK_NONE) break;
		if (i != ',') {
			SetError("invalid use syntax");
			return PPRESULT_ERROR;
		}
	}
	inclist.Put(0);

	bak_wrtbuf = wrtbuf;

	note.Select( inclist.GetBuffer() );
	int max = note.GetMaxLine();
	for (i = 0; i < max; i++) {
		note.GetLine(strtmp, i);
		strcat(strtmp, ".as");
		ppresult_t res = PP_IncludeSub(strtmp, 1);
		if (res == PPRESULT_SUCCESS) {
			note.GetLine(strtmp, i);
			strcat(strtmp, ".hsp");
			res = PP_IncludeSub(strtmp, 1);
		}
		wrtbuf = bak_wrtbuf;
		if ((res == PPRESULT_ERROR)||(res == PPRESULT_SUCCESS)) {
			return PPRESULT_ERROR;
		}
		myres = PPRESULT_INCLUDED;
	}
	return myres;
}

ppresult_t CToken::PP_Const( void )
{
	//		#const解析
	//
	enum ConstType { Indeterminate, Double, Int };
	ConstType valuetype = ConstType::Indeterminate;

	char *word;
	int id, res, glmode;
	char keyword[256];
	char strtmp[512];
	CALCVAR cres;
	glmode = 0;
	word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	// 型指定キーワード
	if ( tstrcmp(word, "double") ) {
		valuetype = ConstType::Double;
	} else if ( tstrcmp(word, "int") ) {
		valuetype = ConstType::Int;
	}
	if (valuetype != ConstType::Indeterminate) {
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad #const syntax" ); return PPRESULT_ERROR;
		}
		strcase(word);
	}

	strcpy( keyword, word );
	if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
	res = lb->Search( keyword );if ( res != -1 ) {
		SetErrorSymbolOverdefined(keyword, res);
		return PPRESULT_ERROR;
	}
	GenerateLabelListAndTagPP(keyword, LABBUF_FLAG_MACRO);

	if ( Calc(cres) ) return PPRESULT_ERROR;

	//		AHT keyword check
	if ( ahtkeyword != NULL ) {

		if ( ahtbuf != NULL ) {						// AHT出力時
			AHTPROP *prop;
			CALCVAR dbval;
			prop = ahtmodel->GetProperty( keyword );
			if ( prop != NULL ) {
				id = lb->Regist( keyword, LAB_TYPE_PPVAL, prop->GetValueInt() );
				if ( cres != floor( cres ) ) {
					dbval = prop->GetValueDouble();
					lb->SetData2( id, (char *)(&dbval), sizeof(CALCVAR) );
				}
				if ( glmode ) lb->SetEternal( id );
				return PPRESULT_SUCCESS;
			}
		} else {									// AHT読み出し時
			if ( cres != floor( cres ) ) {
				ahtmodel->SetPropertyDefaultDouble( keyword, (double)cres );
			} else {
				ahtmodel->SetPropertyDefaultInt( keyword, (int)cres );
			}
			if ( ahtmodel->SetAHTPropertyString( keyword, ahtkeyword ) ) {
				SetError( "AHT parameter syntax error" ); return PPRESULT_ERROR;
			}
		}
	}


	id = lb->Regist( keyword, LAB_TYPE_PPVAL, (int)cres );
	if ( valuetype == ConstType::Double
		|| (valuetype == ConstType::Indeterminate && cres != floor(cres)) ) {
		lb->SetData2( id, (char *)(&cres), sizeof(CALCVAR) );
	}
	if ( glmode ) lb->SetEternal( id );

	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Enum( void )
{
	//		#enum解析
	//
	char *word;
	int id,res,glmode;
	CALCVAR cres;
	char keyword[256];
	char strtmp[512];
	glmode = 0;
	word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	strcpy( keyword, word );
	if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
	res = lb->Search( keyword );if ( res != -1 ) {
		SetErrorSymbolOverdefined(keyword, res);
		return PPRESULT_ERROR;
	}
	GenerateLabelListAndTagPP(keyword, LABBUF_FLAG_MACRO);

	if ( GetToken() == '=' ) {
		if ( Calc( cres ) ) return PPRESULT_ERROR;
		enumgc = (int)cres;
	}
	res = enumgc++;
	id = lb->Regist( keyword, LAB_TYPE_PPVAL, res );
	if ( glmode ) lb->SetEternal( id );
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_VarFix(char* rootword)
{
	//		#var解析
	//
	int i,prm;
	int glmode;
	char* word;
	word = (char*)s3;

	wrtbuf->PutStrf("#%s ", rootword);
	prm = 0;
	glmode = 0;

	while (1) {
		i = GetToken();
		if (i != TK_OBJ) { SetError("invalid variable name"); return PPRESULT_ERROR; }
		strcase(word);
		if (prm == 0) {
			if (tstrcmp(word, "global")) {		// global macro
				glmode = 1;
				i = GetToken();
				if (wp == NULL) break;
				if (i != TK_OBJ) { SetError("invalid variable name"); return PPRESULT_ERROR; }
				strcase(word);
			}
		}
		if (glmode) FixModuleName(word); else AddModuleName(word);
		if (prm>0) wrtbuf->PutStrf(",");
		wrtbuf->PutStrf(word);

		if (wp == NULL) break;

		i = GetToken();
		if (i == TK_NONE) break;
		if (i != ',') {
			SetError("invalid variable syntax");
			return PPRESULT_ERROR;
		}
		prm++;
	}

	wrtbuf->PutCR();
	return PPRESULT_WROTE_LINE;
}



/*
	rev 54
	mingw : warning : 比較は常に…
	に対処。
*/

char *CToken::CheckValidWord( void )
{
	//		行末までにコメントがあるか調べる
	//			( return : 有効文字列の先頭ポインタ )
	//
	char *res;
	char *p;
	char *p2;
	unsigned char a1;
	int qqflg, qqchr;
	res = (char *)wp;
	if ( res == NULL ) return res;
	qqflg = 0;
	p = res;
	while(1) {
		a1 = *p;
		if ( a1==0 ) break;


		if ( qqflg==0 ) {						// コメント検索フラグ
		
			if ( a1==0x22 ) { qqflg=1; qqchr=a1; }
			if ( a1==0x27 ) { qqflg=1; qqchr=a1; }
			if ( a1==';' ) {						// コメント
				*p = 0; break;
			}
			if ( a1=='/' ) {						// Cコメント
				if (p[1]=='/') {
					*p = 0; break;
				}
				if (p[1]=='*') {
					mulstr = LMODE_COMMENT;
					p2 = ExpandStrComment( (char *)p+2, 1 );
					while(1) {
						if ( p>=p2 ) break;
						*p++=32;			// コメント部分をspaceに
					}
					continue;
				}
			}
		} else {								// 文字列中はコメント検索せず
			if (a1==0x5c) {							// '\'チェック
				p++; a1 = *p;
				if ( a1>=32 ) p++;
				continue;
			}
			if ( a1==qqchr ) qqflg=0;
		}
		
		p += SkipMultiByte( a1 );			// 全角文字チェック
		p++;
	}
	return res;
}


ppresult_t CToken::PP_Define( void )
{
	//		#define解析
	//
	char *word;
	char *wdata;
	int id,res,type,prms,flg,glmode,ctype;
	char a1;
	MACDEF *macdef;
	int macptr;
	char *macbuf;
	char keyword[256];
	char strtmp[512];

	glmode = 0; ctype = 0;
	word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad macro syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}
	if (tstrcmp(word,"ctype")) {		// C-type macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad macro syntax" ); return PPRESULT_ERROR;
		}
		ctype=1;
		strcase( word );
		if (tstrcmp(word, "global")) {		// global macro
			if (GetToken() != TK_OBJ || glmode==1) {
				SetError("bad macro syntax"); return PPRESULT_ERROR;
			}
			glmode = 1;
			strcase(word);
		}
	}
	strcpy( keyword, word );
	if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
	res = lb->Search( keyword );if ( res != -1 ) {
		SetErrorSymbolOverdefined(keyword, res);
		return PPRESULT_ERROR;
	}
	GenerateLabelListAndTagPP(keyword, LABBUF_FLAG_MACRO);

	//		skip space,tab code
	if ( wp==NULL ) a1=0;
	else {
		a1 = *wp;
		if (a1!='(') a1=0;
	}

	if ( a1==0 ) {					// no parameters
		prms = 0;
		if ( ctype ) prms|=PRM_FLAG_CTYPE;
		wdata = CheckValidWord();

		//		AHT keyword check
		if ( ahtkeyword != NULL ) {
			if ( ahtbuf != NULL ) {						// AHT出力時
				AHTPROP *prop;
				prop = ahtmodel->GetProperty( keyword );
				if ( prop != NULL ) wdata = prop->GetOutValue();
			} else {									// AHT読み込み時
				AHTPROP *prop;
				prop = ahtmodel->SetPropertyDefault( keyword, wdata );
				if ( ahtmodel->SetAHTPropertyString( keyword, ahtkeyword ) ) {
					SetError( "AHT parameter syntax error" ); return PPRESULT_ERROR;
				}
				if ( prop->ahtmode & AHTMODE_OUTPUT_RAW ) {
					ahtmodel->SetPropertyDefaultStr( keyword, wdata );
				}
			}
		}

		id = lb->Regist( keyword, LAB_TYPE_PPMAC, prms );
		lb->SetData( id, wdata );
		if ( glmode ) lb->SetEternal( id );

		return PPRESULT_SUCCESS;
	}

	//		パラメータ定義取得
	//
	macdef = (MACDEF *)linetmp;
	macdef->data[0] = 0;
	macptr = 1;				// デフォルトマクロデータ参照オフセット
	wp++;
	prms=0; flg=0;
	while(1) {
		if ( wp==NULL ) goto bad_macro_param_expr;
		a1 = *wp++;
		if ( a1==')' ) {
			if ( flg==0 ) goto bad_macro_param_expr;
			prms++;
			break;
		}
		switch( a1 ) {
		case 9:
		case 32:
			break;
		case ',':
			if ( flg==0 ) goto bad_macro_param_expr;
			prms++;flg=0;
			break;
		case '%':
			if ( flg!=0 ) goto bad_macro_param_expr;
			type = GetToken();
			if ( type != TK_NUM ) goto bad_macro_param_expr;
			if ( val != (prms+1) ) goto bad_macro_param_expr;
			flg = 1;
			macdef->index[prms] = 0;			// デフォルト(初期値なし)
			break;
		case '=':
			if ( flg!=1 ) goto bad_macro_param_expr;
			flg = 2;
			macdef->index[prms] = macptr;		// 初期値ポインタの設定
			type = GetToken();
			switch(type) {
			case TK_NUM:
#ifdef HSPWIN
				_itoa( val, word, 10 );
#else
				sprintf( word, "%d", val );
#endif
				break;
			case TK_DNUM:
				strcpy( word, (char *)s3 );
				break;
			case TK_STRING:
				sprintf( strtmp,"\"%s\"", word );
				strcpy( word, strtmp );
				break;
			case TK_OBJ:
				break;
			case '-':
				type = GetToken();
				if ( type == TK_DNUM ) {
					sprintf( strtmp,"-%s", s3 );
					strcpy( word, strtmp );
					break;
				}
				if ( type != TK_NUM ) {
					SetError("bad default value");
					return PPRESULT_ERROR;
				}
				//_itoa( val, word, 10 );
				sprintf( word,"-%d",val );
				break;
			default:
				SetError("bad default value");
				return PPRESULT_ERROR;
			}
			macbuf = (macdef->data) + macptr;
			res = (int)strlen( word );
			strcpy( macbuf, word );
			macptr+=res+1;
			break;
		default:
			goto bad_macro_param_expr;
		}
	}

	//		skip space,tab code
	if ( wp==NULL ) a1=0; else {
		while(1) {
			a1=*wp;if (a1==0) break;
			if ( (a1!=9)&&(a1!=32) ) break;
			wp++;
		}
	}
	if ( a1 == 0 ) { SetError("macro contains no data"); return PPRESULT_ERROR; }
	if ( ctype ) prms|=PRM_FLAG_CTYPE;

	//		データ定義
	id = lb->Regist( keyword, LAB_TYPE_PPMAC, prms );
	wdata = CheckValidWord();
	lb->SetData( id, wdata );
	lb->SetData2( id, (char *)macdef, macptr+sizeof(macdef->index) );
	if ( glmode ) lb->SetEternal( id );

	//sprintf( keyword,"[%d]-[%s]",id,wdata );Alert( keyword );
	return PPRESULT_SUCCESS;

bad_macro_param_expr:
	SetError("bad macro parameter expression");
	return PPRESULT_ERROR;
}


ppresult_t CToken::PP_Defcfunc( int mode )
{
	//		#defcfunc解析
	//			mode : 0 = 通常cfunc
	//			       1 = modcfunc
	//
	int i,id;
	char *word;
	char *mod;
	char fixname[128];
	int glmode, premode;

	word = (char *)s3;
	mod = GetModuleName();
	id = -1; glmode = 0; premode = LAB_TYPE_PPMODFUNC;

	i = GetToken();
	if ( i == TK_OBJ ) {
		strcase( word );
		if (tstrcmp(word,"local")) {		// local option
			if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
			glmode = 1;
			i = GetToken();
		}
		if (tstrcmp(word,"prep")) {			// prepare option
			premode = LAB_TYPE_PP_PREMODFUNC;
			i = GetToken();
		}
	}

	strcase2( word, fixname );
	if ( i != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }
	i = lb->Search( fixname );if ( i != -1 ) {
		if ( lb->GetFlag(i) != LAB_TYPE_PP_PREMODFUNC ) {
			SetErrorSymbolOverdefined(fixname, i); return PPRESULT_ERROR;
		}
		id = i;
	}

	if ( glmode ) AddModuleName( fixname );

	if ( premode == LAB_TYPE_PP_PREMODFUNC ) {
		wrtbuf->PutStrf( "#defcfunc prep %s ",fixname );
	} else {
		wrtbuf->PutStrf( "#defcfunc %s ",fixname );
	}

	if ( id == -1 ) {
		id = lb->Regist( fixname, premode, 0, pp_orgfilefull, pp_orgline);
		if ( glmode == 0 ) lb->SetEternal( id );
		if ( *mod != 0 ) { lb->AddRelation( mod, id ); }		// モジュールラベルに依存を追加
	} else {
		lb->SetFlag( id, premode );
	}

	if ( mode ) {
		if ( mode == 1 ) {
			wrtbuf->PutStr( "modvar " );
		} else {
			wrtbuf->PutStr( "modinit " );
		}
		if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
		wrtbuf->PutStr( mod );
		if ( wp != NULL ) wrtbuf->Put( ',' );
	}

	/*
	char resname[512];
	i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid result name"); return PPRESULT_ERROR; }
	strcpy( resname, word );
	*/

	while(1) {

		i = GetToken();
		if ( i == TK_OBJ ) {
			wrtbuf->PutStr( word );
		}
		if ( wp == NULL ) break;
		if ( i != TK_OBJ ) {
			SetError("invalid func param"); return PPRESULT_ERROR;
		}

		i = GetToken();
		if ( i == TK_OBJ ) {
			strcase2( word, fixname );
			AddModuleName( fixname );
			wrtbuf->Put( ' ' );
			wrtbuf->PutStr( fixname );
			i = GetToken();
		}
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid func param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );

	}

	//wrtbuf->PutStr( linebuf );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Deffunc( int mode )
{
	//		#deffunc解析
	//			mode : 0 = 通常func
	//			       1 = modfunc
	//			       2 = modinit
	//			       3 = modterm
	int i,id;
	char *word;
	char *mod;
	char fixname[128];
	int glmode, premode;

	word = (char *)s3;
	mod = GetModuleName();
	id = -1; glmode = 0; premode = LAB_TYPE_PPMODFUNC;

	if ( mode < 2 ) {
		i = GetToken();
		if ( i == TK_OBJ ) {
			strcase( word );
			if (tstrcmp(word,"local")) {		// local option
				if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
				glmode = 1;
				i = GetToken();
			}
			if (tstrcmp(word,"prep")) {			// prepare option
				premode = LAB_TYPE_PP_PREMODFUNC;
				i = GetToken();
			}
		}

		strcase2( word, fixname );
		if ( i != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }
		i = lb->Search( fixname );if ( i != -1 ) {
			if ( lb->GetFlag(i) != LAB_TYPE_PP_PREMODFUNC ) {
				SetErrorSymbolOverdefined(fixname, i); return PPRESULT_ERROR;
			}
			id = i;
		}

		if ( glmode ) AddModuleName( fixname );

		if ( premode == LAB_TYPE_PP_PREMODFUNC ) {
			wrtbuf->PutStrf( "#deffunc prep %s ",fixname );
		} else {
			wrtbuf->PutStrf( "#deffunc %s ",fixname );
		}

		if ( id == -1 ) {
			id = lb->Regist( fixname, premode, 0, pp_orgfilefull, pp_orgline);
			if ( glmode == 0 ) lb->SetEternal( id );
			if ( *mod != 0 ) { lb->AddRelation( mod, id ); }		// モジュールラベルに依存を追加
		} else {
			lb->SetFlag( id, premode );
		}

		if ( mode ) {
			wrtbuf->PutStr( "modvar " );
			if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
			wrtbuf->PutStr( mod );
			if ( wp != NULL ) wrtbuf->Put( ',' );
		}

	} else {
		if ( mode == 2 ) {
			wrtbuf->PutStr( "#deffunc __init modinit " );
		} else {
			wrtbuf->PutStr( "#deffunc __term modterm " );
		}
		if ( *mod == 0 ) { SetError("module name not found"); return PPRESULT_ERROR; }
		wrtbuf->PutStr( mod );
		if ( wp != NULL ) wrtbuf->Put( ',' );
	}

	while(1) {

		i = GetToken();
		if ( i == TK_OBJ ) {
			wrtbuf->PutStr( word );
			strcase( word );
			if (tstrcmp(word,"onexit")) {							// onexitは参照済みにする
				lb->AddReference( id );
			}
		}

		if ( wp == NULL ) break;
		if ( i != TK_OBJ ) {
			SetError("invalid func param"); return PPRESULT_ERROR;
		}

		i = GetToken();
		if ( i == TK_OBJ ) {
			strcase2( word, fixname );
			AddModuleName( fixname );
			wrtbuf->Put( ' ' );
			wrtbuf->PutStr( fixname );
			i = GetToken();
		}
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid func param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );

	}

	//wrtbuf->PutStr( linebuf );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Struct( void )
{
	//		#struct解析
	//
	char *word;
	int i;
	int id,res,glmode;
	char keyword[256];
	char tagname[256];
	char strtmp[0x4000];
	glmode = 0;
	word = (char *)s3;
	if ( GetToken() != TK_OBJ ) {
		sprintf( strtmp,"invalid symbol [%s]", word );
		SetError( strtmp ); return PPRESULT_ERROR;
	}

	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
		strcase( word );
	}

	strcpy( tagname, word );
	if ( glmode ) FixModuleName( tagname ); else AddModuleName( tagname );
	res = lb->Search( tagname );
	if ( res != -1 ) { SetErrorSymbolOverdefined(tagname, res); return PPRESULT_ERROR; }
	id = lb->Regist( tagname, LAB_TYPE_PPDLLFUNC, 0 );
	if ( glmode ) lb->SetEternal( id );

	wrtbuf->PutStrf( "#struct %s ",tagname );

	while(1) {

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != TK_OBJ ) { SetError("invalid struct param"); return PPRESULT_ERROR; }
		wrtbuf->PutStr( word );
		wrtbuf->Put( ' ' );

		i = GetToken();
		if ( i != TK_OBJ ) { SetError("invalid struct param"); return PPRESULT_ERROR; }

		sprintf( keyword,"%s_%s", tagname, word );
		if ( glmode ) FixModuleName( keyword ); else AddModuleName( keyword );
		res = lb->Search( keyword );
		if ( res != -1 ) { SetErrorSymbolOverdefined(keyword, res); return PPRESULT_ERROR; }
		id = lb->Regist( keyword, LAB_TYPE_PPDLLFUNC, 0 );
		if ( glmode ) lb->SetEternal( id );
		wrtbuf->PutStr( keyword );

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid struct param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );

	}

	wrtbuf->PutCR();
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Func( char *name )
{
	//		#func解析
	//
	int i, id;
	int glmode;
	char *word;
	word = (char *)s3;
	i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }

	glmode = 0;
	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
	}

	if ( glmode ) FixModuleName( word ); else AddModuleName( word );
	//AddModuleName( word );
	i = lb->Search( word );
	if ( i != -1 ) { SetErrorSymbolOverdefined(word, i); return PPRESULT_ERROR; }
	id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
	if ( glmode ) lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#%s %s%s",name, word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Cmd( char *name )
{
	//		#cmd解析
	//
	int i, id;
	char *word;
	word = (char *)s3;
	i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid func name"); return PPRESULT_ERROR; }
	i = lb->Search( word );
	if ( i != -1 ) { SetErrorSymbolOverdefined(word, i); return PPRESULT_ERROR; }

	id = lb->Regist( word, LAB_TYPE_PPINTMAC, 0 );		// 内部マクロとして定義
	strcat( word, "@hsp" );
	lb->SetData( id, word );
	lb->SetEternal( id );

	//AddModuleName( word );
	//id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
	//lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#%s %s%s",name, word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Usecom( void )
{
	//		#usecom解析
	//
	int i, id;
	int glmode;
	char *word;
	word = (char *)s3;
	i = GetToken();
	if ( i != TK_OBJ ) { SetError("invalid COM symbol name"); return PPRESULT_ERROR; }

	glmode = 0;
	strcase( word );
	if (tstrcmp(word,"global")) {		// global macro
		if ( GetToken() != TK_OBJ ) {
			SetError( "bad global syntax" ); return PPRESULT_ERROR;
		}
		glmode=1;
	}

	i = lb->Search( word );
	if ( i != -1 ) { SetErrorSymbolOverdefined(word, i); return PPRESULT_ERROR; }
	if ( glmode ) FixModuleName( word ); else AddModuleName( word );
	id = lb->Regist( word, LAB_TYPE_COMVAR, 0 );
	if ( glmode ) lb->SetEternal( id );
	//
	wrtbuf->PutStrf( "#usecom %s%s",word, (char *)wp );
	wrtbuf->PutCR();
	//
	return PPRESULT_WROTE_LINE;
}


ppresult_t CToken::PP_Module( void )
{
	//		#module解析
	//
	int res,i,id,fl;
	char *word;
	char tagname[MODNAME_MAX+1];
	//char tmp[0x4000];

	word = (char *)s3; fl = 0;
	i = GetToken();
	if (( i == TK_OBJ )||( i == TK_STRING )) fl=1;
	if ( i == TK_NONE ) { sprintf( word, "M%d", modgc ); modgc++; fl=1; }
	if ( fl == 0 ) { SetError("invalid module name"); return PPRESULT_ERROR; }
	if ( !IsGlobalMode() ) { SetError("not in global mode"); return PPRESULT_ERROR; }
	if ( CheckModuleName( word ) ) {
		SetError("bad module name"); return PPRESULT_ERROR;
	}
	sprintf( tagname, "%.*s", MODNAME_MAX, word );
	res = lb->Search( tagname );
	if ( res != -1 ) { SetErrorSymbolOverdefined(tagname, res); return PPRESULT_ERROR; }
	id = lb->Regist( tagname, LAB_TYPE_PPDLLFUNC, 0 );
	lb->SetEternal( id );
	SetModuleName( tagname );

	wrtbuf->PutStrf( "#module %s",tagname );
	wrtbuf->PutCR();
	wrtbuf->PutStrf( "goto@hsp *_%s_exit",tagname );
	wrtbuf->PutCR();

	if ( PeekToken() != TK_NONE ) {
	  wrtbuf->PutStrf( "#struct %s ",tagname );
	  while(1) {

		i = GetToken();
		if ( i != TK_OBJ ) { SetError("invalid module param"); return PPRESULT_ERROR; }
		AddModuleName( word );
		res = lb->Search(word);
		if ( res != -1 ) { SetErrorSymbolOverdefined(word, res); return PPRESULT_ERROR; }
		id = lb->Regist( word, LAB_TYPE_PPDLLFUNC, 0 );
		wrtbuf->PutStr( "var " );
		wrtbuf->PutStr( word );

		i = GetToken();
		if ( wp == NULL ) break;
		if ( i != ',' ) {
			SetError("invalid module param"); return PPRESULT_ERROR;
		}
		wrtbuf->Put( ',' );
	  }
	  wrtbuf->PutCR();
	}

	return PPRESULT_WROTE_LINES;
}


ppresult_t CToken::PP_Global( void )
{
	//		#global解析
	//
	if ( IsGlobalMode() ) {
#ifdef JPNMSG
		SetError("#module と対応していない #global があります");
#else
		SetError("already in global mode");
#endif
		return PPRESULT_ERROR;
	}
	//
	wrtbuf->PutStrf( "*_%s_exit",GetModuleName() );
	wrtbuf->PutCR();
	wrtbuf->PutStr( "#global" );
	wrtbuf->PutCR();
	SetModuleName( "" );
	return PPRESULT_WROTE_LINES;
}


ppresult_t CToken::PP_Aht( void )
{
	//		#aht解析
	//
	int i;
	char tmp[512];
	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf != NULL ) return PPRESULT_SUCCESS;					// AHT出力時は無視する

	i = GetToken();
	if ( i != TK_OBJ ) {
		SetError("invalid AHT option name"); return PPRESULT_ERROR;
	}
	strcpy2( tmp, (char *)s3, 512 );
	i = GetToken();
	if (( i != TK_STRING )&&( i != TK_NUM )) {
		SetError("invalid AHT option value"); return PPRESULT_ERROR;
	}
	ahtmodel->SetAHTOption( tmp, (char *)s3 );

	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Ahtout( void )
{
	//		#ahtout解析
	//
	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf == NULL ) return PPRESULT_SUCCESS;
	if ( wp == NULL ) return PPRESULT_SUCCESS;

	ahtbuf->PutStr( (char *)wp );
	ahtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Ahtmes( void )
{
	//		#ahtmes解析
	//
	int i;
	int addprm;

	if ( ahtmodel == NULL ) return PPRESULT_SUCCESS;
	if ( ahtbuf == NULL ) return PPRESULT_SUCCESS;
	if ( wp == NULL ) return PPRESULT_SUCCESS;
	addprm = 0;

	while(1) {

		if ( wp == NULL ) break;

		i = GetToken();
		if ( i == TK_NONE ) break;
		if (( i != TK_OBJ )&&( i != TK_NUM )&&( i != TK_STRING )) {
			SetError("illegal ahtmes parameter"); return PPRESULT_ERROR;
		}
		ahtbuf->PutStr( (char *)s3 );

		if ( wp == NULL ) {	addprm = 0; break; }

		i = GetToken();
		if ( i != '+' ) { SetError("invalid ahtmes format"); return PPRESULT_ERROR; }
		addprm++;

	}
	if ( addprm == 0 ) ahtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_Pack( int mode )
{
	//		#pack,#epack解析
	//			(mode:0=normal/1=encrypt)
	int i;
	if ( packbuf!=NULL ) {
		i = GetToken();
		if ( i != TK_STRING ) {
			SetError("invalid pack name"); return PPRESULT_ERROR;
		}
		if (mode & 2) {
			AddPackfile((char*)s3, mode&1);
		}
		else {
			AddPackfileOrig((char*)s3, mode);
		}
	}
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_PackOpt( void )
{
	//		#packopt解析
	//
	int i;
	char tmp[1024];
	char optname[1024];
	if ( packbuf!=NULL ) {
		i = GetToken();
		if ( i != TK_OBJ ) {
			SetError("illegal option name"); return PPRESULT_ERROR;
		}
		strncpy( optname, (char *)s3, 128 );
		i = GetToken();
		if (( i != TK_OBJ )&&( i != TK_NUM )&&( i != TK_STRING )) {
			SetError("illegal option parameter"); return PPRESULT_ERROR;
		}
		sprintf( tmp, ";!%s=%s", optname, (char *)s3 );
		AddPackfile( tmp, 2 );
	}
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_CmpOpt( void )
{
	//		#cmpopt解析
	//
	int i;
	char optname[1024];

	i = GetToken();
	if ( i != TK_OBJ ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}
	strcase2( (char *)s3, optname );

	i = GetToken();
	if ( i != TK_NUM ) {
		SetError("illegal option parameter"); return PPRESULT_ERROR;
	}

	i = 0;
	if (tstrcmp(optname,"ppout")) {			// preprocessor out sw
		i = CMPMODE_PPOUT;
	}
	if (tstrcmp(optname,"optcode")) {		// code optimization sw
		i = CMPMODE_OPTCODE;
	}
	if (tstrcmp(optname,"case")) {			// case sensitive sw
		i = CMPMODE_CASE;
	}
	if (tstrcmp(optname,"optinfo")) {		// optimization info sw
		i = CMPMODE_OPTINFO;
	}
	if (tstrcmp(optname,"varname")) {		// VAR name out sw
		i = CMPMODE_PUTVARS;
	}
	if (tstrcmp(optname,"varinit")) {		// VAR initalize check
		i = CMPMODE_VARINIT;
	}
	if (tstrcmp(optname,"optprm")) {		// parameter optimization sw
		i = CMPMODE_OPTPRM;
	}
	if (tstrcmp(optname,"skipjpspc")) {		// skip Japanese Space Code sw
		i = CMPMODE_SKIPJPSPC;
	}
	if (tstrcmp(optname,"utf8")) {			// UTF-8 string output sw
		i = CMPMODE_UTF8OUT;
	}

	if ( i == 0 ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}

	if ( val ) {
		hed_cmpmode |= i;
	} else {
		hed_cmpmode &= ~i;
	}
	//Alertf("%s(%d)",optname,val);
	//wrtbuf->PutCR();
	return PPRESULT_SUCCESS;
}


ppresult_t CToken::PP_RuntimeOpt( void )
{
	//		#runtime解析
	//
	int i;
	char tmp[1024];

	i = GetToken();
	if ( i != TK_STRING ) {
		SetError("illegal runtime name"); return PPRESULT_ERROR;
	}
	strncpy( hed_runtime, (char *)s3, sizeof hed_runtime );
	hed_runtime[sizeof hed_runtime - 1] = '\0';

	if ( packbuf!=NULL ) {
		sprintf( tmp, ";!runtime=%s.hrt", hed_runtime );
		AddPackfile( tmp, 2 );
	}

	hed_option |= HEDINFO_RUNTIME;

	sprintf(tmp, "\"%s\"", hed_runtime);
	RegistExtMacro("__runtime__", tmp);			// ランタイム名マクロを更新

	return PPRESULT_SUCCESS;
}



ppresult_t CToken::PP_BootOpt(void)
{
	//		#bootopt解析
	//
	int i;
	char optname[1024];

	i = GetToken();
	if (i != TK_OBJ) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}
	strcase2((char *)s3, optname);

	i = GetToken();
	if (i != TK_NUM) {
		SetError("illegal option parameter"); return PPRESULT_ERROR;
	}

	i = 0;
	if (tstrcmp(optname, "notimer")) {			// No MMTimer sw
		i = HEDINFO_NOMMTIMER;
		hed_autoopt_timer = -1;
	}
	if (tstrcmp(optname, "nogdip")) {			// No GDI+ sw
		i = HEDINFO_NOGDIP;
	}
	if (tstrcmp(optname, "float32")) {			// float32 sw
		i = HEDINFO_FLOAT32;
	}
	if (tstrcmp(optname, "orgrnd")) {			// standard random sw
		i = HEDINFO_ORGRND;
	}
	if (tstrcmp(optname, "utf8")) {				// UTF-8 string sw
		i = HEDINFO_UTF8;
	}
	if (tstrcmp(optname, "hsp64")) {			// 64bit runtime sw
		i = HEDINFO_HSP64;
	}
	if (tstrcmp(optname, "ioresume")) {			// File I/O resume sw
		i = HEDINFO_IORESUME;
	}

	if (i == 0) {
		SetError("illegal option name"); return PPRESULT_ERROR;
	}

	if (val) {
		hed_option |= i;
	}
	else {
		hed_option &= ~i;
	}
	return PPRESULT_SUCCESS;
}




void CToken::PreprocessCommentCheck( char *str )
{
	int qmode;
	unsigned char *vs;
	unsigned char a1;
	vs = (unsigned char *)str;
	qmode = 0;

	while(1) {
		a1=*vs++;
		if (a1==0) break;
		if ( qmode == 0 ) {
			if (( a1 == ';' )&&( *vs == ';' )) {
				vs++;
				ahtkeyword = (char *)vs;
			}
		}
		if (a1==0x22) qmode^=1;
		vs += SkipMultiByte( a1 );			// 全角文字チェック
	}
}


ppresult_t CToken::PreprocessNM( char *str )
{
	//		プリプロセスの実行(マクロ展開なし)
	//
	char *word;
	int id,type;
	ppresult_t res;
	char fixname[128];

	word = (char *)s3;
	wp = (unsigned char *)str;
	unsigned char topUChar = *wp;
	*s3 = 0;

	if ( ahtmodel != NULL ) {
		PreprocessCommentCheck( str );
	}

	type = GetToken();
	if ( topUChar != *s3 ) return PPRESULT_UNKNOWN_DIRECTIVE;
	if ( type != TK_OBJ ) return PPRESULT_UNKNOWN_DIRECTIVE;

	//		ソース生成コントロール
	//
	if (tstrcmp(word,"ifdef")) {		// generate control
		if ( mulstr == LMODE_OFF ) {
			res = PP_SwitchStart( 0 );
		} else {
			res = PPRESULT_ERROR; type = GetToken();
			if ( type == TK_OBJ ) {
				strcase2( word, fixname );
				AddModuleName( fixname );
				id = lb->SearchLocal( word, fixname );

				//id = lb->Search( word );
				res = PP_SwitchStart( (id!=-1) );
			}
		}
		return res;
	}
	if (tstrcmp(word,"ifndef")) {		// generate control
		if ( mulstr == LMODE_OFF ) {
			res = PP_SwitchStart( 0 );
		} else {
			res = PPRESULT_ERROR; type = GetToken();
			if ( type == TK_OBJ ) {
				strcase2( word, fixname );
				AddModuleName( fixname );
				id = lb->SearchLocal( word, fixname );

				//id = lb->Search( word );
				res = PP_SwitchStart( (id==-1) );
			}
		}
		return res;
	}
	if (tstrcmp(word,"else")) {			// generate control
		return PP_SwitchReverse();
	}
	if (tstrcmp(word,"endif")) {		// generate control
		return PP_SwitchEnd();
	}

	//		これ以降は#off時に実行しません
	//
	if ( mulstr == LMODE_OFF ) { return PPRESULT_UNKNOWN_DIRECTIVE; }

	if (tstrcmp(word,"define")) {		// keyword define
		return PP_Define();
	}

	if (tstrcmp(word,"undef")) {		// keyword terminate
		if ( GetToken() != TK_OBJ ) {
			SetError("invalid symbol");
			return PPRESULT_ERROR;
		}

		strcase2( word, fixname );
		AddModuleName( fixname );
		id = lb->SearchLocal( word, fixname );

		//id = lb->Search( word );
		if ( id >= 0 ) {
			lb->SetFlag( id, -1 );
		}
		return PPRESULT_SUCCESS;
	}

	return PPRESULT_UNKNOWN_DIRECTIVE;
}


ppresult_t CToken::Preprocess( char *str )
{
	//		プリプロセスの実行
	//
	char *word;
	int type,a;
	ppresult_t res;
	CALCVAR cres;

	word = (char *)s3;
	*s3 = 0;
	wp = (unsigned char *)str;
	unsigned char topUChar = *wp;

	type = GetToken();
	if (topUChar != *s3) {
		type = TK_NONE;
	}
	//		ソース生成コントロール
	//
	if (type == TK_OBJ) {
		if (tstrcmp(word, "if")) {			// generate control
			if (mulstr == LMODE_OFF) {
				res = PP_SwitchStart(0);
			}
			else {
				res = PPRESULT_SUCCESS;
				if (Calc(cres) == 0) {
					a = (int)cres;
					res = PP_SwitchStart(a);
				}
				else res = PPRESULT_ERROR;
			}
			return res;
		}

	}
	//		これ以降は#off時に実行しません
	//
	if (mulstr == LMODE_OFF) { return PPRESULT_SUCCESS; }

	//		コード生成コントロール
	//
	if (type == TK_OBJ) {
		if (tstrcmp(word, "include")) {		// text include
			res = PP_Include(0);
			return res;
		}
		if (tstrcmp(word, "addition")) {		// text include
			res = PP_Include(1);
			return res;
		}
		if (tstrcmp(word, "const")) {		// constant define
			res = PP_Const();
			return res;
		}
		if (tstrcmp(word, "enum")) {			// constant enum define
			res = PP_Enum();
			return res;
		}
		if (tstrcmp(word, "use")) {			// multiple include
			res = PP_use();
			return res;
		}
		if (tstrcmp(word, "module")) {		// module define
			res = PP_Module();
			return res;
		}
		if (tstrcmp(word, "global")) {		// module exit
			res = PP_Global();
			return res;
		}
		if (tstrcmp(word, "deffunc")) {		// module function
			res = PP_Deffunc(0);
			return res;
		}
		if (tstrcmp(word, "defcfunc")) {		// module function (1)
			res = PP_Defcfunc(0);
			return res;
		}
		if (tstrcmp(word, "modfunc")) {		// module function (2)
			res = PP_Deffunc(1);
			return res;
		}
		if (tstrcmp(word, "modcfunc")) {		// module function (2+)
			res = PP_Defcfunc(1);
			return res;
		}
		if (tstrcmp(word, "modinit")) {		// module function (3)
			res = PP_Deffunc(2);
			return res;
		}
		if (tstrcmp(word, "modterm")) {		// module function (4)
			res = PP_Deffunc(3);
			return res;
		}
		if (tstrcmp(word, "struct")) {		// struct define
			res = PP_Struct();
			return res;
		}
		if (tstrcmp(word, "func")) {			// DLL function
			res = PP_Func("func");
			return res;
		}
		if (tstrcmp(word, "cfunc")) {		// DLL function
			res = PP_Func("cfunc");
			return res;
		}
		if (tstrcmp(word, "cmd")) {			// DLL function (3.0)
			res = PP_Cmd("cmd");
			return res;
		}
		if (tstrcmp(word, "comfunc")) {		// COM Object function
			res = PP_Func("comfunc");
			return res;
		}
		if (tstrcmp(word, "aht")) {			// AHT definition
			res = PP_Aht();
			return res;
		}
		if (tstrcmp(word, "ahtout")) {		// AHT command line output
			res = PP_Ahtout();
			return res;
		}
		if (tstrcmp(word, "ahtmes")) {		// AHT command line output (mes)
			res = PP_Ahtmes();
			return res;
		}
		if (tstrcmp(word, "pack")) {			// packfile process
			res = PP_Pack(0);
			return res;
		}
		if (tstrcmp(word, "epack")) {			// packfile process
			res = PP_Pack(1);
			return res;
		}
		if (tstrcmp(word, "packdir")) {			// packfile process
			res = PP_Pack(2);
			return res;
		}
		if (tstrcmp(word, "epackdir")) {		// packfile process
			res = PP_Pack(3);
			return res;
		}
		if (tstrcmp(word, "packopt")) {		// packfile process
			res = PP_PackOpt();
			return res;
		}
		if (tstrcmp(word, "runtime")) {		// runtime process
			res = PP_RuntimeOpt();
			return res;
		}
		if (tstrcmp(word, "bootopt")) {		// boot option process
			res = PP_BootOpt();
			return res;
		}
		if (tstrcmp(word, "cmpopt")) {		// compile option process
			res = PP_CmpOpt();
			return res;
		}
		if (tstrcmp(word, "usecom")) {		// COM definition
			res = PP_Usecom();
			return res;
		}
		if (tstrcmp(word, "var")) {			// VAR definition
			res = PP_VarFix(word);
			return res;
		}
		if (tstrcmp(word, "varint")) {		// VAR definition
			res = PP_VarFix(word);
			return res;
		}
		if (tstrcmp(word, "varlabel")) {	// VAR definition
			res = PP_VarFix(word);
			return res;
		}
		if (tstrcmp(word, "varstr")) {		// VAR definition
			res = PP_VarFix(word);
			return res;
		}
		if (tstrcmp(word, "vardouble")) {	// VAR definition
			res = PP_VarFix(word);
			return res;
		}
		if (tstrcmp(word, "varmod")) {		// VAR definition
			res = PP_VarFix(word);
			return res;
		}
	}
	//		登録キーワード以外はコンパイラに渡す
	//
	wrtbuf->Put( (char)'#' );
	wrtbuf->PutStr( linebuf );
	wrtbuf->PutCR();
	//wrtbuf->PutStr( (char *)s3 );
	return PPRESULT_WROTE_LINE;
}


int CToken::ExpandTokens( char *vp, CMemBuf *buf, int *lineext, int is_preprocess_line )
{
	//		マクロを展開
	//
	*lineext = 0;				// 1行->複数行にマクロ展開されたか?
	int macloop = 0;			// マクロ展開無限ループチェック用カウンタ
	while(1) {
		if ( mulstr == LMODE_OFF ) {				// １行無視
			if ( wrtbuf!=NULL ) wrtbuf->PutCR();	// 行末CR/LFを追加
			break;
		}

		// {"～"}の処理
		//
		if ( mulstr == LMODE_STR ) {
			wrtbuf = buf;
			vp = ExpandStrEx( vp );
			if ( *vp!=0 ) continue;
		}

		// /*～*/の処理
		//
		if ( mulstr == LMODE_COMMENT ) {
			vp = ExpandStrComment( vp, 0 );
			if ( *vp!=0 ) continue;
		}

		char *vp_bak = vp;
		int type;
		vp = ExpandToken( vp, &type, is_preprocess_line );
		if ( type < 0 ) {
			return type;
		}
		if ( type == TK_EOL ) { (*lineext)++; }
		if ( type == TK_EOF ) {
			if ( wrtbuf!=NULL ) wrtbuf->PutCR();	// 行末CR/LFを追加
			break;
		}
		if ( vp_bak == vp ) {
			macloop++;
			if ( macloop > 999 ) {
				SetError("Endless macro loop");
				return -1;
			}
		}
	}
	return 0;
}


int CToken::ExpandLine( CMemBuf *buf, CMemBuf *src, char *refname )
{
	//		stringデータをmembufへ展開する
	//
	char *p = src->GetBuffer();
	int pline = 1;
	enumgc = 0;
	mulstr = LMODE_ON;
	*errtmp = 0;
	unsigned char a1;

	a1 = *(unsigned char*)p;
	if (a1 == 0xef) {		// BOMをチェック
		a1 = (unsigned char)p[1];
		if (a1 == 0xbb) {
			a1 = (unsigned char)p[2];
			if (a1 == 0xbf) {
				if (pp_utf8 == 0) {
#ifdef JPNMSG
					Mesf("#ファイルに予期しない BOM があります [%s]", refname);
#else
					Mesf("#Unexpected BOM in file.[%s]", refname);
#endif
				}
				p += 3;
			}
		}
	}

	while(1) {
		RegistExtMacro( "__line__", pline );			// 行番号マクロを更新
		pp_orgline = pline;

		if (cg_labout_line == pline) {					// 解析用のラインか?
			if (strcmp(cg_labout_orgfile, refname) == 0) {
				//	解析ラインの状態を保存する
				//Alertf( "#Module %s in file[%s] line %d.", modname, refname, pline );
				strcpy(cg_labout_modname, modname);
				if (hed_cmpmode & CMPMODE_CASE) {			// 将来のため
					cg_labout_caseflag = 1;
				}
			}
		}

		while(1) {
			a1 = *(unsigned char *)p;
			if ( a1 == ' ' || a1 == '\t' ) {
				p++; continue;
			}
#ifdef HSPWIN
			if ( hed_cmpmode & CMPMODE_SKIPJPSPC ) {
				if ( a1 == 0x81 && p[1] == 0x40 ) {		// 全角スペースチェック
					p+=2; continue;
				}
			}
#endif
			break;
		}

		if ( *p==0 ) break;					// 終了(EOF)
		ahtkeyword = NULL;					// AHTキーワードをリセットする

		int is_preprocess_line = *p == '#' &&
		                         mulstr != LMODE_STR &&
		                         mulstr != LMODE_COMMENT;

		//		行データをlinebufに展開
		int mline;
		if ( is_preprocess_line ) {
			p = SendLineBufPP( p + 1, &mline );// 行末までを取り出す('\'継続)
			wrtbuf = NULL;
		} else {
			p = SendLineBuf( p );			// 行末までを取り出す
			mline = 0;
			wrtbuf = buf;
		}

//		Mesf("%d:%s", pline, src->GetFileName() );
//		sprintf( mestmp,"%d:%s:%s(%d)", pline, src->GetFileName(), linebuf, is_preprocess_line );
//		Alert( mestmp );
//		buf->PutStr( mestmp );

		//		マクロ展開前に処理されるプリプロセッサ
		if ( is_preprocess_line ) {
			ppresult_t res = PreprocessNM( linebuf );
			if ( res == PPRESULT_ERROR ) {
				LineError( errtmp, pline, refname );
				return 1;
			}
			if ( res == PPRESULT_SUCCESS ) {			// プリプロセッサで処理された時
				mline++;
				pline += mline;
				for (int i = 0; i < mline; i++) {
					buf->PutCR();
				}
				continue;
			}
			assert( res == PPRESULT_UNKNOWN_DIRECTIVE );
		}

//		if ( wrtbuf!=NULL ) {
//			char ss[64];
//			sprintf( ss,"__%d:",pline );
//			wrtbuf->PutStr( ss );
//		}

		//		マクロを展開
		int lineext;			// 1行->複数行にマクロ展開されたか?
		int res = ExpandTokens( linebuf, buf, &lineext, is_preprocess_line );
		if ( res ) {
			LineError( errtmp, pline, refname );
			return res;
		}

		//		プリプロセッサ処理
		if ( is_preprocess_line ) {
			wrtbuf = buf;
			ppresult_t res = Preprocess( linebuf );
			if ( res == PPRESULT_INCLUDED ) {			// include後の処理
				pline += 1+mline;

				char *fname_literal = to_hsp_string_literal( refname, true );
				RegistExtMacro( "__file__", fname_literal );			// ファイル名マクロを更新
				strcpy(pp_orgfile, refname);

				wrtbuf = buf;
				wrtbuf->PutStrf( "##%d %s\r\n", pline-1, fname_literal );
				free( fname_literal );
				continue;
			}
			if ( res == PPRESULT_WROTE_LINES ) {			// プリプロセスで行が増えた後の処理
				pline += mline;
				wrtbuf->PutStrf( "##%d\r\n", pline );
				pline++;
				continue;
			}
			if ( res == PPRESULT_ERROR ) {
				LineError( errtmp, pline, refname );
				return 1;
			}
			pline += 1+mline;
			if ( res != PPRESULT_WROTE_LINE ) mline++;
			for (int i = 0; i < mline; i++) {
				buf->PutCR();
			}
			assert( res == PPRESULT_SUCCESS || res == PPRESULT_WROTE_LINE );
			continue;
		}

		//		マクロ展開後に行数が変わった場合の処理
		pline += 1+mline;
		if ( lineext != mline ) {
			wrtbuf->PutStrf( "##%d\r\n", pline );
		}
	}
	return 0;
}


int CToken::ExpandFile( CMemBuf *buf, char *fname, char *refname )
{
	//		ソースファイルをmembufへ展開する
	//
	int res;
	char cname[HSP_MAX_PATH];
	char purename[HSP_MAX_PATH];
	char foldername[HSP_MAX_PATH];
	char refname_copy[HSP_MAX_PATH];
	char vaild_file[HSP_MAX_PATH];

	char org_filename[HSP_MAX_PATH];
	char org_filenamefull[HSP_MAX_PATH];
	char org_fileline;

	CMemBuf fbuf;

	//	元の情報を保存する
	strcpy(org_filename, pp_orgfile);
	strcpy(org_filenamefull, pp_orgfilefull);
	org_fileline = pp_orgline;

	getpath( fname, purename, 8 );
	getpath( fname, foldername, 32 );
	if ( *foldername != 0 ) strcpy( search_path, foldername );

	strcpy(vaild_file, refname);
	if ( fbuf.PutFile( fname ) < 0 ) {
		strcpy( cname, common_path );strcat( cname, purename );
		strcpy(vaild_file, common_path); strcat(vaild_file, refname);
		if ( fbuf.PutFile( cname ) < 0 ) {
			strcpy( cname, search_path );strcat( cname, purename );
			strcpy(vaild_file, search_path); strcat(vaild_file, refname);
			if ( fbuf.PutFile( cname ) < 0 ) {
				strcpy(cname, common_path); strcat(cname, search_path); strcat(cname, purename);
				strcpy(vaild_file, common_path); strcat(vaild_file, search_path); strcat(vaild_file, refname);
				if ( fbuf.PutFile( cname ) < 0 ) {
					if ( fileadd == 0 ) {
#ifdef JPNMSG
						Mesf( "#スクリプトファイルが見つかりません [%s]", purename );
#else
						Mesf( "#Source file not found.[%s]", purename );
#endif
					}
					return -1;
				}
			}
		}
	}
	fbuf.Put( (char)0 );
	strcpy(pp_orgfilefull, vaild_file);

	if ( fileadd ) {
		Mesf( "#Use file [%s]",purename );
	}

	char *fname_literal = to_hsp_string_literal( refname, true );
	RegistExtMacro( "__file__", fname_literal );			// ファイル名マクロを更新
	strcpy(pp_orgfile, refname);

	fname_literal = to_hsp_string_literal( pp_orgfilefull, true );
	buf->PutStrf( "##0 %s\r\n", fname_literal );
	free( fname_literal );

	strcpy2( refname_copy, refname, sizeof refname_copy );
	res = ExpandLine( buf, &fbuf, refname_copy );

	//	元の情報に復帰させる
	strcpy(pp_orgfile, org_filename );
	strcpy(pp_orgfilefull, org_filenamefull );
	pp_orgline = org_fileline;

	if ( res == 0 ) {
		//		プリプロセス後チェック
		//
		res = tstack->StackCheck( linebuf );
		if ( res ) {
#ifdef JPNMSG
			Mesf( "#スタックが空になっていないマクロタグが%d個あります [%s]", res, refname_copy );
#else
			Mesf( "#%d unresolved macro(s).[%s]", res, refname_copy );
#endif
			Mes( linebuf );
		}
	}
	
	if ( res ) {

#ifdef JPNMSG
		Mes("#重大なエラーが検出されています");
#else
		Mes( "#Fatal error reported." );
#endif
		return -2;
	}
	return 0;
}


int CToken::SetAdditionMode( int mode )
{
	//		Additionによるファイル追加モード設定(1=on/0=off)
	//
	int i;
	i = fileadd;
	fileadd = mode;
	return i;
}


void CToken::SetCommonPath( char *path )
{
	if ( path==NULL ) { common_path[0]=0; return; }
	strcpy( common_path, path );
}


void CToken::FinishPreprocess( CMemBuf *buf )
{
	//	後ろで定義された関数がある場合、それに書き換える
	//
	//	この関数では foo@modname を foo に書き換えるなどバッファサイズが小さくなる変更しか行わない
	//
	int read_pos = 0;
	int write_pos = 0;
	size_t len = undefined_symbols.size();
	char *p = buf->GetBuffer();
	for ( size_t i = 0; i < len; i ++ ) {
		undefined_symbol_t sym = undefined_symbols[i];
		int pos = sym.pos;
		int len_include_modname = sym.len_include_modname;
		int len = sym.len;
		int id;
		memmove( p + write_pos, p + read_pos, pos - read_pos );
		write_pos += pos - read_pos;
		read_pos = pos;
		// @modname を消した名前の関数が存在したらそれに書き換え
		p[pos+len] = '\0';
		id = lb->Search( p + pos );
		if ( id >= 0 && lb->GetType(id) == LAB_TYPE_PPMODFUNC ) {
			memmove( p + write_pos, p + pos, len );
			write_pos += len;
			read_pos += len_include_modname;
		}
		p[pos+len] = '@';
	}
	memmove( p + write_pos, p + read_pos, buf->GetSize() - read_pos );
	buf->ReduceSize( buf->GetSize() - (read_pos - write_pos) );
}


int CToken::LabelRegist( char **list, int mode )
{
	//		ラベル情報を登録
	//
	if ( mode ) {
		return lb->RegistList( list, "@hsp" );
	}
	return lb->RegistList( list, "" );
}


int CToken::LabelRegist2( char **list )
{
	//		ラベル情報を登録(マクロ)
	//
	return lb->RegistList2( list, "@hsp" );
}


int CToken::LabelRegist3( char **list )
{
	//		ラベル情報を登録(色分け用)
	//
	return lb->RegistList3( list );
}


int CToken::RegistExtMacroPath( char *keyword, char *str )
{
	//		マクロを外部から登録(path用)
	//
	int id, res;
	char path[1024];
	char mm[512];
	unsigned char *p;
	unsigned char *src;
	unsigned char a1;

	p = (unsigned char *)path;
	src = (unsigned char *)str;
	while(1) {
		a1 = *src++;
		if ( a1 == 0 ) break;
		if ( a1 == 0x5c ) {	*p++=a1; }		// '\'チェック
		if ( a1>=129 ) {					// 全角文字チェック
			if (a1<=159) { *p++=a1;a1=*src++; }
			else if (a1>=224) { *p++=a1;a1=*src++; }
		}
		*p++ = a1;
	}
	*p = 0;	

	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// すでにある場合は上書き
		lb->SetData( res, path );
		return -1;
	}
	//		データ定義
	id = lb->Regist( mm, LAB_TYPE_PPMAC, 0 );
	lb->SetData( id, path );
	lb->SetEternal( id );
	return 0;
}


int CToken::RegistExtMacro( char *keyword, char *str )
{
	//		マクロを外部から登録
	//
	int id, res;
	char mm[512];
	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// すでにある場合は上書き
		lb->SetData( res, str );
		return -1;
	}
	//		データ定義
	id = lb->Regist( mm, LAB_TYPE_PPMAC, 0 );
	lb->SetData( id, str );
	lb->SetEternal( id );
	return 0;
}


int CToken::RegistExtMacro( char *keyword, int val )
{
	//		マクロを外部から登録(数値)
	//
	int id, res;
	char mm[512];
	strcpy( mm, keyword );
	FixModuleName( mm );
	res = lb->Search( mm );if ( res != -1 ) {	// すでにある場合は上書き
		lb->SetOpt( res, val );
		return -1;
	}
	//		データ定義
	id = lb->Regist( mm, LAB_TYPE_PPVAL, val );
	lb->SetEternal( id );
	return 0;
}


int CToken::LabelDump(CMemBuf* out, int option, char *match)
{
	//		登録されているラベル情報をerrbufに展開
	//
	int a,max;
	max = lb->GetCount();
	for (a = 0; a < max; a++) {
		lb->DumpHSPLabelById(a, linebuf, option);
		if (match) {
			//	matchが含まれていない場合は無視する
			if (strstr2(linebuf, match) == NULL) *linebuf = 0;
		}
		if (*linebuf!=0) out->PutStr(linebuf);
	}
	return 0;
}


void CToken::SetModuleName( char *name )
{
	//		モジュール名を設定
	//
	if ( *name==0 ) {
		modname[0] = 0; return;
	}
	sprintf( modname, "@%.*s", MODNAME_MAX, name );
	strcase( modname+1 );
}


char *CToken::GetModuleName( void )
{
	//		モジュール名を取得
	//
	if ( *modname == 0 ) return modname;
	return modname+1;
}


void CToken::AddModuleName( char *str )
{
	//		キーワードにモジュール名を付加(モジュール依存ラベル用)
	//
	unsigned char a1;
	unsigned char *wp;
	wp=(unsigned char *)str;
	while(1) {
		a1=*wp;
		if (a1==0) break;
		if (a1=='@') {
			a1=wp[1];
			if (a1==0) *wp=0;
			return;
		}
		if (a1>=129) wp++;
		wp++;
	}
	if ( *modname==0 ) return;
	strcpy( (char *)wp, modname );
}


void CToken::FixModuleName( char *str )
{
	//		キーワードのモジュール名を正規化(モジュール非依存ラベル用)
	//
//	char *wp;
//	wp = str + ( strlen(str)-1 );
//	if ( *wp=='@' ) *wp=0;

	unsigned char a1;
	unsigned char *wp;
	wp=(unsigned char *)str;
	while(1) {
		a1=*wp;
		if (a1==0) break;
		if (a1=='@') {
			a1=wp[1];
			if (a1==0) *wp=0;
			return;
		}
		if (a1>=129) wp++;
		wp++;
	}
	
}


int CToken::IsGlobalMode( void )
{
	//		モジュール内(0)か、グローバル(1)かを返す
	//
	if ( *modname==0 ) return 1;
	return 0;
}


int CToken::GetLabelBufferSize( void )
{
	//		ラベルバッファサイズを得る
	//
	return lb->GetSymbolSize();
}


void CToken::InitSCNV( int size )
{
	//		文字コード変換の初期化
	//		(size<0の場合はメモリを破棄)
	//
	if ( scnvbuf != NULL ) {
		free( scnvbuf );
		scnvbuf = NULL;
	}
	if ( size <= 0 ) return;
	scnvbuf = (char *)malloc(size);
	scnvsize = size;
}


char *CToken::ExecSCNV( char *srcbuf, int opt )
{
	//		文字コード変換
	//
	//int ressize;
	int size;

	if ( scnvbuf == NULL ) InitSCNV( SCNVBUF_DEFAULTSIZE );

	size = (int)strlen( srcbuf );
	switch( opt ) {
	case SCNV_OPT_NONE:
		strcpy( scnvbuf, srcbuf );
		break;
	case SCNV_OPT_SJISUTF8:
#ifdef HSPWIN
		ConvSJis2Utf8( srcbuf, scnvbuf, scnvsize );
#else
		strcpy( scnvbuf, srcbuf );
#endif
		break;
	case SCNV_OPT_UTF8SJIS:
#ifdef HSPWIN
		ConvUtf82SJis(srcbuf, scnvbuf, scnvsize);
#else
		strcpy(scnvbuf, srcbuf);
#endif
		break;
	default:
		*scnvbuf = 0;
		break;
	}

	return scnvbuf;
}

void CToken::SetErrorSymbolOverdefined(char* keyword, int label_id)
{
	// 識別子の多重定義エラー

	char strtmp[0x100];
#ifdef JPNMSG
	sprintf( strtmp,"定義済みの識別子は使用できません [%s]", keyword );
#else
	sprintf( strtmp,"symbol in use [%s]", keyword );
#endif
	SetError(strtmp);
}


int CToken::CheckByteSJIS( unsigned char c )
{
	//	SJISの全角1バイト目を判定する
	//  (戻り値は以降に続くbyte数)
	if (((c>=0x81)&&(c<=0x9f))||((c>=0xe0)&&(c<=0xfc))) return 1;
	return 0;
}


int CToken::CheckByteUTF8( unsigned char c )
{
	//	UTF8の全角1バイト目を判定する
	//  (戻り値は以降に続くbyte数)

	if ( c <= 0x7f ) return 0;

	if ((c >= 0xc2) && (c <= 0xdf)) return 1;
	if ((c >= 0xe0) && (c <= 0xef)) return 2;
	if ((c >= 0xf0) && (c <= 0xf7)) return 3;
	if ((c >= 0xf8) && (c <= 0xfb)) return 4;
	if ((c >= 0xfc) && (c <= 0xfd)) return 5;
	return 0;
}


int CToken::SkipMultiByte( unsigned char byte )
{
	//	マルチバイトコードの2byte目以降をスキップする
	//  ( 1バイト目のcharを渡すと、2byte目以降スキップするbyte数を返す )
	//	( pp_utf8のフラグによってUTF-8とSJISを判断する )
	//
	if ( pp_utf8 ) {
		return CheckByteUTF8( byte );
	}
	return CheckByteSJIS( byte );
}


int CToken::ConvSJis2Utf8(char* pSource, char* pDist, int buffersize)
{
	int size = 0;
	if (pDist == NULL) return -1;

#ifdef HSPWIN
	//ShiftJISからUTF-16へ変換
	const int nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSource, -1, NULL, 0);

	BYTE* buffUtf16 = new BYTE[nSize * 2 + 2];
	::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize);

	//UTF-16からUTF-8へ変換
	size = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL);
	size *= 2;
	if (size > buffersize) size = buffersize;
	ZeroMemory(pDist, size);
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)pDist, size, NULL, NULL);

	size = lstrlen((char*)pDist) + 1;

	delete [] buffUtf16;
#endif
	return size;
}


int CToken::ConvUtf82SJis(char* pSource, char* pDist, int buffersize)
{
	int size = 0;

#ifdef HSPWIN

	// サイズを計算する
	int iLenUnicode = ::MultiByteToWideChar(CP_UTF8, 0, pSource, strlen(pSource) + 1, NULL, 0);
	BYTE* buffUtf16 = new BYTE[iLenUnicode * 2 + 2];

	::MultiByteToWideChar(CP_UTF8, 0, pSource, strlen(pSource) + 1, (LPWSTR)buffUtf16, iLenUnicode);

	size = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffUtf16, iLenUnicode, NULL, 0, NULL, NULL);
	if (size > buffersize) size = buffersize;
	::WideCharToMultiByte(CP_ACP, 0,
				(LPCWSTR)buffUtf16, iLenUnicode,
				pDist, size,
				NULL, NULL);

	delete [] buffUtf16;
	pDist[size] = 0;
#endif
	return size;
}


char* CToken::to_hsp_string_literal(const char* src, bool filename) {
	//		文字列をHSPの文字列リテラル形式に
	//		戻り値のメモリは呼び出し側がfreeする必要がある。
	//		HSPの文字列リテラルで表せない文字は
	//		そのまま出力されるので注意。（'\n'など）
	//		ファイル名の場合はSJISと仮定して処理する(Winのみ)
	//
	int skip;
	char* utftmp;
	size_t length = 2;

	utftmp = (char *)src;
#ifdef HSPWIN
	if (filename) {
		if (pp_utf8) {			// 入力がUTF8でファイル名の場合は変換する
			int len = strlen(src) * 4 + 1;
			utftmp = (char*)malloc(len);
			ConvSJis2Utf8((char*)src, utftmp, len);
		}
	}
#endif
	const unsigned char* s = (unsigned char*)utftmp;
	while (1) {
		unsigned char c = *s;
		if (c == '\0') break;
		switch (c) {
		case '\r':
			if (*(s + 1) == '\n') {
				s++;
			}
			// FALL THROUGH
		case '\t':
		case '"':
		case '\\':
			length += 2;
			break;
		default:
			length++;
			skip = SkipMultiByte(c);
			s += skip;
			length+=skip;
			break;
		}
		s++;
	}
	char* dest = (char*)malloc(length + 1);
	if (dest == NULL) return dest;

	s = (unsigned char*)utftmp;
	unsigned char* d = (unsigned char*)dest;
	*d++ = '"';
	while (1) {
		unsigned char c = *s;
		if (c == '\0') break;
		switch (c) {
		case '\t':
			*d++ = '\\';
			*d++ = 't';
			break;
		case '\r':
			*d++ = '\\';
			if (*(s + 1) == '\n') {
				*d++ = 'n';
				s++;
			}
			else {
				*d++ = 'r';
			}
			break;
		case '"':
			*d++ = '\\';
			*d++ = '"';
			break;
		case '\\':
			*d++ = '\\';
			*d++ = '\\';
			break;
		default:
			skip = SkipMultiByte(c);
			*d++ = c;
			while (skip>0) {
				s++;
				*d++ = *s;
				skip--;
			}
		}
		s++;
	}
	*d++ = '"';
	*d = '\0';
#ifdef HSPWIN
	if (filename) {
		if (pp_utf8) {
			free(utftmp);
		}
	}
#endif
	return dest;
}

int CToken::atoi_allow_overflow(const char* s)
{
	//		オーバーフローチェックをしないatoi
	//
	int result = 0;
	while (isdigit(*s)) {
		result = result * 10 + (*s - '0');
		s++;
	}
	return result;
}


void CToken::GenerateLabelListAndTagPP(char *name, int flag)
{
	if (labbuf == NULL) return;
	GenerateLabelTag(name, flag, 0, pp_orgfilefull, pp_orgline);
}


void CToken::GenerateLabelListAndTagRefPP(char* name, int flag)
{
	if (labbuf == NULL) return;
	if ((cg_labout_mode & LABLIST_MODE_REFERENCE) == 0) return;
	GenerateLabelTag(name, flag| LABBUF_FLAG_REFER, 0, pp_orgfilefull, pp_orgline);
}


char* CToken::GetLabelListLineModule(void)
{
	if (labbuf == NULL) return "";
	return cg_labout_modname;
}


int CToken::GetLabelListLineCaseFlag(void)
{
	if (labbuf == NULL) return 0;
	return cg_labout_caseflag;
}

