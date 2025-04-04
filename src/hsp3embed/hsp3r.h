
//
//	hsp3r.cpp header
//
#ifndef __hsp3r_h
#define __hsp3r_h

#include "../hsp3/hsp3debug.h"
#include "../hsp3/hsp3struct.h"
#include "../hsp3/hsp3code.h"
#include "../hsp3/stack.h"
#include "hspvar_util.h"

//	HSP3r class
//
class Hsp3r {
public:
	//	Functions
	//
	Hsp3r( void );
	~Hsp3r( void );
	void Dispose( void );						// HSP3Rの破棄
	int Reset( int ext_vars, int ext_hpi );		// HSP3Rの初期化を行なう
	void SetPackValue( int sum, int dec );		// packfile用の設定データを渡す
	void SetFileName( char *name );				// axファイル名を指定する
	void SetDataName( char *data );				// Data Segment設定
	void SetFInfo( STRUCTDAT *finfo, int finfo_max );	// FInfo設定
	void SetMInfo( STRUCTPRM *minfo, int minfo_max );	// MInfo設定
	void SetLInfo( LIBDAT *linfo, int linfo_max );		// LInfo設定

	//	Data
	//
	HSPHED hsphed;
	HSPCTX hspctx;
	int	maxvar;
	int max_varhpi;
	int hsp_sum, hsp_dec;

private:
};


#endif
