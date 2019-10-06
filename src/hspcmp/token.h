
//
//	token.cpp structures
//
#ifndef __token_h
#define __token_h

#include <vector>
#include <string>
#include <map>
#include <memory>

// token type
#define TK_NONE 0
#define TK_OBJ 1
#define TK_STRING 2
#define TK_DNUM 3
#define TK_NUM 4
#define TK_CODE 6
#define TK_LABEL 7
#define TK_VOID 0x1000
#define TK_SEPARATE 0x1001
#define TK_EOL 0x1002
#define TK_EOF 0x1003
#define TK_ERROR -1
#define TK_CALCERROR -2
#define TK_CALCSTOP -3

#define DUMPMODE_RESCMD 3
#define DUMPMODE_DLLCMD 4
#define DUMPMODE_ALL 15

#define CMPMODE_PPOUT 1
#define CMPMODE_OPTCODE 2
#define CMPMODE_CASE 4
#define CMPMODE_OPTINFO 8
#define CMPMODE_PUTVARS 16
#define CMPMODE_VARINIT 32
#define CMPMODE_OPTPRM 64
#define CMPMODE_SKIPJPSPC 128
#define CMPMODE_UTF8OUT 256

#define CG_FLAG_ENABLE 0
#define CG_FLAG_DISABLE 1

#define CG_LASTCMD_NONE 0
#define CG_LASTCMD_LET 1
#define CG_LASTCMD_CMD 2
#define CG_LASTCMD_CMDIF 3
#define CG_LASTCMD_CMDMIF 4
#define CG_LASTCMD_CMDELSE 5
#define CG_LASTCMD_CMDMELSE 6

#define CG_IFLEV_MAX 128
#define CG_REPLEV_MAX 128

// option for 'GetTokenCG'
#define GETTOKEN_DEFAULT 0
#define GETTOKEN_NOFLOAT 1		// '.'を小数点と見なさない(整数のみ取得)
#define GETTOKEN_LABEL 2		// '*'に続く名前をラベルとして取得
#define GETTOKEN_EXPRBEG 4		// 式の先頭

#define CG_LOCALSTRUCT_MAX 256

#define CG_IFCHECK_SCOPE 0
#define CG_IFCHECK_LINE 1

#define CG_LIBMODE_NONE -1
#define CG_LIBMODE_DLL 0
#define CG_LIBMODE_DLLNEW 1
#define CG_LIBMODE_COM 2
#define CG_LIBMODE_COMNEW 3

#define	CALCVAR double

#define LINEBUF_MAX 0x10000

// line mode type
#define LMODE_ON 0
#define LMODE_STR 1
#define LMODE_COMMENT 2
#define LMODE_OFF 3

// macro default data storage
typedef struct MACDEF {
	int		index[32];				// offset to data
	char	data[1];
} MACDEF;

// module related define
#define OBJNAME_MAX 60
#define MODNAME_MAX 20

#define COMP_MODE_DEBUG 1
#define COMP_MODE_DEBUGWIN 2
#define COMP_MODE_UTF8 4
#define COMP_MODE_STRMAP 8

#define SWSTACK_MAX 32

#define HEDINFO_RUNTIME 0x1000		// 動的ランタイムを有効にする
#define HEDINFO_NOMMTIMER 0x2000	// マルチメディアタイマーを無効にする
#define HEDINFO_NOGDIP 0x4000		// GDI+による描画を無効にする
#define HEDINFO_FLOAT32 0x8000		// 実数を32bit floatとして処理する
#define HEDINFO_ORGRND 0x10000		// 標準の乱数発生を使用する
#define HEDINFO_UTF8 0x20000		// UTF8ランタイムを使用する(コード識別用)
#define HEDINFO_HSP64 0x40000		// 64bitランタイムを使用する(コード識別用)
#define HEDINFO_IORESUME 0x80000	// ファイルI/Oエラーを無視して処理を続行する
#define HEDINFO_AUTOTIMER 0x100000	// マルチメディアタイマーを強制的に設定にする

enum ppresult_t {
	PPRESULT_SUCCESS,				// 成功
	PPRESULT_ERROR,					// エラー
	PPRESULT_UNKNOWN_DIRECTIVE,		// 不明なプリプロセッサ命令（PreprocessNM）
	PPRESULT_INCLUDED,				// #include された
	PPRESULT_WROTE_LINE,			// 1行書き込まれた
	PPRESULT_WROTE_LINES,			// 2行以上書き込まれた
};

class CLabel;
class CMemBuf;
class CTagStack;
class CStrNote;
class AHTMODEL;

#define SCNVBUF_DEFAULTSIZE 0x8000
#define SCNV_OPT_NONE 0
#define SCNV_OPT_SJISUTF8 1

//  token analysis class
class CToken {
public:
	CToken();
	CToken( char *buf );
	~CToken();
	CLabel *GetLabelInfo( void );
	void SetLabelInfo( CLabel *lbinfo );

	void Error( char *mes );
	void LineError( char *mes, int line, char *fname );
	void SetError( char *mes );
	void Mes( char *mes );
	void Mesf( char *format, ...);
	void SetErrorBuf( CMemBuf *buf );
	void SetAHT( AHTMODEL *aht );
	void SetAHTBuffer( CMemBuf *aht );

	void ResetCompiler( void );
	int GetToken( void );
	int PeekToken( void );
	int Calc( CALCVAR &val );
	char *CheckValidWord( void );
	

	//		For preprocess
	//
	ppresult_t Preprocess( char *str );
	ppresult_t PreprocessNM( char *str );
	void PreprocessCommentCheck( char *str );

	int ExpandLine( CMemBuf *buf, CMemBuf *src, char *refname );
	int ExpandFile( CMemBuf *buf, char *fname, char *refname );
	void FinishPreprocess( CMemBuf *buf );
	void SetCommonPath( char *path );
	int SetAdditionMode( int mode );

	void SetLook( char *buf );
	char *GetLook( void );
	char *GetLookResult( void );
	int GetLookResultInt( void );
	int LabelRegist( char **list, int mode );
	int LabelRegist2( char **list );
	int LabelRegist3( char **list );
	int LabelDump( CMemBuf *out, int option );
	int GetLabelBufferSize( void );
	int RegistExtMacroPath( char *name, char *str );
	int RegistExtMacro( char *name, char *str );
	int RegistExtMacro( char *keyword, int val );
	void SetPackfileOut( CMemBuf *pack );
	int AddPackfile( char *name, int mode );

	void InitSCNV( int size );
	char *ExecSCNV( char *srcbuf, int opt );
	int CheckByteSJIS( unsigned char byte );
	int CheckByteUTF8( unsigned char byte );
	int SkipMultiByte( unsigned char byte );


	//		For Code Generate
	//
	int GenerateCode( char *fname, char *oname, int mode );
	int GenerateCode( CMemBuf *srcbuf, char *oname, int mode );

	void PutCS( int type, int value, int exflg );
	void PutCSSymbol( int label_id, int exflag );
	int GetCS( void );
	void PutCS( int type, double value, int exflg );
	int PutOT( int value );
	int PutDS( double value );
	int PutDS( char *str );
	int PutDSStr( char *str, bool converts_to_utf8 );
	int PutDSBuf( char *str );
	int PutDSBuf( char *str, int size );
	char *GetDS( int ptr );
	void SetOT( int id, int value );
	void PutDI( void );
	void PutDI( int dbg_code, int a, int subid );
	void PutDIVars( void );
	void PutDILabels( void );
	void PutDIParams( void );
	void PutHPI( short flag, short option, char *libname, char *funcname );
	int PutLIB( int flag, char *name );
	void SetLIBIID( int id, char *clsid );
	int PutStructParam( short mptype, int extype );
	int PutStructParamTag( void );
	void PutStructStart( void );
	int PutStructEnd( char *name, int libindex, int otindex, int funcflag );
	int PutStructEnd( int i, char *name, int libindex, int otindex, int funcflag );
	int PutStructEndDll( char *name, int libindex, int subid, int otindex );

	void CalcCG( int ex );

	int GetHeaderOption( void ) { return hed_option; }
	char *GetHeaderRuntimeName( void ) { return hed_runtime; }
	void SetHeaderOption( int opt, char *name ) { hed_option=opt; strcpy( hed_runtime, name ); }
	int GetCmpOption( void ) { return hed_cmpmode; }
	void SetCmpOption( int cmpmode ) { hed_cmpmode = cmpmode; }
	void SetUTF8Input( int utf8mode ) { pp_utf8 = utf8mode; }

private:
	//		For preprocess
	//
	void Pickstr( void );
	char *Pickstr2( char *str );
	void Calc_token( void );
	void Calc_factor( CALCVAR &v );
	void Calc_unary( CALCVAR &v );
	void Calc_muldiv( CALCVAR &v );
	void Calc_addsub( CALCVAR &v );
	void Calc_bool( CALCVAR &v );
	void Calc_bool2( CALCVAR &v );
	void Calc_compare( CALCVAR &v );
	void Calc_start( CALCVAR &v );

	ppresult_t PP_Define( void );
	ppresult_t PP_Const( void );
	ppresult_t PP_Enum( void );
	ppresult_t PP_SwitchStart( int sw );
	ppresult_t PP_SwitchEnd( void );
	ppresult_t PP_SwitchReverse( void );
	ppresult_t PP_Include( int is_addition );
	ppresult_t PP_Module( void );
	ppresult_t PP_Global( void );
	ppresult_t PP_Deffunc( int mode );
	ppresult_t PP_Defcfunc( int mode );
	ppresult_t PP_Struct( void );
	ppresult_t PP_Func( char *name );
	ppresult_t PP_Cmd( char *name );
	ppresult_t PP_Pack( int mode );
	ppresult_t PP_PackOpt( void );
	ppresult_t PP_RuntimeOpt( void );
	ppresult_t PP_CmpOpt( void );
	ppresult_t PP_Usecom( void );
	ppresult_t PP_Aht( void );
	ppresult_t PP_Ahtout( void );
	ppresult_t PP_Ahtmes( void );
	ppresult_t PP_BootOpt( void );

	void SetModuleName( char *name );
	char *GetModuleName( void );
	void AddModuleName( char *str );
	void FixModuleName( char *str );
	int IsGlobalMode( void );
	int CheckModuleName( char *name );

	char *SkipLine( char *str, int *pline );
	char *ExpandStr( char *str, int opt );
	char *ExpandStrEx( char *str );
	char *ExpandStrComment( char *str, int opt );
	char *ExpandStrComment2( char *str );
	char *ExpandAhtStr( char *str );
	char *ExpandBin( char *str, int *val );
	char *ExpandHex( char *str, int *val );
	char *ExpandToken( char *str, int *type, int ppmode );
	int ExpandTokens( char *vp, CMemBuf *buf, int *lineext, int is_preprocess_line );
	char *SendLineBuf( char *str );
	char *SendLineBufPP( char *str, int *lines );
	int ReplaceLineBuf( char *str1, char *str2, char *repl, int macopt, MACDEF *macdef );

	void SetErrorSymbolOverdefined(char* keyword, int label_id);

	//		For Code Generate
	//
	int GenerateCodeMain( CMemBuf *src );
	void RegisterFuncLabels( void );
	int GenerateCodeBlock( void );
	int GenerateCodeSub( void );
	void GenerateCodePP( char *buf );
	void GenerateCodeCMD( int id );
	void GenerateCodeLET( int id );
	void GenerateCodeVAR( int id, int ex );
	void GenerateCodePRM( void );
	void GenerateCodePRMN( void );
	int GenerateCodePRMF( void );
	void GenerateCodePRMF2( void );
	void GenerateCodePRMF3( void );
	int GenerateCodePRMF4( int t );
	void GenerateCodeMethod( void );
	void GenerateCodeLabel( char *name, int ex );

	void GenerateCodePP_regcmd( void );
	void GenerateCodePP_cmd( void );
	void GenerateCodePP_deffunc0( int is_command );
	void GenerateCodePP_deffunc( void );
	void GenerateCodePP_defcfunc( void );
	void GenerateCodePP_uselib( void );
	void GenerateCodePP_module( void );
	void GenerateCodePP_struct( void );
	void GenerateCodePP_func( int deftype );
	void GenerateCodePP_usecom( void );
	void GenerateCodePP_comfunc( void );
	void GenerateCodePP_defvars( int fixedvalue );

	int GetParameterTypeCG( char *name );
	int GetParameterStructTypeCG( char *name );
	int GetParameterFuncTypeCG( char *name );
	int GetParameterResTypeCG( char *name );

	char *GetTokenCG( char *str, int option );
	char *GetTokenCG( int option );
	char *GetSymbolCG( char *str );
	char *GetLineCG( void );
	char *PickStringCG( char *str, int sep );
	char *PickStringCG2( char *str, char **strsrc );
	char *PickLongStringCG( char *str );
	int PickNextCodeCG( void );
	void CheckInternalListenerCMD(int opt);
	void CheckInternalProgCMD( int opt, int orgcs );
	void CheckInternalIF( int opt );
	void CheckCMDIF_Set( int mode );
	void CheckCMDIF_Fin( int mode );

	int SetVarsFixed( char *varname, int fixedvalue );

	void CalcCG_token( void );
	void CalcCG_token_exprbeg( void );
	void CalcCG_token_exprbeg_redo( void );
	void CalcCG_regmark( int mark );
	void CalcCG_factor( void );
	void CalcCG_unary( void );
	void CalcCG_muldiv( void );
	void CalcCG_addsub( void );
	void CalcCG_shift( void );
	void CalcCG_bool( void );
	void CalcCG_compare( void );
	void CalcCG_start( void );

	bool CG_optCode() const { return (hed_cmpmode & CMPMODE_OPTCODE) != 0; }
	bool CG_optInfo() const { return (hed_cmpmode & CMPMODE_OPTINFO) != 0; }
	void CG_MesLabelDefinition(int label_id);

	int	SaveStringMap(char* fname);

	//		Data
	//
	CLabel *lb;						// label object
	CLabel *tmp_lb;					// label object (preprocessor reference)
	CTagStack *tstack;				// tag stack object
	CMemBuf *errbuf;
	CMemBuf *wrtbuf;
	CMemBuf *packbuf;
	CMemBuf *ahtbuf;
	CStrNote *note;
	AHTMODEL *ahtmodel;				// AHT process data
	char common_path[HSP_MAX_PATH];	// common path
	char search_path[HSP_MAX_PATH];	// search path

	int line;
	int val;
	int ttype;						// last token type
	int texflag;
	char *lasttoken;				// last token point
	float val_f;
	double val_d;
	double fpbit;
	unsigned char *wp;
	unsigned char s2[1024];
	unsigned char *s3;
	char linebuf[LINEBUF_MAX];		// Line expand buffer
	char linetmp[LINEBUF_MAX];		// Line expand temp
	char errtmp[128];				// temp for error message
	char mestmp[128];				// meseage temp
	int incinf;						// include level
	int mulstr;						// multiline string flag
	short swstack[SWSTACK_MAX];		// generator sw stack (flag)
	short swstack2[SWSTACK_MAX];	// generator sw stack (mode)
	short swstack3[SWSTACK_MAX];	// generator sw stack (sw)
	int swsp;						// generator sw stack pointer
	int swmode;						// generator sw mode (0=if/1=else)
	int swlevel;					// first stack level ( when off )
	int fileadd;					// File Addition Mode (1=on)
	int swflag;						// generator sw enable flag
	char *ahtkeyword;				// keyword for AHT

	char modname[MODNAME_MAX+2];	// Module Name Prefix
	int	modgc;						// Global counter for Module
	int enumgc;						// Global counter for Enum
	typedef struct undefined_symbol_t {
		int pos;
		int len_include_modname;
		int len;
	} undefined_symbol_t;
	std::vector<undefined_symbol_t> undefined_symbols;
	int cs_lastptr;					// パラメーターの初期CS位置
	int cs_lasttype;				// パラメーターのタイプ(単一時)
	int calccount;					// パラメーター個数
	int pp_utf8;					// ソースコードをUTF-8として処理する(0=無効)

	//		for CodeGenerator
	//
	int cg_flag;
	int cg_debug;
	int cg_iflev;
	int cg_valcnt;
	int cg_typecnt;
	int cg_pptype;
	int cg_locallabel;
	int cg_varhpi;
	int cg_putvars;
	int cg_defvarfix;
	int cg_utf8out;
	int cg_strmap;
	char *cg_ptr;
	char *cg_ptr_bak;
	char *cg_str;
	unsigned char *cg_wp;
	char cg_libname[1024];

	int	replev;
	int repend[CG_REPLEV_MAX];
	int iflev;
	int iftype[CG_IFLEV_MAX];
	int ifmode[CG_IFLEV_MAX];
	int ifscope[CG_IFLEV_MAX];
	int ifptr[CG_IFLEV_MAX];
	int ifterm[CG_IFLEV_MAX];

	int cg_lastcmd;
	int cg_lasttype;
	int cg_lastval;
	int cg_lastcs;

	CMemBuf *cs_buf;
	CMemBuf *ds_buf;
	CMemBuf *ot_buf;
	CMemBuf *di_buf;

	CMemBuf *li_buf;
	CMemBuf *fi_buf;
	CMemBuf *mi_buf;
	CMemBuf *fi2_buf;
	CMemBuf *hpi_buf;

	//		for Header info
	int hed_option;
	char hed_runtime[64];
	int hed_cmpmode;
	int hed_autoopt_timer;
	int hed_autoopt_strexchange;

	//		for Struct
	int	cg_stnum;
	int	cg_stsize;
	int cg_stptr;
	int cg_libindex;
	int cg_libmode;
	int cg_localstruct[CG_LOCALSTRUCT_MAX];
	int cg_localcur;

	//		for Error
	//
	int cg_errline;
	int cg_orgline;
	char cg_orgfile[HSP_MAX_PATH];

	//		for SCNV
	//
	char *scnvbuf;			// SCNV変換バッファ
	int	scnvsize;			// SCNV変換バッファサイズ

};


#endif
