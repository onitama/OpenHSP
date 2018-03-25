
//
//		OMKE Draw Support
//		onion software/onitama 2009/6
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../hsp3/hsp3config.h"
#include "../../hsp3/hsp3debug.h"
#include "../supio.h"
#include "../hspwnd.h"
#include "../hgio.h"

#include "omkedraw.h"

static BMSCR *mem_bmscr;
static int linecnt;
static float mem_ofsx;
static float mem_ofsy;

/*------------------------------------------------------------------------------------*/

void OmkeDrawSetBMSCR( void *bmscr )
{
	mem_bmscr = (BMSCR *)bmscr;
	//Alertf( "Init OmkeDraw:" );
}


void OmkeDrawLineStart( int col )
{
	mem_bmscr->color = col;
	linecnt = 0;
//	gb_color( (col>>16)&255, (col>>8)&255, col&255 );
//	gb_lineStart();
}


void OmkeDrawLineEnd( void )
{
	hgio_line( NULL, 0.0f, 0.0f );
//	gb_lineEnd();
}


void OmkeDrawLineStrip( int cx, int cy )
{
	if ( linecnt ) {
		hgio_line2( (float)cx, (float)cy );
	} else {
		hgio_line( mem_bmscr, (float)cx, (float)cy );
	}
	linecnt++;
//	gb_lineStrip( cx, cy );
}


void OmkeDrawLine( int cx, int cy, int xx,int yy )
{
//	gb_line( cx, cy, xx, yy );
}


void OmkeDrawCircle( int x,int y,int range, int col )
{
	mem_bmscr->color = col;
	hgio_circle( mem_bmscr, (float)x, (float)y, (float)(x+range), (float)(y+range), 1 );
//	gb_color( (col>>16)&255, (col>>8)&255, col&255 );
//	gb_circle( x, y, x+range, y+range, 1 );
}


void OmkeDrawSetGMode( int gmode, int grate, int ofsx, int ofsy )
{
	mem_bmscr->gmode = gmode;
	mem_bmscr->gfrate = grate;
	mem_ofsx = (float)ofsx;
	mem_ofsy = (float)ofsy;
}


void OmkeDrawCelPut( int cx, int cy, void *bm_src, int id, float destx, float desty, float ang )
{
	//		セルをコピー(変倍・回転)
	//
	BMSCR *src;
	int xx,yy,texpx,texpy,psx,psy;
	float dsx,dsy;

	src = (BMSCR *)bm_src;
	psx = src->divsx;
	psy = src->divsy;
	xx = ( id % src->divx ) * psx;
	yy = ( id / src->divx ) * psy;
	texpx = xx + psx;
	texpy = yy + psy;
	if ( texpx < 0 ) return;
	if ( texpx >= src->sx ) {
		if ( xx >= src->sx ) return;
		psx = src->sx - xx;
	}
	if ( texpy < 0 ) return;
	if ( texpy >= src->sy ) {
		if ( yy >= src->sy ) return;
		psy = src->sy - yy;
	}

	dsx = (float)psx * destx;
	dsy = (float)psy * desty;

	mem_bmscr->cx = cx;
	mem_bmscr->cy = cy;

	hgio_copyrot( mem_bmscr, xx, yy, psx, psy, dsx*0.5f, dsy*0.5f, src, dsx, dsy, ang );
}



/*------------------------------------------------------------------------------------*/

