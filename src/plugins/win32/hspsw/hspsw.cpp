
//
//		HSP DB support DLL ( for ver2.6 )
//				onion software/onitama 2003/3
//

#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>

#include "hspsteam.h"
#include "hspdll.h"

static void Alertf(char *format, ...)
{
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	MessageBox(NULL, textbf, "error", MB_ICONINFORMATION | MB_OK);
}

/*------------------------------------------------------------*/

static hspsteam *_hspsteam = NULL;

static void sw_term(void)
{
	if (_hspsteam == NULL) return;
	delete _hspsteam;
	_ShutdownSteam();
}

static int sw_init(void)
{
	sw_term();

	if (_PrepareSteam()) return -1;

	_hspsteam = new hspsteam;
	return 0;
}


/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
	}
	return TRUE ;
}

char *getvptr( HSPEXINFO *hei, PVal **pval, int *size )
{
	//		変数ポインタを得る
	//
	APTR aptr;
	PDAT *pdat;
	HspVarProc *proc;
	aptr = hei->HspFunc_prm_getva( pval );
	proc = hei->HspFunc_getproc( (*pval)->flag );
	(*pval)->offset = aptr;
	pdat = proc->GetPtr( *pval );
	return (char *)proc->GetBlockSize( *pval, pdat, size );
}

/*
static int valsize( PVAL2 *pv )
{
	//		calc object memory used size
	//			result : size(byte)
	int i,j;
	int vm=1;
	i=1;
	while(1) {
		j=pv->len[i];if (j==0) break;
		vm*=j;i++;if (i==5) break;
	}
	return vm<<2;
}
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI steaminit( int p1, int p2, int p3, int p4 )
{
	//	DLL steaminit (type$00)
	//
	int res;
	res = sw_init();
	if (p1 & 1) _hspsteam->setUTF8Mode(1);
	return res;
}


EXPORT BOOL WINAPI steambye(int p1, int p2, int p3, int p4)
{
	//	DLL steambye (type$00)
	//
	sw_term();
	return 0;
}


EXPORT BOOL WINAPI steamunlock_achievement(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamunlock_achievement id (type$202)
	//
	int res;
	int ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_getdi(0);	// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) _hspsteam->unlockAchievement(ep1);

	return res;
}


EXPORT BOOL WINAPI steamunlock_achievementkey(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamunlock_achievementkey "apikey" (type$202)
	//
	int res;
	char *ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_gets();		// パラメータ1:文字列
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) _hspsteam->unlockAchievement(ep1);

	return res;
}


EXPORT BOOL WINAPI steamclear_achievement(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamclear_achievement (type$202)
	//
	int res;
	int ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_getdi(0);	// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) _hspsteam->clearAchievement(ep1);

	return res;
}


EXPORT BOOL WINAPI steamclear_achievementkey(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamclear_achievementkey "apikey" (type$202)
	//
	int res;
	char *ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_gets();		// パラメータ1:文字列
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) _hspsteam->clearAchievement(ep1);

	return res;
}


EXPORT BOOL WINAPI steamset_max(int p1, int p2, int p3, int p4)
{
	//	DLL steamset_max (type$00)
	//
	if (_hspsteam) _hspsteam->setAchivementMax(p1);
	return 0;
}

EXPORT BOOL WINAPI steamupdate(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//	DLL steamupdate option (type$00)
	//
	PVal *pv;
	APTR ap;
	int p1, p2;
	ap = hei->HspFunc_prm_getva(&pv);	// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	if (_hspsteam) {
		_hspsteam->update();
		p2 = _hspsteam->flag;
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &p2);	// 変数に値を代入
	}
	else {
		return -1;
	}
	return 0;
}

EXPORT BOOL WINAPI steamreq_status(int p1, int p2, int p3, int p4)
{
	//	DLL steamset_max (type$00)
	//
	if (_hspsteam) _hspsteam->requestAchivement();
	return 0;
}

EXPORT BOOL WINAPI steamget_achievement(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_achievement var, index  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1, p2;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	if (_hspsteam) {
		p2 = _hspsteam->getAchivementFlag(p1);
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &p2);	// 変数に値を代入
	}
	else {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI steamget_achievementstr(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_achievementstr var, index, type  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1,p2;
	char *res;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	p2 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	if (_hspsteam) {
		res = _hspsteam->getAchivementString( p1, p2 );
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, res);	// 変数に値を代入
	}
	else {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI steamreg_achievement(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamreg_achievement index,"APINAME" (type$202)
	//
	int res;
	int ep1;
	char *ep2 = "";
	res = 0;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) _hspsteam->setAchivementId(ep1, ep2);

	return res;
}


EXPORT BOOL WINAPI steamset_achievement(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamset_achievement option (type$202)
	//
	int res;
	int ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) {
		res = _hspsteam->setAchivementAuto();
	}
	return -res;
}



EXPORT BOOL WINAPI steamget_status(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_status var, "apikey"  (type$202)
	//
	PVal *pv;
	APTR ap;
	char *ep1 = "";
	int res,ep2;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	if (_hspsteam) {
		ep2 = 0;
		res = _hspsteam->getStatInt(ep1,&ep2);
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &ep2);	// 変数に値を代入
		if (res) return -1;
	}
	else {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI steamget_statusf(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_statusf var, "apikey"  (type$202)
	//
	PVal *pv;
	APTR ap;
	char *ep1 = "";
	int res;
	double ep2;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	if (_hspsteam) {
		ep2 = 0;
		res = _hspsteam->getStatDouble(ep1, &ep2);
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_DOUBLE, &ep2);	// 変数に値を代入
		if (res) return -1;
	}
	else {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI steamset_status(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamset_status "name",value (type$202)
	//
	char *ep1;
	int ep2;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) {
		_hspsteam->updateStatusInt(ep1, ep2);
	}
	return 0;
}


EXPORT BOOL WINAPI steamset_statusf(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamset_statusf "name",value (type$202)
	//
	char *ep1;
	double ep2;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdd(0.0);		// パラメータ2:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) {
		_hspsteam->updateStatusDouble(ep1, ep2);
	}
	return 0;
}


EXPORT BOOL WINAPI steamreq_leaderboard(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamreq_leaderboard "name",type (type$202)
	//
	char *ep1;
	int ep2;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	if (_hspsteam) {
		_hspsteam->request_Leaderboard(ep1, ep2);
	}
	return 0;
}


EXPORT BOOL WINAPI steamreq_leaderboarddata(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamreq_leaderboarddata type,start,end (type$202)
	//
	int res,ep1,ep2,ep3;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(1);		// パラメータ1:数値
	ep3 = hei->HspFunc_prm_getdi(10);		// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	res = -1;
	if (_hspsteam) {
		res = _hspsteam->request_LeaderboardData(ep1, ep2, ep3);
	}
	return res;
}


EXPORT BOOL WINAPI steamget_leaderboardmax(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_leaderboardmax var  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p2;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	if (*hei->er) return *hei->er;		// エラーチェック
	p2 = 0;
	if (_hspsteam) {
		p2 = _hspsteam->getLeaderboardMax();
	}
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &p2);	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI steamget_leaderboard(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_leaderboard var, type, index  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1, p2;
	int res = 0;
	char stemp[256];
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	p2 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	if (*hei->er) return *hei->er;		// エラーチェック
	if (p1 & 16) {
		*stemp = 0;
		if (_hspsteam) {
			_hspsteam->getLeaderboardString(stemp, p1 & 15, p2);
		}
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, stemp);	// 変数に値を代入
	}
	else {
		if (_hspsteam) {
			res = _hspsteam->getLeaderboardValue(p1, p2);
		}
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &res);	// 変数に値を代入
	}
	return 0;
}


EXPORT BOOL WINAPI steamget_name(HSPEXINFO *hei, int _p1, int _p2, int _p3)
{
	//
	//		steamget_name var,p1  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1;
	char stemp[256];
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	if (*hei->er) return *hei->er;		// エラーチェック
	*stemp = 0;
	if (_hspsteam) {
		_hspsteam->getPlayerName(stemp, p1);
	}
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, stemp);	// 変数に値を代入
	return 0;
}



EXPORT BOOL WINAPI steamset_leaderboarddata(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	DLL steamset_leaderboarddata score,option (type$202)
	//
	int res, ep1, ep2;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	res = -1;
	if (_hspsteam) {
		res = _hspsteam->update_LeaderboardData(ep1, ep2);
	}
	return res;
}


