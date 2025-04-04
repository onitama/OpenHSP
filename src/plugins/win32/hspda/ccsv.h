
//
//	ccsv.cpp structures
//
#ifndef __ccsv_h
#define __ccsv_h

#define FLAGMAX 256
#define CCSV_FLAG_NONE 0
#define CCSV_FLAG_ENABLE 1

#define CCSV_OPT_NOCASE 1
#define CCSV_OPT_ANDMATCH 2
#define CCSV_OPT_ZENKAKU 4
#define CCSV_OPT_ADDLINE 8
#define CCSV_OPT_EXPRESSION 128

class ccsv {
public:
	ccsv();
	ccsv( char *ptr );
	void SetBuffer( char *ptr );
	void SetResultBuffer( char *ptr, int size );
	void SetSeparate( char chr );
	void SetFlag( int id, int val );
	void AddResult( char *str );
	int Search( char *search, int max, int start );
	int SearchSub( char *search );
	void SetOption( int val );
	void StrCnvZ2H( char *src, char *result );
	void StrCnvLC( char *src );
	void CsvCopy( char *target, char *src );
	char *StrStrZH( char *target, char *src );
	~ccsv();
private:
	//		Settings
	//
	int opt;
	char *buf;
	char *resbuf;
	char *resp;
	char *resmax;
	int ressize;
	int cline;
	int cres;
	int startid;
	int rescnt;
	int maxres;
	int flagsel;
	char flag[FLAGMAX];
	char spchr;
	char chkstr[0x8000];
	char orgstr[0x8000];
};


#endif
