
//
//	CHsp3.cpp structures
//
#ifndef __CHsp3_h
#define __CHsp3_h

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/hsp3struct.h"
#include "../hspcmp/localinfo.h"
#include "../hspcmp/label.h"
#include "membuf.h"
#include "csstack.h"
#include "supio.h"

#define HSP3CNV_VERSION "3.5b5"

#define MAX_IFLEVEL 128			// ifのネスト読み出し最大レベル
#define VAREXP_BUFFER_MAX 1024	// 配列要素の読み出し用バッファ最大サイズ
#define OUTMES_BUFFER_MAX 65536	// cpp用一時出力バッファ最大サイズ

//	HSP3ライブラリ用定義
//
typedef struct HSP3RLIBCNF {

	//		HSP3RLIB上で動作させるために必要な設定
	//
	int		version;			// version number info
	int		max_val;			// max count of VAL Object
	short	max_hpi;			// size of HPIDAT(3.0)
	short	max_varhpi;			// Num of Vartype Plugins(3.0)
	int		bootoption;			// bootup options


} HSP3RLIBCNF;

//	HSP3解析コンテキスト用定義
//
typedef struct MCSCONTEXT {

	//		オブジェクトコード解析に必要な設定
	//
	unsigned short *mcs;				// CS Code read pointer
	unsigned short *mcs_last;			// CS Code read pointer (original)
	int cstype;
	int csval;
	int exflag;

} MCSCONTEXT;


//	HSP3(.ax)操作用クラス
//
class CHsp3 {
public:
	CHsp3();
	~CHsp3();

	void NewObjectHeader( void );
	int LoadObjectFile( char *fname );
	void DeleteObjectHeader( void );
	int SaveOutBuf( char *fname );
	int SaveOutBuf2( char *fname );
	char *GetOutBuf( void );

	//		Data Output
	//
	void OutMes( char *format, ... );
	void OutLine( char *format, ... );
	void OutLineBuf( CMemBuf *outbuf, char *format, ... );
	void OutCR( void );
	void SetIndent( int val );

	//		Data Retrieve
	//
	LIBDAT *GetLInfo( int index );
	STRUCTDAT *GetFInfo( int index );
	STRUCTPRM *GetMInfo( int index );

	int GetOTCount( void );
	int GetLInfoCount( void );
	int GetFInfoCount( void );
	int GetFInfo2Count( void );
	int GetMInfoCount( void );

	void initCS( void *ptr );
	int getCS( void );
	int getNextCS( int *type );
	int getEXFLG( void );
	char *GetDS( int offset );
	char *GetDS_fmt( int offset );
	double GetDSf( int offset );
	int GetOT( int index );
	int GetOTInfo( int index );

	void GetContext( MCSCONTEXT *ctx );
	void SetContext( MCSCONTEXT *ctx );

	//		Test Function
	//
	void MakeObjectInfo( void );
	void MakeProgramInfo( void );
	int CheckExtLibrary( void );

	//		Virtual Function
	//
	virtual int MakeSource( int option, void *ref );

protected:
	//		Settings
	//
	CMemBuf *buf;
	CMemBuf *out;
	CMemBuf *out2;
	HSPHED *hsphed;


	CMemBuf *mem_cs;
	CMemBuf *mem_ds;
	CMemBuf *mem_ot;
	CMemBuf *dinfo;

	CMemBuf *linfo;
	CMemBuf *finfo;
	CMemBuf *minfo;
	CMemBuf *finfo2;
	CMemBuf *hpidat;

	CMemBuf *ot_info;

	unsigned short *mcs;				// CS Code read pointer
	unsigned short *mcs_last;			// CS Code read pointer (original)
	unsigned short *mcs_start;			// CS Code start pointer
	unsigned short *mcs_end;			// CS Code end pointer
	unsigned char *mem_di_val;			// Debug VALS info ptr
	int cstype;
	int csval;
	int exflag;

	char orgname[HSP_MAX_PATH];
	CLocalInfo localinfo;

	//	for Program Trace
	//
	CLabel *lb;							// label object
	char hspoptmp[4];
	char hspvarmp[16];
	int iflevel;
	int ifmode[MAX_IFLEVEL];
	unsigned short *ifptr[MAX_IFLEVEL];
	int iftaskid[MAX_IFLEVEL];
	int indent;
	char strtmp[4096];

	//		Private function
	//
	int UpdateValueName( void );
	void MakeOTInfo( void );

	int MakeProgramInfoParam( void );
	int MakeProgramInfoParam2( void );
	void MakeProgramInfoLabel( void );
	void MakeProgramInfoFuncParam( int structid );
	void MakeProgramInfoHSPName( bool putadr = true );
	int MakeImmidiateName( char *mes, int type, int val );
	int MakeImmidiateHSPName( char *mes, int type, int val, char *opt = NULL );
	void MakeHspStyleString( char *str, CMemBuf *eout );
	void MakeHspStyleString2( char *str, char *dst );

	char *GetHSPOperator( int val );
	char *GetHSPOperator2( int val );
	char *GetHSPName( int type, int val );
	char *GetHSPVarName( int varid );
	char *GetHSPVarTypeName( int type );
	char *GetHSPCmdTypeName( int type );
	int GetHSPExpression( CMemBuf *eout );
	int GetHSPVarExpression( char *mes );

};

#endif
