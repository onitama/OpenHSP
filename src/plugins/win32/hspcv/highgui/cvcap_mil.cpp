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

#include "_highgui.h"
#include "mil.h" 

#if _MSC_VER >= 1200
  #pragma warning( disable: 4711 )
  #pragma comment(lib,"mil.lib")
  #pragma comment(lib,"milmet2.lib")
#endif

#if defined WIN64 && defined EM64T && defined _MSC_VER && !defined __ICL
  #pragma optimize("",off)
#endif




/********************* Capturing video from camera via MIL *********************/

struct 
{      
    MIL_ID MilApplication;
    int MilUser;
} g_Mil = {0,0}; //global structure for handling MIL application

typedef struct CvCaptureCAM_MIL
{
    CvCaptureVTable* vtable;
    MIL_ID 
		MilSystem,       /* System identifier.       */
		MilDisplay,      /* Display identifier.      */
		MilDigitizer,    /* Digitizer identifier.    */ 
		MilImage;        /* Image buffer identifier. */
    IplImage* rgb_frame;
}
CvCaptureCAM_MIL;

// Initialize camera input
static int icvOpenCAM_MIL( CvCaptureCAM_MIL* capture, int wIndex )
{
    if( g_Mil.MilApplication == M_NULL )
    {
        assert(g_Mil.MilUser == 0);
        MappAlloc(M_DEFAULT, &(g_Mil.MilApplication) );
        g_Mil.MilUser = 1;
    }
    else
    {
        assert(g_Mil.MilUser>0);
        g_Mil.MilUser++;
    }
    
    int dev_table[16] = { M_DEV0, M_DEV1, M_DEV2, M_DEV3,
		M_DEV4, M_DEV5, M_DEV6, M_DEV7,
		M_DEV8, M_DEV9, M_DEV10, M_DEV11,
		M_DEV12, M_DEV13, M_DEV14, M_DEV15 };
    
    //set default window size
    int w = 320/*160*/;
    int h = 240/*120*/;
    
    
    for( ; wIndex < 16; wIndex++ ) 
    {
        MsysAlloc( M_SYSTEM_SETUP, //we use default system,
                                   //if this does not work 
                                   //try to define exact board 
                                   //e.g.M_SYSTEM_METEOR,M_SYSTEM_METEOR_II...
                   dev_table[wIndex], 
                   M_DEFAULT, 
                   &(capture->MilSystem) ); 
		
        if( capture->MilSystem != M_NULL )
            break;
    }
    if( capture->MilSystem != M_NULL )
    {
        MdigAlloc(capture->MilSystem,M_DEFAULT,
                  M_CAMERA_SETUP, //default. May be M_NTSC or other
                  M_DEFAULT,&(capture->MilDigitizer));
        
        capture->rgb_frame = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 3 );
        MdigControl(capture->MilDigitizer, M_GRAB_SCALE,  1.0 / 2);
        
        /*below line enables getting image vertical orientation 
			consistent with VFW but it introduces some image corruption 
			on MeteorII, so we left the image as is*/  
        //MdigControl(capture->MilDigitizer, M_GRAB_DIRECTION_Y, M_REVERSE );
		
        capture->MilImage = MbufAllocColor(capture->MilSystem, 3, w, h,
										   8+M_UNSIGNED,
										   M_IMAGE + M_GRAB,                                                      
										   M_NULL);
    }
    
    return capture->MilSystem != M_NULL;
}

static  void icvCloseCAM_MIL( CvCaptureCAM_MIL* capture )
{
    if( capture->MilSystem != M_NULL )
    {
        MdigFree( capture->MilDigitizer );
        MbufFree( capture->MilImage );
        MsysFree( capture->MilSystem );
        cvReleaseImage(&capture->rgb_frame ); 
        capture->rgb_frame = 0;
        
        g_Mil.MilUser--;
        if(!g_Mil.MilUser)
            MappFree(g_Mil.MilApplication);
		
        capture->MilSystem = M_NULL;
        capture->MilDigitizer = M_NULL;
    }
}         


static int icvGrabFrameCAM_MIL( CvCaptureCAM_MIL* capture )
{
    if( capture->MilSystem )
    {
        MdigGrab(capture->MilDigitizer, capture->MilImage);
        return 1;
    }
    return 0;
}


static IplImage* icvRetrieveFrameCAM_MIL( CvCaptureCAM_MIL* capture )
{
    MbufGetColor(capture->MilImage, M_BGR24+M_PACKED, M_ALL_BAND, (void*)(capture->rgb_frame->imageData)); 
    //make image vertical orientation consistent with VFW
    //You can find some better way to do this
    capture->rgb_frame->origin = IPL_ORIGIN_BL;
    cvFlip(capture->rgb_frame,capture->rgb_frame,0);
    return capture->rgb_frame;
}

static double icvGetPropertyCAM_MIL( CvCaptureCAM_MIL* capture, int property_id )
{
    switch( property_id )
    {
		case CV_CAP_PROP_FRAME_WIDTH:
			if( capture->rgb_frame) return capture->rgb_frame->width;
		case CV_CAP_PROP_FRAME_HEIGHT:
			if( capture->rgb_frame) return capture->rgb_frame->height;
    } 
    return 0;
}

static CvCaptureVTable captureCAM_MIL_vtable = 
{
    6,
    (CvCaptureCloseFunc)icvCloseCAM_MIL,
    (CvCaptureGrabFrameFunc)icvGrabFrameCAM_MIL,
    (CvCaptureRetrieveFrameFunc)icvRetrieveFrameCAM_MIL,
    (CvCaptureGetPropertyFunc)icvGetPropertyCAM_MIL,
    (CvCaptureSetPropertyFunc)0,
    (CvCaptureGetDescriptionFunc)0
};

CvCapture* cvCaptureFromCAM_MIL( int index )
{
	CvCaptureCAM_MIL* capture = (CvCaptureCAM_MIL*)cvAlloc( sizeof(*capture));
	memset( capture, 0, sizeof(*capture));
	capture->vtable = &captureCAM_MIL_vtable;
	
	if( icvOpenCAM_MIL( capture, index ))
		return (CvCapture*)capture;
	
	cvReleaseCapture( (CvCapture**)&capture );
	return 0;
}



