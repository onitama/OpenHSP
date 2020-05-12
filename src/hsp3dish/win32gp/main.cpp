/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
									  2011/5  onitama
  --------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "hsp3dish.h"

/*----------------------------------------------------------*/

#ifndef HSP3DLL

int APIENTRY WinMain ( HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPSTR lpCmdParam,
					   int iCmdShow )
{
	int res;
	res = app_init();
	if (res) return res;

#ifdef HSPDEBUG
	res = hsp3dish_init( hInstance, lpCmdParam, NULL);
#else
	res = hsp3dish_init( hInstance, NULL, NULL);
#endif
	if ( res == 0 ) {
		res = hsp3dish_reset();
		if (res == 0) {
			res = hsp3dish_exec();
			hsp3dish_bye();
		}
	}
	app_bye();
	return res;
}

#else

#define EXPORT extern "C" __declspec (dllexport)

static int hsp3dish_flag;
static int hsp3dish_quit;
static int hsp3dish_result;
static HINSTANCE m_hInst;

static bool threadflag;
static HANDLE hevent;
static HANDLE hthread;
static DWORD threadid;

DWORD WINAPI vThreadFunc(LPVOID pParam) {

	// ストリームスレッドループ
	threadflag = true;
	hsp3dish_reset();
	hsp3dish_result = hsp3dish_exec();
	hsp3dish_bye();

	threadflag = false;
	return 0;
}

static int StartThread(void)
{
	// スレッドを開始する
	threadflag = false;

	// イベントオブジェクトを作成する
	hevent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// スレッドを生成する
	hthread = CreateThread(NULL, 0x40000, (LPTHREAD_START_ROUTINE)vThreadFunc, (LPVOID)NULL, 0, &threadid);
	if (hthread == NULL) {
		return -1;
	}
	// スレッドの優先順位を変更する
	SetThreadPriority(hthread, THREAD_PRIORITY_TIME_CRITICAL);
	return 0;
}

static int StopThread(void)
{
	// スレッドを停止する
	if (threadflag == FALSE) return -1;

	// イベントオブジェクト破棄
	threadflag = FALSE;
	SetEvent(hevent);
	CloseHandle(hevent);
	hevent = NULL;
	// スレッド停止を待つ
	DWORD	dActive = 0;
	GetExitCodeThread(hthread, &dActive);
	if (dActive == STILL_ACTIVE) {
		WaitForSingleObject(hthread, INFINITE);
	}
	// スレッド破棄
	CloseHandle(hthread);

	hthread = NULL;
	threadid = 0;

	return 0;
}





int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		hsp3dish_flag = 0;
		hsp3dish_quit = 0;
		m_hInst = hInstance;
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
		if (hsp3dish_flag) {
			hsp3dish_bye();
			hsp3dish_flag = 0;
		}
	}
	return TRUE;
}

EXPORT BOOL WINAPI hsp3dll_init(int p1, int p2, int p3, int p4)
{
	//		HSP初期化
	//		p1: bit0=window非表示
	//			bit1=カレントディレクトリ変更禁止
	//		p2: X size
	//		p3: Y size
	//		p4: 親window handle
	//
	int i;
	HWND parent = (HWND)p4;
	hsp3dish_setwindow(p1, p2, p3 );
	i = hsp3dish_init(m_hInst,"test1.ax",parent);
	if (i) return -1;
	hsp3dish_flag = 1;
	return 0;
}


EXPORT BOOL WINAPI hsp3dll_exec(int p1, int p2, int p3, void *p4)
{
	//		HSPのパラメーター設定/取得
	//		p1 : 0     = default X,Y (p2,p3)
	//		     1     = seed1,seed2 (p2,p3)
	//		     0x100 = HSPCTX (->p4)
	//		     0x101 = ADDFUNC (<-p4)
	//		     0x102 = filename (<-p4)
	//
	int res;
	hsp3dish_quit = 0;
	res = StartThread();
//	res = hsp3dish_exec();
//	hsp3dish_bye();
	if (res) return -res;
	return 0;
}


EXPORT BOOL WINAPI hsp3dll_quit(int p1, int p2, int p3, int p4)
{
	//		HSP終了処理
	//
	hsp3dish_quit = 1;
	return 0;
}


EXPORT BOOL WINAPI hspexec(int p1, int p2, int p3, int p4)
{
	return 0;
}




#endif



