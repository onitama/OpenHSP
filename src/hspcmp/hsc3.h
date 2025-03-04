
//
//	hsc3.cpp structures
//
#ifndef __hsc3_h
#define __hsc3_h

#define HSC3TITLE "HSP script preprocessor"
#define HSC3TITLE2 "HSP code generator"

#define HSC3_OPT_NOHSPDEF 1
#define HSC3_OPT_DEBUGMODE 2
#define HSC3_OPT_MAKEPACK 4
#define HSC3_OPT_READAHT 8
#define HSC3_OPT_MAKEAHT 16
#define HSC3_OPT_UTF8IN 32		// UTF8ソースを入力
#define HSC3_OPT_UTF8OUT 64		// UTF8コードを出力

#define HSC3_MODE_DEBUG 1
#define HSC3_MODE_DEBUGWIN 2
#define HSC3_MODE_UTF8 4		// UTF8コードを出力
#define HSC3_MODE_STRMAP 8		// strmapを出力
#define HSC3_MODE_LABOUT 16		// キーワードを出力

class CMemBuf;
class CToken;

/*
	rev 54
	lb_info の型を void * から CLabel * に変更。

	hsc3.cpp:207
	mingw : warning : void * 型の delete は未定義
	に対処。
*/

class CLabel;

// HSC3 class
class CHsc3 {
public:
	CHsc3();
	~CHsc3();
	char *GetError( void );
	int GetErrorSize( void );
	void ResetError( void );
	int PreProcess( char *fname, char *outname, int option, char *rname, void *ahtoption=NULL );
	int PreProcessAht( char *fname, void *ahtoption, int mode=0 );
	void PreProcessEnd( void );
	int Compile(char* fname, char* outname, int mode);
	int CompileStrMap(char* fname, char* outname, int mode);
	int CompileLabelOut(char* fname, int mode);
	void SetCommonPath( char *path );

	//		Service
	void Print(char* mes);
	int GetCmdList(int option, char *match=NULL);
	int OpenPackfile( void );
	void ClosePackfile( void );
	void GetPackfileOption( char *out, char *keyword, char *defval );
	int GetPackfileOptionInt( char *keyword, int defval );
	int GetRuntimeFromHeader( char *fname, char *res );
	int SaveOutbuf( char *fname );
	int SaveAHTOutbuf( char *fname );

	//		Analyse
	void InitAnalysisInfo(int mode, char* match, int line=0 );
	char* GetAnalysisInfo(void);
	int GetAnalysisInfoSize(void);
	void DeleteAnalysisInfo(void);
	char* GetAnalysisLineInfo(int type);

	//		Data
	//
	CMemBuf *errbuf;
	CMemBuf *pfbuf;
	CMemBuf *addkw;
	CMemBuf *outbuf;
	CMemBuf* ahtbuf;
	CMemBuf* anabuf;

private:
	//		Private Data
	//
	int process_option;
	void AddSystemMacros( CToken *tk, int option );

	char common_path[512];			// common path

	//		for Header info
	int hed_option;
	char hed_runtime[64];

	//		for Compile Optimize
	int cmpopt;
	CLabel *lb_info;

	//		for Analyse
	int analyse_mode;
	int analyse_line;
	char* analyse_match;
	int analyse_caseflag;
	char analyse_module[256];
	char analyse_keyword[256];
};


#endif
