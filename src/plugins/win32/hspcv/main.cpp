
//
//		HSP3.1 plugin
//		onion software/onitama 2007/3
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "cvobj.h"
#include "supio.h"
#include "../hpi3sample/hsp3plugin.h"

/* ライブラリ */
#ifdef _DEBUG

#pragma comment(lib, "cv_staticd.lib")
#pragma comment(lib, "cxcore_staticd.lib")
#pragma comment(lib, "highgui_staticd.lib")

#pragma comment(lib, "libjpegd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libtiffd.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "zlibd.lib")

#else

#pragma comment(lib, "cv_static.lib")
#pragma comment(lib, "cxcore_static.lib")
#pragma comment(lib, "highgui_static.lib")

#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "libtiff.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "zlib.lib")

#endif

#pragma comment(lib, "comctl32.lib")

#ifdef HSPCV_USE_VIDEO
#pragma comment(lib, "vfw32.lib")
#endif

int WINAPI DllMain_CX(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved );

 /*------------------------------------------------------------*/
/*
		service
*/
/*------------------------------------------------------------*/

#define OPTSTR_MAX 2048

static	CVOBJ cvobj[CVOBJ_MAX];
static	int area_x, area_y, area_sx, area_sy;
static	int curid;

static	CvSeq* faces;
static	CvMemStorage* storage;
static	CvHaarClassifierCascade* cascade;
static	int cvface_id;
static	int cvface_total;
static	double cvface_scale;
static	char fmtstr[16];
static	char optstr[OPTSTR_MAX];


#ifdef HSPCV_USE_VIDEO
static	CvVideoWriter *wvideo;
static	CvCapture *cvideo;
static	int wvideo_id;
static	int cvideo_id;
#endif


CVOBJ *getcvobj( int id )
{
	if (( id < 0 )||( id >= CVOBJ_MAX )) {
		Alertf( "Illegal cvobj ID(%d)", id );
		return NULL;
	}
	return &cvobj[id];
}


void hspcv_release( int id )
{
	CVOBJ *cv;
	cv = getcvobj(id);
	if ( cv->flag != CVOBJ_FLAG_NONE ) {
		if ( cv->flag == CVOBJ_FLAG_USED ) {
			cvReleaseImage( &cv->img );
		}
		cv->flag = CVOBJ_FLAG_NONE;
		cv->img = NULL;
	}
}


void hspcv_exchange( int id, IplImage *img )
{
	//		idのバッファを差し替える
	//
	CVOBJ *cv;
	cv = getcvobj(id);
	if ( cv->flag != CVOBJ_FLAG_NONE ) {
		hspcv_release( id );
	}
	cv->flag = CVOBJ_FLAG_USED;
	cv->mode = 0;
	cv->img = img;
}


IplImage *hspcv_temp( int sx, int sy )
{
	//		テンポラリバッファを作成する
	//
	IplImage *img;
	CvSize r_size;
	r_size = cvSize( sx, sy );
	img = cvCreateImage( r_size, IPL_DEPTH_8U, 3);
	return img;
}


IplImage *hspcv_tempgray( int sx, int sy )
{
	//		テンポラリバッファを作成する
	//
	IplImage *img;
	CvSize r_size;
	r_size = cvSize( sx, sy );
	img = cvCreateImage( r_size, IPL_DEPTH_8U, 1);
	return img;
}


IplImage *hspcv_temp( int id )
{
	//		idと同じ条件でテンポラリバッファを作成する
	//
	CVOBJ *cv;
	int sx,sy;

	cv = getcvobj(id);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return NULL;

	sx = cv->img->width;
	sy = cv->img->height;

	if(cv->img->nChannels==1){
		return hspcv_tempgray( sx, sy );
	}else{
		return hspcv_temp( sx, sy );
	}
}


int hspcv_picload( int id, char *fname )
{
	CVOBJ *cv;
	hspcv_release( id );
	cv = getcvobj(id);
	cv->img = cvLoadImage( fname, CV_LOAD_IMAGE_COLOR );	// フルカラーで画像をロード
	if ( cv->img == NULL ) return -1;
	cv->flag = CVOBJ_FLAG_USED;
	cv->mode = 0;
	return 0;
}


int hspcv_picsave( int id, char *fname, int opt )
{
	int qu, res;
	CVOBJ *cv;
	cv = getcvobj(id);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;

	qu = opt;if ( qu <= 0 ) qu=95;
	cvSaveSetQuality( qu );
	res = cvSaveImage( fname, cv->img );
	if ( res == 0 ) return -1;
	return 0;
}


void hspcv_init( void )
{
	int i;
	CVOBJ *cv;
	for(i=0;i<CVOBJ_MAX;i++) {
		cv = getcvobj(i);
		cv->flag = CVOBJ_FLAG_NONE;
		cv->mode = 0;
		cv->img = NULL;
	}
	curid = 0;
	area_x = area_y = area_sx = area_sy = 0;
	cvSaveSetJasperFormat( "jp2", "" );

	storage = NULL;
	cascade = NULL;
#ifdef HSPCV_USE_VIDEO
	wvideo = NULL;
	cvideo = NULL;
#endif
}


void hspcv_term( void )
{
	int i;

#ifdef HSPCV_USE_VIDEO
	if ( wvideo != NULL ) { cvReleaseVideoWriter( &wvideo ); }
	if ( cvideo != NULL ) { cvReleaseCapture( &cvideo ); }
#endif
	if ( storage != NULL ) {
		cvReleaseMemStorage( &storage );
		storage = NULL;
	}
	for(i=0;i<CVOBJ_MAX;i++) {
		hspcv_release( i );
	}
}


int hspcv_getinfo( int id, int infoid )
{
	int res;
	CVOBJ *cv;
	IplImage *img;

	res = 0;
	cv = getcvobj(id);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;

	img = cv->img;
	switch( infoid ) {
	case CVOBJ_INFO_SIZEX:
		res = img->width;
		break;
	case CVOBJ_INFO_SIZEY:
		res = img->height;
		break;
	case CVOBJ_INFO_CHANNEL:
		res = img->nChannels;
		break;
	case CVOBJ_INFO_BIT:
		res = img->depth;
		break;
	default:
		break;
	}
	return res;
}


/*------------------------------------------------------------*/
/*
		DLL interface
*/
/*------------------------------------------------------------*/

//		cxcore側のDllMainを使用する
//
int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved )
{
	DllMain_CX( hInstance, fdwReason, pvReserved );
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		hspcv_init();
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		hspcv_term();
	}
	return TRUE ;
}

 /*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI cvreset( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	hspcv_term();
	return 0;
}


EXPORT BOOL WINAPI cvsel( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	CVOBJ *cv;
	curid = p1;
	cv = getcvobj(p1);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	return 0;
}


EXPORT BOOL WINAPI cvbuffer( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	//		cvbuffer cvid, sx, sy
	IplImage *img;
	int sx,sy;

	sx = p2; if ( sx <= 0 ) sx = 640;
	sy = p3; if ( sy <= 0 ) sy = 480;
	img = hspcv_temp( sx, sy );
	hspcv_exchange( p1, img );
	return 0;
}


EXPORT BOOL WINAPI cvgetimg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$02)
	//		cvgetimg cvid
	//
	int x,y,sx,sy;
	char *p;
	char *src;
	char *base;
	char a1;
	IplImage *img;
	CVOBJ *cv;
	int ep1,ep2,ep3;
	BMSCR *bm;

	ep1 = hei->HspFunc_prm_getdi(curid);	// パラメータ1:数値
	//ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	//ep3 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	ep2 = 0;
	ep3 = 0;

	bm = (BMSCR *)hei->HspFunc_getbmscr( *(hei->actscr) );
	cv = getcvobj(ep1);
	img = cv->img;
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;

	sx = img->width;
	sy = img->height;
//	Alertf( "%d x %d (col%d:depth%d)",sx,sy,img->nChannels,img->depth );

	if ( sx > bm->sx ) sx = bm->sx;
	if ( sy > bm->sy ) sy = bm->sy;
	base = ((char *)bm->pBit) + ( ep2 * 3 ) + ( bm->sx2 * ( bm->sy - 1 ) );

	if ( img->nChannels == 3 ) {			// RGB
		for(y=0;y<sy;y++) {
			src = img->imageData + ( img->widthStep * y );
			p = base - ( bm->sx2 * ( y + ep3 ) );
			for(x=0;x<(sx*3);x++) {
				*p++ = *src++;
			}
		}
	}
	if ( img->nChannels == 1 ) {			// GRAYSCALE
		for(y=0;y<sy;y++) {
			src = img->imageData + ( img->widthStep * y );
			p = base - ( bm->sx2 * ( y + ep3 ) );
			for(x=0;x<sx;x++) {
				a1 = *src++;
				*p++ = a1;
				*p++ = a1;
				*p++ = a1;
			}
		}
	}
	return 0;

}


EXPORT BOOL WINAPI cvputimg( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$02)
	//		cvputimg cvid
	//
	int x,y,sx,sy;
	char *p;
	char *src;
	char *base;
	IplImage *img;
	CVOBJ *cv;
	int ep1,ep2,ep3;
	BMSCR *bm;

	ep1 = hei->HspFunc_prm_getdi(curid);	// パラメータ1:数値
	ep2 = 0;
	ep3 = 0;

	bm = (BMSCR *)hei->HspFunc_getbmscr( *(hei->actscr) );
	cv = getcvobj(ep1);
	img = cv->img;
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;

	sx = bm->sx;
	sy = bm->sy;
//	Alertf( "%d x %d (col%d:depth%d)",sx,sy,img->nChannels,img->depth );

	if ( sx > img->width ) sx = img->width;
	if ( sy > img->height ) sy = img->height;
	base = ((char *)bm->pBit) + ( ep2 * 3 ) + ( bm->sx2 * ( bm->sy - 1 ) );

	if ( img->nChannels == 3 ) {			// RGB
		for(y=0;y<sy;y++) {
			src = img->imageData + ( img->widthStep * y );
			p = base - ( bm->sx2 * ( y + ep3 ) );
			for(x=0;x<(sx*3);x++) {
				*src++ = *p++;
			}
		}
	}
	return 0;

}


EXPORT BOOL WINAPI cvload( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvload "filename",id
	//
	char *ep1;
	int ep2;
	int i;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdi(curid);	// パラメータ2:数値
	i = hspcv_picload( ep2, ep1 );
	return i;
}


EXPORT BOOL WINAPI cvsave( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvsave "filename", id, opt
	//
	char *ep1;
	int ep2,ep3;
	int i;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdi(curid);	// パラメータ2:数値
	ep3 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	i = hspcv_picsave( ep2, ep1, ep3 );
	return i;
}


EXPORT BOOL WINAPI cvj2opt( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvj2opt "format", "option"
	//
	char *ep1;
	char *ep2;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	strncpy( fmtstr, ep1, 15 );
	ep2 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy( optstr, ep2, OPTSTR_MAX-1 );

	//cvSaveSetJasperFormat( "jp2", "rate=0.1" );
	cvSaveSetJasperFormat( fmtstr, optstr );
	return 0;
}


EXPORT BOOL WINAPI cvgetinfo( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	情報を得る(変数に値を代入)
	//		cvgetinfo 変数, infoid, id
	//
	PVal *pv;
	APTR ap;
	int ep1,ep2;
	int res;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	ep2 = hei->HspFunc_prm_getdi(curid);	// パラメータ3:数値
	res = hspcv_getinfo( ep1, ep2 );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &res );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI cvresize( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$00)
	//		cvresize sx, sy, cvid, opt
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep2,ep3,ep4;

	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	ep3 = hei->HspFunc_prm_getdi(curid);	// パラメータ3:数値
	ep4 = hei->HspFunc_prm_getdi(CV_INTER_LINEAR);		// パラメータ4:数値

	cv = getcvobj(ep3);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	img = hspcv_temp( ep1, ep2 );
	cvResize( cv->img, img, ep4 );
	hspcv_exchange( ep3, img );
	return 0;
}


EXPORT BOOL WINAPI cvsmooth( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvsmooth type, opt1, opt2, opt3, id
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep2,ep3,ep4,ep5;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	ep3 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	ep4 = hei->HspFunc_prm_getdi(0);		// パラメータ4:数値
	ep5 = hei->HspFunc_prm_getdi(curid);	// パラメータ5:数値

	cv = getcvobj(ep5);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	img = hspcv_temp( ep5 );
	cvSmooth( cv->img, img, ep1, ep2, ep3, ep4 );
	//cvCopy( img, cv->img );
	hspcv_exchange( ep5, img );

	return 0;
}


EXPORT BOOL WINAPI cvthreshold( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvthreshold type, opt1, opt2, id
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep4;
	double ep2,ep3;
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdd(0.0);		// パラメータ2:実数値
	ep3 = hei->HspFunc_prm_getdd(0.0);		// パラメータ3:実数値
	ep4 = hei->HspFunc_prm_getdi(curid);	// パラメータ4:数値

	cv = getcvobj(ep4);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	img = hspcv_temp( ep4 );
	cvThreshold( cv->img, img, ep2, ep3, ep1 );
	hspcv_exchange( ep4, img );

	return 0;
}


EXPORT BOOL WINAPI cvrotate( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvrotate angle, scale, offsetx, offsety, type, id
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep2;
	double ang, scale, ofsx, ofsy;
	CvPoint2D32f center;
	CvMat *rot_mat;

	ang = hei->HspFunc_prm_getdd( 0.0 );
	scale = hei->HspFunc_prm_getdd( 1.0 );
	ofsx = hei->HspFunc_prm_getdd( 0.0 );
	ofsy = hei->HspFunc_prm_getdd( 0.0 );
	ep1 = hei->HspFunc_prm_getdi(CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS);		// パラメータ5:数値
	ep2 = hei->HspFunc_prm_getdi(curid);		// パラメータ6:数値
	cv = getcvobj(ep2);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	img = hspcv_temp( ep2 );
	center = cvPoint2D32f( (double)img->width/2.0+ofsx, (double)img->height/2.0+ofsy );	//回転の中心
	rot_mat = cvCreateMat( 2, 3, CV_32FC1);				//    変換行列
	cv2DRotationMatrix( center, ang, scale, rot_mat);
	cvWarpAffine( cv->img, img, rot_mat, ep1 );
	hspcv_exchange( ep2, img );
	cvReleaseMat( &rot_mat);
	return 0;
}


EXPORT BOOL WINAPI cvarea( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvarea x,y,sx,sy
	area_x  = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	area_y  = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	area_sx = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	area_sy = hei->HspFunc_prm_getdi(0);		// パラメータ4:数値
	return 0;
}


EXPORT BOOL WINAPI cvcopy( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvcopy src_id, dst_x, dst_y, dst_id, opt
	CVOBJ *cv_src;
	CVOBJ *cv_dst;
	int ep1,ep2,ep3,ep4,ep5;
	int dsx,dsy,ssx,ssy;
	int res;

	ep3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ3:数値
	ep4 = hei->HspFunc_prm_getdi(curid);	// パラメータ4:数値
	ep5 = hei->HspFunc_prm_getdi(0);		// パラメータ5:数値

	cv_src = getcvobj(ep3);
	cv_dst = getcvobj(ep4);
	if ( ep3 == ep4 ) return -1;
	if ( cv_src->flag == CVOBJ_FLAG_NONE ) return -1;
	if ( cv_dst->flag == CVOBJ_FLAG_NONE ) return -1;

	dsx = area_sx; if ( dsx <= 0 ) dsx = cv_dst->img->width;
	dsy = area_sy; if ( dsy <= 0 ) dsy = cv_dst->img->height;

	ssx = area_sx; if ( ssx <= 0 ) ssx = cv_src->img->width;
	ssy = area_sy; if ( ssy <= 0 ) ssy = cv_src->img->height;

	if (( dsx != ssx )||( dsy != ssy )) return -1;

	cvSetImageROI( cv_dst->img, cvRect(ep1,ep2,dsx,dsy) );
	cvSetImageROI( cv_src->img, cvRect(area_x,area_y,ssx,ssy) );

	res = 0;
	switch( ep5 ) {
	case CVCOPY_SET:
		cvCopy( cv_src->img, cv_dst->img );
		break;
	case CVCOPY_ADD:
		cvAdd( cv_src->img, cv_dst->img, cv_dst->img );
		break;
	case CVCOPY_SUB:
		cvSub( cv_src->img, cv_dst->img, cv_dst->img );
		break;
	case CVCOPY_MUL:
		cvMul( cv_src->img, cv_dst->img, cv_dst->img );
		break;
	case CVCOPY_DIF:
		cvAbsDiff( cv_src->img, cv_dst->img, cv_dst->img );
		break;
	case CVCOPY_AND:
		cvAnd( cv_src->img, cv_dst->img, cv_dst->img );
		break;
	default:
		res = -1;
		break;
	}

	cvResetImageROI( cv_dst->img );
	cvResetImageROI( cv_src->img );
	return res;
}


EXPORT BOOL WINAPI cvxors( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvarea r,g,b,id
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep2,ep3,ep4;

	ep1 = hei->HspFunc_prm_getdi(255);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(255);		// パラメータ2:数値
	ep3 = hei->HspFunc_prm_getdi(255);		// パラメータ3:数値
	ep4 = hei->HspFunc_prm_getdi(curid);	// パラメータ4:数値

	cv = getcvobj(ep4);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	img = hspcv_temp( ep4 );
	cvXorS( cv->img, CV_RGB(ep1,ep2,ep3), img, NULL );
	hspcv_exchange( ep4, img );
	return 0;
}


EXPORT BOOL WINAPI cvflip( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvflip mode,id
	//		( mode: 0 = X-axis / 1 = Y-axis / -1 = XY axis )
	CVOBJ *cv;
	int ep1,ep2;

	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(curid);	// パラメータ2:数値

	cv = getcvobj(ep2);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	cvFlip( cv->img, NULL, ep1 );
	return 0;
}


EXPORT BOOL WINAPI cvconvert( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvconvert type, id
	//		(type:0=RGB->GRAY/1=GRAY->RGB)
	CVOBJ *cv;
	IplImage *img;
	int ep1,ep2;
	int sx,sy;
	ep1 = hei->HspFunc_prm_getdi(0);				// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(curid);			// パラメータ2:数値

	cv = getcvobj(ep2);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;

	sx = cv->img->width;
	sy = cv->img->height;
	switch( ep1 ) {
	case 0:
		img = hspcv_tempgray( sx, sy );
	    cvCvtColor( cv->img, img, CV_RGB2GRAY );
		break;
	case 1:
		img = hspcv_temp( sx, sy );
	    cvCvtColor( cv->img, img, CV_GRAY2RGB );
		break;
	}
	hspcv_exchange( ep2, img );
	return 0;
}


EXPORT BOOL WINAPI cvloadxml( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvloadxml "filename"
	//
	char *fname;
	fname = hei->HspFunc_prm_gets();		// パラメータ1:文字列
    cascade = (CvHaarClassifierCascade*)cvLoad( fname, 0, 0, 0 );
	if ( cascade == NULL ) return -1;
	return 0;
}


EXPORT BOOL WINAPI cvfacedetect( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvfacedetect id, scale
	//		(stat = num of faces)
	CVOBJ *cv;
	IplImage *img;
	int ep1;

	ep1 = hei->HspFunc_prm_getdi(curid);	// パラメータ1:数値
	cvface_scale = hei->HspFunc_prm_getdd(1.0);	// パラメータ2:数値

	cv = getcvobj(ep1);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return 0;
	img = cv->img;

	if ( cascade == NULL ) return 0;
	if ( storage != NULL ) {
		cvReleaseMemStorage( &storage );
		storage = NULL;
	}
    storage = cvCreateMemStorage(0);

	IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/cvface_scale), cvRound (img->height/cvface_scale)), 8, 1 );

    cvCvtColor( img, gray, CV_BGR2GRAY );
	if ( cvface_scale != 1.0 ) {
	    cvResize( gray, small_img, CV_INTER_LINEAR );
	    cvEqualizeHist( small_img, small_img );
	} else {
	    cvEqualizeHist( gray, small_img );
	}
    cvClearMemStorage( storage );

	faces = cvHaarDetectObjects( small_img, cascade, storage,
								 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
								 cvSize(30, 30) );

	cvface_id = 0;
	cvface_total = faces->total;

    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );

	return -cvface_total;
}


EXPORT BOOL WINAPI cvgetface( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvgetface var_x, var_y, var_sx, var_sy
	//		(stat:0=ok/1=no data)
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	PVal *pv4;
	APTR ap4;
	int x,y,sx,sy;

	ap  = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ2:変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );		// パラメータ3:変数
	ap4 = hei->HspFunc_prm_getva( &pv4 );		// パラメータ4:変数

	if ( cvface_id >= cvface_total ) {
		return -1;
	}

	CvRect* r = (CvRect*)cvGetSeqElem( faces, cvface_id );

	x = cvRound( r->x * cvface_scale );
	y = cvRound( r->y * cvface_scale );
	sx = cvRound( r->width * cvface_scale );
	sy = cvRound( r->height * cvface_scale );

	hei->HspFunc_prm_setva( pv,  ap,  HSPVAR_FLAG_INT, &x );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, HSPVAR_FLAG_INT, &y );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, HSPVAR_FLAG_INT, &sx );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv4, ap4, HSPVAR_FLAG_INT, &sy );	// 変数に値を代入

	cvface_id++;
	if ( cvface_id >= cvface_total ) {
		//		データを最後まで取ったらメモリを解放する
		cvReleaseMemStorage( &storage );
		storage = NULL;
	}

	return 0;
}


EXPORT BOOL WINAPI cvmatch( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvmatch x, y, mode, src_id, target_id
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	CVOBJ *cv_src;
	CVOBJ *cv_target;
	int ep1,ep2,ep3;
	IplImage *result;
	double  min_val, max_val;
	CvPoint min_loc, max_loc;
	int x,y;

	ap  = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ2:変数
	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ3:数値
	ep2 = hei->HspFunc_prm_getdi(0);			// パラメータ4:数値
	ep3 = hei->HspFunc_prm_getdi(curid);		// パラメータ5:数値

	cv_src = getcvobj(ep2);
	if ( cv_src->flag == CVOBJ_FLAG_NONE ) return -1;
	cv_target = getcvobj(ep3);
	if ( cv_target->flag == CVOBJ_FLAG_NONE ) return -1;

	result = cvCreateImage( cvSize(cv_target->img->width - cv_src->img->width +1,  cv_target->img->height - cv_src->img->height+1 ), IPL_DEPTH_32F, 1 );
	cvMatchTemplate( cv_target->img, cv_src->img, result, ep1 );
	cvMinMaxLoc( result, &min_val, &max_val, &min_loc, &max_loc, NULL );

	x = min_loc.x;
	y = min_loc.y;
	hei->HspFunc_prm_setva( pv,  ap,  HSPVAR_FLAG_INT, &x );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, HSPVAR_FLAG_INT, &y );	// 変数に値を代入

	cvReleaseImage( &result );
	return 0;
}


/*
		For Video Input/Output
*/

#ifdef HSPCV_USE_VIDEO

EXPORT BOOL WINAPI cvcapture( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvcapture camid,id
	int ep1,ep2;
	CVOBJ *cv;

	ep1  = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値
	ep2  = hei->HspFunc_prm_getdi(curid);		// パラメータ2:数値

	cv = getcvobj(ep2);
	//if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	if ( cvideo != NULL ) return -1;
	cvideo_id = ep2;
	cvideo = cvCaptureFromCAM( ep1 );
	if ( cvideo == NULL ) return -1;
/*
	int width = (int)cvGetCaptureProperty(cvideo,CV_CAP_PROP_FRAME_WIDTH);
	int height = (int)cvGetCaptureProperty(cvideo,CV_CAP_PROP_FRAME_HEIGHT);
	int fps = (int)cvGetCaptureProperty(cvideo,CV_CAP_PROP_FPS);
	Alertf( "%dx%d fps%d", width, height, fps );
*/
	return 0;
}


EXPORT BOOL WINAPI cvgetcapture( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvgetcapture
	CVOBJ *cv;
	IplImage *img;
	if ( cvideo == NULL ) return -1;
	cv = getcvobj(cvideo_id);
	img = cvQueryFrame( cvideo );
	hspcv_exchange( cvideo_id, img );
	cv->flag = CVOBJ_FLAG_VIDEOCAP;			// このIDは解放の必要なし

	return 0;
}


EXPORT BOOL WINAPI cvendcapture( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvendcapture
	if ( cvideo == NULL ) return -1;
	cvReleaseCapture( &cvideo );
	cvideo = NULL;
	return 0;
}


EXPORT BOOL WINAPI cvopenavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvmakeavi "filename",id
	char *fname;
	int ep1;
	CVOBJ *cv;

	fname = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	ep1  = hei->HspFunc_prm_getdi(curid);		// パラメータ2:数値

	cv = getcvobj(ep1);
	//if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	if ( cvideo != NULL ) return -1;
	cvideo_id = ep1;
	cvideo = cvCaptureFromAVI( fname );
	if ( cvideo == NULL ) return -1;

	return 0;
}


EXPORT BOOL WINAPI cvgetavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvgetavi
	CVOBJ *cv;
	IplImage *img;
	if ( cvideo == NULL ) return -1;
	cv = getcvobj(cvideo_id);
	img = cvQueryFrame( cvideo );
	if ( img == NULL ) return -1;
	hspcv_exchange( cvideo_id, img );
	cv->flag = CVOBJ_FLAG_VIDEOCAP;			// このIDは解放の必要なし

	return 0;
}


EXPORT BOOL WINAPI cvcloseavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvcloseavi
	if ( cvideo == NULL ) return -1;
	cvReleaseCapture( &cvideo );
	cvideo = NULL;
	return 0;
}


EXPORT BOOL WINAPI cvmakeavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvmakeavi "filename",codec,fps,id
	char *fname;
	int ep1,sx,sy,codec;
	double fps;
	CVOBJ *cv;

	fname = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	codec = hei->HspFunc_prm_getdi(-1);			// パラメータ2:数値
	fps = hei->HspFunc_prm_getdd(29.97);		// パラメータ3:数値
	ep1  = hei->HspFunc_prm_getdi(curid);		// パラメータ4:数値

	cv = getcvobj(ep1);
	if ( cv->flag == CVOBJ_FLAG_NONE ) return -1;
	if ( wvideo != NULL ) return -1;
	sx = cv->img->width;
	sy = cv->img->height;
	wvideo_id = ep1;

	wvideo = cvCreateVideoWriter( fname, codec, fps, cvSize( sx, sy ), 1 );
	if ( wvideo == NULL ) return -1;

	return 0;
}


EXPORT BOOL WINAPI cvputavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvputavi
	CVOBJ *cv;

	if ( wvideo == NULL ) return -1;
	cv = getcvobj(wvideo_id);
	cvWriteFrame( wvideo, cv->img );
	return 0;
}


EXPORT BOOL WINAPI cvendavi( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		cvendavi
	if ( wvideo == NULL ) return -1;
	cvReleaseVideoWriter( &wvideo );
	wvideo = NULL;
	return 0;
}

#endif


