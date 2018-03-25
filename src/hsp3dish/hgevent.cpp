
//
//		High performance Graphic Image access (event)
//			onion software/onitama 2004/12
//
#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif

#ifdef HSPWIN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "hgevent.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		hgevent interface
*/
/*------------------------------------------------------------*/

hgevent::hgevent( void )
{
	Reset();
}


hgevent::~hgevent( void )
{
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgevent::Reset( void )
{
	id = 0;
	flag = HGEVENT_FLAG_NONE;
	timer = 0.0f;
	target = 0;
	source = HGEVENT_SOURCE_VEC;
	value = 0;
	nextvec = NULL;
	SetEventVector( 0.0f, 0.0f, 0.0f );
	SetEventVector2( 0.0f, 0.0f, 0.0f );
	SetNextEvent( NULL );
}

