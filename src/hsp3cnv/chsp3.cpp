
//
//			HSP3 manager class
//			onion software/onitama 2008/4
//
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "supio.h"
#include "chsp3.h"

#define DELCLASS(o)	if (o) { delete (o); (o)=NULL; }

/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

#include "../hspcmp/hspcmd.cpp"

/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

CHsp3::CHsp3( void )
{
	buf = new CMemBuf;
	out = new CMemBuf;
	out2 = new CMemBuf;
	lb = new CLabel;
	lb->RegistList( hsp_prestr, "" );

	mem_cs = NULL;
	mem_ds = NULL;
	mem_ot = NULL;
	dinfo = NULL;
	linfo = NULL;
	finfo = NULL;
	finfo2 = NULL;
	minfo = NULL;
	hpidat = NULL;
	ot_info = NULL;
}


CHsp3::~CHsp3( void )
{
	DeleteObjectHeader();
	delete lb;
	delete out2;
	delete out;
	delete buf;
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void CHsp3::DeleteObjectHeader( void )
{
	//		HSPヘッダー情報を破棄する
	//
	DELCLASS( mem_cs );
	DELCLASS( mem_ds );
	DELCLASS( mem_ot );
	DELCLASS( dinfo );
	DELCLASS( linfo );
	DELCLASS( finfo );
	DELCLASS( finfo2 );
	DELCLASS( minfo );
	DELCLASS( hpidat );
	DELCLASS( ot_info );
}


void CHsp3::NewObjectHeader( void )
{
	//		新規のHSPヘッダー情報を作成する
	//
	hsphed = (HSPHED *)buf->GetBuffer();
	mem_cs = new CMemBuf( 0x1000 );
	mem_ds = new CMemBuf( 0x1000 );
	mem_ot = new CMemBuf( 0x1000 );
	dinfo = new CMemBuf( 0x1000 );
	linfo = new CMemBuf( 0x1000 );
	finfo = new CMemBuf( 0x1000 );
	finfo2 = new CMemBuf( 0x1000 );
	minfo = new CMemBuf( 0x1000 );
	hpidat = new CMemBuf( 0x1000 );
	orgname[0]=0;
}


int CHsp3::LoadObjectFile( char *fname )
{
	//		axファイルをロードする
	//
	char *baseptr;

	if ( buf->PutFile( fname ) < 0 ) return -1;

	strcpy( orgname, fname );
	hsphed = (HSPHED *)buf->GetBuffer();
	baseptr = (char *)buf->GetBuffer();

	if ((hsphed->h1!='H')||(hsphed->h2!='S')||(hsphed->h3!='P')||(hsphed->h4!='3')) {
		return -2;
	}
	//if ( hsphed->version < vercode ) {
	//	return -3;
	//}

	mem_cs = new CMemBuf( 0x1000 );
	mem_cs->PutData( baseptr+hsphed->pt_cs, hsphed->max_cs );
	mem_ds = new CMemBuf( 0x1000 );
	mem_ds->PutData( baseptr+hsphed->pt_ds, hsphed->max_ds );
	mem_ot = new CMemBuf( 0x1000 );
	mem_ot->PutData( baseptr+hsphed->pt_ot, hsphed->max_ot );
	dinfo = new CMemBuf( 0x1000 );
	dinfo->PutData( baseptr+hsphed->pt_dinfo, hsphed->max_dinfo );
	linfo = new CMemBuf( 0x1000 );
	linfo->PutData( baseptr+hsphed->pt_linfo, hsphed->max_linfo );
	finfo = new CMemBuf( 0x1000 );
	finfo->PutData( baseptr+hsphed->pt_finfo, hsphed->max_finfo );
	finfo2 = new CMemBuf( 0x1000 );
	finfo2->PutData( baseptr+hsphed->pt_finfo2, hsphed->max_finfo2 );
	minfo = new CMemBuf( 0x1000 );
	minfo->PutData( baseptr+hsphed->pt_minfo, hsphed->max_minfo );
	hpidat = new CMemBuf( 0x1000 );
	hpidat->PutData( baseptr+hsphed->pt_hpidat, hsphed->max_hpi );

	UpdateValueName();				// 変数名のデバッグ情報を検索する
	MakeOTInfo();					// OT情報を検索する

	//	トレース用の初期設定
	//
	int i;
	initCS( mem_cs->GetBuffer() );
	mcs_start = ( unsigned short * )( mem_cs->GetBuffer() );
	mcs_end = ( unsigned short * )( mem_cs->GetBuffer() + hsphed->max_cs );
	getCS();
	exflag = 0;
	iflevel = 0;
	for( i=0;i<MAX_IFLEVEL;i++ ) { ifmode[i]=0; ifptr[i]=0; }
	SetIndent( 0 );

	return 0;
}


void CHsp3::GetContext( MCSCONTEXT *ctx )
{
	//		コンテキストを取得
	//		(ctxに書き込む)
	//
	ctx->mcs = mcs;
	ctx->mcs_last = mcs_last;
	ctx->cstype = cstype;
	ctx->csval = csval;
	ctx->exflag = exflag;
}


void CHsp3::SetContext( MCSCONTEXT *ctx )
{
	//		コンテキストを設定
	//		(ctxの内容を設定)
	//
	mcs = ctx->mcs;
	mcs_last = ctx->mcs_last;
	cstype = ctx->cstype;
	csval = ctx->csval;
	exflag = ctx->exflag;
}


/*------------------------------------------------------------*/

LIBDAT *CHsp3::GetLInfo( int index )
{
	//		linfoから値を取得する
	//
	LIBDAT *baseptr;
	baseptr = (LIBDAT *)linfo->GetBuffer();
	baseptr += index;
	return baseptr;
}


STRUCTDAT *CHsp3::GetFInfo( int index )
{
	//		finfoから値を取得する
	//
	STRUCTDAT *baseptr;
	baseptr = (STRUCTDAT *)finfo->GetBuffer();
	baseptr += index;
	return baseptr;
}

STRUCTPRM *CHsp3::GetMInfo( int index )
{
	//		minfoから値を取得する
	//
	STRUCTPRM *baseptr;
	baseptr = (STRUCTPRM *)minfo->GetBuffer();
	baseptr += index;
	return baseptr;
}


int CHsp3::GetOTCount( void )
{
	//		OTのindex量を得る
	//
	return ( mem_ot->GetSize() / sizeof( int ) );
}


int CHsp3::GetLInfoCount( void )
{
	//		linfoのindex量を得る
	//
	return ( linfo->GetSize() / sizeof( LIBDAT ) );
}

int CHsp3::GetFInfoCount( void )
{
	//		finfoのindex量を得る
	//
	return ( finfo->GetSize() / sizeof( STRUCTDAT ) );
}


int CHsp3::GetMInfoCount( void )
{
	//		minfoのindex量を得る
	//
	return ( minfo->GetSize() / sizeof( STRUCTPRM ) );
}

/*------------------------------------------------------------*/

int CHsp3::SaveOutBuf( char *fname )
{
	//		出力バッファをセーブする
	//
	return ( out->SaveFile( fname ) );
}


int CHsp3::SaveOutBuf2( char *fname )
{
	//		出力バッファ2をセーブする
	//
	return ( out2->SaveFile( fname ) );
}


char *CHsp3::GetOutBuf( void )
{
	//		出力バッファの先頭ポインタを得る
	//
	return ( out->GetBuffer() );
}


void CHsp3::MakeObjectInfo( void )
{
	//		読み込んだオブジェクトファイルの情報を出力(結果は出力バッファに)
	//
	char mes[512];

	sprintf( mes,"Original File : %s\n", orgname );
	out->PutStr( mes );
	sprintf( mes,"Code Segment Size : %d\n", mem_cs->GetSize() );
	out->PutStr( mes );
	sprintf( mes,"Data Segment Size : %d\n", mem_ds->GetSize() );
	out->PutStr( mes );
	sprintf( mes,"Object Temp Size : %d\n", mem_ot->GetSize() );
	out->PutStr( mes );
	sprintf( mes,"LibInfo(%d) FInfo(%d) MInfo(%d)\n", GetLInfoCount(), GetFInfoCount(), GetMInfoCount() );
	out->PutStr( mes );

	out->PutStr( "--------------------------------------------\n" );

	{	//	LInfoの表示(DLLインポート情報)
		int i,max;
		LIBDAT *lib;
		max = GetLInfoCount();
		for(i=0;i<max;i++) {
			lib = GetLInfo(i);
			sprintf( mes,"LInfo#%d : flag=%x name=%s clsid=%x\n", i, lib->flag, GetDS(lib->nameidx), lib->clsid );
			out->PutStr( mes );
		}
	}

	{	//	FInfoの表示(拡張命令情報)
		int i,max;
		STRUCTDAT *fnc;
		char *p;
		max = GetFInfoCount();
		for(i=0;i<max;i++) {
			fnc = GetFInfo(i);
			if ( fnc->nameidx < 0 ) p = ""; else p = GetDS( fnc->nameidx );
			sprintf( mes,"FInfo#%d : index=%d ot=%d subid=%d name=%s param=%d\n", i, fnc->index, fnc->otindex, fnc->subid, p, fnc->prmmax );
			out->PutStr( mes );
		}
	}
/*
	{	//	Otの表示(未使用)
		int i,max,otv;
		max = GetOTCount();
		for(i=0;i<max;i++) {
			otv = GetOT(i);
			sprintf( mes,"Ot#%d : value=%x\n", i, otv );
			out->PutStr( mes );
		}
	}
*/
/*
	{	//	MInfoの表示(未使用)
		int i,max;
		MODDAT *mod;
		max = GetMInfoCount();
		for(i=0;i<max;i++) {
			mod = GetMInfo(i);
			sprintf( mes,"MInfo#%d : flag=%x name=%s\n", i, mod->flag, mod->name );
			out->PutStr( mes );
		}
	}
*/
	out->PutStr( "--------------------------------------------\n" );
}


int CHsp3::UpdateValueName( void )
{
	//		mem_di_valを更新
	//
	unsigned char *mem_di;
	unsigned char ofs;
	int cl,a;

	mem_di_val = NULL;
	if ( dinfo->GetSize() == 0 ) {
		return -1;
	}
	mem_di = (unsigned char *)dinfo->GetBuffer();
	if ( mem_di[0]==255) return -1;
	cl=0;a=0;
	while(1) {
		ofs=mem_di[a++];
		switch( ofs ) {
		case 252:
			a+=2;
			break;
		case 253:
			mem_di_val=&mem_di[a-1];
			return 0;
		case 254:
			cl=(mem_di[a+4]<<8)+mem_di[a+3];
			a+=5;
			break;
		case 255:
			return -1;
		default:
			cl++;
			break;
		}
	}
	return cl;
}


char *CHsp3::GetHSPVarName( int varid )
{
	//		変数名を取得
	//
	int i;
	unsigned char *mm;

	if ( mem_di_val == NULL ) {						// 変数名情報がない場合
		sprintf( hspvarmp, "_HspVar%d", varid );
		return hspvarmp;
	}
	mm = mem_di_val + ( varid * 6 );
	i = (mm[3]<<16)+(mm[2]<<8)+mm[1];
	return GetDS( i );
}


void CHsp3::MakeOTInfo( void )
{
	//		ot_infoを更新
	//		(ot_infoにはFInfoのIDが入る)
	//		(通常ラベルの場合は、ot_infoが-1になる)
	//
	int i,maxot;
	int *oi;
	int *ot;
	int maxfnc;
	STRUCTDAT *fnc;
	maxot = GetOTCount();
	ot_info = new CMemBuf( maxot * sizeof(int) );
	oi = (int *)ot_info->GetBuffer();
	ot = (int *)mem_ot->GetBuffer();
	for(i=0;i<maxot;i++) { oi[i]=-1; }
	maxfnc = GetFInfoCount();
	for(i=0;i<maxfnc;i++) {
		fnc = GetFInfo(i);
		switch( fnc->index ) {
		case STRUCTDAT_INDEX_FUNC:					// 定義命令
		case STRUCTDAT_INDEX_CFUNC:					// 定義関数
			oi[fnc->otindex] = i;
			break;
		}
	}
}


int CHsp3::GetOTInfo( int index )
{
	int *oi;
	oi = (int *)ot_info->GetBuffer();
	return oi[index];
}


void CHsp3::OutMes( char *format, ... )
{
	//		outbufに文字列を出力(printf互換)(OUTMES_BUFFER_MAXまで)
	//
	char textbf[OUTMES_BUFFER_MAX];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	out->PutStr( textbf );
}


void CHsp3::OutLine( char *format, ... )
{
	//		outbufに文字列を出力(printf互換)(OUTMES_BUFFER_MAXまで)
	//
	char textbf[OUTMES_BUFFER_MAX];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);

	for(int i=0;i<=indent;i++) { out->PutStr("\t"); }
	out->PutStr( textbf );
}


void CHsp3::OutLineBuf( CMemBuf *outbuf, char *format, ... )
{
	//		outbufに文字列を出力(printf互換)(OUTMES_BUFFER_MAXまで)
	//
	char textbf[OUTMES_BUFFER_MAX];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);

	for(int i=0;i<=indent;i++) { outbuf->PutStr("\t"); }
	outbuf->PutStr( textbf );
}


void CHsp3::OutCR( void )
{
	out->PutCR();
}


void CHsp3::SetIndent( int val )
{
	indent = val;
}

/*------------------------------------------------------------*/

void CHsp3::initCS( void *ptr )
{
	//		読み取り先csポインタを初期化
	//
	mcs = (unsigned short *)ptr;
}


int CHsp3::getCS( void )
{
	//		コマンドを１つ読み取る
	//		(ver3.0以降用)
	//
	register unsigned short a;
	mcs_last=mcs;
	a=*mcs++;
	exflag = a & (EXFLG_1|EXFLG_2);
	cstype = a & CSTYPE;
	if ( a & 0x8000 ) {
		//	 32bit val code
		//
		csval = (int)*((int *)mcs);
		mcs+=2;
	} else {
		// 16bit val code
		csval = (int)(*mcs++);
	}
	return csval;
}

int CHsp3::getEXFLG( void )
{
	//		次のコマンドのEXFLAGを読み取る
	//
	unsigned short a;
	a=*mcs;
	return (int)( a & (EXFLG_1|EXFLG_2));
}

int CHsp3::getNextCS( int *type )
{
	//		次のコマンドのTYPE,VALを読み取る
	//
	unsigned short a;
	a=*mcs;
	*type = a & CSTYPE;
	if ( a & 0x8000 ) {
		//	 32bit val code
		//
		return (int)*((int *)mcs);
	}
	// 16bit val code
	return (int)(mcs[1]);
}


char *CHsp3::GetDS_fmt( int offset )
{
	//		DSから文字列を取得する(フォーマット済み文字列)
	//
	MakeHspStyleString2( GetDS(offset), strtmp );
	return strtmp;
}


char *CHsp3::GetDS( int offset )
{
	//		DSから文字列を取得する
	//
	char *baseptr;
	baseptr = mem_ds->GetBuffer();
	baseptr += offset;
	return baseptr;
}


double CHsp3::GetDSf( int offset )
{
	//		DSからdouble値を取得する
	//
	char *ptr;
	ptr = GetDS( offset );
	return *(double *)ptr;
}


int CHsp3::GetOT( int index )
{
	//		OTから値を取得する
	//
	int *baseptr;
	baseptr = (int *)mem_ot->GetBuffer();
	return baseptr[index];
}


/*------------------------------------------------------------*/

char *CHsp3::GetHSPName( int type, int val )
{
	//		type,valに対応したHSP登録名を得る
	//
	int max,i;
	LABOBJ *lobj;
	max = lb->GetNumEntry();
	for(i=0;i<max;i++) {
		lobj = lb->GetLabel(i);
		if ( lobj->type == type ) {
			if ( lobj->opt == val ) {
				return lobj->name;
			}
		}
	}

	switch( type ) {
	case TYPE_MODCMD:
		{
		STRUCTDAT *fnc;
		fnc = GetFInfo( val );
		if ( fnc->index == STRUCTDAT_INDEX_FUNC ) return GetDS( fnc->nameidx );
		if ( fnc->index == STRUCTDAT_INDEX_CFUNC ) return GetDS( fnc->nameidx );
		break;
		}
	case TYPE_DLLFUNC:
		{
		STRUCTDAT *fnc;
		fnc = GetFInfo( val );
		return GetDS( fnc->nameidx );
		}
	default:
		break;
	}

	return "";
}


char *CHsp3::GetHSPOperator( int val )
{
	//		HSPの演算子を文字列(記号)で返す
	//
	switch( val ) {
	case CALCCODE_ADD:
		return "+";
	case CALCCODE_SUB:
		return "-";
	case CALCCODE_MUL:
		return "*";
	case CALCCODE_DIV:
		return "/";
	case CALCCODE_MOD:
		return "%";
	case CALCCODE_AND:
		return "&";
	case CALCCODE_OR:
		return "|";
	case CALCCODE_XOR:
		return "^";
	case CALCCODE_EQ:
		return "=";
	case CALCCODE_NE:
		return "!=";
	case CALCCODE_GT:
		return ">";
	case CALCCODE_LT:
		return "<";
	case CALCCODE_GTEQ:
		return ">=";
	case CALCCODE_LTEQ:
		return "<=";
	case CALCCODE_RR:
		return ">>";
	case CALCCODE_LR:
		return "<<";
	}
	hspoptmp[0] = (char)val;
	hspoptmp[1] = 0;
	return hspoptmp;
}


char *CHsp3::GetHSPOperator2( int val )
{
	//		HSPの演算子を文字列(メソッド)で返す
	//
	switch( val ) {
	case CALCCODE_ADD:
		return "AddI";
	case CALCCODE_SUB:
		return "SubI";
	case CALCCODE_MUL:
		return "MulI";
	case CALCCODE_DIV:
		return "DivI";
	case CALCCODE_MOD:
		return "ModI";
	case CALCCODE_AND:
		return "AndI";
	case CALCCODE_OR:
		return "OrI";
	case CALCCODE_XOR:
		return "XorI";
	case CALCCODE_EQ:
		return "EqI";
	case CALCCODE_NE:
		return "NeI";
	case CALCCODE_GT:
		return "GtI";
	case CALCCODE_LT:
		return "LtI";
	case CALCCODE_GTEQ:
		return "GtEqI";
	case CALCCODE_LTEQ:
		return "LtEqI";
	case CALCCODE_RR:
		return "RrI";
	case CALCCODE_LR:
		return "LrI";
	}
	return "";
}


char *CHsp3::GetHSPVarTypeName( int type )
{
	//		HSPのタイプ値を文字列で返す
	//
	switch( type ) {
	case HSPVAR_FLAG_LABEL:
		return "label";
	case HSPVAR_FLAG_STR:
		return "str";
	case HSPVAR_FLAG_DOUBLE:
		return "double";
	case HSPVAR_FLAG_INT:
		return "int";
	case HSPVAR_FLAG_STRUCT:
		return "module";
	case HSPVAR_FLAG_COMSTRUCT:
		return "comst";
	}
	return "";
}


char *CHsp3::GetHSPCmdTypeName( int type )
{
	//		HSPのコマンドタイプ値を文字列で返す
	//
	switch( type ) {
	case TYPE_MARK:
		return "Mark";
	case TYPE_VAR:
		return "Var";
	case TYPE_STRING:
		return "Str";
	case TYPE_DNUM:
		return "Double";
	case TYPE_INUM:
		return "Int";
	case TYPE_STRUCT:
		return "Module";
	case TYPE_XLABEL:
		return "Label";
	case TYPE_LABEL:
		return "Label";
	case TYPE_INTCMD:
		return "Intcmd";
	case TYPE_EXTCMD:
		return "Extcmd";
	case TYPE_EXTSYSVAR:
		return "Extvar";
	case TYPE_CMPCMD:
		return "Cmp";
	case TYPE_MODCMD:
		return "Modcmd";
	case TYPE_INTFUNC:
		return "Intfunc";
	case TYPE_SYSVAR:
		return "Sysvar";
	case TYPE_PROGCMD:
		return "Prgcmd";
	case TYPE_DLLFUNC:
		return "Dllfunc";
	case TYPE_DLLCTRL:
		return "Dllctrl";
	case TYPE_USERDEF:
		return "Usrfunc";
	}
	return "";
}


/*------------------------------------------------------------*/


void CHsp3::MakeProgramInfoFuncParam( int structid )
{
	//		定義命令パラメーター生成のテスト
	//		structid : パラメーターID
	//
	char mes[256];
	STRUCTDAT *fnc;
	STRUCTPRM *prm;
	int i,max;
	fnc = GetFInfo( structid );

	switch( fnc->index ) {
	case STRUCTDAT_INDEX_FUNC:					// 定義命令
		out->PutStr( "#deffunc" );
		break;
	case STRUCTDAT_INDEX_CFUNC:					// 定義関数
		out->PutStr( "#defcfunc" );
		break;
	}
	sprintf( mes," %s ", GetDS(fnc->nameidx) );
	out->PutStr( mes );

	prm = GetMInfo( fnc->prmindex );
	max = fnc->prmmax;
	for(i=0;i<max;i++) {
		if ( i ) out->PutStr( ", " );
		switch( prm->mptype ) {
		case MPTYPE_VAR:
			out->PutStr( "var" );
			break;
		case MPTYPE_STRING:
		case MPTYPE_LOCALSTRING:
			out->PutStr( "str" );
			break;
		case MPTYPE_DNUM:
			out->PutStr( "double" );
			break;
		case MPTYPE_INUM:
			out->PutStr( "int" );
			break;
		case MPTYPE_LABEL:
			out->PutStr( "label" );
			break;
		case MPTYPE_LOCALVAR:
			out->PutStr( "local" );
			break;
		case MPTYPE_ARRAYVAR:
			out->PutStr( "array" );
			break;
		case MPTYPE_SINGLEVAR:
			out->PutStr( "var" );
			break;
		default:
			sprintf( mes, "???(%d)", prm->mptype );
			out->PutStr( mes );
			break;
		}
		sprintf( mes," _prm%d", fnc->prmindex + i );
		out->PutStr( mes );
		prm++;
	}
	out->PutCR();
}


void CHsp3::MakeProgramInfoLabel( void )
{
	//		ラベルを生成のテスト
	//
	int otmax;
	char mes[256];
	int labindex;
	int myot;

	otmax = GetOTCount();
	if ( otmax == 0 ) return;
	myot = (int)(mcs_last - mcs_start);

	labindex = 0;
	while(1) {
		if ( labindex>=otmax ) break;
		if ( myot == GetOT( labindex ) ) {
			if ( GetOTInfo( labindex ) == -1 ) {
				sprintf( mes,"*L%04x\n", labindex );
				out->PutStr( mes );
			} else {
				MakeProgramInfoFuncParam( GetOTInfo( labindex ) );
			}
		}
		labindex++;
	}
}


int CHsp3::GetHSPVarExpression( char *mes )
{
	//	変数名直後に続くパラメーター(配列)を展開する
	//	mes : 配列内容をmesに出力
	//
	int i;
	int prm;
	if ( cstype == TYPE_MARK ) {
		if ( csval == '(' ) {
			CMemBuf arname;
			arname.PutStr( "(" );
			getCS();
			prm = 0;
			while(1) {
				if ( exflag & EXFLG_1) break;		// パラメーター列終端
				if ( mcs > mcs_end ) break;			// データ終端チェック
				if ( prm ) arname.PutStr( ", " );
				i = GetHSPExpression( &arname );
				if ( i > 0 ) break;
				if ( i < -1 ) return i;
				prm++;
			}
			arname.PutStr( ")" );
			if ( arname.GetSize() < VAREXP_BUFFER_MAX ) {
				strcpy( mes, arname.GetBuffer() );
			} else {
				Alert( "CHsp3:Expression buffer overflow." );
				*mes = 0;
			}
			getCS();
			return 1;
		}
	}
	return 0;
}


int CHsp3::GetHSPExpression( CMemBuf *eout )
{
	//		HSPの計算式フォーマットでパラメーターを展開する
	//		eout : 出力先
	//
	int i;
	int op;
	int res;
	CsStack st;
	OPSTACK *stm1;
	OPSTACK *stm2;
	char mes[8192];								// 展開される式の最大文字数
	char mestmp[8192];							// 展開される式の最大文字数
	char mm[4096];

	if (exflag&EXFLG_1) return 1;				// パラメーター終端
	if (exflag&EXFLG_2) {						// パラメーター区切り(デフォルト時)
		exflag^=EXFLG_2;
		return -1;
	}
	if ( cstype == TYPE_MARK ) {
		if ( csval == 63 ) {					// パラメーター省略時('?')
			getCS();
			exflag&=~EXFLG_2;
			return -1;
		}
		if ( csval == ')' ) {					// 関数内のパラメーター省略時
			exflag&=~EXFLG_2;
			return 2;
		}
	}

	*mes = 0;
	res = 0;

	while(1) {
		if ( mcs > mcs_end ) return 1;			// データ終端チェック

		switch(cstype) {
		case TYPE_MARK:
			//		記号(スタックから取り出して演算)
			//
			if ( csval == ')' ) {					// 引数の終了マーク
				exflag |= EXFLG_2;
				res = 2;
				break;
			}
			op = csval;
			stm1 = st.Peek2();
			stm2 = st.Peek();
			if ( stm1->type == -1 ) {				// (?)+(result)の場合
				MakeImmidiateHSPName( mm, stm2->type, stm2->val, stm2->opt );
				strcpy( mestmp, mes );
				sprintf( mes,"(%s)%s%s", mestmp, GetHSPOperator(op), mm );
			} else {
				if ( stm2->type == -1 ) {			// (result)+(?)の場合
					MakeImmidiateHSPName( mm, stm1->type, stm1->val, stm1->opt );
					strcat( mes, GetHSPOperator(op) );
					strcat( mes, mm );
				} else {							// (?)+(?)の場合
					MakeImmidiateHSPName( mm, stm1->type, stm1->val, stm1->opt );
					strcat( mes, mm );
					strcat( mes, GetHSPOperator(op) );
					MakeImmidiateHSPName( mm, stm2->type, stm2->val, stm2->opt );
					strcat( mes, mm );
				}
			}
			stm1->type = -1;
			st.Pop();
			getCS();
			break;
		case TYPE_VAR:
			//		変数をスタックに積む
			//
			stm1 = st.Push( cstype, csval );
			getCS();
			i = GetHSPVarExpression( mm );
			if ( i ) st.SetExStr( stm1, mm );
			break;
		case TYPE_INUM:
		case TYPE_STRING:
		case TYPE_DNUM:
		case TYPE_LABEL:
			//		直値をスタックに積む
			//
			st.Push( cstype, csval );
			getCS();
			break;
		case TYPE_STRUCT:
			//		パラメーター
			//
			stm1 = st.Push( cstype, csval );
			getCS();
			i = GetHSPVarExpression( mm );
			if ( i ) st.SetExStr( stm1, mm );
			break;
		default:
			//		関数として展開する
			//
			stm1 = st.Push( cstype, csval );
			getCS();
			i = GetHSPVarExpression( mm );
			if ( i ) st.SetExStr( stm1, mm );
			break;
		}

		if ( exflag ) {								// パラメーター終端チェック
			exflag&=~EXFLG_2;
			break;
		}
	}
	if ( st.GetLevel() > 1 ) {
		eout->PutStr( "/*ERROR*/" );
		Alert( "Invalid end stack" ); return -5;
	}
	if ( st.GetLevel() == 1 ) {
			stm1 = st.Peek();
			if ( stm1->type != -1 ) {
				MakeImmidiateHSPName( mm, stm1->type, stm1->val, stm1->opt );
				strcat( mes, mm );
			}
	}
	eout->PutStr( mes );
	return res;
}


int CHsp3::MakeImmidiateName( char *mes, int type, int val )
{
	//		直値(int,double,str)を追加
	//		(追加できない型の場合は-1を返す)
	//
	switch( type ) {
	case TYPE_STRING:
		//sprintf( mes, "\"%s\"", GetDS( val ) );
		sprintf( mes, "\"%s\"", GetDS_fmt( val ) );
		break;
	case TYPE_DNUM:
		{
		double dval;
		dval = GetDSf(val);
		sprintf( mes, "%f", dval );
		break;
		}
	case TYPE_INUM:
		sprintf( mes, "%d", val );
		break;
	default:
		*mes = 0;
		return -1;
	}
	return 0;
}


int CHsp3::MakeImmidiateHSPName( char *mes, int type, int val, char *opt )
{
	//		直値(int,double,str)を追加
	//		(追加できない型の場合は-1を返す)
	//
	int i;
	i = MakeImmidiateName( mes, type, val );
	if ( i == 0 ) return 0;
	switch( type ) {
	case TYPE_VAR:
		sprintf( mes, "%s", GetHSPVarName(val) );
		if ( opt != NULL ) strcat( mes, opt );
		break;
	case TYPE_STRUCT:
		{
		STRUCTPRM *prm;
		prm = GetMInfo( val );
		if ( prm->subid != STRUCTPRM_SUBID_STACK ) {
			sprintf( mes, "_modprm%d", val );
		} else {
			sprintf( mes, "_prm%d", val );
		}
		break;
		}
	case TYPE_LABEL:
		sprintf( mes, "*L%04x", val );
		break;
	default:
		strcpy( mes, GetHSPName( type, val ) );
		if ( opt != NULL ) strcat( mes, opt );
		if ( *mes == 0 ) return -1;
		break;
	}
	return 0;
}


void CHsp3::MakeHspStyleString( char *str, CMemBuf *eout )
{
	//		HSPのエスケープを使用した文字列を生成する
	//		str  : 元の文字列
	//		eout : 出力先
	//
	char *p;
	char a1;
	p = str;
	eout->Put( (char)0x22 );
	while(1) {
		a1 = *p++;
		if ( a1 == 0 ) break;
		if ( a1 == 13 ) {
			if ( *p==10 ) p++;
			eout->Put( '\\' );
			a1='n';
		}
		if ( a1 == '\t' ) {
			eout->Put( '\\' );
			a1='t';
		}
		if ( a1 == '\\' ) {
			eout->Put( '\\' );
		}
		if ( a1 == 0x22 ) {
			eout->Put( '\\' );
			a1 = 0x22;
		}
		eout->Put( a1 );
	}
	eout->Put( (char)0x22 );
}
	

void CHsp3::MakeHspStyleString2( char *str, char *dst )
{
	//		HSPのエスケープを使用した文字列を生成する
	//		str : 元の文字列
	//		dst : 出力先
	//
	char *p;
	char *pdst;
	char a1;
	p = str;
	pdst = dst;
	while(1) {
		a1 = *p++;
		if ( a1 == 0 ) break;
		if ( a1 == 13 ) {
			if ( *p==10 ) p++;
			*pdst++ = '\\';
			a1='n';
		}
		if ( a1 == '\t' ) {
			*pdst++ = '\\';
			a1='t';
		}
		if ( a1 == '\\' ) {
			*pdst++ = '\\';
		}
		if ( a1 == 0x22 ) {
			*pdst++ = '\\';
			a1 = 0x22;
		}
		*pdst++ = a1;
	}
	*pdst++ = 0;
}
	

void CHsp3::MakeProgramInfoHSPName( bool putadr )
{
	//		HSP名を追加(表示)
	//		putadr : アドレス表示ON/OFF
	//
	char mes[4096];
	if ( putadr ) {
		sprintf( mes,"#%06x:", mcs - mcs_start );
		out->PutStr( mes );
	} else {
		out->PutStr( "        " );
	}
	switch( cstype ) {
	case TYPE_MARK:
		sprintf( mes, "Mark'%s'\n", GetHSPOperator( csval ) );
		break;
	case TYPE_VAR:
		sprintf( mes, "VAR:%d(%s)\n", csval, GetHSPVarName(csval) );
		break;
	case TYPE_STRING:
		sprintf( mes, "Str:%s\n", GetDS( csval ) );
		break;
	case TYPE_DNUM:
		{
		double dval;
		dval = GetDSf(csval);
		sprintf( mes, "Double:%f\n", dval );
		break;
		}
	case TYPE_INUM:
		sprintf( mes, "Int:%d\n", csval );
		break;
	case TYPE_STRUCT:
		sprintf( mes, "Struct:%d\n", csval );
		break;
	case TYPE_LABEL:
		sprintf( mes, "Label:%d\n", csval );
		break;
	case TYPE_PROGCMD:
	case TYPE_INTCMD:
	case TYPE_EXTCMD:
	case TYPE_INTFUNC:
		char *p;
		p = GetHSPName( cstype, csval );
		if ( *p == 0 ) {
			sprintf( mes, "cmd?:CSTYPE%d VAL%d\n", cstype, csval );
		} else {
			sprintf( mes, "Cmd:%s\n", p );
		}
		break;
	case TYPE_SYSVAR:
		sprintf( mes, "SysVar:%d\n", csval );
		break;
	case TYPE_EXTSYSVAR:
		sprintf( mes, "ExtSysVar:%d\n", csval );
		break;
	case TYPE_CMPCMD:
		sprintf( mes, "CMP:%d\n", csval );
		break;
	case TYPE_MODCMD:
		sprintf( mes, "Module:%d\n", csval );
		break;
	case TYPE_DLLFUNC:
		sprintf( mes, "DLLFUNC:%d\n", csval );
		break;
	case TYPE_DLLCTRL:
		sprintf( mes, "DLLCTRL:%d\n", csval );
		break;
	default:
		sprintf( mes, "???:CSTYPE%d VAL%d\n", cstype, csval );
		break;
	}
	out->PutStr( mes );
}


int CHsp3::MakeProgramInfoParam( void )
{
	//		パラメーターのトレース
	//
	int prm;
	//char mes[4096];

	prm = 0;
	while(1) {
		if ( mcs > mcs_end ) return -1;
		if ( exflag == EXFLG_1 ) break;		// 行末
		if ( exflag & EXFLG_2 ) {
			if (prm) {				// パラメーター区切り
				out->PutStr( "        ----\n" );
			}
		}
		MakeProgramInfoHSPName( false );
		//sprintf( mes, "        CSTYPE%d VAL%d\n", cstype, csval );
		//out->PutStr( mes );
		prm++;
		getCS();
	}
	return 0;
}
	

int CHsp3::MakeProgramInfoParam2( void )
{
	//		パラメーターのトレース(展開あり)
	//
	int i;
	int prm;
//	char mes[4096];

	prm = 0;
	while(1) {
//		sprintf( mes,"---%04x:%x:CSTYPE%d VAL%d\n", mcs - mcs_start, exflag, cstype, csval );
//		out->PutStr( mes );

		if ( exflag & EXFLG_1) break;		// パラメーター列終端
		if ( mcs > mcs_end ) break;			// データ終端チェック

		if ( prm ) out->PutStr( ", " );
		i = GetHSPExpression( out );
		if ( i > 0 ) break;
		if ( i < -1 ) return i;
		prm++;
	}
	out->PutCR();
	return 0;
}
	

void CHsp3::MakeProgramInfo( void )
{
	//		プログラムのトレーステスト
	//
	int i;
	int op;
	int cmdtype, cmdval;
	char mes[OUTMES_BUFFER_MAX];
	char arbuf[VAREXP_BUFFER_MAX];
	while(1) {
		if ( mcs > mcs_end ) break;

		//		endifのチェック
		//
		if ( ifmode[iflevel]==2 ) {		// if end
			if ( mcs_last>=ifptr[iflevel] ) {
				ifmode[iflevel] = 0;
				if ( iflevel == 0 ) { Alert( "Invalid endif." ); return; }
				iflevel--;
				out->PutStr( "endif\n" );
				continue;
			}
		}

		//		ラベルチェック
		//
		MakeProgramInfoLabel();

		//		行頭のコード
		//
		cmdtype = cstype;
		cmdval = csval;
		//MakeProgramInfoHSPName();
		sprintf( mes,"#%06x:CSTYPE%d VAL%d\n", mcs_last - mcs_start, cstype, csval );
		//Alert( mes );
		out->PutStr( mes );

		//		パラメーター
		//
		switch( cmdtype ) {
		case TYPE_STRUCT:						// 代替変数(struct)
		case TYPE_VAR:							// 変数代入
			MakeImmidiateHSPName( mes, cmdtype, cmdval );
			out->PutStr( mes );
			getCS();
			i = GetHSPVarExpression( arbuf );
			if ( i ) { out->PutStr( arbuf ); }

			if ( cstype == TYPE_MARK ) {
				if ( csval == CALCCODE_EQ ) {
					out->PutStr( "=" );
					getCS();
					MakeProgramInfoParam2();
					break;
				}
				op = csval;
				getCS();
				if ( exflag & EXFLG_1) {		// ++ or --
					out->PutStr( GetHSPOperator(op) );
					out->PutStr( GetHSPOperator(op) );
					MakeProgramInfoParam2();
					break;
				}
				out->PutStr( GetHSPOperator(op) );
				out->PutStr( "=" );
				//getCS();
				MakeProgramInfoParam2();
				break;
			}
			Alert( "CHsp3:Var Syntax unknown." );
			break;
		case TYPE_CMPCMD:						// 比較命令
			if ( cmdval == 0 ) {
				out->PutStr( "if " );
				iflevel++;
				if ( iflevel >= MAX_IFLEVEL ) {
					Alert( "Stack(If) overflow." );
					return;
				}
				ifmode[iflevel] = 1;
				i = (int)*mcs;
				ifptr[iflevel] = mcs + i + 1;
				ifmode[iflevel]++;
			} else {
				out->PutStr( "} else {" );
				ifmode[iflevel] = 3;
				i = (int)*mcs;
				ifptr[iflevel] = mcs + i + 1;
			}
			mcs++;
			getCS();
			MakeProgramInfoParam2();
			break;
		default:
			out->PutStr( GetHSPName( cstype, csval ) );
			out->PutStr( " " );
			getCS();
			MakeProgramInfoParam2();
			break;
		}
	}
	out->PutStr( "--------------------------------------------\n" );

}


/*------------------------------------------------------------*/


int CHsp3::MakeSource( int option, void *ref )
{
	MakeObjectInfo();
	MakeProgramInfo();
	return 0;
}


int CHsp3::CheckExtLibrary( void )
{
	if ( GetLInfoCount() > 0 ) {
		return -1;
	}
	return 0;
}


