/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __HIGHGUI_H_
#define __HIGHGUI_H_

#include "highgui.h"
#include "cxmisc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#ifndef WIN32
#include "cvconfig.h"
#else
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin );
#endif

/* Errors */
#define HG_OK          0 /* Don't bet on it! */
#define HG_BADNAME    -1 /* Bad window or file name */
#define HG_INITFAILED -2 /* Can't initialize HigHGUI. Possibly, can't find vlgrfmts.dll */
#define HG_WCFAILED   -3 /* Can't create a window */
#define HG_NULLPTR    -4 /* The null pointer where it should not appear */
#define HG_BADPARAM   -5 

#define CV_WINDOW_MAGIC_VAL     0x00420042
#define CV_TRACKBAR_MAGIC_VAL   0x00420043

/***************************** CvCapture structure ******************************/

#define CV_CAPTURE_BASE_API_COUNT 6

typedef void         (CV_CDECL* CvCaptureCloseFunc)         ( CvCapture* capture );
typedef int          (CV_CDECL* CvCaptureGrabFrameFunc)     ( CvCapture* capture );
typedef IplImage   * (CV_CDECL* CvCaptureRetrieveFrameFunc) ( CvCapture* capture );
typedef double       (CV_CDECL* CvCaptureGetPropertyFunc)   ( CvCapture* capture, int id );
typedef int          (CV_CDECL* CvCaptureSetPropertyFunc)   ( CvCapture* capture,
                                                              int id, double value );
typedef const char * (CV_CDECL* CvCaptureGetDescriptionFunc)( CvCapture* capture );

typedef struct CvCaptureVTable
{
    int                           count;
    CvCaptureCloseFunc            close;
    CvCaptureGrabFrameFunc        grab_frame;
    CvCaptureRetrieveFrameFunc    retrieve_frame;
    CvCaptureGetPropertyFunc      get_property;
    CvCaptureSetPropertyFunc      set_property;
    CvCaptureGetDescriptionFunc   get_description;
}
CvCaptureVTable;

typedef struct CvCapture
{
    CvCaptureVTable* vtable;
}
CvCapture;


#ifdef WIN32
#define HAVE_VFW 1

/* uncomment to enable OpenEXR codec (will not compile under MSVC6) */ 
//#define HAVE_ILMIMF 1

/* uncomment to enable CMUCamera1394 fireware camera module */
//#define HAVE_CMU1394 1
#endif


#if defined (HAVE_CAMV4L) || defined (HAVE_CAMV4L2)
CvCapture * cvCaptureFromCAM_V4L( int index );
#endif

#ifdef HAVE_DC1394
CvCapture * cvCaptureFromCAM_DC1394( int index );
#endif

#ifdef HAVE_MIL
CvCapture* cvCaptureFromCAM_MIL( int index );
#endif

#ifdef HAVE_CMU1394
CvCapture * cvCaptureFromCAM_CMU( int index );
#endif

#ifdef HAVE_TYZX
CV_IMPL CvCapture * cvCaptureFromCAM_TYZX( int index );
#endif

#ifdef WIN32
CvCapture* cvCaptureFromCAM_VFW( int index );
CvCapture* cvCaptureFromFile_VFW( const char* filename );
#endif

#ifdef HAVE_XINE
CvCapture* cvCaptureFromFile_XINE   (const char* filename);
#endif

#ifdef HAVE_FFMPEG
CvCapture* cvCaptureFromFile_FFMPEG (const char* filename);
#endif

#ifdef HAVE_QUICKTIME
CvCapture * cvCaptureFromFile_QT (const char  * filename);
CvCapture * cvCaptureFromCAM_QT  (const int     index);
#endif

#endif /* __HIGHGUI_H_ */
