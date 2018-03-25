
/*----------------------------------------------------------------*/
//		local info related routines
/*----------------------------------------------------------------*/

#ifdef HSPWIN
#define USE_WINDOWS_API		// WINDOWS APIを使用する
#endif


#ifdef USE_WINDOWS_API
#include <windows.h>
#endif

#ifdef HSPLINUX
#include <sys/time.h>
#include <time.h>
#endif

#include <stdio.h>
#include <string.h>
#include "localinfo.h"

//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CLocalInfo::CLocalInfo()
{
}


CLocalInfo::~CLocalInfo()
{
}


//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

int CLocalInfo::GetTime( int index )
{
/*
	Get system time entries
	index :
	    0 wYear
	    1 wMonth
	    2 wDayOfWeek
	    3 wDay
	    4 wHour
	    5 wMinute
	    6 wSecond
	    7 wMilliseconds
*/
#ifdef USE_WINDOWS_API
	SYSTEMTIME st;
	short *a;
	GetLocalTime( &st );
	a=(short *)&st;
	return (int)(a[index]);
#endif
#ifdef HSPLINUX
	struct timeval tv;
	struct tm *lt;

	gettimeofday( &tv, NULL );	// MinGWだとVerによって通りません
	lt = localtime( &tv.tv_sec );

	switch( index ) {
	case 0:
		return lt->tm_year+1900;
	case 1:
		return lt->tm_mon+1;
	case 2:
		return lt->tm_wday;
	case 3:
		return lt->tm_mday;
	case 4:
		return lt->tm_hour;
	case 5:
		return lt->tm_min;
	case 6:
		return lt->tm_sec;
	case 7:
		return (int)tv.tv_usec/10000;
	case 8:
		/*	一応マイクロ秒まで取れる	*/
		return (int)tv.tv_usec%10000;
	}
#endif
	return 0;
}


char *CLocalInfo::CurrentTime( void )
{
	sprintf( curtime, "\"%02d:%02d:%02d\"",
		GetTime(4),GetTime(5),GetTime(6) );
	return curtime;
}


char *CLocalInfo::CurrentDate( void )
{
	sprintf( curdate, "\"%04d/%02d/%02d\"",
		GetTime(0),GetTime(1),GetTime(3) );
	return curdate;
}

