
//
//		AHT manager class
//			onion software/onitama 2006/3
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "supio.h"
#include "strnote.h"
#include "ahtobj.h"

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void CAht::Mesf( char *format, ... )
{
	char textbf[1024];

	if ( stdbuf == NULL ) return;

	va_list args;
	va_start( args, format );
	vsprintf( textbf, format, args );
	va_end( args );

	stdbuf->PutStr( textbf );
}

void CAht::Reset( void )
{
	char p[512];

	DisposeModel();

	dirinfo( p, 0x10005 );
	SetPrjDir( p );						// マイドキュメントを設定
	dirinfo( p, 1 );
	SetToolDir( p );					// 自分のディレクトリ
	SetPrjFile( "ahttmp" );
	SetPage( 0, 0 );
}

void CAht::DisposeModel( void )
{
	int i;

	if ( mem_ahtmodel_size ) {
		for(i=0;i<model_cnt;i++) {
			DeleteModel( i );
		}
		if ( mem_ahtmodel != NULL ) {
			mem_bye( mem_ahtmodel );
		}
		model_cnt = 0;
	}
	mem_ahtmodel = NULL;				// model data
	mem_ahtmodel_size = 0;
}

AHTMODEL *CAht::AddModel( void )
{
	//			モデルを新規IDに追加
	int i;
	for(i=0;i<model_cnt;i++) {
		if ( mem_ahtmodel[i] == NULL ) {
			return EntryModel( i );
		}
	}
	return EntryModel( model_cnt );
}

void CAht::DeleteModel( int id )
{
	//			モデルを削除
	int i;
	int myid;
	AHTMODEL *model;
	AHTMODEL *m;
	model = GetModel( id );
	if ( model == NULL ) return;

	//			自分にリンクしているIDを探す
	myid = model->GetId();
	for(i=0;i<model_cnt;i++) {
		m = GetModel( i );
		if ( m != NULL ) {
			if ( m->GetNextID() == myid ) m->SetNextID( -1 );
			if ( m->GetPrevID() == myid ) m->SetPrevID( -1 );
		}
	}

	//			完全に削除
	delete model;
	mem_ahtmodel[id] = NULL;

	//			全体の情報を更新
	BuildGlobalID();
}

AHTMODEL *CAht::EntryModel( int id )
{
	//			指定IDにモデルを追加
	//			(バッファは自動確保される)
	int i,sz;
	AHTMODEL *model;

	i = id;
	if ( i >= model_cnt ) {
		model_cnt = i + 1;
		sz = sizeof( void * ) * (( model_cnt + 15 ) & 0xfff0 );
		mem_ahtmodel = (AHTMODEL **)mem_alloc( mem_ahtmodel, sz, mem_ahtmodel_size );
		mem_ahtmodel_size = sz;
	}

	if ( mem_ahtmodel[ i ] != NULL ) DeleteModel( i );

	model = new AHTMODEL;
	model->SetId( i );
	mem_ahtmodel[ i ] = model;
	return model;
}

AHTMODEL *CAht::GetModel( int id )
{
	//		モデルオブジェクを取得
	AHTMODEL *model;
	if (( id < 0 )||( id >= model_cnt )) return NULL;
	model = mem_ahtmodel[ id ];
	return model;
}

void CAht::LinkModel( int id, int next_id )
{
	//		モデルをリンク
	AHTMODEL *model;
	AHTMODEL *model2;
	model = GetModel( id );
	if ( model == NULL ) return;
	model2 = GetModel( next_id );
	if ( model2 == NULL ) return;
	if ( id == next_id ) return;

	UnlinkModel( id );

	if ( model->GetFlag() & AHTMODEL_FLAG_NOLINK ) return;
	if ( model2->GetFlag() & AHTMODEL_FLAG_NOLINK ) return;

	model->SetNextID( next_id );
	model2->SetPrevID( id );
}

void CAht::UnlinkModel( int id )
{
	//		モデルのリンクを解除
	int next_id;
	AHTMODEL *model;
	AHTMODEL *model2;
	model = GetModel( id );
	if ( model == NULL ) return;
	next_id = model->GetNextID();
	if ( next_id < 0 ) return;

	model2 = GetModel( next_id );
	if ( model2 == NULL ) return;

	model->SetNextID( -1 );
	model2->SetPrevID( -1 );
}

int CAht::BuildGlobalIDSub( char *fname, char *pname, int i )
{
	int j,myid;
	AHTMODEL *m;
	j = i + 1; myid = 0;
	while(1) {								// 同じファイル、パスを探す
		if ( j >= model_cnt ) break;
		m = GetModel( j );
		if ( m != NULL ) {
			if ( m->GetGlobalId() == -1 ) {
				if ( tstrcmp( m->GetSource(), fname ) ) {
					if ( tstrcmp( m->GetSourcePath(), pname ) ) {
						myid++;
						m->SetGlobalId( myid );
					}
				}
			}
		}
		j++;
	}
	return myid;
}

void CAht::BuildGlobalID( void )
{
	//		モデルのグローバルIDの更新
	//
	int i,myid;
	AHTMODEL *model;
	for(i=0;i<model_cnt;i++) {
		model = GetModel( i );
		if ( model != NULL ) { model->SetGlobalId( -1 ); }
	}
	for(i=0;i<model_cnt;i++) {
		model = GetModel( i );
		if ( model != NULL ) {
			if ( model->GetGlobalId() == -1 ) {
				myid = BuildGlobalIDSub( model->GetSource(), model->GetSourcePath(), i );
				if ( myid ) { model->SetGlobalId( 0 ); }
			}
		}
	}
}


char *CAht::SearchModelByClassName( char *clsname )
{
	//		クラス名(前方一致)からモデルをリストアップ
	//
	int i;
	char tmp[64];
	AHTMODEL *model;

	if ( objlist != NULL ) delete objlist;
	objlist = new CMemBuf;

	for(i=0;i<model_cnt;i++) {
		model = GetModel( i );
		if ( model != NULL ) {
			if ( strncmp( clsname, model->GetClass(), strlen(clsname) ) == 0 ) {
				sprintf( tmp, "%d:" , i );
				objlist->PutStr( tmp );
				objlist->PutStr( model->GetName() );
				objlist->PutCR();
			}
		}
	}
	objlist->Put(0);
	return objlist->GetBuffer();
}


void CAht::FindModelStart( void )
{
	//		モデル検索開始(スクリプト生成用)
	//
	findmode = AHTMODELFIND_MODE_START;
	parentid = -1;
}


int CAht::FindModel( void )
{
	//		モデル検索実行(スクリプト生成用)
	//		(有効なモデルIDを返す、-1ならば終了)
	//
	int i;
	AHTMODEL *model;
refind:
	switch( findmode ) {
	case AHTMODELFIND_MODE_START:			// 検索の初期化
		for(i=0;i<model_cnt;i++) {
			model = GetModel( i );if ( model != NULL ) {
				model->ClearFindCheck();
			}
		}
		//		そのまま次に

	case AHTMODELFIND_MODE_ARRAYSEEK:		// リンクの先頭を調べる
		for(i=0;i<model_cnt;i++) {
			model = GetModel( i );if ( model != NULL ) {
				if (( model->GetNextID() != -1 )&&( model->GetPrevID() == -1 )) {
					if ( model->GetFindCheck() == false ) {
						model->SetFindCheck();
						findmode = AHTMODELFIND_MODE_ARRAYPICK;
						findid = i;
						parentid = i;
						return i;
					}
				}
			}
		}
		findmode = AHTMODELFIND_MODE_LEFTPICK;
		goto refind;

	case AHTMODELFIND_MODE_ARRAYPICK:		// リンクを辿る

		model = GetModel( findid );
		if ( model == NULL ) {
			findmode = AHTMODELFIND_MODE_ARRAYSEEK; goto refind;	// ありえないはず
		}
		findid = model->GetNextID();
		if ( findid == -1 ) {
			findmode = AHTMODELFIND_MODE_ARRAYSEEK; goto refind;
		}
		model = GetModel( findid );
		model->SetFindCheck();
		return findid;

	case AHTMODELFIND_MODE_LEFTPICK:		// その他モデル検索
		for(i=0;i<model_cnt;i++) {
			model = GetModel( i );if ( model != NULL ) {
				if ( model->GetFindCheck() == false ) {
					model->SetFindCheck();
					parentid = i;
					return i;
				}
			}
		}
		break;

	default:
		break;
	}
	return -1;
}


void CAht::UpdateModelProperty( int id )
{
	//		プロパティの参照データを更新する
	//
	int i;
	int target;
	int a;
	AHTMODEL *model;
	AHTMODEL *m;
	AHTPROP *p;
	AHTPROP *p2;
	char *res;
	CStrNote note;
	char *list;
	char tmp[256];

	model = GetModel( id );
	if ( model == NULL ) return;
	target = id;

	for(i=0;i<model->GetPropCount();i++) {
		p = model->GetProperty( i );
		switch( p->ahttype ) {
		case AHTTYPE_PARTS_INT:
			res = p->newval;
			if ( res == NULL ) {
				list = SearchModelByClassName( p->defval2 );
				note.Select( list );
				note.GetLine( tmp, 0, 255 );
				p->SetNewVal( tmp );
			}
			target = p->GetValueInt();
			break;
		case AHTTYPE_PARTS_PROP_STRING:
			a = target;if ( *p->defval3 == 'm' ) a = id;				// 自分を参照するオプション
			m = GetModel( a );
			if ( m != NULL ) {
				p2 = m->GetProperty( p->defval2 );
				//Alertf( "%d:%d:%s:%s:%x",i,a,p->defval, p->defval2,p2 );
				if ( p2 != NULL ) {
					res = p2->GetValue();
					p->SetNewVal( res );
				}
			}
			break;
		case AHTTYPE_PARTS_OPT_STRING:
			a = target;if ( *p->defval3 == 'm' ) a = id;				// 自分を参照するオプション
			m = GetModel( a );
			if ( m != NULL ) {
				res = m->GetAHTOption( p->defval2 );
				if ( res != NULL ) {
					p->SetNewVal( res );
				}
			}
			break;
		default:
			break;
		}
	}
}


//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CAht::CAht( void )
{
	stdbuf = new CMemBuf( 0x1000 );		// debug message buffer
	Mesf( "AHT processor ready.\r\n" );
	mem_ahtmodel = NULL;				// model data
	model_cnt = 0;
	mem_ahtmodel_size = 0;
	mem_parts = NULL;
	objlist = NULL;
	ahtwrt_buf = NULL;
	ahtini_buf = NULL;

	Reset();
}


CAht::~CAht( void )
{
	if ( objlist != NULL ) delete objlist;
	if ( stdbuf != NULL ) delete stdbuf;

	DisposeMakeBuffer();
	DisposeModel();
	DisposeParts();
}


char *CAht::GetStdBuffer( void )
{
	return stdbuf->GetBuffer();
}


int CAht::LoadProject( char *fname )
{
	FILE *fp;
	char *p;
	int res;
	int bufsize,strsize;

	Reset();

	res = 0;
	fp=fopen( fname, "rb" );
	if (fp == NULL) return -1;

	fread( &hed, 1, sizeof(HTPHED), fp );

	if (( hed.h1 != HTP_MAGIC1 )||
	    ( hed.h2 != HTP_MAGIC2 )||
		( hed.h3 != HTP_MAGIC3 )||
		( hed.h4 != HTP_MAGIC4 )) {
		fclose(fp);
		return -2;
	}

	bufsize = hed.modsize;
	strsize = hed.strsize;

	curpage = hed.curpage;
	maxpage = hed.maxpage;

	objbuf = new CMemBuf;
	strbuf = new CMemBuf;

	if ( bufsize ) {
		p = objbuf->PreparePtr( bufsize );
		fread( p, 1, bufsize, fp );
	}
	if ( strsize ) {
		p = strbuf->PreparePtr( strsize );
		fread( p, 1, strsize, fp );
	}

	fclose(fp);
	return res;
}


int CAht::LoadProjectApply( int modelid, int fileid )
{
	HTPOBJ *obj;
	AHTPROP *prop;
	AHTMODEL *model;

	model = GetModel( modelid );
	if ( model == NULL ) return -1;

	obj = GetProjectFileObject( fileid );
	model->SetObj( (AHTOBJ *)GetProjectFileString( obj->ahtobj ) );
	obj++;

	//			プロパティ全設定
	while(1) {
		if ( obj->ahtsource != -1 ) break;
		//Alertf( "%s=%s", GetProjectFileString( obj->propname), GetProjectFileString( obj->defvalue )  );
		prop = model->GetProperty( GetProjectFileString( obj->propname) );
		if ( prop != NULL ) {
			prop->SetNewVal( GetProjectFileString( obj->defvalue ) );
		}
		//model->SetPropertyDefault( GetProjectFileString( obj->propname), GetProjectFileString( obj->defvalue )  );
		obj++;
	}
	return 0;
}


void CAht::LoadProjectEnd( void )
{
	delete objbuf;
	delete strbuf;
}


int CAht::GetProjectFileModelMax( void )
{
	int i;
	int res;
	HTPOBJ *obj;
	obj = (HTPOBJ *)(objbuf->GetBuffer());

	res = 0;
	for(i=0;i<hed.max_mod;i++) {
		if ( obj->ahtsource >= 0 ) res++;
		obj++;
	}
	return res;
}


char *CAht::GetProjectFileModel( int id )
{
	HTPOBJ *obj;
	obj = GetProjectFileObject( id );
	return GetProjectFileString( obj->ahtsource );
}


char *CAht::GetProjectFileModelPath( int id )
{
	HTPOBJ *obj;
	obj = GetProjectFileObject( id );
	return GetProjectFileString( obj->propname );
}


int CAht::GetProjectFileModelID( int id )
{
	HTPOBJ *obj;
	obj = GetProjectFileObject( id );
	return obj->defvalue;
}


char *CAht::GetProjectFileString( int ptr )
{
	char *p;
	p = (char *)(strbuf->GetBuffer());
	p += ptr;
	return p;
}


HTPOBJ *CAht::GetProjectFileObject( int id )
{
	int i;
	int res;
	HTPOBJ *obj;

	obj = (HTPOBJ *)(objbuf->GetBuffer());
	if ( id < 0 ) return NULL;

	res = 0;
	for(i=0;i<hed.max_mod;i++) {
		if ( obj->ahtsource >= 0 ) {
			if ( id == res ) return obj;
			res++;
		}
		obj++;
	}
	return NULL;
}


void CAht::SaveProjectSub( AHTMODEL *model )
{
	int i;
	HTPOBJ obj;
	AHTPROP *prop;
	if ( model == NULL ) return;
	if ( model->GetFlag() == AHTMODEL_FLAG_NONE ) return;

	//Alertf( "[%s][%s]",model->GetSourcePath(),model->GetSource() );

	//		マスターHTPOBJを書き込む
	//
	obj.ahtobj = strbuf->GetSize();
	strbuf->PutData( model->GetObj(), sizeof(AHTOBJ) );
	obj.ahtsource = strbuf->GetSize();
	strbuf->PutStrBlock( model->GetSource() );
	obj.propname = strbuf->GetSize();			// マスターHTPOBJのみpropnameにpathを入れる
	strbuf->PutStrBlock( model->GetSourcePath() );
	obj.defvalue = model->GetId();				// マスターHTPOBJのみdefvalueにObjectIDを入れる

	objbuf->PutData( &obj, sizeof(HTPOBJ) );
	hed.max_mod++;

	//		サブHTPOBJを書き込む
	//		(サブHTPOBJは、ahtsourceが-1で判断する)
	//
	for(i=0;i<model->GetPropCount();i++) {

		prop = model->GetProperty( i );

		obj.propname = strbuf->GetSize();
		strbuf->PutStrBlock( prop->orgname );

		obj.defvalue = strbuf->GetSize();
		strbuf->PutStrBlock( prop->GetValue() );

		obj.ahtsource = -1;

		objbuf->PutData( &obj, sizeof(HTPOBJ) );
		hed.max_mod++;
	}
}


int CAht::SaveProject( char *fname )
{
	FILE *fp;
	HTPOBJ *obj;
	int i,res;
	int bufsize,strsize;

	strbuf = new CMemBuf;
	objbuf = new CMemBuf;

	hed.h1 = HTP_MAGIC1;
	hed.h2 = HTP_MAGIC2;
	hed.h3 = HTP_MAGIC3;
	hed.h4 = HTP_MAGIC4;
	hed.ver = 0x1000;
	hed.max_mod = 0;

	//	Send Modules
	//
	if ( mem_ahtmodel_size ) {
		for(i=0;i<model_cnt;i++) {
			SaveProjectSub( mem_ahtmodel[i] );
		}
	}

	//	Output file
	//
	res = 0;
	fp=fopen( fname, "wb" );
	if (fp != NULL) {

		strsize = strbuf->GetSize() & 15;
		if ( strsize > 0 ) {				// strbufを切りのいいサイズにする
			for(i=0;i<(16-strsize);i++) { strbuf->Put( (char)0 ); }
		}

		bufsize = objbuf->GetSize();
		strsize = strbuf->GetSize();
		obj = (HTPOBJ *)objbuf->GetBuffer();

		hed.modtable = sizeof(HTPHED);
		hed.strtable = sizeof(HTPHED) + bufsize;
		hed.modsize = bufsize;
		hed.strsize = strsize;
		hed.curpage = curpage;
		hed.maxpage = maxpage;

		hed.opt1 = 0;
		hed.opt2 = 0;
		hed.opt3 = 0;
		hed.opt4 = 0;

		fwrite( &hed, sizeof(HTPHED), 1, fp );
		if ( bufsize ) fwrite( obj, bufsize, 1, fp );
		if ( strsize ) fwrite( strbuf->GetBuffer(), strsize, 1, fp );

		fclose(fp);

	} else {
		res = -1;
	}

	delete objbuf;
	delete strbuf;
	return res;
}


void CAht::SetPage( int cur, int max )
{
	curpage = cur;
	maxpage = max;
}


int CAht::GetCurrentPage( void )
{
	return curpage;
}


int CAht::GetMaxPage( void )
{
	return maxpage;
}


void CAht::DisposeParts( void )
{
	if ( mem_parts != NULL ) {
		mem_bye( mem_parts );
		mem_parts = NULL;
	}
}


void CAht::PickLineBuffer( char *out )
{
	int a;
	int dq;
	char a1;
	a = 0;
	dq = 0;
	while(1) {
		a1 = linebuf[pickptr];
		if ( a1 == 0 ) break;
		if (( a1 != 9 )&&( a1 != 32 )) break;
		pickptr++;
	}
	if ( a1 == 0x22 ) { dq = 1; pickptr++; }

	while(1) {
		a1 = linebuf[pickptr];
		if (( a1 == 0 )||( a1 == 10 )||( a1 == 13 )) break;
		pickptr++;
		if (( a1 == 9 )||( a1 == 32 )) break;
		if ( dq ) {
			if ( a1 == 0x22 ) break;
		}
		out[a++]=a1;
		if ( a1 & 128 ) {
			a1 = linebuf[pickptr++];
			out[a++]=a1;
		}
	}
	out[a]=0;
}


int CAht::BuildPartsSub( int id, char *fname )
{
	//		簡易ahtパース
	//
	int i,res,maxline;
	CStrNote note;
	CMemBuf tmp;
	AHTPARTS *p;
	char s1[256];

	p = GetParts( id );
	if ( p == NULL ) return -2;

	p->classname[0] = 0;
	p->name[0] = 0;
	p->icon = 0;

	res = tmp.PutFile( fname );
	if ( res < 0 ) {
		return -1;
	}
	note.Select( tmp.GetBuffer() );
	maxline = note.GetMaxLine();
	getpath( fname, p->name, 1+8+16 );			// 仮にファイル名を入れておく

	for(i=0;i<maxline;i++) {
		pickptr = 0;
		note.GetLine( linebuf, i, 255 );
		PickLineBuffer( s1 );
		if ( tstrcmp( s1,"#aht" ) ) {
			PickLineBuffer( s1 );
			if ( tstrcmp( s1,"iconid" ) ) {
				PickLineBuffer( s1 );
				p->icon = atoi( s1 );
			}
			if ( tstrcmp( s1,"name" ) ) {
				PickLineBuffer( s1 );
				strcpy( p->name, s1 );
			}
			if ( tstrcmp( s1,"class" ) ) {
				PickLineBuffer( s1 );
				strcpy( p->classname, s1 );
			}
		}
	}

	return 0;
}


int CAht::BuildParts( char *list, char *path )
{
	int i;
	char fullpath[256];
	char fname[256];
	CStrNote note;

	note.Select( list );
	maxparts = note.GetMaxLine();
	DisposeParts();
	mem_parts = (AHTPARTS *)mem_ini( sizeof(AHTPARTS) * maxparts );
	for(i=0;i<maxparts;i++) {
		note.GetLine( fname, i, 255 );
		strcpy( fullpath, path );
		strcat( fullpath, fname );
		//Alertf( "#%d [%s]",i, fullpath );
		BuildPartsSub( i, fullpath );
	}
	return maxparts;
}


AHTPARTS *CAht::GetParts( int id )
{
	if (( id<0 )||( id >= maxparts )) return NULL;
	return &mem_parts[id];
}


char *CAht::GetPartsName( int id )
{
	return GetParts(id)->name;
}


char *CAht::GetPartsClassName( int id )
{
	return GetParts(id)->classname;
}


int CAht::GetPartsIconID( int id )
{
	return GetParts(id)->icon;
}


//-------------------------------------------------------------
//		Routines for Make Buffer
//-------------------------------------------------------------

void CAht::InitMakeBuffer( void )
{
	DisposeMakeBuffer();
	ahtwrt_buf = new CMemBuf;
	ahtini_buf = new CMemBuf;
}


void CAht::DisposeMakeBuffer( void )
{
	if ( ahtwrt_buf != NULL ) { delete ahtwrt_buf; ahtwrt_buf = NULL; }
	if ( ahtini_buf != NULL ) { delete ahtini_buf; ahtini_buf = NULL; }
}


int CAht::SaveMakeBuffer( char *fname )
{
	//		初期化スクリプトバッファ+スクリプトバッファを保存
	//
	int res;
	ahtwrt_buf->Put( 0 );							// 終端を登録
	ahtini_buf->PutStr( ahtwrt_buf->GetBuffer() );
	res = ahtini_buf->SaveFile( fname );
	if ( res < 0 ) return -1;
	return 0;
}


void CAht::AddMakeBufferInit( char *str, int size )
{
	//		初期化スクリプトバッファに追加(重複は無視される)
	//
	int i,sz,len;
	char *p;
	len = size;
	if ( size <= 0 ) {
		len = strlen(str);
	}
	if ( len ) {
		sz = ahtini_buf->GetSize() - len;
		p = ahtini_buf->GetBuffer();
		for(i=0;i<sz;i++) {
			if ( strncmp( str, p, len ) == 0 ) return;
			p++;
		}
		if ( size <= 0 ) {
			ahtini_buf->PutStr( str );
		} else {
			ahtini_buf->PutData( str, size );
		}
	}
	ahtini_buf->PutCR();
}


void CAht::AddMakeBufferMain( char *str, int size )
{
	//		スクリプトバッファに追加
	//
	if ( size <= 0 ) {
		ahtwrt_buf->PutStr( str );
	} else {
		ahtwrt_buf->PutData( str, size );
	}
	ahtwrt_buf->PutCR();
}

