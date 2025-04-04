
//
//		Easy Sprite Control DLL for HSP2
//				onion software/onitama 1998/7
//

#include <windows.h>
#include <string.h>
#include <math.h>
#include "ddsub.h"
#include "hspdll.h"
//#include "dtbl.h"

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
     {
     return TRUE ;
     }


/*------------------------------------------------------------*/

#define UCHAR	unsigned char
#define UINT	unsigned int

static	UCHAR	*vramtop;
static	UCHAR	*vram;
static	UCHAR	*vrbase;

static	int		dxflag=0;			// DLL init flag
static	int		tpx,tpy,gx,gy;		// for sprite window
static	int		ofsx,ofsy;			// for sprite offset
static	int		sx,sy,vx,vy;		// for target vram
static	int		vbx,vby,bx,by;		// for source vram
static	int		tpcol;				// transparent color code
static	int		ox1,oy1,ox2,oy2;	// valid area
static	int		land_x,land_y;		// common ground axis
static	double	pi,pans;
static	int		resox,resoy;		// 画面解像度
static	char	winmode=FALSE;		// Window mode
static	int		rrate;				// 周の分解能
static	int		*vpx=NULL,*vpy=NULL;// sin,cosテーブル

static	int		frame;
static	int		fps;
static	UINT	time_sec=GetTickCount();

//
//	sprite move flag (fl) condition :
//			( fl=0 : no sprite )
//
//			bit 0-7 : countdown wipe (0=none)
//
//			bit 8 :  $100 static sprite
//			bit 9 :  $200 move (px,py) every
//			bit10 :  $400 gravity move
//			bit11 :  $800 background link move
//			bit12 : $1000 no border wipe
//			bit13 : $2000 X-border bounce
//			bit14 : $4000 Y-border bounce
//
//
//		info number :
//			0,1 = flag (32bit)
//			2,3 = X axis (32bit)
//			4,5 = Y axis (32bit)
//			6,7 = PX prm (32bit)
//			8,9 = PY prm (32bit)
//			 10 = progress counter
//			 11 = Animation counter
//			 12 = CHR code
//			 13 = TYPE code
//		  14,15 = Exec ptr.
//

typedef struct SPOBJ
{
	int	fl;				//	exist flag (0=none)
	short xh,xx;		//  X axis (16+16bit)
	short yh,yy;		//	Y axis (16+16bit)
	short pxh,px;		//	Gravity/Move X parameters
	short pyh,py;		//	Gravity/Move Y parameters
	short prg;			//	Move progress counter
	short ani;			//	chr anim counter
	short chr;			//	chr code
	short type;			//	Attribute type
	short *exec;		//	Exec program ptr.
	short fspx,fspy;	//  落下速度
	short bound;		//  固さ
	char boundflag;		//  バウンドさせるフラグ(1=X 2=Y)
} SPOBJ;


typedef struct ISPOBJ
{
	int	fl;				//	exist flag (0=none)
	int xx;				//  X axis (32bit)
	int yy;				//	Y axis (32bit)
	int px;				//	Gravity/Move X parameters
	int py;				//	Gravity/Move X parameters
	short prg;			//	Move program counter
	short ani;			//	chr anim counter
	short chr;			//	chr code
	short type;			//	Attribute type
	short *exec;		//	Exec program ptr.
	short fspx,fspy;	//  落下速度
	short bound;		//  固さ
	char boundflag;		//  バウンドさせるフラグ(1=X 2=Y)
} ISPOBJ;

static	int		spkaz;	// Max sprite object
static	SPOBJ	*mem_sp=NULL;
static	SPOBJ	*sp;
static	ISPOBJ	*isp;


typedef struct CHRREF
{
	short wid;			//  buffer ID
	short tpflag;		//  Transparent flag
	short lktime,lknum;	//	chr link information
	short bx,by;		//	chr base X,Y
	short bsx,bsy;		//	chr base size of X,Y
	short colx,coly;	//	Collision X,Y offset
	short colsx,colsy;	//	Collision size of X,Y
} CHRREF;

static	int		chrkaz;	// Max pattern object
static	CHRREF	*mem_chr=NULL;
static	CHRREF	*chr;
static	int		df_bsx,df_bsy,df_colx,df_coly;
static	int		df_colsx,df_colsy,df_tpflag;
static	int		colx,coly,colex,coley;
static	int		fade_mode, fade_upd, fade_val, fade_tar;
static	char	*lpPal;
static	BMSCR	*bmscr;
static	int		palflag=0;
static	int		wtime=10;
static	int		dbgval=0;
static	short	def_fspx=0,def_fspy=0x100;
static	short	def_bound=128;
static	char	def_boundflag=3;

#ifdef DEBUG_BUILD
extern void DEBUG(const char *txt);
#endif

/*------------------------------------------------------*/

UCHAR *vtcalc( int px, int py )
{
	return (UCHAR *)((vy-py-1)*vx+px+vramtop);
}

UCHAR *vcalc( int px, int py )
{
	return (UCHAR *)(vram+px-py*vx);
}

UCHAR *bcalc( int px, int py )
{
	return (UCHAR *)(vrbase+px-py*vbx);
}

/*------------------------------------------------------*/

void spput( int xx, int yy, int chrno )
{
	//		sprite put
	//
	int a,x,y,ix,iy,nx,ny;
	RECT src;

	x=xx;y=yy;
	chr=&mem_chr[chrno];
	nx=chr->bsx;ny=chr->bsy;
	ix=chr->bx;iy=chr->by;

	if (x<0) {
		a=x+nx;if (a<=0) return;
		x=0; ix+=nx-a; nx=a;
	}
	a=sx-nx;
	if (x>a) {
		if (x>sx) return;
		nx-=x-a;
	}

	if (y<0) {
		a=y+ny;if (a<=0) return;
		y=0; iy+=ny-a; ny=a;
	}
	a=sy-ny;
	if (y>a) {
		if (y>sy) return;
		ny-=y-a;
	}

	a=chr->wid;
	src.left  = ix;
	src.top   = iy;
	src.right = ix+nx;
	src.bottom= iy+ny;

	if (chr->tpflag) dd_fcopyn( x+tpx, y+tpy, a, (LPRECT)&src );
				else dd_fcopy( x+tpx, y+tpy, a, (LPRECT)&src );
}


void rectput( int x1, int y1, int x2, int y2, int wid, int x3, int y3, int x4, int y4, int mode )
{
	//		pattern put
	//		wid (x3,y3,x4,y4) -> (x1,y1,x2,y2)
	//
	RECT src;
	RECT dst;
	SetRect( &dst,x1,y1,x1+x2,y1+y2 );
	SetRect( &src,x3,y3,x3+x4,y3+y4 );

	if(src.left>=resox||src.top>=resoy||src.right<0||src.bottom<0)
		return;
	if(dst.left>=resox||dst.top>=resoy||dst.right<0||dst.bottom<0)
		return;

	if(dst.left<0){
		src.left+=(int)((float)(src.right-src.left)*(-dst.left)/(dst.right-dst.left));
		dst.left=0;
	}
	if(dst.right>resox){
		src.right-=(int)((float)(src.right-src.left)*(dst.right-resox)/(dst.right-dst.left));
		dst.right=resox;
	}
	if(dst.top<0){
		src.top+=(int)((float)(src.bottom-src.top)*(-dst.top)/(dst.bottom-dst.top));
		dst.top=0;
	}
	if(dst.bottom>resoy){
		src.bottom-=(int)((float)(src.bottom-src.top)*(dst.bottom-resoy)/(dst.bottom-dst.top));
		dst.bottom=resoy;
	}

	if (mode==0) dd_copyn( (LPRECT)&dst, wid, (LPRECT)&src );
			else dd_copy( (LPRECT)&dst, wid, (LPRECT)&src );
}


void spsetchr( int spno, int chrno )
{
	//		sprite chr code set
	//
	sp=&mem_sp[spno];
	sp->chr=chrno;
	chr=&mem_chr[chrno];
	sp->ani=chr->lktime;
}


void spsetpos( int spno, int x, int y )
{
	//		sprite chr code set
	//
	sp=&mem_sp[spno];
	sp->xx=x;sp->xh=0;
	sp->yy=y;sp->yh=0;
}


void spset( int spno, int xx, int yy, int chrno )
{
	//		sprite set
	//
	sp=&mem_sp[spno];
	sp->fl=0x100;sp->type=1;
	sp->px=0;sp->py=0;
	sp->pxh=0;sp->pyh=0;
	sp->prg=0;
	sp->fspx=def_fspx;
	sp->fspy=def_fspy;
	sp->bound=def_bound;
	sp->boundflag=def_boundflag;
	spsetpos( spno, xx, yy );
	spsetchr( spno, chrno );
}


int spnew( int sp_from, int sp_to, int step )
{
	//		sprite collision check
	//
	int b;
	b=sp_from;
	while(1){
		sp=&mem_sp[b];
		if (sp->fl==0) return b;
		b+=step;
		if( (b<0)||
			(b>=spkaz)||
			((step<0)&&(b<sp_to))||
			((step<0)&&(b>sp_from))||
			((step>0)&&(b>sp_to))||
			((step>0)&&(b<sp_from))||
			(step==0) )
			break;
	}
	return -1;
}


int chksub( void )
{
	int x1,y1;
	chr=&mem_chr[sp->chr];
	x1=(sp->xx) + (chr->colx);
	if (colex<=x1) return 0;
	y1=(sp->yy) + (chr->coly);
	if (coley<=y1) return 0;

	if ((x1+chr->colsx)>colx)
		if ((y1+chr->colsy)>coly)
			return 1;
	return 0;
}

int spchk( int spno, int chktype )
{
	//		sprite collision check
	//
	int a,cc;
	SPOBJ *ssp;

	cc=chktype;
	if (cc==0) cc=0xffff;
	ssp=&mem_sp[spno];
	chr=&mem_chr[ssp->chr];
	colx=(ssp->xx) + (chr->colx);
	coly=(ssp->yy) + (chr->coly);
	colex=colx + chr->colsx;
	coley=coly + chr->colsy;

	for(a=0;a<spkaz;a++) {
		sp=&mem_sp[a];
		if (sp->fl)
		 if (sp->type & cc)
		  if (spno!=a) {
			if (chksub()) return a;
		}
	}
	return -1;
}


void spdraw( void )
{
	//		1 sprite draw (on sp)
	//
	int fl,x,y,land;
	short next;
	isp=(ISPOBJ *)sp;

	//		flag pre check
	//
	fl=sp->fl;

	//		add pos move
	//
	if (fl&0x200) {
		isp->xx+=isp->px;
		isp->yy+=isp->py;
	}
	if (fl&0x400) {
		isp->xx+=isp->px;
		isp->yy+=isp->py;

		//	なんだこりゃ・・・
		//
		if(isp->fspy!=0){
			isp->py+=isp->fspy<<8;
			chr=&mem_chr[sp->chr];
			if(isp->boundflag&2){
				if(isp->fspy>0){
					land=land_y - (chr->bsy);
					if (sp->yy>land) {
						sp->yy=land;
						y=isp->py;
						if (y<0) y=0;
						if (y<0x18000) {
							sp->fl=(fl^0x400) | 0x300;
							isp->py=0;
						}
						else isp->py=-((int)(y&0xffff0000)*isp->bound>>8);
					}
				}else{
					land=tpy;
					if(sp->yy<land){
						sp->yy=land;
						y=abs(isp->py);
						if(y<0) y=0;
						if(y<0x18000){
							sp->fl=(fl^0x400)|0x300;
							isp->py=0;
						}
						else isp->py=((int)(y&0xffff0000)*isp->bound>>8);
					}
				}
			}
		}
	
		if(isp->fspx!=0){
			isp->px+=isp->fspx<<8;
			chr=&mem_chr[sp->chr];
			if(isp->boundflag&1){
				if(isp->fspx>0){
					land=land_x-chr->bsx;
					if (sp->xx>land) {
						sp->xx=land;
						x=isp->px;
						if (x<0) x=0;
						if (x<0x18000) {
							sp->fl=(fl^0x400) | 0x300;
							isp->px=0;
						}
						else isp->px=-((int)(x&0xffff0000)*isp->bound>>8);
					}
				}else{
					land=tpx;
					if(sp->xx<land){
						sp->xx=land;
						x=abs(isp->px);
						if(x<0) x=0;
						if(x<0x18000){
							sp->fl=(fl^0x400)|0x300;
							isp->px=0;
						}
						else isp->px=((int)(x&0xffff0000)*isp->bound>>8);
					}
				}
			}
		}
	}

	//		border check
	//
	x=sp->xx;y=sp->yy;
	if (fl&0x2000) {
		if (x<ox1) { x=ox1;sp->px=abs(sp->px); }
		if (x>ox2) { x=ox2;sp->px=-abs(sp->px); }
	}
	if (fl&0x4000) {
		if (y<oy1) { y=oy1;sp->py=abs(sp->py); }
		if (y>oy2) { y=oy2;sp->py=-abs(sp->py); }
	}

	if ((fl&0x1000)==0) {
		if(((int)((x-ox1)|(y-oy1)|(ox2-x)|(oy2-y)))>>31){
			sp->fl=0;
			return;
		}
	}
	

	//		Flag blink check
	//
	if ( fl&255 ) {
		fl--;
		if (( fl & 255 )==0) fl=0;
		if (( fl & 255 )==128) fl^=128;
		sp->fl=fl;
		if ((fl&0x8001)==1) goto nodraw;
	}

	//		send to vram
	//
	spput( x+ofsx, y+ofsy, sp->chr );

nodraw:
	//		chr animation process
	//
	if (sp->ani) {
		sp->ani--;
		if (sp->ani==0) {
			chr=&mem_chr[sp->chr];
			next=chr->lknum;
			if (next<0) { sp->fl=0;return; }	// next<0 wipe
			sp->chr=next;
			chr=&mem_chr[sp->chr];
			sp->ani=chr->lktime;
		}
	}

}


void adirx(int p1,int p2,int p3)
{
	int ax,ay,aa,adir;

	aa=p3;if (aa==0) aa=100;
	adir=p2%rrate;
	ax=(vpx[adir]<<6)*aa/100;
	ay=(vpy[adir]<<6)*aa/100;
	isp=(ISPOBJ *)&mem_sp[p1];
	isp->px=ax;
	isp->py=ay;
	isp->fl |= 0x200;
}


void xarea(int p1,int p2,int p3,int p4)
{
	ox1=p1;oy1=p2;ox2=p3;oy2=p4;
}


void xwindow(int p1,int p2,int p3,int p4)
{
	tpx=p1;tpy=p2;
	sx=p3;if (sx==0) sx=gx;
	sy=p4;if (sy==0) sy=gy;
	xarea(-128,-128,sx+128,sy+128);
	land_x=sx;
	land_y=sy;
}


void xsize(int p1,int p2,int p3,int p4)
{
	int rate,nx,ny;
	rate=p3;
	if (rate==0) rate=100;
	df_bsx=p1; df_bsy=p2;
	nx=df_bsx * rate / 100;
	ny=df_bsy * rate / 100;
	df_colx=(df_bsx-nx)/2;
	df_coly=(df_bsy-ny)/2;
	df_colsx=nx;
	df_colsy=ny;
	df_tpflag=p4;
}


/*------------------------------------------------------*/


EXPORT BOOL WINAPI es_area ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		set border area (type0)
	//			es_area x1,y1,x2,y2
	//

	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	xarea(p1,p2,p3,p4);
	return 0;
}


EXPORT BOOL WINAPI es_opt ( int p1, int p2, int dum3, int dum4 )
{
	//		set ESCD option (type0)
	//			es_opt land_y
	//
	land_x=p1;
	land_y=p2;
	return 0;
}


EXPORT BOOL WINAPI es_window ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set window area (type0)
	//			es_window tx,ty,sx,sy
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_getdi(gx);
	int p4=hei->HspFunc_prm_getdi(gy);
	if(*hei->er) return *hei->er;

	xwindow(p1,p2,p3,p4);
	return 0;
}


EXPORT BOOL WINAPI es_size ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		define character size (type0)
	//			es_size x,y,collision_rate,tpflag
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_getdi(100);
	int p4=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	xsize(p1,p2,p3,p4);
	return 0;
}


/*------------------------------------------------------*/


EXPORT BOOL WINAPI es_pat ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		define character pattern base (type0)
	//			es_pat chrno, x, y, anim_wait
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	int p4=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	int a,x,y,wt;
	a=p1;x=p2;y=p3;wt=p4;
	chr=&mem_chr[a];
	chr->bx=x;chr->by=y;
	chr->bsx=df_bsx;chr->bsy=df_bsy;
	chr->colx=df_colx;chr->coly=df_coly;
	chr->colsx=df_colsx;chr->colsy=df_colsy;
	chr->lktime=wt;chr->lknum=a+1;
	chr->wid=dd_getwid();
	chr->tpflag=df_tpflag;
	return 0;
}


EXPORT BOOL WINAPI es_link ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		define character base link (type0)
	//			es_link chrno, nextno
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	chr=&mem_chr[p1];
	chr->lknum=p2;
	return 0;
}


EXPORT BOOL WINAPI es_ini ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		initalize ESCD system (type2)
	//			es_ini maxspr,maxchr (base vram set)
	//
	int a;
//	int dumand;
//	int *palpt;

	int p1=hei->HspFunc_prm_getdi(512);
	int p2=hei->HspFunc_prm_getdi(1024);
	int p3=hei->HspFunc_prm_getdi(64);
	if(*hei->er) return *hei->er;
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	bmscr=bm;
	vramtop=bm->pBit;				// vramの先頭アドレス
	vx=bm->sx;vy=bm->sy;
	vrbase=vtcalc( bm->cx, bm->cy );
	vbx=vx;vby=vy;
	spkaz=p1;
	if (spkaz<=0) spkaz=512;
	chrkaz=p2;
	if (chrkaz<=0) chrkaz=1024;
	rrate=p3;
	if(rrate>255){
		if((rrate%256!=0)&&(rrate%360!=0)){
			int r256=(rrate<<8)>>8;
			int r360=(rrate/360)*360;
			rrate=((rrate-r256)<(rrate-r360))? r256 : r360;
		}
	}else{
		rrate=64;
	}
	if(rrate>4096) rrate=4096;
/*
	//	透明色(RGB=0)の割り出し
	//
	if(p3<0){
		palpt=(int *)bm->pal;pand=0xffffff;
		for(a=255;a>=0;a--) {
			if ((palpt[a]&pand)==0) tpcol=a;
		}
	}else{
		tpcol=p3;
	}
*/
	//	Memory release
	//
	if (mem_chr!=NULL){
		free(mem_chr);
		mem_chr=NULL;
	}
	if (mem_sp!=NULL){
		free(mem_sp);
		mem_sp=NULL;
	}
	if (vpx!=NULL){
		free(vpx);
		vpx=NULL;
	}
	if (vpy!=NULL){
		free(vpy);
		vpy=NULL;
	}

	//	Memory alloc
	//
	mem_sp=(SPOBJ *)malloc(sizeof(SPOBJ)*spkaz);
	if(mem_sp==NULL){
#ifdef DEBUG_BUILD
		DEBUG("es_ini : スプライト用メモリの確保に失敗");
#endif
		return -1;
	}

	mem_chr=(CHRREF *)malloc(sizeof(CHRREF)*chrkaz);
	if(mem_chr==NULL){
#ifdef DEBUG_BUILD
		DEBUG("es_ini : キャラクタ用メモリの確保に失敗");
#endif
		return -1;
	}

	vpx=(int *)malloc(sizeof(int)*rrate);
	if(vpx==NULL){
#ifdef DEBUG_BUILD
		DEBUG("es_ini : 角度テーブル用メモリの確保に失敗");
#endif
		return -1;
	}

	vpy=(int *)malloc(sizeof(int)*rrate);
	if(vpy==NULL){
#ifdef DEBUG_BUILD
		DEBUG("es_ini : 角度テーブル用メモリの確保に失敗");
#endif
		return -1;
	}

	//	direction aim parameters
	//
	pi=(double)3.141592;pans=pi*2/rrate;

	for(a=0;a<rrate;a++){
		vpx[a]=(int)(cos(pi/2-pans*a)*1024);
		vpy[a]=(int)(sin(pi/2-pans*a)*1024);
	}

	//	Object initalize
	//
	for(a=0;a<spkaz;a++) {
		mem_sp[a].fl=0;
	}
	for(a=0;a<chrkaz;a++) {
		chr=&mem_chr[a];
		chr->wid=0;
		chr->bx=0;chr->by=0;
		chr->bsx=32;chr->bsy=32;
		chr->colx=0;chr->coly=0;
		chr->colsx=32;chr->colsy=32;
		chr->lktime=0;chr->lknum=0;
	}

	xsize( 32,32,0,0 );
	dxflag=-1;
	palflag=1;

	return 0;
	//return 0x10001;		// request "await 1"
}


EXPORT BOOL WINAPI es_bye ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		release ESCD system (type0)
	//			es_bye
	//
	if (dxflag==0) return 0;

	//		Cursor on
	//
	ShowCursor( TRUE );

	free(mem_chr);
	mem_chr=NULL;
	free(mem_sp);
	mem_sp=NULL;
	free(vpx);
	vpx=NULL;
	free(vpy);
	vpy=NULL;
	dd_bye();
	dxflag=0;
	if(winmode==FALSE)
		SetWindowLong(bmscr->hwnd,GWL_STYLE,WS_POPUPWINDOW|WS_MINIMIZEBOX|WS_CAPTION);

	SetWindowPos(bmscr->hwnd,HWND_TOP,0,0,bmscr->sx,bmscr->sy,SWP_DRAWFRAME|SWP_NOMOVE);
	ShowWindow(bmscr->hwnd,SW_SHOW);
	UpdateWindow(bmscr->hwnd);
	SetFocus(bmscr->hwnd);

	return 0x10001;
}


EXPORT BOOL WINAPI es_debug ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		for DEBUG (type0)
	//			es_debug p1
	//

	int p1;
	p1=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	int a;
	wtime=p1;
	a=dd_getdbg();
	return -a;
}


EXPORT BOOL WINAPI es_clear ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		sprite data clear (type0)
	//			es_clear start,kazz
	//

	int p1=hei->HspFunc_prm_getdi(0);
	int p2=hei->HspFunc_prm_getdi(spkaz);
	if(*hei->er) return *hei->er;

	int a,b,n;
	b=p2;if (b==0) b=spkaz;
	n=p1;
	for(a=0;a<b;a++) {
		mem_sp[n].type=0;
		mem_sp[n++].fl=0;
		if(n>=spkaz) break;
	}
	return 0;
}


EXPORT BOOL WINAPI es_screen ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		initalize ESCD system (type0)
	//			es_screen sx,sy,bpp,mode,window
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	int p4=hei->HspFunc_prm_getdi(0);
	int p5=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	int a,bp;
	int col_st,col_len;
	char sysmemflag=FALSE;

	col_st=0;col_len=0;

	vx=p1;if (vx==0) vx=640;
	vy=p2;if (vy==0) vy=480;
	bp=p3;
	bp=(bp>>3)<<3;
	if (bp<8) bp=8;
	if (bp>=24) bp=32;
	//bp=8;
	if (bp==8) {					// palette mode option
		col_st=10;col_len=236;
		if (p4==1) { col_st=1;col_len=254; }
		if (p4==2) { col_st=0;col_len=256; }
	}
	if ((p4>2)||(p5!=0)) {winmode=TRUE;}else{winmode=FALSE;}
	if ((p4==4)||(p5==2)) sysmemflag=TRUE;

	//		Initalize for DDRAW
	//
	sx=vx;sy=vy;
	gx=vx;gy=vy;
	resox=vx;
	resoy=vy;
	a=dd_ini( bmscr->hwnd, sx, sy, bp, winmode, sysmemflag );
	if ((a!=0)&&(bp!=32)) return a;
	if ((a!=0)&&(bp==32)){
		bp=24;
		a=dd_ini(bmscr->hwnd,sx,sy,bp,winmode,sysmemflag);
		if(a) return a;
	}

	//		Wait for setup (NT problem)
	//
	Sleep(100);

	//		Check memory resource
	//
	if (mem_sp==NULL) return -1;

	//		Cursor off
	//
	if(winmode==FALSE)
		ShowCursor( FALSE );

	//		Set border
	//
	xwindow( 0,0,sx,sy );

	//		Palette system reset
	//
	dd_palarea( col_st,col_len );
	fade_mode=0; fade_val=0; fade_upd=0;
	dd_palinit();
	palflag=0;

	//		Make mask window
	//
	dd_dwindow( (HINSTANCE)bmscr->hInst,bmscr->hwnd);
	/*if(winmode==FALSE)
		ShowWindow(bmscr->hwnd,SW_HIDE);*/

	return 0x10000+wtime;		// request "await"
}


EXPORT BOOL WINAPI es_buffer ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		initalize offscreen buffer (type2)
	//		          ( buffer & palette set )
	//			es_buffer bufid, sysmem_flag, colorkey, palletemode;
	//
	int a;

	BMSCR *bm;
	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_getdi(0);
	p4=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;
	bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	//if (palflag) { palflag=0; dd_palinit(); }

	vramtop=bm->pBit;				// vramの先頭アドレス
	vx=bm->sx;vy=bm->sy;
	vram=vtcalc( 0, 0 );

	lpPal=(char *)bm->pal;
	//if(winmode==TRUE)				// WinMode時はVRAMには取らない
	//	p2=1;
	a=dd_buffer( p1, bm->hdc, vx, vy, -vx, lpPal, p2, p3,(unsigned char *)vram,p4);
	if (a==0)	return 0x10001;		// request "await 1"
	return a;
}


EXPORT BOOL WINAPI es_buffer_np ( BMSCR *bm, int p1, int p2, int p3 )
{
	//		initalize offscreen buffer (type2)
	//		          ( buffer & palette set )
	//			es_buffer bufid, sysmem_flag, colorkey
	//
	int a;

	//if (palflag) { palflag=0; dd_palinit(); }

	vramtop=bm->pBit;				// vramの先頭アドレス
	vx=bm->sx;vy=bm->sy;
	vram=vtcalc( 0, 0 );
	lpPal=(char *)bm->pal;
	//if(winmode==TRUE)				// WinMode時はVRAMには取らない
	//	p2=1;
	a=dd_buffer( p1, bm->hdc, vx, vy, -vx, lpPal, p2, p3,(unsigned char *)vram,1);
	if (a==0)	return 0x10001;		// request "await 1"
	return a;
}


EXPORT BOOL WINAPI es_buffer_pm ( BMSCR *bm, int p1, int p2, int p3 )
{
	//		initalize offscreen buffer (type2)
	//		          ( buffer & palette set )
	//			es_buffer bufid, sysmem_flag, colorkey
	//
	int a;

	//if (palflag) { palflag=0; dd_palinit(); }

	vramtop=bm->pBit;				// vramの先頭アドレス
	vx=bm->sx;vy=bm->sy;
	vram=vtcalc( 0, 0 );
	lpPal=(char *)bm->pal;
	//if(winmode==TRUE)				// WinMode時はVRAMには取らない
	//	p2=1;
	a=dd_buffer( p1, bm->hdc, vx, vy, -vx, lpPal, p2, p3,(unsigned char *)vram,2);
	if (a==0)	return 0x10001;		// request "await 1"
	return a;
}


EXPORT BOOL WINAPI es_palcopy(HSPEXINFO *hei,int dum,int dum2,int dum3)
{
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);
	dd_palsethsp((char *)bm->pal);
	return 0;
}


EXPORT BOOL WINAPI es_getbuf ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		offscreen buffer -> HSP buffer (type2)
	//			es_getbuf wid
	//
	int p1=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	int a;
	vramtop=bm->pBit;				// vramの先頭アドレス
	vx=bm->sx;vy=bm->sy;
	vram=vtcalc( 0, 0 );
	lpPal=(char *)bm->pal;
	a=dd_getbuf( p1, bm->hdc, vx, vy, -vx );
	return a;
}


EXPORT BOOL WINAPI es_release ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		release offscreen buffer (type0)
	//			es_release wid
	//
	int p1=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	dd_release(p1);
	return 0;
}


EXPORT BOOL WINAPI es_put ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		put a character (type0)
	//			es_put x,y,chr
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	spput(p1,p2,p3);
	return 0;
}


EXPORT BOOL WINAPI es_boxf ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		screen rect fill (type0)
	//			es_boxf x1,y1,x2,y2
	//

	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	RECT frect;
	SetRect( &frect,p1,p2,p3,p4 );
	dd_boxf( &frect, bmscr->color );
	return 0;
}


EXPORT BOOL WINAPI es_fill ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		screen rect fill (type0)
	//			( ref : gmode )
	//			es_fill x,y,color
	//

	int p1,p2,p3;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	RECT frect;
	SetRect( &frect, p1, p2, p1+bmscr->gx, p2+bmscr->gy );
	dd_boxf2( &frect, p3 );
	return 0;
}


EXPORT BOOL WINAPI es_copy ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		screen rect copy (type0)
	//			( ref : pos,gmode )
	//			es_copy wid,x,y
	//

	int p1,p2,p3,p4,p5;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_getdi(bmscr->gx);
	p5=hei->HspFunc_prm_getdi(bmscr->gy);
	if(*hei->er) return *hei->er;

	int x,y,sx,sy,dx,dy;
	RECT src;
	x=bmscr->cx;
	y=bmscr->cy;
	sx=p2;
	sy=p3;
	dx=p4;
	dy=p5;
	if(x<0){
		sx-=x;
		dx+=x;
		x=0;
	}
	if(x+dx>resox){
		dx-=x+dx-resox;
	}
	if(y<0){
		sy-=y;
		dy+=y;
		y=0;
	}
	if(y+dy>resoy){
		dy-=y+dy-resoy;
	}
	if((dx<1)||(dy<1))	return 0;

	src.left  = sx;
	src.top   = sy;
	src.right = sx+dx;
	src.bottom= sy+dy;

	if(bmscr->gmode==0)
		dd_fcopyn( x,y,p1,(LPRECT)&src);
	else
		dd_fcopy( x,y,p1,(LPRECT)&src);
	return 0;
}


EXPORT BOOL WINAPI es_zoom ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		screen rect copy (type0)
	//			( ref : pos,objsize,gmode )
	//			es_copy wid,x,y
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	int p4=hei->HspFunc_prm_getdi(bmscr->gx);
	int p5=hei->HspFunc_prm_getdi(bmscr->gy);
	int p6=hei->HspFunc_prm_getdi(bmscr->ox);
	int p7=hei->HspFunc_prm_getdi(bmscr->oy);
	if(*hei->er) return *hei->er;

	rectput( bmscr->cx,bmscr->cy,p6,p7,
		     p1,p2,p3,p4,p5,bmscr->gmode );
	return 0;
}


EXPORT BOOL WINAPI es_xfer ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		screen rect xfer (type0)
	//			( ref : pos,gmode )
	//			es_xfer tid,sid,x,y
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	int p4=hei->HspFunc_prm_geti();
	int p5=hei->HspFunc_prm_getdi(bmscr->gx);
	int p6=hei->HspFunc_prm_getdi(bmscr->gy);
	if(*hei->er) return *hei->er;

	int a,x,y,sx,sy;
	RECT src;
	RECT dst;
	x=bmscr->cx;y=bmscr->cy;
	sx=p5;sy=p6;
	SetRect( &dst, x, y, x+sx, y+sy );
	SetRect( &src, p3, p4, p3+sx, p4+sy );
	a=dd_bufcopy( p1, &dst, p2, &src, bmscr->gmode );
	return a;
}


EXPORT BOOL WINAPI es_cls ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		screen clear (type0)
	//			es_cls r,g,b
	//

	int p1,p2,p3;
	p1=hei->HspFunc_prm_getdi(0);
	p2=hei->HspFunc_prm_getdi(0);
	p3=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	//if (palflag) { palflag=0; dd_palinit(); }

	dd_boxf( NULL, RGB(p1,p2,p3) );
	return 0;
}


EXPORT BOOL WINAPI es_mes ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		message out (type6)
	//			es_mes "message"
	//
	char *p1;
	int p2;
	BMSCR *bm;
	p1=hei->HspFunc_prm_gets();
	p2=hei->HspFunc_prm_getdi(-2);
	if(*hei->er) return *hei->er;
	bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	dd_print( bm->cx, bm->cy, p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI es_fmes ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		message with font settings (type6)
	//			es_fmes "message"
	//
	char *p1;
	int p2;
	BMSCR *bm;
	p1=hei->HspFunc_prm_gets();
	p2=hei->HspFunc_prm_getdi(-2);
	if(*hei->er) return *hei->er;
	bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	dd_fprint( bm->cx, bm->cy, p1, bm->hfont, bm->color, p2 );
	return 0;
}


EXPORT BOOL WINAPI es_mesbuf ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		message out to buffer (type6)
	//			es_mesbuf "message",buffer_id
	//
	char *p1=hei->HspFunc_prm_gets();
	int   p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	dd_print( bm->cx, bm->cy, p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI es_fmesbuf ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		message with font settings to buffer (type6)
	//			es_fmes "message",buffer_id
	//
	char *p1=hei->HspFunc_prm_gets();
	int   p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	dd_fprint( bm->cx, bm->cy, p1, bm->hfont, bm->color, p2 );
	return 0;
}


EXPORT BOOL WINAPI es_palfade ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		palette fade execute (type0)
	//			es_palfade val, speed
	//

	//if (palflag) { palflag=0; dd_palinit(); }
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	if((p1<-256)||(p1>256)) p1=256*((p1>0) ? 1 : -1);
	if (p2==0) {
		dd_palfade( p1 );
		fade_val=p1;
		return 0;
	}
	if ((p2<0)||(p1==fade_val)) return -1;
	fade_tar=p1;
	fade_mode=p2;
	if (fade_val>p1) fade_mode=-fade_mode;
	return 0;
}


EXPORT BOOL WINAPI es_draw ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		execute drawing ESCD system (type0)
	//			es_draw start,kazz
	//

	int p1,p2;
	p1=hei->HspFunc_prm_getdi(0);
	p2=hei->HspFunc_prm_getdi(spkaz-p1);
	if(*hei->er) return *hei->er;

	int a,a1,a2;
	a1=p1;a2=p2;
	if (a2==0) a2=spkaz-a1;
	a1=a1+a2-1;
	for(a=0;a<a2;a++) {
		sp=&mem_sp[a1--];
		if (sp->fl) spdraw();
	}
	return 0;
}


EXPORT BOOL WINAPI es_palset ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		palette change (type0)
	//			es_pal palno, rval, gval, bval
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_getdi(0);
	int p3=hei->HspFunc_prm_getdi(0);
	int p4=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	//if (palflag) { palflag=0; dd_palinit(); }

	dd_palset(p1,p2,p3,p4);
	if (fade_mode==0) fade_upd=1;
	return 0;
}


EXPORT BOOL WINAPI es_sync ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		wait v-sync and update screen (type0)
	//			es_sync times
	//
	int p1=hei->HspFunc_prm_getdi(0);
	int p2=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;
	BMSCR *bm=(BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);

	int a;

	a=dd_flip(bm->hdc,-p1,p2);
	if (a<0) a=0;

	if (fade_mode) {
		fade_val+=fade_mode;
		if (fade_mode>0) {
			if (fade_val>=fade_tar) { fade_val=fade_tar;fade_mode=0; }
		}
		else {
			if (fade_val<=fade_tar) { fade_val=fade_tar;fade_mode=0; }
		}
		dd_palfade( fade_val );
	}
	else {
		if (fade_upd) { dd_palfade( fade_val );fade_upd=0; }
	}

	frame++;
	unsigned int time_now=timeGetTime();
	if(time_sec>time_now){
		time_now+=~time_sec;
		time_sec=0;
	}
	if(time_now-time_sec>1000){
		fps=frame;
		frame=0;
		time_sec=time_now;
	}

	return -a;
}


EXPORT BOOL WINAPI es_caps ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		check sprite collision (type1)
	//			es_caps val, id, factor, emu_flag
	//
	int a;
	
	int *p1,p2,p3,p4;
	p1=(int *)hei->HspFunc_prm_getv();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	a=dd_getcaps(p2,p4);
	if (p3==0) {
		*p1=a;
		return 0;
	}
	a&=1<<(p3-1);
	if (a) *p1=1; else *p1=0;
	return 0;
}


EXPORT BOOL WINAPI es_get ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		get sprite parameters (type1)
	//			es_get val, spno, prm_code
	//
	int *p1=(int *)hei->HspFunc_prm_getv();
	int  p2=hei->HspFunc_prm_geti();
	int  p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	short *p;
	p=(short *)&mem_sp[p2];
	*p1=p[p3];
	return 0;
}


EXPORT BOOL WINAPI es_setp ( int p1, int p2, int p3, int p4 )
{
	//		set sprite parameters (type0)
	//			es_get spno, prm_code, new_prm
	//
	short *p;
	p=(short *)&mem_sp[p1];
	p[p2]=(short)p3;
	return 0;
}


EXPORT BOOL WINAPI es_find ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		find specified sprite (type1)
	//			es_find val, type, start_spno
	//

	int *p1,p2,p3;
	p1=(int *)hei->HspFunc_prm_getv();
	p2=hei->HspFunc_prm_getdi(0xffff);
	p3=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	int a,res;
	int cc;
	a=p3;
	cc=p2;
	if (cc==0) cc=0xffff;
	res=-1;
	while(1) {
		if (a>=spkaz) break;
		sp=&mem_sp[a];
		if (sp->fl)
		 if (sp->type & cc) { res=a;break; }
		a++;
	}
	*p1=res;
	return 0;
}


EXPORT BOOL WINAPI es_check ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		check sprite collision (type1)
	//			es_check val, spno, type
	//

	int *p1,p2,p3;
	p1=(int *)hei->HspFunc_prm_getv();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	*p1=spchk(p2,p3);
	return 0;
}


EXPORT BOOL WINAPI es_offset ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set sprite offset (type0)
	//			es_offset x,y
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	ofsx=p1;ofsy=p2;
	return 0;
}


EXPORT BOOL WINAPI es_new ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		find empty sprite ID (type1)
	//			es_new val, start, kazz
	//
	int *p1=(int *)hei->HspFunc_prm_getv();
	int  p2=hei->HspFunc_prm_geti();
	int  p3=hei->HspFunc_prm_getdi(spkaz);
	int  p4=hei->HspFunc_prm_getdi(1);
	if(*hei->er) return *hei->er;

	int a;
	if(p2<0)
		p2=0;
	if(p2>=spkaz)
		p2=spkaz-1;
	a=p3;
	if (a==0)
		a=spkaz;
	else
		a+=p2;
	p4=1;
	*p1=spnew(p2,a,p4);
	return 0;
}

EXPORT BOOL WINAPI es_exnew ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		find empty sprite ID (type1)
	//			es_new val, start, kazz
	//

	int *p1,p2,p3,p4;
	p1=(int *)hei->HspFunc_prm_getv();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	int a;
	if(p2<0)
		p2=0;
	if(p2>=spkaz)
		p2=spkaz-1;
	if(p3<-1)
		p3=-1;
	if(p3>=spkaz)
		p3=spkaz-1;
	a=p3;
	if (a==-1){
		if(p4>=0)
			a=spkaz;
		else
			a=0;
	}
	if(p4==0){
		if(a>=p2)
			p4=1;
		else
			p4=-1;
	}
	*p1=spnew(p2,a,p4);
	return 0;
}


EXPORT BOOL WINAPI es_set ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set sprite data (type0)
	//			es_set spno, x, y, chr
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	int p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	spset( p1,p2,p3,p4 );
	return 0;
}


EXPORT BOOL WINAPI es_flag ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set sprite flag data (type0)
	//			es_flag spno, flag
	//
	int p1,p2;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	mem_sp[p1].fl=p2;
	return 0;
}


EXPORT BOOL WINAPI es_pos ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set sprite x,y pos data (type0)
	//			es_pos spno, x, y
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	spsetpos( p1, p2, p3 );
	return 0;
}


EXPORT BOOL WINAPI es_posd ( int p1, int p2, int p3, int p4 )
{
	//		set sprite x,y pos data (32bit detail) (type0)
	//			es_pos spno, x, y
	//
	isp=(ISPOBJ *)&mem_sp[p1];
	isp->xx=p2;
	isp->yy=p3;
	return 0;
}


EXPORT BOOL WINAPI es_chr ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		set sprite chr code (type0)
	//			es_chr spno, chr_code
	//

	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	spsetchr( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI es_apos ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		sprite axis add data set (type0)
	//			es_apos spno, px, py, x-prm%
	//
	int ax,ay,aa;
	
	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_getdi(100);
	if(*hei->er) return *hei->er;

	aa=p4;if (aa==0) aa=100;
	ax=(p2<<16)*aa/100;
	ay=(p3<<16)*aa/100;
	isp=(ISPOBJ *)&mem_sp[p1];
	isp->px=ax;
	isp->py=ay;
	isp->fl |= 0x200;
	return 0;
}


EXPORT BOOL WINAPI es_aposd ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		sprite axis add data set (type0)
	//			es_apos spno, px, py
	//

	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	isp=(ISPOBJ *)&mem_sp[p1];
	isp->px=p2;
	isp->py=p3;
	isp->fl |= 0x200;
	return 0;
}


EXPORT BOOL WINAPI es_adir ( HSPEXINFO *hei,int dum1, int dum2, int dum3 )
{
	//		sprite axis add direction set (type0)
	//			es_apos spno, dir, x-prm%
	//
	int p1,p2,p3;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_getdi(100);
	if(*hei->er) return *hei->er;

	adirx(p1,p2,p3);
	return 0;
}


EXPORT BOOL WINAPI es_type ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		set sprite type data (type0)
	//			es_type spno, type
	//
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	mem_sp[p1].type=p2;
	return 0;
}


EXPORT BOOL WINAPI es_kill ( HSPEXINFO *hei, int dum2, int dum3, int dum4 )
{
	//		kill sprite data (type0)
	//			es_kill spno
	//
	int p1=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	mem_sp[p1].fl=0;
	mem_sp[p1].type=0;
	return 0;
}


EXPORT BOOL WINAPI es_aim ( HSPEXINFO *hei, int dum1, int dum2, int dum3 )
{
	//		Set AimDir (type0)
	//			es_aim spno, x, y, x-prm%
	//
	int xx,yy,lx,ly;
	int ang;

	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_getdi(100);
	if(*hei->er) return *hei->er;

	xx=p2;yy=p3;
	sp=&mem_sp[p1];
	if (sp->fl) {
		lx=sp->xx;ly=sp->yy;
		ang=(int)((atan2( (double)(lx-xx), (double)(ly-yy) )+pi+(3.141592/rrate))/pans+rrate)%rrate;
		adirx( p1,ang,p4 );
	}
	return -ang;
}

EXPORT BOOL WINAPI es_ang (HSPEXINFO *hei,int dum1,int dum2,int dum3)
{
	
	int p1,p2,p3,p4;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	p4=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	return -((int)((atan2((double)(p1-p3),(double)(p2-p4))+pi+(3.141592/rrate))/pans+rrate)%rrate);
}

EXPORT BOOL WINAPI es_timer (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int *p1=(int *)hei->HspFunc_prm_getv();
	int  p2=hei->HspFunc_prm_getdi(0);
	if(*hei->er) return *hei->er;

	switch(p2){
	case 0:
		*p1=timeGetTime();
		break;

	case 1:
		*p1=GetTickCount();
		break;
	}
	return 0;
}

EXPORT BOOL WINAPI es_getfps (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int *p1=(int *)hei->HspFunc_prm_getv();
	if(*hei->er) return *hei->er;

	*p1=fps;
	return 0;
}

EXPORT BOOL WINAPI es_islost (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int p1=hei->HspFunc_prm_getdi(-2);
	int p2=hei->HspFunc_prm_getdi(63);
	if(*hei->er) return *hei->er;

	return -(dd_islost(p1,p2));
}

EXPORT BOOL WINAPI es_sin (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int *p1=(int *)hei->HspFunc_prm_getv();
	int  p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	*p1=vpy[(p2+rrate)%rrate];
	return 0;
}

EXPORT BOOL WINAPI es_cos (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int *p1=(int *)hei->HspFunc_prm_getv();
	int  p2=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	*p1=vpx[(p2+rrate)%rrate];
	return 0;
}

EXPORT BOOL WINAPI es_gravity (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	int p1=hei->HspFunc_prm_geti();
	int p2=hei->HspFunc_prm_geti();
	int p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	if(p1>=spkaz) return -1;
	if(p2>32767) p2=32767;
	if(p2<-32767) p2=-32767;
	if(p3>32767) p3=32767;
	if(p3<-32767) p3=-32767;
	if(p1<0){
		def_fspx=p2;
		def_fspy=p3;
	}
	else{
		isp=(ISPOBJ *)&mem_sp[p1];
		isp->fspx=(short)p2;
		isp->fspy=(short)p3;
	}
	return 0;
}

EXPORT BOOL WINAPI es_bound (HSPEXINFO *hei,int dum1,int dum2,int dum3)
{

	int p1,p2,p3;
	p1=hei->HspFunc_prm_geti();
	p2=hei->HspFunc_prm_geti();
	p3=hei->HspFunc_prm_geti();
	if(*hei->er) return *hei->er;

	if(p1>=spkaz) return -1;
	if(p2>0x100) p2=0x100;
	if(p2<0) p2=0;
	if(p3<0) p3=0;
	if(p3>3) p3=3;
	if(p1<0){
		def_bound=p2;
		def_boundflag=p3;
	}else{
		isp=(ISPOBJ *)&mem_sp[p1];
		isp->bound=p2;
		isp->boundflag=p3;
	}
	return 0;
}

EXPORT BOOL WINAPI es_saver (HSPEXINFO *hei,int dum2,int dum3,int dum4)
{
	dd_saver();
	return 0;
}