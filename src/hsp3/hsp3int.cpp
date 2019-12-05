
//
//	HSP3 internal command
//	(内蔵コマンド・関数処理)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <limits>
#include <limits.h>

#include "hsp3config.h"

#ifdef HSPRANDMT
#include <random>
#endif

#ifdef HSPWIN
#include <windows.h>
#include <direct.h>
#endif

#include "hspwnd.h"
#include "supio.h"
#include "dpmread.h"
#include "strbuf.h"
#include "strnote.h"

#include "hsp3int.h"
#include "hsp3code.h"

static const double DBLINF = std::numeric_limits<double>::infinity();

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static int *type;
static int *val;
static HSPCTX *ctx;			// Current Context
static HSPEXINFO *exinfo;	// Info for Plugins
static CStrNote note;
#ifdef HSPRANDMT
static std::mt19937 mt;
#endif



/*------------------------------------------------------------*/
/*
		Sort Routines
*/
/*------------------------------------------------------------*/

static int qsort_order;

typedef struct {
	union {
		int ikey;
		double dkey;
		char *skey;
	} as;
	int info; // ソートの前にこの要素があった位置
} DATA;

static bool less_int_1(DATA const& lhs, DATA const& rhs)
{
	int cmp = (lhs.as.ikey - rhs.as.ikey);
	return (cmp < 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}

static bool less_int_0(DATA const& lhs, DATA const& rhs)
{
	int cmp = (lhs.as.ikey - rhs.as.ikey);
	return (cmp > 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}

static bool less_double_1(DATA const& lhs, DATA const& rhs)
{
	int cmp = (lhs.as.dkey < rhs.as.dkey ? -1 : (lhs.as.dkey > rhs.as.dkey ? 1 : 0));
	return (cmp < 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}

static bool less_double_0(DATA const& lhs, DATA const& rhs)
{
	int cmp = (lhs.as.dkey < rhs.as.dkey ? -1 : (lhs.as.dkey > rhs.as.dkey ? 1 : 0));
	return (cmp > 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}

static bool less_str_1(DATA const& lhs, DATA const& rhs)
{
	int cmp = (strcmp(lhs.as.skey, rhs.as.skey));
	return (cmp < 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}

static bool less_str_0(DATA const& lhs, DATA const& rhs)
{
	int cmp = (strcmp(lhs.as.skey, rhs.as.skey));
	return (cmp > 0)
		|| (cmp == 0 && lhs.info < rhs.info);
}


static int NoteToData( char *adr, DATA *data )
{
	char *p = adr;
	int line = 0;
	while (*p != '\0') {
		data[line].as.skey=p;
		data[line].info=line;
		while (*p != '\0') {
			char c = *p;
			if (c == '\n' || c == '\r') {
				*p = '\0';
			}
			p ++;
			if (c == '\n') break;
			if (c == '\r') {
				if (*p == '\n') p++;
				break;
			}
		}
		line ++;
	}
	return line;
}


static int GetNoteLines( char *adr )
{
	int line = 0;
	char *p = adr;
	while (*p != '\0') {
		while (*p != '\0') {
			char c = *p++;
			if (c == '\n') break;
			if (c == '\r') {
				if (*p == '\n') p ++;
				break;
			}
		}
		line ++;
	}
	return line;
}


static size_t DataToNoteLen( DATA *data, int num )
{
	size_t len = 0;
	int i;
	for (i = 0; i < num; i++) {
		char *s = data[i].as.skey;
		len += strlen(s) + 2;	// strlen("\r\n")
	}
	return len;
}


static void DataToNote( DATA *data, char *adr, int num )
{
	int a;
	char *p;
	char *s;
	p=adr;
	for(a=0;a<num;a++) {
		s=data[a].as.skey;
		strcpy( p, s );
		p+=strlen( s );
		*p++=13; *p++=10;			// Add CR/LF
	}
	*p=0;
}


/*------------------------------------------------------------*/
/*
		Sort Interface
*/
/*------------------------------------------------------------*/

static	DATA *dtmp = NULL;
static	int dtmp_size;

static void DataBye( void )
{
	if (dtmp!=NULL) {
		mem_bye(dtmp);
	}
}

static void DataIni( int size )
{
	DataBye();
	dtmp=(DATA *)mem_ini( sizeof(DATA)*size );
	dtmp_size = size;
}

static void DataExpand( int size )
{
	if (size <= dtmp_size) return;
	int new_size = dtmp_size;
	if (new_size < 16) new_size = 16;
	while (size > new_size) {
		new_size *= 2;
	}
	dtmp = (DATA *)realloc( dtmp, sizeof(DATA)*new_size );
	memset( dtmp + dtmp_size, 0, sizeof(DATA)*(new_size - dtmp_size) );
	dtmp_size = new_size;
}


static void DataInc( int n )
{
	DataExpand( n + 1 );
	dtmp[n].info ++;
}


/*------------------------------------------------------------*/
/*
		Easing Function
*/
/*------------------------------------------------------------*/

static int ease_type;
static int ease_reverse;
static HSPREAL ease_start;
static HSPREAL ease_diff;
static HSPREAL ease_4096;
static HSPREAL ease_start_org;
static HSPREAL ease_diff_org;

#define EASE_LINEAR 0
#define EASE_QUAD_IN 1
#define EASE_QUAD_OUT 2
#define EASE_QUAD_INOUT 3
#define EASE_CUBIC_IN 4
#define EASE_CUBIC_OUT 5
#define EASE_CUBIC_INOUT 6
#define EASE_QUARTIC_IN 7
#define EASE_QUARTIC_OUT 8
#define EASE_QUARTIC_INOUT 9
#define EASE_BOUNCE_IN 10
#define EASE_BOUNCE_OUT 11
#define EASE_BOUNCE_INOUT 12
#define EASE_SHAKE_IN 13
#define EASE_SHAKE_OUT 14
#define EASE_SHAKE_INOUT 15
#define EASE_LOOP 4096

/*------------------------------------------------------------*/

static HSPREAL _ease_linear( HSPREAL t )
{
	return ease_diff * t + ease_start;
}

static HSPREAL _ease_quad_in( HSPREAL t )
{
	return ease_diff * t * t + ease_start;
}

static HSPREAL _ease_quad_out( HSPREAL t )
{
	return -ease_diff * t * (t - 2) + ease_start;
}

static HSPREAL _ease_quad_inout( HSPREAL t )
{
	HSPREAL tt;
	tt = t * 2;
	if ( tt < 1 ) {
		return ease_diff * 0.5 * tt * tt + ease_start;
	}
	tt = tt - 1;
	return -ease_diff * 0.5 * (tt * (tt - 2) - 1) + ease_start;
}

static HSPREAL _ease_cubic_in( HSPREAL t )
{
	return ease_diff * t * t * t + ease_start;
}

static HSPREAL _ease_cubic_out( HSPREAL t )
{
	HSPREAL tt;
	tt = t - 1;
	return ease_diff*(tt*tt*tt + 1) + ease_start;
}

static HSPREAL _ease_cubic_inout( HSPREAL t )
{
	HSPREAL tt;
	tt = t * 2;
	if ( tt < 1 ) {
		return ease_diff * 0.5 *tt*tt*tt + ease_start;
	}
	tt = tt - 2;
	return ease_diff * 0.5 * (tt*tt*tt + 2) + ease_start;
}

static HSPREAL _ease_quartic_in( HSPREAL t )
{
	return ease_diff * t * t * t * t + ease_start;
}

static HSPREAL _ease_quartic_out( HSPREAL t )
{
	HSPREAL tt;
	tt = t - 1;
	return -ease_diff*(tt*tt*tt*tt - 1) + ease_start;
}

static HSPREAL _ease_quartic_inout( HSPREAL t )
{
	HSPREAL tt;
	tt = t * 2;
	if ( tt < 1 ) {
		return ease_diff * 0.5 *tt*tt*tt*tt + ease_start;
	}
	tt = tt - 2;
	return -ease_diff * 0.5 * (tt*tt*tt*tt - 2) + ease_start;
}

static HSPREAL _ease_bounce( HSPREAL t )
{
	if ( t < (1/2.75)) {
		return ease_diff*(7.5625*t*t);
	} else if (t < (2/2.75)) {
		return ease_diff*(7.5625*(t-=(1.5/2.75))*t + .75);
	} else if (t < (2.5/2.75)) {
		return ease_diff*(7.5625*(t-=(2.25/2.75))*t + .9375);
	} else {
		return ease_diff*(7.5625*(t-=(2.625/2.75))*t + .984375);
	}
}

static HSPREAL _ease_bounce_in( HSPREAL t )
{
	HSPREAL tt;
	tt = (HSPREAL)1 - t;
	return ease_diff - _ease_bounce( tt ) + ease_start;
}

static HSPREAL _ease_bounce_out( HSPREAL t )
{
	return _ease_bounce(t) + ease_start;
}

static HSPREAL _ease_bounce_inout( HSPREAL t )
{
	HSPREAL tt;
	if (t < 0.5) {
		tt = (HSPREAL)1 - (t * 2);
		return ( ease_diff - _ease_bounce( tt )) * 0.5 + ease_start;
	}
	return _ease_bounce( t*2 - 1 ) * 0.5 + ease_diff*0.5 + ease_start;
}

static HSPREAL _ease_shake( HSPREAL t )
{
	int pulse;
	HSPREAL tt;
	tt = t * t * 8;
	pulse = (int)tt;
	tt -= (HSPREAL)pulse;
	if ( pulse & 1 ) {
		return ( (HSPREAL)1 - tt );
	}
	return tt;
}
static HSPREAL _ease_shake_in( HSPREAL t )
{
	HSPREAL tt;
	tt = (HSPREAL)1 - t;
	return ( ease_diff * _ease_shake(tt) ) * tt - ease_diff * 0.5 * tt + ease_start;
}

static HSPREAL _ease_shake_out( HSPREAL t )
{
	return ( ease_diff * _ease_shake(t) ) * t - ease_diff * 0.5 * t + ease_start;
}

static HSPREAL _ease_shake_inout( HSPREAL t )
{
	HSPREAL tt;
	tt = t * 2;
	if ( tt < 1 ) {
		return _ease_shake_in( tt );
	}
	tt = tt - 1;
	return _ease_shake_out( tt );
}

/*------------------------------------------------------------*/

static void initEase( void )
{
	ease_4096 = (HSPREAL)1.0 / (HSPREAL)4096.0;
}

static void setEase( int type, HSPREAL value_start, HSPREAL value_end )
{
	ease_type = type;
	ease_reverse = 0;
	ease_start_org = ease_start = value_start;
	ease_diff_org = ease_diff = value_end - value_start;
}

static HSPREAL getEase( HSPREAL value )
{
	int type;
	int reverse;
	HSPREAL t;
	t = value;
	type = ease_type & ( EASE_LOOP - 1 );
	reverse = 0;
	if ( ease_type & EASE_LOOP ) {
		int ival;
        HSPREAL dval;
        t = modf( t, &dval );
		ival = (int)dval;
		reverse = ival & 1;
	} else {
		if ( t < 0 ) t = (HSPREAL)0;
		if ( t > 1 ) t = (HSPREAL)1;
	}

	if ( ease_reverse != reverse ) {
		ease_reverse = reverse;			// リバース時の動作
		if ( ease_reverse ) {
			ease_start = ease_start_org + ease_diff_org;
			ease_diff = -ease_diff_org;
		} else {
			ease_start = ease_start_org;
			ease_diff = ease_diff_org;
		}
	}

	switch( type ) {
	case EASE_QUAD_IN:
		return _ease_quad_in( t );
	case EASE_QUAD_OUT:
		return _ease_quad_out( t );
	case EASE_QUAD_INOUT:
		return _ease_quad_inout( t );

	case EASE_CUBIC_IN:
		return _ease_cubic_in( t );
	case EASE_CUBIC_OUT:
		return _ease_cubic_out( t );
	case EASE_CUBIC_INOUT:
		return _ease_cubic_inout( t );

	case EASE_QUARTIC_IN:
		return _ease_quartic_in( t );
	case EASE_QUARTIC_OUT:
		return _ease_quartic_out( t );
	case EASE_QUARTIC_INOUT:
		return _ease_quartic_inout( t );

	case EASE_BOUNCE_IN:
		return _ease_bounce_in( t );
	case EASE_BOUNCE_OUT:
		return _ease_bounce_out( t );
	case EASE_BOUNCE_INOUT:
		return _ease_bounce_inout( t );

	case EASE_SHAKE_IN:
		return _ease_shake_in( t );
	case EASE_SHAKE_OUT:
		return _ease_shake_out( t );
	case EASE_SHAKE_INOUT:
		return _ease_shake_inout( t );

	case EASE_LINEAR:
	default:
		break;
	}
	return _ease_linear( t );
}

static HSPREAL getEase( HSPREAL value, HSPREAL maxvalue )
{
	if ( maxvalue == 0 ) return (HSPREAL)0;
	return getEase( value / maxvalue );
}

static int getEaseInt( int i_value, int i_maxvalue )
{
	int i;
	HSPREAL value;
	if ( i_maxvalue > 0 ) {
		value = (HSPREAL)i_value / (HSPREAL)i_maxvalue;
	} else {
		value = ease_4096 * i_value;
	}
	i = (int)getEase(value);
	return i;
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static char *note_update( void )
{
	char *p;
	if ( ctx->note_pval == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
	p = (char *)HspVarCorePtrAPTR( ctx->note_pval, ctx->note_aptr );
	note.Select( p );
	return p;
}

// デストラクタで自動的に sbFree を呼ぶ
class CAutoSbFree {
public:
	CAutoSbFree(char **pptr);
	~CAutoSbFree();
	
private:
	// uncopyable;
	CAutoSbFree( CAutoSbFree const & );
	CAutoSbFree const & operator =( CAutoSbFree const & );

private:
	char **pptr_;
};

CAutoSbFree::CAutoSbFree(char **pptr)
 : pptr_(pptr)
{}

CAutoSbFree::~CAutoSbFree() {
	sbFree(*pptr_);
}

static void cnvformat_expand( char **p, int *capacity, int len, int n )
{
	int needed_size = len + n + 1;
	int capa = *capacity;
	if ( needed_size > capa ) {
		while ( needed_size > capa ) {
			capa *= 2;
		}
		*p = sbExpand( *p, capa );
		*capacity = capa;
	}
}

static char *cnvformat( void )
{
	//		フォーマット付き文字列を作成する
	//
#if ( WIN32 || _WIN32 ) && ! __CYGWIN__
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

	char fstr[1024];
	char *fp;
	int capacity;
	int len;
	char *p;
	
	strncpy( fstr, code_gets(), sizeof fstr );
	fstr[sizeof(fstr)-1] = '\0';
	fp = fstr;
	capacity = 1024;
	p = sbAlloc(capacity);
	len = 0;
	
	CAutoSbFree autofree(&p);
	
	while (1) {
		char fmt[32];
		int i;
		int val_type;
		void *val_ptr;

		// '%' までをコピー
		i = 0;
		while( fp[i] != '\0' && fp[i] != '%' ) {
			i ++;
		}
		cnvformat_expand( &p, &capacity, len, i );
		memcpy( p + len, fp, i );
		len += i;
		fp += i;
		if ( *fp == '\0' ) break;

		// 変換指定を読み fmt にコピー
		i = (int)strspn( fp + 1, " #+-.0123456789" ) + 1;
		strncpy( fmt, fp, sizeof fmt );
		fmt[sizeof(fmt)-1] = '\0';
		if ( i + 1 < (int)(sizeof fmt) ) fmt[i+1] = '\0';
		fp += i;

		char specifier = *fp;
		fp ++;

#if ( WIN32 || _WIN32 ) && ! __CYGWIN__
		if ( specifier == 'I' ) {				// I64 prefix対応(VC++のみ)
			if ((fp[0]=='6')&&(fp[1]='4')) {
				memcpy( fmt+i+1, fp, 3 );
				fmt[i+4] = 0;
				specifier = 'f';
				fp += 3;
			}
		}
#endif

		if ( specifier == '\0' ) break;
		if ( specifier == '%' ) {
			cnvformat_expand( &p, &capacity, len, 1 );
			p[len++] = '%';
			continue;
		}

		// 引数を取得
		if ( code_get() <= PARAM_END ) throw HSPERR_INVALID_FUNCPARAM;
		switch (specifier) {
		case 'd': case 'i': case 'c': case 'o': case 'x': case 'X': case 'u': case 'p':
			val_type = HSPVAR_FLAG_INT;
			val_ptr = HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_INT );
			break;
		case 'f': case 'e': case 'E': case 'g': case 'G':
			val_type = HSPVAR_FLAG_DOUBLE;
			val_ptr = HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_DOUBLE );
			break;
		case 's':
			val_type = HSPVAR_FLAG_STR;
			val_ptr = HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_STR );
			break;
		default:
			throw HSPERR_INVALID_FUNCPARAM;
		}

		// snprintf が成功するまでバッファを広げていき、変換を行う
		while (1) {
			int n;
			int space = capacity - len - 1;
			if ( val_type == HSPVAR_FLAG_INT ) {
				n = SNPRINTF( p + len, space, fmt, *(int *)val_ptr );
			} else if ( val_type == HSPVAR_FLAG_DOUBLE ) {
				n = SNPRINTF( p + len, space, fmt, *(HSPREAL *)val_ptr );
			} else {
				n = SNPRINTF( p + len, space, fmt, (char *)val_ptr );
			}

			if ( n >= 0 && n < space ) {
				len += n;
				break;
			}
			if ( n >= 0 ) {
				space = n + 1;
			} else {
				space *= 2;
				if ( space < 32 ) space = 32;
			}
			cnvformat_expand( &p, &capacity, len, space );
		}
	}
	p[len] = '\0';
	
	char *result = code_stmp(len + 1);
	strcpy(result, p);
	return result;
}


static void var_set_str_len( PVal *pval, APTR aptr, char *str, int len )
{
	//		変数にstrからlenバイトの文字列を代入する
	//
	HspVarProc *proc = HspVarCoreGetProc( HSPVAR_FLAG_STR );
	if ( pval->flag != HSPVAR_FLAG_STR ) {
		if ( aptr != 0 ) throw HSPERR_INVALID_ARRAYSTORE;
		HspVarCoreClear( pval, HSPVAR_FLAG_STR );
	}
	pval->offset = aptr;
	HspVarCoreAllocBlock( pval, proc->GetPtr( pval ), len + 1 );
	char *ptr = (char *)proc->GetPtr( pval );
	memcpy( ptr, str, len );
	ptr[len] = '\0';
}



static int cmdfunc_intcmd( int cmd )
{
	//		cmdfunc : TYPE_INTCMD
	//		(内蔵コマンド)
	//
	int p1,p2,p3;
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)

	switch( cmd ) {							// サブコマンドごとの分岐

	case 0x00:								// onexit
	case 0x01:								// onerror
	case 0x02:								// onkey
	case 0x03:								// onclick
	case 0x04:								// oncmd
		{

/*
	rev 45
	不具合 : (onxxx系命令) (ラベル型変数)  形式の書式でエラー
	に対処
*/

		int tval = *type;
		int opt = IRQ_OPT_GOTO;
		int cust;
		int actid;
		IRQDAT *irq;
		unsigned short *sbr;

		if ( tval == TYPE_VAR ) {
			if ( ( ctx->mem_var + *val )->flag == HSPVAR_FLAG_LABEL )
				tval = TYPE_LABEL;
		}

		if (( tval != TYPE_PROGCMD )&&( tval != TYPE_LABEL )) {		// ON/OFF切り替え
			int i = code_geti();
			code_enableirq( cmd, i );
			break;
		}

		if ( tval == TYPE_PROGCMD ) {	// ジャンプ方法指定
			opt = *val;
			if ( opt >= 2 ) throw HSPERR_SYNTAX;
			code_next();
		}

		sbr = code_getlb2();
		if ( cmd != 0x04 ) {
			code_setirq( cmd, opt, -1, sbr );
			break;
		}
		cust = code_geti();
		actid = *(exinfo->actscr);
		irq = code_seekirq( actid, cust );
		if ( irq == NULL ) irq = code_addirq();
		irq->flag = IRQ_FLAG_ENABLE;
		irq->opt = opt;
		irq->ptr = sbr;
		irq->custom = cust;
		irq->custom2 = actid;
		break;
		}

	case 0x11:								// exist
	case 0x12:								// delete
	case 0x13:								// mkdir
	case 0x14:								// chdir
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		code_event( HSPEVENT_FEXIST + (cmd - 0x11), 0, 0, NULL );
		break;

	case 0x15:								// dirlist
		{
		PVal *pval;
		APTR aptr;
		char *ptr;
		aptr = code_getva( &pval );
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		p1=code_getdi(0);
		code_event( HSPEVENT_FDIRLIST1, p1, 0, &ptr );
		code_setva( pval, aptr, TYPE_STRING, ptr );
		code_event( HSPEVENT_FDIRLIST2, 0, 0, NULL );
		break;
		}
	case 0x16:								// bload
	case 0x17:								// bsave
		{
		PVal *pval;
		char *ptr;
		int size;
		int tmpsize;
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		ptr = code_getvptr( &pval, &size );
		p1 = code_getdi( -1 );
		p2 = code_getdi( -1 );
		if (( p1 < 0 )||( p1 > size )) p1 = size;
		if ( cmd == 0x16 ) {
			tmpsize = p2;if ( tmpsize<0 ) tmpsize = 0;
			code_event( HSPEVENT_FREAD, tmpsize, p1, ptr );
		} else {
			code_event( HSPEVENT_FWRITE, p2, p1, ptr );
		}
		break;
		}
	case 0x18:								// bcopy
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		code_event( HSPEVENT_FCOPY, 0, 0, code_gets() );
		break;
	case 0x19:								// memfile
		{
		PVal *pval;
		char *ptr;
		int size;
		ptr = code_getvptr( &pval, &size );
		p1=code_getdi( 0 );
		p2=code_getdi( 0 );
		if ( p2==0 ) p2 = size - p1;
		dpm_memfile( ptr+p1, p2 );
		break;
		}

	case 0x1a:								// poke
	case 0x1b:								// wpoke
	case 0x1c:								// lpoke
		{
		PVal *pval;
		char *ptr;
		int size;
		int fl;
		int len;
		char *bp;
		ptr = code_getvptr( &pval, &size );
		p1 = code_getdi( 0 );
		if ( p1<0 ) throw HSPERR_BUFFER_OVERFLOW;
		ptr += p1;

		if ( code_get() <= PARAM_END ) {
			fl = HSPVAR_FLAG_INT;
			bp = (char *)&p2; p2 = 0;
		} else {
			fl = mpval->flag;
			bp = mpval->pt;
		}

		if ( cmd == 0x1a ) {
			switch( fl ) {
			case HSPVAR_FLAG_INT:
				if ( p1 >= size ) throw HSPERR_BUFFER_OVERFLOW;
				*ptr = *bp;
				break;
			case HSPVAR_FLAG_STR:
				len = (int)strlen( bp );
				ctx->strsize = len;
				len++;
				if ( (p1+len)>size ) throw HSPERR_BUFFER_OVERFLOW;
				strcpy( ptr, bp );
				break;
			default:
				throw HSPERR_TYPE_MISMATCH;
			}
			break;
		}

		if ( fl != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		if ( cmd == 0x1b ) {
			if ( (p1+2)>size ) throw HSPERR_BUFFER_OVERFLOW;
			*(short *)ptr = (short)(*(short *)bp);
		} else {
			if ( (p1+4)>size ) throw HSPERR_BUFFER_OVERFLOW;
			*(int *)ptr = (*(int *)bp);
		}
		break;
		}

	case 0x1d:								// getstr
		{
		PVal *pval2;
		PVal *pval;
		APTR aptr;
		char *ptr;
		char *p;
		int size;
		aptr = code_getva( &pval );
		ptr = code_getvptr( &pval2, &size );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 1024 );
		if ( p1 >= size ) throw HSPERR_BUFFER_OVERFLOW;
		ptr += p1;
		p = code_stmp( p3 + 1 );
		strsp_ini();
		ctx->stat = strsp_get( ptr, p, p2, p3 );
		ctx->strsize = strsp_getptr();
		code_setva( pval, aptr, HSPVAR_FLAG_STR, p );
		break;
		}
	case 0x1e:								// chdpm
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		p1 = code_getdi( -1 );
		dpm_bye();
		p2 = dpm_ini( ctx->fnbuffer, 0, -1, p1 );
		if ( p2 ) throw HSPERR_FILE_IO;
#ifndef HSP3IMP
#ifdef HSPWIN
		Sleep( 1000 );
#endif
#endif
		break;
	case 0x1f:								// memexpand
		{
		PVal *pval;
		APTR aptr;
		PDAT *ptr;
		aptr = code_getva( &pval );
		ptr = HspVarCorePtrAPTR( pval, aptr );
		if (( pval->support & HSPVAR_SUPPORT_FLEXSTORAGE ) == 0 ) throw HSPERR_TYPE_MISMATCH;
		p1 = code_getdi( 0 );
		if ( p1 < 64 ) p1 = 64;
		HspVarCoreAllocBlock( pval, ptr, p1 );
		break;
		}

	case 0x20:								// memcpy
		{
		PVal *pval;
		char *sptr;
		char *tptr;
		int bufsize_t,bufsize_s;

		tptr = code_getvptr( &pval, &bufsize_t );
		sptr = code_getvptr( &pval, &bufsize_s );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		if( p2 < 0 || p3 < 0 ) throw HSPERR_BUFFER_OVERFLOW;

		tptr += p2;
		sptr += p3;
		if ( (p1+p2)>bufsize_t ) throw HSPERR_BUFFER_OVERFLOW;
		if ( (p1+p3)>bufsize_s ) throw HSPERR_BUFFER_OVERFLOW;
		if ( p1>0 ) {
			memmove( tptr, sptr, p1 );
		}
		break;
		}
	case 0x21:								// memset
		{
		PVal *pval;
		char *ptr;
		int size;
		ptr = code_getvptr( &pval, &size );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		if ( p3 < 0 ) throw HSPERR_BUFFER_OVERFLOW;
		ptr += p3;
		if ( (p3+p2)>size ) throw HSPERR_BUFFER_OVERFLOW;
		if ( p2>0 ) {
			memset( ptr, p1, p2 );
		}
		break;
		}

	case 0x22:								// notesel
		ctx->notep_aptr = ctx->note_aptr;
		ctx->notep_pval = ctx->note_pval;
		ctx->note_aptr = code_getva( &ctx->note_pval );
		if ( ctx->note_pval->flag != HSPVAR_FLAG_STR ) {
			code_setva( ctx->note_pval, ctx->note_aptr, TYPE_STRING, "" );
		}
		break;
	case 0x23:								// noteadd
		{
		char *np;
		char *ps;
		char *tmp;
		int size;
		np = note_update();
		ps = code_gets();
		size = (int)strlen( ps ) + 8;
		HspVarCoreAllocBlock( ctx->note_pval, (PDAT *)np, (int)strlen(np) + size );

		tmp = code_stmpstr( ps );

		p1 = code_getdi( -1 );
		p2 = code_getdi( 0 );
		np = note_update();
		note.PutLine( tmp, p1, p2 );
		break;
		}
	case 0x24:								// notedel
		p1 = code_getdi( 0 );
		note_update();
		note.PutLine( NULL, p1, 1 );
		break;
	case 0x25:								// noteload
		{
		int size;
		char *ptr;
		char *pdat;

		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		p1 = code_getdi( -1 );
		code_event( HSPEVENT_FEXIST, 0, 0, NULL );
		size = ctx->strsize;
		if ( size < 0 ) throw HSPERR_FILE_IO;
		if ( p1>=0 ) if ( size >= p1 ) { ctx->strsize = size = p1; }

		pdat = note_update();
		HspVarCoreAllocBlock( ctx->note_pval, (PDAT *)pdat, size+1 );
		ptr = (char *)note_update();
		code_event( HSPEVENT_FREAD, 0, size, ptr );
		ptr[size] = 0;
		break;
		}
	case 0x26:								// notesave
		{
		char *pdat;
		int size;
		code_event( HSPEVENT_FNAME, 0, 0, code_gets() );
		pdat = note_update();
		size = (int)strlen( pdat );
		code_event( HSPEVENT_FWRITE, -1, size, pdat );
		break;
		}
	case 0x27:								// randomize
#ifdef HSPWIN
		p2 = (int)GetTickCount();	// Windowsの場合はtickをシード値とする
#else
		p2 = (int)time(0);			// Windows以外のランダムシード値
#endif
		p1 = code_getdi( p2 );
#ifdef HSPRANDMT
		mt.seed( p1 );
#else
		srand( p1 );
#endif
		break;
	case 0x28:								// noteunsel
		ctx->note_aptr = ctx->notep_aptr;
		ctx->note_pval = ctx->notep_pval;
		break;
	case 0x29:								// noteget
		{
		PVal *pval;
		APTR aptr;
		char *p;
		note_update();
		aptr = code_getva( &pval );
		p1 = code_getdi( 0 );
		p = note.GetLineDirect( p1 );
		code_setva( pval, aptr, TYPE_STRING, p );
		note.ResumeLineDirect();
		break;
		}
	case 0x2a:								// split
		{
		//	指定した文字列で分割された要素を代入する(fujidig)
		PVal *pval = NULL;
		int aptr = 0;
		char *sptr;
		char *sep;
		char *newsptr;
		int size;
		int sep_len;
		int n = 0;
		int is_last = 0;
		
		sptr = code_getvptr( &pval, &size );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		sep = code_gets();
		sep_len = (int)strlen( sep );
		
		while (1) {
			newsptr = strstr2( sptr, sep );
			if ( !is_last && *exinfo->npexflg & EXFLG_1 ) {
				// 分割結果の数が格納する変数より多ければ最後の変数に配列で格納していく
				// ただし最後の要素が a.2 のように要素指定があればそれ以降は全く格納しない
				if ( aptr != 0 ) pval = NULL;
				is_last = 1;
				aptr = 0;
			}
			if ( is_last ) {
				aptr ++;
				if ( pval != NULL && aptr >= pval->len[1] ) {
					if ( pval->len[2] != 0 ) throw HSPVAR_ERROR_ARRAYOVER;
					HspVarCoreReDim( pval, 1, aptr+1 );
				}
			} else {
				aptr = code_getva( &pval );
			}
			if ( pval != NULL ) {
				if ( newsptr == NULL ) {
					code_setva( pval, aptr, HSPVAR_FLAG_STR, sptr );
				} else {
					var_set_str_len( pval, aptr, sptr, (int)(newsptr - sptr) );
				}
			}
			n ++;
			if ( newsptr == NULL ) {
				// 格納する変数の数が分割できた数より多ければ残った変数それぞれに空文字列を格納する
				while( ( *exinfo->npexflg & EXFLG_1 ) == 0 ) {
					aptr = code_getva( &pval );
					code_setva( pval, aptr, HSPVAR_FLAG_STR, "" );
				}
				break;
			}
			sptr = newsptr + sep_len;
		}
		ctx->stat = n;
		break;
		}

	case 0x02b:								// strrep
		{
		PVal *pval;
		APTR aptr;
		char *ss;
		char *s_rep;
		char *s_buffer;
		char *s_match;
		char *s_result;
		int len_match;
		int len_result;
		int len_buffer;

		aptr = code_getva( &pval );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		s_buffer = (char *)HspVarCorePtrAPTR( pval, aptr );

		ss = code_gets();
		if ( *ss == 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		len_match = (int)strlen( ss );
		s_match = sbAlloc( len_match + 1 );
		memcpy( s_match, ss, len_match + 1 );

		len_buffer = (int)strlen( s_buffer );
		len_result = len_buffer + 0x4000;
		if ( len_result < 0x8000 ) len_result = 0x8000;
		s_result = sbAlloc( len_result );
		*s_result = 0;

		s_rep = code_gets();

		ReplaceSetMatch( s_buffer, s_match, s_result, len_buffer, len_match, len_result );
		ReplaceStr( s_rep );

		code_setva( pval, aptr, TYPE_STRING, s_result );
		ctx->stat = ReplaceDone();
		sbFree( s_match );
		sbFree( s_result );
		break;
		}

	case 0x02c:								// setease
		{
		HSPREAL dval;
		HSPREAL dval2;
		dval = code_getd();
		dval2 = code_getd();
		p1 = code_getdi( ease_type );
		setEase( p1, dval, dval2 );
		break;
		}

	case 0x02d:								// sortval
		{
		int a,i;
		PVal *p1;
		APTR ap;
		int order;

		ap = code_getva( &p1 );		// パラメータ1:変数
		order = code_getdi( 0 );	// パラメータ2:数値

		i=p1->len[1];
		if (i<=0) throw HSPERR_ILLEGAL_FUNCTION;
		switch(p1->flag) {
			case HSPVAR_FLAG_DOUBLE:
			{
			double *dp;
			dp=(double *)p1->pt;
			DataIni( i );
			for(a=0;a<i;a++) {
				dtmp[a].as.dkey=dp[a];
				dtmp[a].info=a;
			}
			if (order == 0) {
				std::sort(dtmp, dtmp + i, less_double_1);
			}
			else {
				std::sort(dtmp, dtmp + i, less_double_0);
			}
			for(a=0;a<i;a++) {
				code_setva( p1, a, HSPVAR_FLAG_DOUBLE, &(dtmp[a].as.dkey) );	// 変数に値を代入
			}
			break;
			}
		case HSPVAR_FLAG_INT:
			{
			int *p;
			p=(int *)p1->pt;
			DataIni( i );
			for(a=0;a<i;a++) {
				dtmp[a].as.ikey=p[a];
				dtmp[a].info=a;
			}
			if (order == 0) {
				std::sort(dtmp, dtmp + i, less_int_1);
			}
			else {
				std::sort(dtmp, dtmp + i, less_int_0);
			}
			for(a=0;a<i;a++) {
				p[a]=dtmp[a].as.ikey;
			}
			break;
			}
		default:
			throw HSPERR_ILLEGAL_FUNCTION;
		}
		break;
		}

	case 0x02e:								// sortstr
		{
		int i,len,order;
		char *p;
		PVal *pv;
		APTR ap;
		HspVarProc *proc;
		char **pvstr;

		ap = code_getva( &pv );		// パラメータ1:変数
		order = code_getdi( 0 );	// パラメータ2:数値

		if ( pv->flag != 2 ) throw HSPERR_TYPE_MISMATCH;
		if (( pv->len[2] != 0 )||( ap != 0 )) throw HSPERR_ILLEGAL_FUNCTION;

		proc = HspVarCoreGetProc( pv->flag );

		len = pv->len[1];
		DataIni( len );

		for(i=0;i<len;i++) {
			p = (char *)HspVarCorePtrAPTR( pv, i );
			dtmp[i].as.skey = p;
			dtmp[i].info = i;
		}

		if (order == 0) {
			std::sort(dtmp, dtmp + i, less_str_1);
		}
		else {
			std::sort(dtmp, dtmp + i, less_str_0);
		}

		pvstr = (char **)(pv->master);	// 変数に直接sbポインタを書き戻す
		for(i=0;i<len;i++) {
			if ( i == 0 ) {
				pv->pt = dtmp[i].as.skey;
				sbSetOption(pv->pt, &pv->pt);
			} else {
				pvstr[i] = dtmp[i].as.skey;
				sbSetOption(pvstr[i], &pvstr[i]);
			}
		}
		break;
		}

	case 0x02f:								// sortnote
		{
		int i,sflag;
		char *p;
		char *stmp;
		PVal *pv;
		APTR ap;

		ap = code_getva( &pv );		// パラメータ1:変数
		p = (char*)HspVarCorePtrAPTR(pv, ap);

		sflag = code_getdi( 0 );	// パラメータ2:数値

		i = GetNoteLines(p);
		if ( i <= 0 ) throw HSPERR_ILLEGAL_FUNCTION;

		DataIni( i );

		NoteToData( p, dtmp );
		if (sflag == 0) {
			std::sort(dtmp, dtmp + i, less_str_1);
		}
		else {
			std::sort(dtmp, dtmp + i, less_str_0);
		}

		stmp = code_stmp( (int)DataToNoteLen( dtmp, i ) + 1 );
		DataToNote( dtmp, stmp, i );

		code_setva( pv, ap, HSPVAR_FLAG_STR, stmp );	// 変数に値を代入

		break;
		}

	case 0x030:								// sortget
		{
		PVal *pv;
		APTR ap;
		int result;
		int n;

		ap = code_getva( &pv );
		n = code_getdi( 0 );

		if ( dtmp == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		if (0 <= n && n < dtmp_size ) {
			result=dtmp[n].info;
		} else {
			result=0;
		}
		code_setva( pv, ap, HSPVAR_FLAG_INT, &result );
		break;
		}

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}

static int reffunc_intfunc_ivalue;
static HSPREAL reffunc_intfunc_value;

static void *reffunc_intfunc( int *type_res, int arg )
{
	//		reffunc : TYPE_INTFUNC
	//		(内蔵関数)
	//
	void *ptr;
	int chk;
	HSPREAL dval;
	HSPREAL dval2;
	int ival;
	char *sval;
	int p1,p2,p3;

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != '(' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	//		返値のタイプをargをもとに設定する
	//		0～255   : int
	//		256～383 : string
	//		384～511 : double(HSPREAL)
	//
	switch( arg>>7 ) {
		case 2:										// 返値がstr
			*type_res = HSPVAR_FLAG_STR;			// 返値のタイプを指定する
			ptr = NULL;								// 返値のポインタ
			break;
		case 3:										// 返値がdouble
			*type_res = HSPVAR_FLAG_DOUBLE;			// 返値のタイプを指定する
			ptr = &reffunc_intfunc_value;			// 返値のポインタ
			break;
		default:									// 返値がint
			*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
			ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ
			break;
	}

	switch( arg ) {

	//	int function
	case 0x000:								// int
		{
		int *ip;
		chk = code_get();
		if ( chk <= PARAM_END ) { throw HSPERR_INVALID_FUNCPARAM; }
		ip = (int *)HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_INT );
		reffunc_intfunc_ivalue = *ip;
		break;
		}
	case 0x001:								// rnd
		ival = code_geti();
		if ( ival == 0 ) throw HSPERR_DIVIDED_BY_ZERO;
#ifdef HSPRANDMT
		{
			std::uniform_int_distribution<int> dist( 0, ival - 1 );
			reffunc_intfunc_ivalue = dist( mt );
		}
#else
		reffunc_intfunc_ivalue = rand()%ival;
#endif
		break;
	case 0x002:								// strlen
		sval = code_gets();
		reffunc_intfunc_ivalue = (int) STRLEN( sval );
		break;

	case 0x003:								// length(3.0)
	case 0x004:								// length2(3.0)
	case 0x005:								// length3(3.0)
	case 0x006:								// length4(3.0)
		{
		PVal *pv;
		pv = code_getpval();
		reffunc_intfunc_ivalue = pv->len[ arg - 0x002 ];
		break;
		}

	case 0x007:								// vartype(3.0)
		{
		PVal *pv;
		HspVarProc *proc;
		if ( *type == TYPE_STRING ) {
			sval = code_gets();
			proc = HspVarCoreSeekProc( sval );
			if ( proc == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
			reffunc_intfunc_ivalue = proc->flag;
		} else {
			code_getva( &pv );
			reffunc_intfunc_ivalue = pv->flag;
		}
		break;
		}

	case 0x008:								// gettime
		ival = code_geti();
		reffunc_intfunc_ivalue = gettime( ival );
		break;

	case 0x009:								// peek
	case 0x00a:								// wpeek
	case 0x00b:								// lpeek
		{
		PVal *pval;
		char *ptr;
		int size;
		ptr = code_getvptr( &pval, &size );
		p1 = code_getdi( 0 );
		if ( p1<0 ) throw HSPERR_ILLEGAL_FUNCTION;
		ptr += p1;
		if ( arg == 0x09 ) {
			if ( (p1+1)>size ) throw HSPERR_ILLEGAL_FUNCTION;
			reffunc_intfunc_ivalue = ((int)(*ptr)) & 0xff;
		} else if ( arg == 0x0a ) {
			if ( (p1+2)>size ) throw HSPERR_ILLEGAL_FUNCTION;
			reffunc_intfunc_ivalue = ((int)(*(short *)ptr)) & 0xffff;
		} else {
			if ( (p1+4)>size ) throw HSPERR_ILLEGAL_FUNCTION;
			reffunc_intfunc_ivalue = *(int *)ptr;
		}
		break;
		}
	case 0x00c:								// varptr
		{
		PVal *pval;
		APTR aptr;
		PDAT *pdat;
		STRUCTDAT *st;
		if ( *type == TYPE_DLLFUNC ) {
			st = &(ctx->mem_finfo[ *val ]);
			reffunc_intfunc_ivalue = (int)(size_t)(st->proc);
			code_next();
			break;
		}
		aptr = code_getva( &pval );
		pdat = HspVarCorePtrAPTR( pval, aptr );
		reffunc_intfunc_ivalue = (int)(size_t)(pdat);
		HspVarCoreGetBlockSize(pval, pdat, &ctx->strsize);
		break;
		}
	case 0x00d:								// varuse
		{
		PVal *pval;
		APTR aptr;
		PDAT *pdat;
		aptr = code_getva( &pval );
		if ( pval->support & HSPVAR_SUPPORT_VARUSE ) {
			pdat = HspVarCorePtrAPTR( pval, aptr );
			reffunc_intfunc_ivalue = HspVarCoreGetUsing( pval, pdat );
		} else throw HSPERR_TYPE_MISMATCH;
		break;
		}
	case 0x00e:								// noteinfo
		ival = code_getdi(0);
		note_update();
		switch( ival ) {
		case 0:
			reffunc_intfunc_ivalue = note.GetMaxLine();
			break;
		case 1:
			reffunc_intfunc_ivalue = note.GetSize();
			break;
		default:
			throw HSPERR_ILLEGAL_FUNCTION;
		}
		break;

	case 0x00f:								// instr
		{
		PVal *pval;
		char *ptr;
		char *ps;
		char *ps2;
		int size;
		int p1;
		ptr = code_getvptr( &pval, &size );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		p1 = code_getdi(0);
		if ( p1 >= size ) throw HSPERR_BUFFER_OVERFLOW;
		ps = code_gets();
		if ( p1 >= 0 ) {
			ptr += p1;
			ps2 = strstr2( ptr, ps );
		} else {
			ps2 = NULL;
		}
		if ( ps2 == NULL ) {
			reffunc_intfunc_ivalue = -1;
		} else {
			reffunc_intfunc_ivalue = (int)(ps2 - ptr);
		}
		break;
		}

	case 0x010:								// abs
		reffunc_intfunc_ivalue = code_geti();
		if ( reffunc_intfunc_ivalue < 0 ) reffunc_intfunc_ivalue = -reffunc_intfunc_ivalue;
		break;

	case 0x011:								// limit
		p1 = code_geti();
		p2 = code_getdi(INT_MIN);
		p3 = code_getdi(INT_MAX);
		reffunc_intfunc_ivalue = GetLimit( p1, p2, p3 );
		break;

	case 0x012:								// getease
		p1 = code_geti();
		p2 = code_getdi(-1);
		reffunc_intfunc_ivalue = getEaseInt( p1, p2 );
		break;

	case 0x013:								// notefind
		{
		char *ps;
		char *p;
		int findopt;
		ps = code_gets();
		p = code_stmpstr( ps );
		findopt = code_getdi(0);
		note_update();
		reffunc_intfunc_ivalue = note.FindLine( p, findopt );
		break;
		}

	case 0x014:								// varsize
		{
		PVal *pval;
		APTR aptr;
		PDAT *pdat;
		STRUCTDAT *st;
		if ( *type == TYPE_DLLFUNC ) {
			st = &(ctx->mem_finfo[ *val ]);
			reffunc_intfunc_ivalue = (int)(st->size);
			code_next();
			break;
		}
		aptr = code_getva( &pval );
		pdat = HspVarCorePtrAPTR( pval, aptr );
		HspVarCoreGetBlockSize(pval, pdat, &reffunc_intfunc_ivalue);
		break;
		}


	// str function
	case 0x100:								// str
		{
		char *sp;
		chk = code_get();
		if ( chk <= PARAM_END ) { throw HSPERR_INVALID_FUNCPARAM; }
		sp = (char *)HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_STR );
		ptr = (void *)sp;
		break;
		}
	case 0x101:								// strmid
		{
		PVal *pval;
		char *sptr;
		char *p;
		char chrtmp;
		int size;
		int i;
		int slen;
		sptr = code_getvptr( &pval, &size );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		p1 = code_geti();
		p2 = code_geti();

		slen=(int)strlen( sptr );
		if ( p1 < 0 ) {
			p1=slen - p2;
			if ( p1 < 0 ) p1 = 0;
		}
		if ( p1 >= slen )
			p2 = 0;
		if ( p2 > slen ) p2 = slen;
		sptr += p1;
		ptr = p = code_stmp( p2 + 1 );
		for(i=0;i<p2;i++) {
			chrtmp = *sptr++;
			*p++ = chrtmp;
			if (chrtmp==0) break;
		}
		*p = 0;
		break;
		}

	case 0x103:								// strf
		ptr = cnvformat();
		break;
	case 0x104:								// getpath
		{
		char *p;
		char pathname[HSP_MAX_PATH];
#if defined(HSPWIN)&&defined(HSPUTF8)
		HSPAPICHAR *hactmp1 = 0;
		HSPAPICHAR pw[HSP_MAX_PATH];
		HSPCHAR *hctmp1 = 0;
		p = ctx->stmp;
		strncpy( pathname, code_gets(), HSP_MAX_PATH-1 );
		p1=code_geti();
		getpathW( chartoapichar(pathname,&hactmp1), pw, p1 );
		freehac(&hactmp1);
		apichartohspchar(pw, &hctmp1);
		strncpy(p, hctmp1, HSP_MAX_PATH - 1);
		freehc(&hctmp1);
#else
		p = ctx->stmp;
		strncpy( pathname, code_gets(), HSP_MAX_PATH-1 );
		p1=code_geti();
		getpath( pathname, p, p1 );
#endif
		ptr = p;
		break;
		}
	case 0x105:								// strtrim
		{
		PVal *pval;
		char *sptr;
		char *p;
		int size;
		sptr = code_getvptr( &pval, &size );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		p1 = code_getdi(0);
		p2 = code_getdi(32);
		ptr = p = code_stmp( size + 1 );
		strcpy( p, sptr );
		switch( p1 ) {
		case 0:
			TrimCodeL( p, p2 );
			TrimCodeR( p, p2 );
			break;
		case 1:
			TrimCodeL( p, p2 );
			break;
		case 2:
			TrimCodeR( p, p2 );
			break;
		case 3:
			TrimCode( p, p2 );
			break;
		}
		break;
		}

	//	double function
	case 0x180:								// sin
		dval = code_getd();
		reffunc_intfunc_value = sin( dval );
		break;
	case 0x181:								// cos
		dval = code_getd();
		reffunc_intfunc_value = cos( dval );
		break;
	case 0x182:								// tan
		dval = code_getd();
		reffunc_intfunc_value = tan( dval );
		break;
	case 0x183:								// atan
		dval = code_getd();
		dval2 = code_getdd( 1.0 );
		reffunc_intfunc_value = atan2( dval, dval2 );
		break;
	case 0x184:								// sqrt
		dval = code_getd();
		reffunc_intfunc_value = sqrt( dval );
		break;
	case 0x185:								// double
		{
		HSPREAL *dp;
		chk = code_get();
		if ( chk <= PARAM_END ) { throw HSPERR_INVALID_FUNCPARAM; }
		dp = (HSPREAL *)HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_DOUBLE );
		reffunc_intfunc_value = *dp;
		break;
		}
	case 0x186:								// absf
		reffunc_intfunc_value = code_getd();
		if ( reffunc_intfunc_value < 0 ) reffunc_intfunc_value = -reffunc_intfunc_value;
		break;
	case 0x187:								// expf
		dval = code_getd();
		reffunc_intfunc_value = exp( dval );
		break;
	case 0x188:								// logf
		dval = code_getd();
		reffunc_intfunc_value = log( dval );
		break;
	case 0x189:								// limitf
		{
		HSPREAL d1,d2,d3;
		d1 = code_getd();
		d2 = code_getdd(-DBLINF);
		d3 = code_getdd(DBLINF);
		if ( d1 < d2 ) d1 = d2;
		if ( d1 > d3 ) d1 = d3;
		reffunc_intfunc_value = d1;
		break;
		}
	case 0x18a:								// powf
		dval = code_getd();
		dval2 = code_getd();
		reffunc_intfunc_value = pow( dval, dval2 );
		break;
	case 0x18b:								// geteasef
		dval = code_getd();
		dval2 = code_getdd(1.0);
		if ( dval2 == 1.0 ) {
			reffunc_intfunc_value = getEase( dval );
		} else {
			reffunc_intfunc_value = getEase( dval, dval2 );
		}
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	return ptr;
}




/*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

static int termfunc_intcmd( int option )
{
	//		termfunc : TYPE_INTCMD
	//		(内蔵)
	//
	return 0;
}

void hsp3typeinit_intcmd( HSP3TYPEINFO *info )
{
	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	initEase();

	info->cmdfunc = cmdfunc_intcmd;
	info->termfunc = termfunc_intcmd;
}

void hsp3typeinit_intfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_intfunc;
}


