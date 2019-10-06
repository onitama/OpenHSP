
//
//	hgio(HSP graphics I/O) DG functions
//
#ifndef __hgio_dg_h
#define __hgio_dg_h

#include "hspwnd_dish.h"
#include "../hsp3/dpmread.h"
#include "geometry.h"
#include "hginc.h"

#include "hgmodel.h"
#include "hgevent.h"
#include "hgemitter.h"

void hgio_setcenter( float x, float y );
void hgio_drawsprite( hgmodel *mdl, HGMODEL_DRAWPRM *prm );

#endif

