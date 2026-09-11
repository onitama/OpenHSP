
//
//	membuf.cpp structures
//
#ifndef __membuf_h
#define __membuf_h

#include <string>

//  growmem class

/*
	rev 53
	mingw : warning : クラスは仮想関数を持つのに仮想デストラクタでない。
	に対処。
*/

class CMemBuf {
public:
	CMemBuf();
	CMemBuf( int sz );

	char *InitSubBuffer( int sz );
	void ExchangeSubToMainBuffer(int size);

	virtual ~CMemBuf();
	void AddIndexBuffer( void );
	void AddIndexBuffer( int sz );

	char *GetBuffer( void );
	int GetBufferSize( void );
	int *GetIndexBuffer( void );
	void SetIndex( int idx, int val );
	int GetIndex( int idx );
	int GetIndexBufferSize( void );
	int SearchIndexValue( int val );
	int SearchIndexedData(char *data, int size);

	void RegistIndex( int val );
	void Index( void );
	void IndexExclusive( void );
	bool Put( int data );
	bool Put( short data );
	bool Put( char data );
	bool Put( unsigned char data );
	bool Put( float data );
	bool Put( double data );
	bool PutStr( const char *data );
	bool PutStrDQ( char *data );
	bool PutStrBlock( char *data );
	bool PutCR( void );
	bool PutData( void *data, int sz );
	bool PutStrf( char *format, ... );
	int PutFile( const char *fname );
	int SaveFile( const char *fname );
	char *GetFileName( void );
	int GetSize( void ) { return cur; }
	void ReduceSize( int new_cur );
	bool TryPreparePtr( int sz, char **result );
	char *PreparePtr( int sz );

private:
	virtual void InitMemBuf( int sz );
	virtual void InitIndexBuf( int sz );

	//		Data
	//
	int		limit_size;			// Separate size
	int		size;				// Main Buffer Size
	int		cur;				// Current Size
	char	*mem_buf;			// Main Buffer

	int		idxflag;			// index Mode Flag
	int		*idxbuf;			// Index Buffer
	int		idxmax;				// Index Buffer Max
	int		curidx;				// Current Index

	char	*sub_buf;			// Sub Buffer

	std::string name;			// File Name
};


#endif
