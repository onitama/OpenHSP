
//
//		AHT manager class
//			onion software/onitama 2006/3
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "supio.h"
#include "ahtmodel.h"

//-------------------------------------------------------------
//		Static Data
//-------------------------------------------------------------

static	int strdummy = 0;

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void AHTMODEL::Mesf( char *format, ... )
{
	char textbf[1024];

	if ( stdbuf == NULL ) return;

	va_list args;
	va_start( args, format );
	vsprintf( textbf, format, args );
	va_end( args );

	stdbuf->PutStr( textbf );
}


AHTPROP *AHTMODEL::GetPropertyFromAlias( char *propname )
{
	//		プロパティ名から検索
	//
	int i;
	char tmp[128];
	strcpy2( tmp, propname, 128 );
	strcase( tmp );
	for(i=0;i<prop_cnt;i++) {
		if ( tstrcmp( mem_prop[i]->name, tmp ) ) {
			return mem_prop[i];
		}
	}
	return NULL;
}

AHTPROP *AHTMODEL::GetProperty( char *propname )
{
	//		プロパティ名から検索
	//
	int i;
	char tmp[128];
	strcpy2( tmp, propname, 128 );
	strcase( tmp );
	for(i=0;i<prop_cnt;i++) {
		if ( tstrcmp( mem_prop[i]->orgname, tmp ) ) {
			return mem_prop[i];
		}
	}
	return NULL;
}


int AHTMODEL::GetPropertyID( char *propname )
{
	//		プロパティ名から検索
	//
	int i;
	char tmp[128];
	strcpy2( tmp, propname, 128 );
	strcase( tmp );
	for(i=0;i<prop_cnt;i++) {
		if ( tstrcmp( mem_prop[i]->orgname, tmp ) ) {
			return i;
		}
	}
	return -1;
}


int AHTMODEL::GetGlobalId2( void )
{
	//		グローバルIDを返す
	//
	if ( global_id < 0 ) return 0;
	return global_id;
}


int AHTMODEL::GetPropertyPossibleLines( void )
{
	//		プロパティ編集が使用する行数を計算する
	//
	int i;
	int lines;
	char a1;
	AHTPROP *p;
	lines = 0;
	for(i=0;i<prop_cnt;i++) {
		p = GetProperty( i );
		switch( p->ahttype ) {
		case AHTTYPE_EDIT_STRING:
			a1 = *(p->defval3);
			switch( a1 ) {
			case 'm':
				lines+=4;
				if ( *p->help != 0 ) { lines++; }
				break;
			case 'w':
				if ( *p->help != 0 ) { lines++; }
				break;
			default:
				break;
			}
			lines++;
			break;
		case AHTTYPE_CBOX_STRING:
		case AHTTYPE_FONT_STRING:
		case AHTTYPE_FILE_STRING:
		case AHTTYPE_EXTF_STRING:
		case AHTTYPE_PARTS_INT:
			if ( *p->help != 0 ) { lines++; }
		default:
			lines++;
			break;
		}
	}
	return lines;
}


void AHTMODEL::SetAHTOption( char *name, char *value )
{
	if (tstrcmp(name,"flag")) {
		int i;
		i = atoi( value );
		flag = i | AHTMODEL_FLAG_READY;
		return;
	}
	if (tstrcmp(name,"name")) {
		SetName( value );
		return;
	}
	if (tstrcmp(name,"class")) {
		SetClass( value );
		return;
	}
	if (tstrcmp(name,"author")) {
		SetAuthor( value );
		return;
	}
	if (tstrcmp(name,"ver")) {
		SetVersion( value );
		return;
	}
	if (tstrcmp(name,"icon")) {
		SetIconFile( value );
		return;
	}
	if (tstrcmp(name,"iconid")) {
		SetIconId( atoi( value ) );
		return;
	}
	if (tstrcmp(name,"exp")) {
		exp->PutStr( value );
		exp->PutCR();
		return;
	}
	if (tstrcmp(name,"helpkw")) {
		SetHelpKeyword( value );
		return;
	}
}


char *AHTMODEL::GetAHTOption( char *name )
{
	if (tstrcmp(name,"flag")) {
		sprintf( s_flag, "%d", GetFlag() );
		return s_flag;
	}
	if (tstrcmp(name,"glid")) {
		sprintf( s_glid, "%d", GetGlobalId2() );
		return s_glid;
	}
	if (tstrcmp(name,"name")) {
		return GetName();
	}
	if (tstrcmp(name,"class")) {
		return GetClass();
	}
	if (tstrcmp(name,"author")) {
		return GetAuthor();
	}
	if (tstrcmp(name,"ver")) {
		return GetVersion();
	}
	if (tstrcmp(name,"icon")) {
		return GetIconFile();
	}
	if (tstrcmp(name,"exp")) {
		return exp->GetBuffer();
	}
	if (tstrcmp(name,"source")) {
		return fname;
	}
	if (tstrcmp(name,"helpkw")) {
		return GetHelpKeyword();
	}
	return (char *)&dummy;
}


AHTPROP *AHTMODEL::SetPropertyType( char *propname, int type )
{
	AHTPROP *p;
	p = GetProperty( propname );
	if ( p == NULL ) {
		char ptmp[128];
		strcpy2( ptmp, propname, 128 );
		strcase( ptmp );
		p = AddProperty();
		SetPropOrgName( p, ptmp );
	}
	p->ahttype = type;
	return p;
}


AHTPROP *AHTMODEL::SetPropertyDefault( char *propname, char *value )
{
	//		プロパティのデフォルト値を設定
	//		("～"を判別する)
	//
	unsigned char *vp;
	unsigned char a1;
	char *pname;
	int qmode;
	AHTPROP *prop;
	vp = (unsigned char *)value;
	qmode = 0;
	while(1) {
		a1=*vp;if ((a1!=32)&&(a1!=9)) break;
		vp++;
	}

	if ( a1 == 0x22 ) {
		qmode = 1;
		vp++;
	}

	pname = (char *)vp;
	while(1) {
		a1=*vp;
		if ( a1 == 0 ) break;
		if ( qmode ) {
			if ( a1 == 0x22 ) break;
		} else {
			if ( a1 <= 32 ) break;
		}
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				vp++;
			}
		}
		vp++;
	}
	*vp = 0;
	prop = SetPropertyDefaultStr( propname, pname );
	if ( qmode ) prop->SetMode( AHTMODE_QUOTATION | AHTMODE_OUTPUT_PURE );
	//Alertf("%s(%d)",value,qmode);
	return prop;
}


AHTPROP *AHTMODEL::SetPropertyDefaultInt( char *propname, int value )
{
	AHTPROP *p;
	char tmp[64];
	sprintf( tmp,"%d",value );
	p = SetPropertyType( propname, AHTTYPE_EDIT_INT );
	SetPropDefval( p, tmp );
	return p;
}


AHTPROP *AHTMODEL::SetPropertyDefaultStr( char *propname, char *value )
{
	AHTPROP *p;
	p = SetPropertyType( propname, AHTTYPE_EDIT_STRING );
	SetPropDefval( p, value );
	return p;
}


AHTPROP *AHTMODEL::SetPropertyDefaultDouble( char *propname, double value )
{
	AHTPROP *p;
	char tmp[64];
	sprintf( tmp,"%f",value );
	p = SetPropertyType( propname, AHTTYPE_EDIT_DOUBLE );
	SetPropDefval( p, tmp );
	return p;
}


int AHTMODEL::SetProp( AHTPROP *prop, char *name, char *value )
{
	int i,type;
	char tmp[32];
	AHTPROP *p;

	p = prop;

	strcpy2( tmp, name, 16 );
	strcase( tmp );
	i = -1;
	type = 0;
	if (tstrcmp(tmp,"name")) i = 0;
	if (tstrcmp(tmp,"help")) i = 1;
	if (tstrcmp(tmp,"default")) i = 2;

	if (tstrcmp(tmp,"sub")) i = 3;
	if (tstrcmp(tmp,"sub2")) i = 4;
	if (tstrcmp(tmp,"min")) i = 3;
	if (tstrcmp(tmp,"max")) i = 4;
	if (tstrcmp(tmp,"prm")) i = 3;
	if (tstrcmp(tmp,"opt")) i = 4;

	if (tstrcmp(tmp,"int")) { i = 5; type = AHTTYPE_EDIT_INT; }
	if (tstrcmp(tmp,"double")) { i = 5; type = AHTTYPE_EDIT_DOUBLE; }
	if (tstrcmp(tmp,"str")) { i = 5; type = AHTTYPE_EDIT_STRING; }
	if (tstrcmp(tmp,"combox")) { i = 5; type = AHTTYPE_CBOX_STRING; }
	if (tstrcmp(tmp,"chkbox")) { i = 5; type = AHTTYPE_CHKB_INT; }
	if (tstrcmp(tmp,"color")) { i = 5; type = AHTTYPE_COLS_INT; }
	if (tstrcmp(tmp,"font")) { i = 5; type = AHTTYPE_FONT_STRING; }
	if (tstrcmp(tmp,"file")) { i = 5; type = AHTTYPE_FILE_STRING; }
	if (tstrcmp(tmp,"exec")) { i = 5; type = AHTTYPE_EXTF_STRING; }

	if (tstrcmp(tmp,"parts")) { i = 5; type = AHTTYPE_PARTS_INT; }
	if (tstrcmp(tmp,"pprop")) { i = 5; type = AHTTYPE_PARTS_PROP_STRING; }
	if (tstrcmp(tmp,"popt")) { i = 5; type = AHTTYPE_PARTS_OPT_STRING; }

	//Alertf( "%s=%s(%d)", tmp, value, i );
	if ( i < 0 ) return -1;

	switch( i ) {
	case 0:
		SetPropName( p, value );
		break;
	case 1:
		SetPropHelp( p, value );
		break;
	case 2:
		SetPropDefval( p, value );
		break;
	case 3:
		SetPropDefval2( p, value );
		break;
	case 4:
		SetPropDefval3( p, value );
		break;
	case 5:
		p->ahttype = type;
		break;
	}

	return i;
}


AHTPROP *AHTMODEL::SetProperty( char *propname, char *name, char *value )
{
	AHTPROP *p;
	p = GetProperty( propname );
	if ( p == NULL ) {
		char ptmp[128];
		strcpy2( ptmp, propname, 128 );
		strcase( ptmp );
		p = AddProperty();
		SetPropOrgName( p, ptmp );
	}
	if (( name == NULL )||( value == NULL )) return p;
	if ( SetProp( p, name, value ) < 0 ) return NULL;
	return p;
}


/*
	rev 54
	mingw : warning : 比較は常に…
	に対処。
*/

int AHTMODEL::SetAHTPropertyString( char *propname, char *str )
{
	//		AHT設定文字列を解析する
	//
	int res;
	int qmode;
	int amb;
	unsigned char a1;
	char *vp;
	char *pname;
	char *pvalue;
	AHTPROP *p;
	vp = str;
	res = 0;
	p = SetProperty( propname, NULL, NULL );

	while(1) {
		//		パラメーター名を抽出
		while(1) {
			a1=*vp;if ((a1!=32)&&(a1!=9)) break;
			vp++;
		}
		if ( a1 == 0 ) break;
		pname = vp;					// パラメーター名
		while(1) {
			a1=*vp;
			if ( a1 == 0 ) break;
			if ( a1 == ',' ) break;
			if ( a1 == '=' ) break;
			if (a1>=129) {					// 全角文字チェック
				if ((a1<=159)||(a1>=224)) {
					vp++;
				}
			}
			vp++;
		}
		*vp = 0;
		if ( a1 == '=' ) {
			//		パラメーターを抽出
			vp++;
			qmode = 0;
			if ( *vp == 0x22 ) { qmode = 1; vp++; }
			pvalue = vp;
			while(1) {
				a1=*vp;
				if ( a1 == 0 ) break;
				if ( qmode == 0 ) {
					if ( a1 == ',' ) { *vp++=0; break; }
				} else {
					if ( a1 == 0x22 ) { *vp=0; qmode=0; }
					if ( a1 == 0x5c ) {					// '\' extra control
						if ( vp[1] == 'n' ) {
							vp[0] = 13; vp[1] = 10;
						}
						vp++;
					}
				}
				if (a1>=129) {					// 全角文字チェック
					if ((a1<=159)||(a1>=224)) {
						vp++;
					}
				}
				vp++;
			}
			if ( SetProp( p, pname, pvalue ) < 0 ) res = 1;
		} else {
			//		単独タイプ
			//
			amb = 1;
			if (tstrcmp(pname,"withid")) {
				p->SetMode( AHTMODE_WITH_ID );
				if ( p->newval == NULL ) {
					char tmp[512];
					strcpy( tmp, p->defval );
					sprintf( tmp, "%s_%d", p->defval,id );
					p->SetNewVal( tmp );
				}
				amb = 0;
			}
			if (tstrcmp(pname,"read")) {
				p->SetMode( AHTMODE_READ_ONLY );
				amb = 0;
			}
			if (tstrcmp(pname,"pure")) {
				p->SetMode( AHTMODE_OUTPUT_PURE );
				amb = 0;
			}
			if (tstrcmp(pname,"raw")) {
				p->SetMode( AHTMODE_OUTPUT_RAW );
				amb = 0;
			}
			if (tstrcmp(pname,"mes")) {
				p->ClearMode( AHTMODE_OUTPUT_PURE );
				amb = 0;
			}
			if (tstrcmp(pname,"refname")) {
				refprop = p;
				amb = 0;
			}

			if ( amb ) {
				if ( SetProp( p, pname, "" ) < 0 ) res = 1;
			}
			if ( a1 == ',' ) vp++;
		}
	}
	return res;
}


//-------------------------------------------------------------
//		AHTModel Interfaces
//-------------------------------------------------------------

AHTMODEL::AHTMODEL( void )
{
	flag = AHTMODEL_FLAG_NONE;
	dummy = 0;
	SetCur( 0, 0 );
	SetPage( 0 );
	SetNextID( -1 );
	SetPrevID( -1 );
	SetGlobalId( 0 );

	obj.option1 = 0;
	obj.option2 = 0;
	obj.option3 = 0;
	obj.option4 = 0;

	prop_cnt = 0;
	mem_prop_size = 0;
	mem_prop = NULL;

	source = NULL;
	propstr = NULL;
	exp = NULL;
	stdbuf = NULL;

	*name = 0;
	*classname = 0;
	*author = 0;
	*ver = 0;
	*fname = 0;
	*fpath = 0;
	refprop = NULL;

	*icon = 0;
	iconid = 0;
}


AHTMODEL::~AHTMODEL( void )
{
	int i;
	for(i=0;i<prop_cnt;i++) {
		delete mem_prop[i];
	}
	if ( source != NULL ) delete source;
	if ( propstr != NULL ) delete propstr;
	if ( exp != NULL ) delete exp;
}


AHTPROP *AHTMODEL::AddProperty( void )
{
	int i,sz;
	AHTPROP *obj;
	i = prop_cnt++;
	sz = sizeof( void * ) * (( prop_cnt + 15 ) & 0xfff0 );
	mem_prop = (AHTPROP **)mem_alloc( mem_prop, sz, mem_prop_size );
	mem_prop_size = sz;
	obj = new AHTPROP;
	obj->SetId( i );
	mem_prop[ i ] = obj;
	return obj;
}


void AHTMODEL::SetSource( char *filename )
{
	strcpy2( fname, filename, AHTMODEL_FNMAX );
	propstr = new CMemBuf;
	exp = new CMemBuf( 0x1000 );
	flag = AHTMODEL_FLAG_READY;
}


void AHTMODEL::SetSourcePath( char *filename )
{
	strcpy2( fpath, filename, AHTMODEL_FNMAX );
}


void AHTMODEL::SetName( char *dname )
{
	strcpy2( name, dname, 128 );
}


void AHTMODEL::SetAuthor( char *name )
{
	strcpy2( author, name, 32 );
}


void AHTMODEL::SetVersion( char *name )
{
	strcpy2( ver, name, 32 );
}


void AHTMODEL::SetClass( char *name )
{
	strcpy2( classname, name, 128 );
}


void AHTMODEL::SetIconFile( char *name )
{
	strcpy2( icon, name, 32 );
}


void AHTMODEL::SetIconId( int id )
{
	iconid = id;
}


void AHTMODEL::SetStdBuffer( CMemBuf *buf )
{
	stdbuf = buf;
}


void AHTMODEL::SetPropName( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->name = p;
}


void AHTMODEL::SetPropOrgName( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->orgname = p;
	prop->name = p;
}


void AHTMODEL::SetPropHelp( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->help = p;
}


void AHTMODEL::SetPropDefval( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->defval = p;
}


void AHTMODEL::SetPropDefval2( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->defval2 = p;
}


void AHTMODEL::SetPropDefval3( AHTPROP *prop, char *value )
{
	char *p;
	p = propstr->GetBuffer() + propstr->GetSize();
	propstr->PutStrBlock( value );
	prop->defval3 = p;
}


void AHTMODEL::SetNextID( int id )
{
	obj.next=id;
}


void AHTMODEL::SetPrevID( int id )
{
	obj.prev=id;
}


char *AHTMODEL::GetName( void )
{
	if ( refprop != NULL ) {
		return refprop->GetValue();
	}
	return name;
}


void AHTMODEL::SetHelpKeyword( char *name )
{
	strcpy2( helpkw, name, 256 );
}


//-------------------------------------------------------------
//		AHTProperty Interfaces
//-------------------------------------------------------------

AHTPROP::AHTPROP( void )
{
	char *p;
	id = 0;
	ahttype = AHTTYPE_EDIT_INT;		// property type (AHTTYPE_*)
	ahtmode = 0;
	p = (char *)&strdummy;
	name = p;						// property name
	orgname = p;					// property name(org)
	help = p;						// property name (help)
	defval  = p;					// default value
	defval2 = p;					// default value (sub)
	defval3 = p;					// default value (sub2)
	nvsize = 0;
	newval = NULL;
	outname = NULL;
}


AHTPROP::~AHTPROP( void )
{
	if ( newval != NULL ) free( newval );
	if ( outname != NULL ) free( outname );
}


void AHTPROP::SetOutValue( char *data )
{
	//		ダブルクォート付加時の書式展開
	//
	int i;
	unsigned char a1;
	unsigned char *p;
	unsigned char *src;

	if ( outname != NULL ) { free( outname ); outname = NULL; }
	i = ( strlen( data ) * 2 ) + 1;
	if ( i < 64 ) i = 64;
	outname = (char *)malloc( i );
	*outname = 0;
	p = (unsigned char *)outname;
	src = (unsigned char *)data;

	//*p++ = 0x22;
	//strcpy( (char *)p, data );
	//strcat( (char *)p, "\"" );

	if ( ahtmode & AHTMODE_OUTPUT_PURE ) {
		*p++ = 0x22;
	} else {
		*p++ = 0x22;
		*p++ = '\\';
		*p++ = 0x22;
	}

	while(1) {
		a1 = *src++;
		if ( a1 == 0 ) break;
		if ( a1 == '\\' ) {
			if (( ahtmode & AHTMODE_OUTPUT_PURE )==0) {
			*p++ = a1;
			*p++ = a1;
			}
			*p++ = a1;
		}
		if ( a1 == 13 ) {
			*p++ = '\\';
			if ( *src == 10 ) src++;
			a1 = 'n';
		}
		if ( a1 == 0x22 ) {
			*p++ = '\\';
			a1 = 0x22;
		}
		if (a1>=129) {						// 全角文字チェック
			if (a1<=159) { *p++=a1;a1=*src++; }
			else if (a1>=224) { *p++=a1;a1=*src++; }
			if ( a1 == 0 ) break;
		}
		*p++ = a1;
	}

	if ( ahtmode & AHTMODE_OUTPUT_PURE ) {
		strcpy( (char *)p, "\"" );
	} else {
		strcpy( (char *)p, "\\\"\"" );
	}
}


char *AHTPROP::GetOutValue( void )
{
	if ( ahtmode & AHTMODE_OUTPUT_RAW ) {
		return GetValue();
	}
	if ( ahtmode & AHTMODE_QUOTATION ) {
		SetOutValue( GetValue() );
		return outname;
	}
	return GetValue();
}


void AHTPROP::SetNewVal( char *data )
{
	int i;
	i = strlen( data ) + 1;
	if ( i <= nvsize ) {
		strcpy( newval, data );
	}
	if ( newval != NULL ) free( newval );
	if ( i < 64 ) i = 64;
	nvsize = i;
	newval = (char *)malloc( i );
	strcpy( newval, data );
}

char *AHTPROP::GetValue( void )
{
	if ( newval != NULL ) return newval;
	return defval;
}


int AHTPROP::GetValueInt( void )
{
	char *p;
	p = GetValue();
	return atoi( p );
}


double AHTPROP::GetValueDouble( void )
{
	char *p;
	p = GetValue();
	return atof( p );
}

