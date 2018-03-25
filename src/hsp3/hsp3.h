
//
//	hsp3.cpp header
//
#ifndef __hsp3_h
#define __hsp3_h

#include "hsp3debug.h"
#include "hsp3struct.h"
#include "hsp3ext.h"
#include "hsp3code.h"

#define HSP3_AXTYPE_NONE 0
#define HSP3_AXTYPE_ENCRYPT 1

//	HSP3 class
//
class Hsp3 {
public:
	//	Functions
	//
	Hsp3( void );
	~Hsp3( void );
	void Dispose( void );						// HSP axの破棄
	int Reset( int mode );						// HSP axの初期化を行なう
	void SetPackValue( int sum, int dec );		// packfile用の設定データを渡す
	void SetFileName( char *name );				// axファイル名を指定する

	//	Data
	//
	HSPCTX hspctx;
	char *axname;
	char *axfile;
	int	maxvar;
	int hsp_sum, hsp_dec;
	int axtype;									// axファイルの設定(hsp3imp用)

private:

	void *copy_DAT(char* ptr, size_t size);
	LIBDAT *copy_LIBDAT(HSPHED *hsphed, char *ptr, size_t size);
	STRUCTDAT *copy_STRUCTDAT(HSPHED *hsphed, char *ptr, size_t size);


};


#endif
