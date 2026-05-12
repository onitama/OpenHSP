
//
//	cezdb.cpp structures
//
#ifndef __cezdb_h
#define __cezdb_h

enum {
CEZDB_MODE_NONE = 0,
CEZDB_MODE_ERROR,
CEZDB_MODE_ENVREADY,
CEZDB_MODE_DBCREADY,
CEZDB_MODE_SQLFETCH,
CEZDB_MODE_MAX
};

#define CEZDB_OUTBUFSIZE 1024

//
//	cezdb.cpp functions
//

class cezdb {
public:
	cezdb();
	~cezdb();
	int Connect( char *dsn, int mode );
	void Disconnect( void );
	int SendSQL( char *str );
	int GetResult( char *buf, int size );
	char *GetResultBuffer( void );
	int GetMode( void );
	void SetSeparator( int chr );

private:
	//		Settings
	//
	int	flag;
	void *henv;
	void *hdbc;
	void *hstmt;
	unsigned char *ResultOut;
	unsigned char *StringOut;
	char sepchr;
};


#endif
