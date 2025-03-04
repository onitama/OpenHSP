
//
//	CHsp3cpp.cpp structures
//
#ifndef __CHsp3cpp_h
#define __CHsp3cpp_h

#include "chsp3.h"

#define CPPHED_HSPVAR "Var_"

//		CHSP3 Task callback function
//
typedef void (* CHSP3_TASK) (void);

//		Param analysis info
//
struct PRMAINFO {
	int	num;						// number of stack
	int lasttype;					// last value type(TYPE_*)
	int lastval;					// last value (int)
};

#define MAX_PRMAINFO 128			// パラメーター保持バッファ最大数
#define MAX_CALCINFO 512			// パラメーター演算スタックの最大数

//	HSP3(.ax)->C++(.cpp) conversion class
//
class CHsp3Cpp : public CHsp3 {
public:

	int MakeSource( int option, void *ref );

private:
	//		Settings
	//
	int makeoption;
	int tasknum;
	int curot;						// 追加用のタスク(ラベル)テーブルID
	int curprmindex;				// 現在のパラメーター先頭インデックス
	int curprmlocal;				// 現在のローカル変数スタック数
	int prmcnv_locvar[64];			// パラメーター変換用バッファ(ローカル変数用)
	int curprm_type, curprm_val;	// 最後に評価したtype,val値
	int curprm_stack;				// パラメータースタック数

	CMemBuf *prmbuf;				// パラメーター解析・変換用バッファ
	int	maxprms;					// パラメーター解析結果(全パラメーター数)
	PRMAINFO prma[MAX_PRMAINFO];	// パラメーター解析結果

	//		Internal Function
	//
	int MakeCPPMain( void );
	void MakeCPPLabel( void );
	void MakeCPPTask( int nexttask );
	void MakeCPPTask2( int nexttask, int newtask );
	void MakeCPPTask( char *funcdef, int nexttask=-1 );
	int MakeCPPParam( int addprm=0 );
	void MakeCppStyleString( char *str, char *dst );
	char *GetDS_cpp( int offset );

	void MakeCPPSub( int cmdtype, int cmdval );
	void MakeCPPSubModCmd( int cmdtype, int cmdval );
	void MakeCPPSubProgCmd( int cmdtype, int cmdval );
	void MakeCPPSubExtCmd( int cmdtype, int cmdval );
	void OutputCPPParam( void );
	void DisposeCPPParam( void );

	int GetCPPExpression( CMemBuf *eout, int *result );
	int GetCPPExpressionSub( CMemBuf *eout );
	int AnalysisCPPCalcParam( PRMAINFO *cp1, PRMAINFO *cp2, CMemBuf *eout, int op );
	int AnalysisCPPCalcInt( int prm1, int prm2, int op );
	double AnalysisCPPCalcDouble( double prm1, double prm2, int op, int *result );

	int MakeCPPVarForHSP( void );
	void MakeCPPVarName( char *outbuf, int varid );
	int MakeCPPVarExpression( CMemBuf *arname, bool flag_array = false );
	int MakeImmidiateCPPName( char *mes, int type, int val, char *opt=NULL );
	void MakeCPPProgramInfoFuncParam( int structid );
	int MakeCPPParamForVar( char *varname, int va, char *arrayname );

	int	GetVarFixedType( int varid );
};


#endif
