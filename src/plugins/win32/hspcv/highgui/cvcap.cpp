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

#if _MSC_VER >= 1200
#pragma warning( disable: 4711 )
#endif

#if defined WIN64 && defined EM64T && defined _MSC_VER && !defined __ICL
#pragma optimize("",off)
#endif


/************************* Reading AVIs & Camera data **************************/

CV_IMPL void cvReleaseCapture( CvCapture** pcapture )
{
    if( pcapture && *pcapture )
    {
        CvCapture* capture = *pcapture;
        if( capture && capture->vtable &&
            capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
            capture->vtable->close )
            capture->vtable->close( capture );
        cvFree( pcapture );
    }
}

CV_IMPL IplImage* cvQueryFrame( CvCapture* capture )
{
    if( capture && capture->vtable &&
        capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
        capture->vtable->grab_frame && capture->vtable->retrieve_frame &&
        capture->vtable->grab_frame( capture ))
        return capture->vtable->retrieve_frame( capture );
    return 0;
}

CV_IMPL int cvGrabFrame( CvCapture* capture )
{
    if( capture && capture->vtable &&
        capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
        capture->vtable->grab_frame )
        return capture->vtable->grab_frame( capture );
    return 0;
} 

CV_IMPL IplImage* cvRetrieveFrame( CvCapture* capture )
{
    if( capture && capture->vtable &&
        capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
        capture->vtable->retrieve_frame )
        return capture->vtable->retrieve_frame( capture );
    return 0;
}                                       

CV_IMPL double cvGetCaptureProperty( CvCapture* capture, int id )
{
    if( capture && capture->vtable &&
        capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
        capture->vtable->get_property )
        return capture->vtable->get_property( capture, id );
    return 0;
}

CV_IMPL int cvSetCaptureProperty( CvCapture* capture, int id, double value )
{
    if( capture && capture->vtable &&
        capture->vtable->count >= CV_CAPTURE_BASE_API_COUNT &&
        capture->vtable->set_property )
        return capture->vtable->set_property( capture, id, value );
    return 0;
}


/**
 * Camera dispatching method: index is the camera number.
 * If given an index from 0 to 99, it tries to find the first
 * API that can access a given camera index.
 * Add multiples of 100 to select an API.
 */
CV_IMPL CvCapture * cvCaptureFromCAM (int index)
{
    int  domains[] = 
	{
		CV_CAP_IEEE1394,   // identical to CV_CAP_DC1394 
		CV_CAP_STEREO, 
		CV_CAP_VFW,        // identical to CV_CAP_V4L
		CV_CAP_MIL, 
		CV_CAP_QT,
		-1 
	};
	

    // interpret preferred interface (0 = autodetect)
    int pref = (index / 100) * 100;
    if (pref) 
	{
		domains[0]=pref;
		index %= 100;
		domains[1]=-1;
    }

    // try every possibly installed camera API
    for (int i = 0; domains[i] >= 0; i++) 
	{

		// local variable to memorize the captured device
		CvCapture *capture;

		switch (domains[i]) 
		{
		#ifdef HAVE_TYZX
		case CV_CAP_STEREO:
			capture = cvCaptureFromCAM_TYZX (index);
			if (capture)
				return capture;
		#endif

		#if   defined (HAVE_VFW)
		case CV_CAP_VFW:
			capture = cvCaptureFromCAM_VFW (index);
			if (capture) 
				return capture;
		#elif defined (HAVE_CAMV4L) || defined (HAVE_CAMV4L2)
		case CV_CAP_V4L:
			capture = cvCaptureFromCAM_V4L (index);
			if (capture) 
				return capture;
		#endif

		#if   defined (HAVE_DC1394)
		case CV_CAP_DC1394:
			capture = cvCaptureFromCAM_DC1394 (index);
			if (capture)
				return capture;
		#elif defined (HAVE_CMU1394)        
			case CV_CAP_IEEE1394:
			capture = cvCaptureFromCAM_CMU (index);
			if (capture)
				return capture;
		#endif

		#ifdef HAVE_MIL
		case CV_CAP_MIL:
			capture = cvCaptureFromCAM_MIL (index);
			if (capture) 
				return capture;
		#endif
		
		#ifdef HAVE_QUICKTIME
		case CV_CAP_QT:
			capture = cvCaptureFromCAM_QT (index);
			if (capture)
				return capture;
		#endif
        }
    }

	// failed open a camera
    return 0;
}

/**
 * Videoreader dispatching method: it tries to find the first
 * API that can access a given filename.
 */
CV_IMPL CvCapture * cvCaptureFromFile (const char * filename)
{
	CvCapture * result = 0;
	
	#ifdef HAVE_VFW
	if (! result)
		result = cvCaptureFromFile_VFW (filename);
	#endif
	
	#ifdef HAVE_XINE
	if (! result)
		result = cvCaptureFromFile_XINE (filename);
	#endif
	
	#ifdef HAVE_FFMPEG
	if (! result)
		result = cvCaptureFromFile_FFMPEG (filename);
	#endif
	
	#ifdef HAVE_QUICKTIME
	if (! result)
		result = cvCaptureFromFile_QT (filename);
	#endif
	
	return result;
}

#ifndef HAVE_FFMPEG
#ifndef HAVE_VFW
#ifndef HAVE_QUICKTIME

// quick fix for rc1

CV_IMPL CvVideoWriter* cvCreateVideoWriter( const char* filename, int fourcc,
                                            double fps, CvSize frameSize, int is_color )
{
    return 0;
}

CV_IMPL int cvWriteFrame( CvVideoWriter* _writer, const IplImage* image )
{
  return 0;
}

CV_IMPL void cvReleaseVideoWriter( CvVideoWriter** writer )
{
}

#endif
#endif
#endif
