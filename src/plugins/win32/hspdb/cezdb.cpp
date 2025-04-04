
//
//		Easy Database(ODBC) access
//			onion software/onitama 2003/3
//
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <sql.h>
#include <sqlext.h>

#include "cezdb.h"

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

cezdb::cezdb( void )
{
	RETCODE res;
	flag = CEZDB_MODE_NONE;
	henv = NULL;
	StringOut = NULL;
	ResultOut = NULL;
	sepchr = ',';
	res = SQLAllocEnv( &henv );
	if ( res != SQL_SUCCESS ) {
		flag = CEZDB_MODE_ERROR;
		return;
	}
	//		Env Ready
	//
	StringOut = (unsigned char *)malloc ( CEZDB_OUTBUFSIZE );
	if ( StringOut == NULL ) return;
	ResultOut = (unsigned char *)malloc ( CEZDB_OUTBUFSIZE );
	if ( ResultOut == NULL ) return;
	//
	flag = CEZDB_MODE_ENVREADY;
}


cezdb::~cezdb( void )
{
	Disconnect();

	if ( henv != NULL ) {
		SQLFreeEnv( henv );
		henv = NULL;
	}
	if ( ResultOut != NULL ) {
		free( ResultOut );
		ResultOut = NULL;
	}
	if ( StringOut != NULL ) {
		free( StringOut );
		StringOut = NULL;
	}
	flag = CEZDB_MODE_NONE;
}


int cezdb::Connect( char *dsn, int mode )
{
	RETCODE res;

	if ( flag != CEZDB_MODE_ENVREADY ) return -1;

	res = SQLAllocConnect( henv, &hdbc );
	if ( res != SQL_SUCCESS ) {
		flag = CEZDB_MODE_ERROR;
		return -2;
	}

	switch( mode ) {
	case 1:
		res = SQLDriverConnect( hdbc, 0, (unsigned char *)dsn, SQL_NTS, StringOut, CEZDB_OUTBUFSIZE, NULL, SQL_DRIVER_COMPLETE_REQUIRED );
		break;
//	case 2:
//		res = SQLBrowseConnect( hdbc,(unsigned char *)dsn, SQL_NTS,StringOut, CEZDB_OUTBUFSIZE, &len );
	default:
		res = SQLDriverConnect( hdbc, 0, (unsigned char *)dsn, SQL_NTS, StringOut, CEZDB_OUTBUFSIZE, NULL, SQL_DRIVER_NOPROMPT );
		break;
	}

	if ( res != SQL_SUCCESS ) {
		SQLFreeConnect( hdbc );
		flag = CEZDB_MODE_ERROR;
		return -3;
	}
	//MessageBox( NULL, (char *)StringOut, "WOW", 0 );
	
	//		DBC Ready
	//
	flag = CEZDB_MODE_DBCREADY;
	return 0;
}


void cezdb::Disconnect( void )
{
	if ( flag >= CEZDB_MODE_DBCREADY ) {
		if ( flag == CEZDB_MODE_SQLFETCH ) SQLFreeStmt( hstmt, SQL_DROP );
		SQLDisconnect( hdbc );
		SQLFreeConnect( hdbc );
		flag = CEZDB_MODE_ENVREADY;
	}
}


int cezdb::SendSQL( char *str )
{
	RETCODE res;
	if ( flag != CEZDB_MODE_DBCREADY ) return -1;

	res = SQLAllocStmt( hdbc, &hstmt );
	if ( res != SQL_SUCCESS ) {
		return -2;
	}
//	strcpy( (char *)sendsql, str );
//	res = SQLExecDirect( hstmt, sendsql, strlen( (char *)sendsql ) );
	res = SQLExecDirect( hstmt, (unsigned char *)str, SQL_NTS );
	if (( res != SQL_SUCCESS )&&( res != SQL_SUCCESS_WITH_INFO )) {
		return -3;
	}

	flag = CEZDB_MODE_SQLFETCH;
	return 0;
}


int cezdb::GetResult( char *buf, int size )
{
	RETCODE res;
	int leftsize;
	int column;
	short maxcol;
	int sz;
	long len;
	char *p;
	
	if ( flag != CEZDB_MODE_SQLFETCH ) return -1;

	res = SQLFetch( hstmt );
	if (( res != SQL_SUCCESS )&&( res != SQL_SUCCESS_WITH_INFO )) {
		SQLFreeStmt( hstmt, SQL_DROP );
		flag = CEZDB_MODE_DBCREADY;
		return -2;
	}
	column = 1;
	res = SQLNumResultCols( hstmt, &maxcol );
	if ( res != SQL_SUCCESS ) { maxcol = 1; }
	leftsize = size;
	p = buf;
	*p = 0;
	while(1) {
		if ( column > maxcol ) break;
		if ( leftsize < 1 ) break;
		res = SQLGetData( hstmt, column, SQL_C_CHAR, p, leftsize, &len );
		if (( res != SQL_SUCCESS )&&( res != SQL_SUCCESS_WITH_INFO )) { break; }
		column++;
		len = strlen( p );
		p += len;
		leftsize -= len;

		// セパレータを追加
		if ( leftsize < 1 ) break;
		*p++ = sepchr;
		leftsize --;
	  }
	*p = 0;
	sz = strlen( buf );
	if ( sz ) {
		if ( buf[ sz-1 ] == sepchr ) buf[sz-1] = 0;
	}
	return sz;
}


char *cezdb::GetResultBuffer( void )
{
	if ( GetResult( (char *)ResultOut, CEZDB_OUTBUFSIZE ) < 0 ) return NULL;
	return (char *)ResultOut;
}


int cezdb::GetMode( void )
{
	return flag;
}


void cezdb::SetSeparator( int chr )
{
	sepchr = (char)chr;
}
