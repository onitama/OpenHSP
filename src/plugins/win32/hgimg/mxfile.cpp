
//---------------------------------------------------------------------------
//		mx file parse
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
#include "mxfile.h"
#include "mxinc.h"

#define MAXMDBUF 256			// ç≈ëÂÉÇÉfÉãäKëw(siblingä‹Çﬁ)
static MODEL *mdbuf[MAXMDBUF];

static MXHED *mx;
static MXHED2 *mx2;
static MDINF *mdinf;
static NDINF *ndinf;
static MATINF *matinf;
static EMTINF *emtinf;
static int mdnum;
static int matnum;
static int emtnum;
static int numpoly;
static int textop;
static int lastmodel;
static char *wp;
static char s1[256];

static int mxver;
static int mxflag;
static int mxload_mode;

static void DebugMes( char *ss )
{
	MessageBox( NULL, ss, "error",MB_ICONINFORMATION | MB_OK );
}

void mxInit( hgdraw *hg, char *fileptr, int mode )
{
	mx = (MXHED *)fileptr;
	mx2 = (MXHED2 *)fileptr;
	mdinf = (MDINF *)(fileptr + mx->pt_mdinf);
	ndinf = (NDINF *)(fileptr + mx->pt_ndinf);
	matinf = (MATINF *)(fileptr + mx->pt_matinf);
	mdnum = mx->size_mdinf / sizeof(MDINF);
	matnum = mx->size_matinf / sizeof(MATINF);
	numpoly = mx->size_ndinf / sizeof(NDINF);
	textop = hg->io.GetNextTexID();
	mxver = mx->version;
	if ( mxver>=0x200 ) {
		mxflag = mx2->flags;
		emtnum = mx2->size_emtinf / sizeof(EMTINF);
		emtinf = (EMTINF *)( fileptr + mx2->pt_emtinf );
	} else {
		mxflag = 0;
		emtnum = 0;
		emtinf = NULL;
	}
	mxload_mode = mode;
}


void mxTerm( void )
{
}


int mxGetMaxImage( void )
{
	return matnum;
}


char *mxGetImage( int id )
{
	return matinf[id].name;
}


int mxGetNumPoly( void )
{
	return numpoly;
}


static void MakeNodes( hgdraw *hg, MODEL *model, MDINF *md )
{
	int a,i;
	int ndnum,ndmax;
	int tex;
	float tsx,tsy;
	NDINF *nd;
	PNODE *n;
	VECTOR *fv;
	VECTOR *fn;
	IAXIS *uv;
	TEXINF *t;
	EMTINF *emt;
	int *colp;
	int mat,matshade;

	ndnum = md->ndinf;
	ndmax = md->num_ndinf;
	nd = &ndinf[ndnum];
	if ( ndmax==0 ) return;

	for(i=0;i<ndmax;i++) {

		//		shade modeÇèâä˙âª
		matshade = 0; mat = 0;
		if ( mxver>=0x200 ) {
			mat = nd->mat;
			if ( mat & NODE_ATTR_USEMATERIAL ) {
				emt = &emtinf[ mat & 0x3ff ];
				matshade = emt->shade;
			} else {
				matshade = nd->mat & 7;
			}
		}

		//		nodeÇçÏê¨
		n = hg->MakeNode( PRIM_POLY4T+(matshade<<1), 4, 4 );
		n->attr = mat;
		if ( nd->tex>=0 ) {
			if ( mxflag & MX_FLAG_GLOBALTEX ) {
				tex = nd->tex + mx2->texoffset;
			} else {
				tex = nd->tex + textop;
			}
			n->tex = tex;
			t = hg->io.GetTex( tex );
			tsx = (float)t->sx;
			tsy = (float)t->sy;
		}
		else {
			colp = (int *)&n->color;
			*colp = nd->color;
/*			{
				float ip;
				int a1,a2,a3,col;
				ip = nd->n[0].x * 0.5f + nd->n[0].y * -0.5f;
				ip = ip * 128.0f + 128.0f + 32.0f;
				if ( ip < 0.0f ) ip = 0.0f;
				if ( ip > 256.0f ) ip = 256.0f;
				a1 = ((nd->color>>16)&0xff);
				a2 = ((nd->color>>8)&0xff);
				a3 = (nd->color&0xff);
				col = (int)ip;
				a1 = (a1*col)>>8;
				a2 = (a2*col)>>8;
				a3 = (a3*col)>>8;
				*colp = (a1<<16)|(a2<<8)|a3;
			}*/
			n->code = PRIM_POLY4F+(matshade<<1);
		}
		fv = n->fv;
		fn = n->fn;
		uv = n->uv;
		if ( mxload_mode ) {		// è„â∫ãtÇ…ÇµÇƒÉçÅ[Éh
			for(a=0;a<4;a++) {
				CopyVector( &fv[3-a], (VECTOR *)&nd->v[a] );
				fv[3-a].y *= -1.0f;
				CopyVector( &fn[3-a], (VECTOR *)&nd->n[a] );
				fn[3-a].y *= -1.0f;
				if ( tex>=0 ) {
					if ( mxflag & MX_FLAG_UV2 ) {
						uv[3-a].x = (int)(nd->uv[a].u * tsx );
						uv[3-a].y = (int)(nd->uv[a].v * tsy );
					} else {
						uv[3-a].x = (int)(nd->uv[a].u );
						uv[3-a].y = (int)(nd->uv[a].v );
					}
				}
			}
		} else {
			for(a=0;a<4;a++) {
				CopyVector( &fv[a], (VECTOR *)&nd->v[a] );
				CopyVector( &fn[a], (VECTOR *)&nd->n[a] );
				if ( tex>=0 ) {
					if ( mxflag & MX_FLAG_UV2 ) {
						uv[a].x = (int)(nd->uv[a].u * tsx );
						uv[a].y = (int)(nd->uv[a].v * tsy );
					} else {
						uv[a].x = (int)(nd->uv[a].u );
						uv[a].y = (int)(nd->uv[a].v );
					}
				}
			}
		}
		hg->SetModelNode( model, n );
		nd++;
	}
}


int mxConvHG( hgdraw *hg )
{
	int i,id,objn;
	MODEL *model;
	MODEL *mdtmp;
	MDINF *md;
	objn = -1;

	for(i=0;i<MAXMDBUF;i++) { mdbuf[i]=NULL; }
	for(i=0;i<mdnum;i++) {
		id = -1;
		md = &mdinf[i];
		if ( md->flag ) {
			id = hg->RegistModel();
			model = hg->GetModel(id);
		}
		else {
			model = hg->MakeModel();
		}
		mdbuf[i] = model;
		hg->SetModelPos( model, (VECTOR *)&md->pos );
		hg->SetModelAng( model, (VECTOR *)&md->ang );
		hg->SetModelScale( model, (VECTOR *)&md->scale );
		MakeNodes( hg, model, md );
		if ( id != -1 ) {
			//id = hg->RegistObj( 0, model );
			if ( objn == -1 ) objn = id;
		}
	}

	for(i=0;i<mdnum;i++) {
		md = &mdinf[i];
		model = mdbuf[i];
		if ( md->child ) {
			mdtmp = mdbuf[ md->child ];
			if ( mdtmp != NULL ) {
				hg->SetModelChild( model, mdtmp );
			} else {
				DebugMes( "Invalid Child Link." );
			}
		}
		if ( md->sibling ) {
			mdtmp = mdbuf[ md->sibling ];
			if ( mdtmp != NULL ) {
				hg->SetModelSibling( model, mdtmp );
			} else {
				DebugMes( "Invalid Sibling Link." );
			}
		}
	}
	hg->RebuildModelTree( objn, NULL );
	lastmodel = objn;
	return objn;
}



static int GetLine( void )
{
	int a;
	char a1;
	if ( *wp == 0 ) return 1;
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


int mxConvHGAnim( hgdraw *hg, char *fileptr )
{
	MODEL *model;
	ANMINF *ani;
	ANMINF *cur;
	int num;
	int frame;
	int maxframe;
	char a1;

	wp = fileptr;
	ani = (ANMINF *)malloc( sizeof(ANMINF) * 4096 );
	model = NULL;
	frame = 0;
	maxframe = 0;

	while(1) {
		if ( GetLine() ) {
			if ( frame ) {
				hg->SetModelAnim( model, ani, frame );
			}
			break;
		}
		a1 = s1[0];
		if ( a1=='*' ) {			// '*0000' format

			if ( frame ) {
				hg->SetModelAnim( model, ani, frame );
			}

			s1[5] = 0;
			num = atoi( s1+1 );
			model = mdbuf[num];
			frame = 0;
		}
		if ( a1=='s' ) {
			cur = &ani[frame];
			sscanf( s1, "s %f,%f,%f", &cur->sx, &cur->sy, &cur->sz );
		}
		if ( a1=='r' ) {
			cur = &ani[frame];
			sscanf( s1, "r %f,%f,%f", &cur->rx, &cur->ry, &cur->rz );
		}
		if ( a1=='t' ) {
			cur = &ani[frame];
			cur->flag = 1;
			cur->time = 1;
			sscanf( s1, "t %f,%f,%f", &cur->tx, &cur->ty, &cur->tz );
			frame++;
			if ( frame > maxframe ) maxframe = frame;
		}
		if ( a1=='e' ) {
			cur = &ani[frame];
			cur->time = 0;
		}
		if ( a1=='g' ) {
			cur = &ani[frame];
			cur->flag = 2;
			cur->time = 0;
		}
	}
	hg->SetModelAnimFrame( lastmodel, maxframe, 0 );
	free( ani );
	return 0;
}


//---------------------------------------------------------------------------
//		mx file output
//---------------------------------------------------------------------------

int mxSaveHG( hgdraw *hg, char *sname, int modelid, char *texlist )
{
	int a,b,i,objn,code,max;
	char *tp;
	MODEL *m;
	PNODE *n;

	//		MXä÷òA
	FILE *fp;
	MXHED2 *mem_mxhed;
	MDINF *mem_mdinf;
	NDINF *mem_ndinf;
	MATINF *mem_matinf;
	MDINF *md;
	NDINF *nd;
	int ndnum;
	int mdnum;
	int matnum;
	int pt_mdinf;
	int pt_ndinf;
	int pt_matinf;
	int pt_emtinf;
	int size_mdinf;
	int size_ndinf;
	int size_matinf;

	mem_mxhed = (MXHED2 *)malloc( sizeof(MXHED2) );
	mem_mdinf = (MDINF *)malloc( sizeof(MDINF)*512 );	// max 512
	mem_ndinf = (NDINF *)malloc( sizeof(NDINF)*8192 );	// max 4096
	mem_matinf = (MATINF *)malloc( sizeof(MATINF)*256 );// max 256

	ndnum = 0; mdnum = 0; matnum = 0;

	max = hg->GetModelSubMax(modelid);

	for(i=0;i<max;i++) {
		m = hg->GetModelEx( modelid, i );
		if ( m!=NULL ) if ( m->nummax > 0 ) {
			objn = m->num;
			md = &mem_mdinf[mdnum];
			md->flag = 0;if ( i==0 ) md->flag = 1;
			md->ndinf = ndnum;
			md->num_ndinf = objn;
			md->child = 0; md->sibling = 0;
			CopyVector( (VECTOR *)&md->pos, &m->pos );
			CopyVector( (VECTOR *)&md->ang, &m->ang );
			CopyVector( (VECTOR *)&md->scale, &m->scale );
			a = hg->GetModelSubID( modelid, (MODEL *)m->child ); if ( a != -1 ) md->child = a;
			a = hg->GetModelSubID( modelid, (MODEL *)m->sibling ); if ( a != -1 ) md->sibling = a;
			for(a=0;a<objn;a++) {
				n = hg->GetModelNode( m, a );
				code = n->code;
				nd = &mem_ndinf[ndnum];
				nd->mat = ( n->attr & 0xf800 );
				nd->color = n->color;
				if ((code==PRIM_POLY4F)||(code==PRIM_POLY4FS)||(code==PRIM_POLY4FGS))
					{ nd->tex = -1; } else { nd->tex = n->tex; }
				if (( n->attr & NODE_ATTR_USEMATERIAL )==0) {
					nd->mat |= (code-PRIM_POLY4F)>>1;
				}
				for(b=0;b<4;b++) {
					CopyVector( (VECTOR *)&nd->v[b], &n->fv[b] );
					CopyVector( (VECTOR *)&nd->n[b], &n->fn[b] );
					nd->uv[b].u = (float)n->uv[b].x;
					nd->uv[b].v = (float)n->uv[b].y;
				}
				ndnum++;
			}
			mdnum++;
		}
	}

	tp = texlist;
	if ( tp != NULL ) {
		char texname[64];
		char a1;
		a=0;
		while(1) {
			a1=*tp++;
			if ((a1==0)||(a1==13)) {
				if ( *tp==10 ) tp++;
				if ( a ) {
					texname[a]=0;
					strcpy( mem_matinf[matnum].name, texname );
					matnum++;
					a=0;
				}
				if ( a1==0 ) break;
			} else { texname[a++]=a1; }
		}
	}

	mem_mxhed->h1 = 'M';					// magic code1
	mem_mxhed->h2 = 'X';					// magic code2
	mem_mxhed->h3 = 'F';					// magic code3
	mem_mxhed->h4 = 0;						// magic code4
	mem_mxhed->version = 0x200;				// version number info

	size_mdinf = sizeof(MDINF) * mdnum;
	size_ndinf = sizeof(NDINF) * ndnum;
	size_matinf = sizeof(MATINF) * matnum;
	pt_mdinf = sizeof(MXHED2);
	pt_ndinf = pt_mdinf + size_mdinf;
	pt_matinf = pt_ndinf + size_ndinf;
	pt_emtinf = pt_matinf + size_matinf;

	mem_mxhed->flags = 0;					// flags
	mem_mxhed->texoffset = 0;
	mem_mxhed->matoffset = 0;

	mem_mxhed->size_mdinf = size_mdinf;		// size of Model section
	mem_mxhed->pt_mdinf = pt_mdinf;			// ptr to Model Segment
	mem_mxhed->size_ndinf = size_ndinf;		// size of Node section
	mem_mxhed->pt_ndinf = pt_ndinf;			// ptr to Node Segment
	mem_mxhed->size_matinf = size_matinf;	// size of Matinf section
	mem_mxhed->pt_matinf = pt_matinf;		// ptr to Matinf Segment
	mem_mxhed->size_emtinf = 0;
	mem_mxhed->pt_emtinf = pt_emtinf;
	
	fp=fopen( sname,"wb" );
	if ( fp == NULL ) {
		return -1;
	}
	fwrite( mem_mxhed, 1, sizeof(MXHED2), fp );
	if (size_mdinf) fwrite( mem_mdinf, 1, size_mdinf, fp );
	if (size_ndinf) fwrite( mem_ndinf, 1, size_ndinf, fp );
	if (size_matinf) fwrite( mem_matinf, 1, size_matinf, fp );
	fclose(fp);

	free( mem_matinf );
	free( mem_ndinf );
	free( mem_mdinf );
	free( mem_mxhed );

	return 0;
}





