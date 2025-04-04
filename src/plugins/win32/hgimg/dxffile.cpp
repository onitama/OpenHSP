
//---------------------------------------------------------------------------
//		dxf file parse
//---------------------------------------------------------------------------

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "hgcnf.h"
#include "hginc.h"
#ifdef HGIMG_DIRECTX
	#include "../hgimgx/hgiox.h"
#else
	#include "hgio.h"
	#include "hgiof.h"
#endif
#include "hgdraw.h"
#include "dxffile.h"

static char *wp;
static char s1[256];
static int numpoly;
static int dxfmodel;


static void DebugMes( char *ss )
{
	MessageBox( NULL, ss, "error",MB_ICONINFORMATION | MB_OK );
}

static int GetLine( void )
{
	int a;
	char a1;
	if ( *wp == 0 ) { s1[0]=0; return 1; }
	a=0;
	while(1) {
		a1 = *wp;
		if ( a1 == 0 ) break;
		if ( a1 == 13 ) {
			wp++;
			if ( *wp==10 ) wp++;
			break;
		}
		s1[a++] = a1;
		wp++;
	}
	s1[a] = 0;
	return 0;
}


static int GetLineStr( char *str )
{
	while(1) {
		if ( GetLine() ) return 1;
		if ( strcmp( s1, str )==0 ) break;
	}
	return 0;
}


static float GetValF( void )
{
	if ( GetLine() ) return 0.0f;
	return (float)atof(s1);
}


int dxfGetNumPoly( void )
{
	return numpoly;
}


static int MakeNodes( hgdraw *hg, MODEL *model )
{
	PNODE *n;
	VECTOR *fv;
	VECTOR *fn;
	VECTOR v[4];
//	VECTOR normal;
//	VECTOR v1,v2;
	int a;

	if ( GetLineStr( "3DFACE" ) ) return -1;
	GetLine();
	GetLine();

	if ( GetLineStr( "10" ) ) return -1;
	v[0].x = GetValF();
	if ( GetLineStr( "20" ) ) return -1;
	v[0].y = GetValF();
	if ( GetLineStr( "30" ) ) return -1;
	v[0].z = GetValF();
	v[0].w = 1.0f;
	
	if ( GetLineStr( "11" ) ) return -1;
	v[1].x = GetValF();
	if ( GetLineStr( "21" ) ) return -1;
	v[1].y = GetValF();
	if ( GetLineStr( "31" ) ) return -1;
	v[1].z = GetValF();
	v[1].w = 1.0f;

	if ( GetLineStr( "12" ) ) return -1;
	v[2].x = GetValF();
	if ( GetLineStr( "22" ) ) return -1;
	v[2].y = GetValF();
	if ( GetLineStr( "32" ) ) return -1;
	v[2].z = GetValF();
	v[2].w = 1.0f;

	if ( GetLineStr( "13" ) ) return -1;
	v[3].x = GetValF();
	if ( GetLineStr( "23" ) ) return -1;
	v[3].y = GetValF();
	if ( GetLineStr( "33" ) ) return -1;
	v[3].z = GetValF();
	v[3].w = 1.0f;

	n = hg->MakeNode( PRIM_POLY4F, 4, 4 );
	fv = n->fv;
	fn = n->fn;
//	SubVector( &v1, &v[1], &v[0] );
//	SubVector( &v2, &v[2], &v[1] );
//	OuterProduct( &normal, &v1, &v2 );
//	UnitVector( &normal );
	for(a=0;a<4;a++) {
		CopyVector( &fv[a], &v[a] );
//		CopyVector( &fn[a], &normal );
	}
	n->color = 0xffffff;
	n->attr = 0;
	hg->SetModelNode( model, n );
	return 0;
}


int dxfConvHG( hgdraw *hg, char *fileptr )
{
	//
	//		DXFƒtƒ@ƒCƒ‹•ÏŠ·( 3DFACE‚Ì‚Ý‘Î‰ž )
	//			http://homepage1.nifty.com/atmori/cg/shade/dxf/dxf.htm
	//			http://www.tanaka.is.uec.ac.jp/~ishigaki/dxfcg0J.html
	//
	VECTOR tmp;
	MODEL *model;

	numpoly = 0;
	wp = fileptr;

	dxfmodel = hg->RegistModel();
	model = hg->GetModel( dxfmodel );
	SetVector( &tmp, 0.0f, 0.0f, 0.0f, 1.0f );
	hg->SetModelPos( model, &tmp );
	hg->SetModelAng( model, &tmp );
	SetVector( &tmp, 1.0f, 1.0f, 1.0f, 1.0f );
	hg->SetModelScale( model, &tmp );
	while(1) {
		if ( MakeNodes( hg, model ) ) break;
	}
	hg->MakeModelNormals( dxfmodel, 0 );
	return dxfmodel;
}

