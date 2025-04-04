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

#include <vfw.h>
#if _MSC_VER >= 1200
#pragma warning( disable: 4711 )
#endif

#if defined WIN64 && defined EM64T && defined _MSC_VER && !defined __ICL
#pragma optimize("",off)
#endif


CvCapture* cvCaptureFromFile_VFW (const char* filename);
CvCapture* cvCaptureFromCAM_VFW  (int index);

/********************* Capturing video from AVI via VFW ************************/

static BITMAPINFOHEADER icvBitmapHeader( int width, int height, int bpp, int compression = BI_RGB )
{
    BITMAPINFOHEADER bmih;
    memset( &bmih, 0, sizeof(bmih));
    bmih.biSize = sizeof(bmih);
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biBitCount = (WORD)bpp;
    bmih.biCompression = compression;
    bmih.biPlanes = 1;

    return bmih;
}

static void icvInitCapture_VFW()
{
    static int isInitialized = 0;
    if( !isInitialized )
    {
        AVIFileInit();
        isInitialized = 1;
    }
}


typedef struct CvCaptureAVI_VFW
{
    CvCaptureVTable   * vtable;
    PAVIFILE            avifile;
    PAVISTREAM          avistream;
    PGETFRAME           getframe;
    AVISTREAMINFO       aviinfo;
    BITMAPINFOHEADER  * bmih;
    CvSlice             film_range;
    double              fps;
    int                 pos;
    IplImage            frame;
    CvSize              size;
}
CvCaptureAVI_VFW;


static void icvCloseAVI_VFW( CvCaptureAVI_VFW* capture )
{
    if( capture->getframe )
    {
        AVIStreamGetFrameClose( capture->getframe );
        capture->getframe = 0;
    }
    if( capture->avistream )
    {
        AVIStreamRelease( capture->avistream );
        capture->avistream = 0;
    }
    if( capture->avifile )
    {
        AVIFileRelease( capture->avifile );
        capture->avifile = 0;
    }
    capture->bmih = 0;
    capture->pos = 0;
    capture->film_range.start_index = capture->film_range.end_index = 0;
    memset( &capture->frame, 0, sizeof(capture->frame));
}

static int icvOpenAVI_VFW( CvCaptureAVI_VFW* capture, const char* filename )
{
    HRESULT hr;

    icvInitCapture_VFW();

    if( !capture )
        return 0;

    hr = AVIFileOpen( &capture->avifile, filename, OF_READ, NULL );
    if( SUCCEEDED(hr))
    {
        hr = AVIFileGetStream( capture->avifile, &capture->avistream, streamtypeVIDEO, 0 );
        if( SUCCEEDED(hr))
        {
            hr = AVIStreamInfo( capture->avistream, &capture->aviinfo,
                                    sizeof(capture->aviinfo));
            if( SUCCEEDED(hr))
            {
                capture->size.width = capture->aviinfo.rcFrame.right -
                                      capture->aviinfo.rcFrame.left;
                capture->size.height = capture->aviinfo.rcFrame.bottom -
                                      capture->aviinfo.rcFrame.top;
                BITMAPINFOHEADER bmih = icvBitmapHeader(
                    capture->size.width, capture->size.height, 24 );
                
                capture->film_range.start_index = (int)capture->aviinfo.dwStart;
                capture->film_range.end_index = capture->film_range.start_index +
                                                (int)capture->aviinfo.dwLength;
                capture->fps = ((double)capture->aviinfo.dwRate)/capture->aviinfo.dwScale;
                capture->pos = capture->film_range.start_index;
                capture->getframe = AVIStreamGetFrameOpen( capture->avistream, &bmih );
                if( capture->getframe != 0 )
                    return 1;
            }
        }
    }

    icvCloseAVI_VFW( capture );
    return 0;
}

static int icvGrabFrameAVI_VFW( CvCaptureAVI_VFW* capture )
{
    if( capture->avistream )
    {
        capture->bmih = (BITMAPINFOHEADER*)
            AVIStreamGetFrame( capture->getframe, capture->pos++ );
    }

    return capture->bmih != 0;
}

static const IplImage* icvRetrieveFrameAVI_VFW( CvCaptureAVI_VFW* capture )
{
    if( capture->avistream && capture->bmih )
    {
        cvInitImageHeader( &capture->frame,
                           cvSize( capture->bmih->biWidth,
                                   capture->bmih->biHeight ),
                           IPL_DEPTH_8U, 3, IPL_ORIGIN_BL, 4 );
        capture->frame.imageData = capture->frame.imageDataOrigin =
            (char*)(capture->bmih + 1);
        return &capture->frame;
    }

    return 0;
}

static double icvGetPropertyAVI_VFW( CvCaptureAVI_VFW* capture, int property_id )
{
    switch( property_id )
    {
    case CV_CAP_PROP_POS_MSEC:
        return cvRound(capture->pos*1000./capture->fps);
    case CV_CAP_PROP_POS_FRAMES:
        return capture->pos;
    case CV_CAP_PROP_POS_AVI_RATIO:
        return (capture->pos - capture->film_range.start_index)/
               (capture->film_range.end_index - capture->film_range.start_index + 1e-10);
    case CV_CAP_PROP_FRAME_WIDTH:
        return capture->size.width;
    case CV_CAP_PROP_FRAME_HEIGHT:
        return capture->size.height;
    case CV_CAP_PROP_FPS:
        return capture->fps;
    case CV_CAP_PROP_FOURCC:
        return capture->aviinfo.fccHandler;
    case CV_CAP_PROP_FRAME_COUNT:
        return capture->film_range.end_index - capture->film_range.start_index;
    }
    return 0;
}


static int icvSetPropertyAVI_VFW( CvCaptureAVI_VFW* capture,
                                  int property_id, double value )
{
    switch( property_id )
    {
    case CV_CAP_PROP_POS_MSEC:
    case CV_CAP_PROP_POS_FRAMES:
    case CV_CAP_PROP_POS_AVI_RATIO:
        {
            int pos;
            switch( property_id )
            {
            case CV_CAP_PROP_POS_MSEC:
                pos = cvRound(value*capture->fps*0.001);
                break;
            case CV_CAP_PROP_POS_AVI_RATIO:
                pos = cvRound(value*(capture->film_range.end_index - 
                                     capture->film_range.start_index) +
                              capture->film_range.start_index);
                break;
            default:
                pos = cvRound(value);
            }
            if( pos < capture->film_range.start_index )
                pos = capture->film_range.start_index;
            if( pos > capture->film_range.end_index )
                pos = capture->film_range.end_index;
            capture->pos = pos;
        }
        break;
    default:
        return 0;
    }

    return 1;
}

static CvCaptureVTable captureAVI_VFW_vtable = 
{
    6, 
    (CvCaptureCloseFunc)          icvCloseAVI_VFW,
    (CvCaptureGrabFrameFunc)      icvGrabFrameAVI_VFW,
    (CvCaptureRetrieveFrameFunc)  icvRetrieveFrameAVI_VFW,
    (CvCaptureGetPropertyFunc)    icvGetPropertyAVI_VFW,
    (CvCaptureSetPropertyFunc)    icvSetPropertyAVI_VFW,
    (CvCaptureGetDescriptionFunc) 0
};


CvCapture* cvCaptureFromFile_VFW (const char* filename)
{
    CvCaptureAVI_VFW* capture = 0;

    if( filename )
    {
        capture = (CvCaptureAVI_VFW*)cvAlloc( sizeof(*capture));
        memset( capture, 0, sizeof(*capture));

        capture->vtable = &captureAVI_VFW_vtable;

        if( !icvOpenAVI_VFW( capture, filename ))
            cvReleaseCapture( (CvCapture**)&capture );
    }

    return (CvCapture*)capture;
}

/********************* Capturing video from camera via VFW *********************/

typedef struct CvCaptureCAM_VFW
{
    CvCaptureVTable* vtable;
    CAPDRIVERCAPS caps;
    HWND   capWnd;
    VIDEOHDR* hdr;
    DWORD  fourcc;
    HIC    hic;
    IplImage* rgb_frame;
    IplImage frame;
}
CvCaptureCAM_VFW;


static LRESULT PASCAL FrameCallbackProc( HWND hWnd, VIDEOHDR* hdr ) 
{ 
    CvCaptureCAM_VFW* capture = 0;

    if (!hWnd) return FALSE;

    capture = (CvCaptureCAM_VFW*)capGetUserData(hWnd);
    capture->hdr = hdr;

    return (LRESULT)TRUE; 
} 


// Initialize camera input
static int icvOpenCAM_VFW( CvCaptureCAM_VFW* capture, int wIndex )
{
    char szDeviceName[80];
    char szDeviceVersion[80];
    HWND hWndC = 0;
    
    if( (unsigned)wIndex >= 10 )
        wIndex = 0;

    for( ; wIndex < 10; wIndex++ ) 
    {
        if( capGetDriverDescription( wIndex, szDeviceName, 
            sizeof (szDeviceName), szDeviceVersion, 
            sizeof (szDeviceVersion))) 
        {
            hWndC = capCreateCaptureWindow ( "My Own Capture Window", 
                WS_POPUP | WS_CHILD, 0, 0, 320, 240, 0, 0);
            if( capDriverConnect (hWndC, wIndex))
                break;
            DestroyWindow( hWndC );
            hWndC = 0;
        }
    }
    
    if( hWndC )
    {
        capture->capWnd = hWndC;
        capture->hdr = 0;
        capture->hic = 0;
        capture->fourcc = (DWORD)-1;
        capture->rgb_frame = 0;
        
        memset( &capture->caps, 0, sizeof(capture->caps));
        capDriverGetCaps( hWndC, &capture->caps, sizeof(&capture->caps));
        ::MoveWindow( hWndC, 0, 0, 320, 240, TRUE );
        capSetUserData( hWndC, (size_t)capture );
        capSetCallbackOnFrame( hWndC, FrameCallbackProc ); 
        CAPTUREPARMS p;
        capCaptureGetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
        p.dwRequestMicroSecPerFrame = 66667/2;
        capCaptureSetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
        //capPreview( hWndC, 1 );
        capPreviewScale(hWndC,FALSE);
        capPreviewRate(hWndC,1);
    }
    return capture->capWnd != 0;
}

static  void icvCloseCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture && capture->capWnd )
    {
        capSetCallbackOnFrame( capture->capWnd, NULL ); 
        capDriverDisconnect( capture->capWnd );
        DestroyWindow( capture->capWnd );
        cvReleaseImage( &capture->rgb_frame );
        if( capture->hic )
        {
            ICDecompressEnd( capture->hic );
            ICClose( capture->hic );
        }

        capture->capWnd = 0;
        capture->hic = 0;
        capture->hdr = 0;
        capture->fourcc = 0;
        capture->rgb_frame = 0;
        memset( &capture->frame, 0, sizeof(capture->frame));
    }
}


static int icvGrabFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture->capWnd )
    {
        SendMessage( capture->capWnd, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0 );
        return 1;
    }
    return 0;
}


static IplImage* icvRetrieveFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture->capWnd )
    {
        BITMAPINFO vfmt;
        memset( &vfmt, 0, sizeof(vfmt));
        int sz = capGetVideoFormat( capture->capWnd, &vfmt, sizeof(vfmt));

        if( capture->hdr && capture->hdr->lpData && sz != 0 )
        {
            long code = ICERR_OK;
            char* frame_data = (char*)capture->hdr->lpData;

            if( vfmt.bmiHeader.biCompression != BI_RGB ||
                vfmt.bmiHeader.biBitCount != 24 )
            {
                BITMAPINFOHEADER& vfmt0 = vfmt.bmiHeader;
                BITMAPINFOHEADER vfmt1 = icvBitmapHeader( vfmt0.biWidth, vfmt0.biHeight, 24 );
                code = ICERR_ERROR;

                if( capture->hic == 0 ||
                    capture->fourcc != vfmt0.biCompression ||
                    capture->rgb_frame == 0 ||
                    vfmt0.biWidth != capture->rgb_frame->width ||
                    vfmt0.biHeight != capture->rgb_frame->height )
                {
                    if( capture->hic )
                    {
                        ICDecompressEnd( capture->hic );
                        ICClose( capture->hic );
                    }
                    capture->hic = ICOpen( MAKEFOURCC('V','I','D','C'),
                                            vfmt0.biCompression, ICMODE_DECOMPRESS );
                    if( capture->hic &&
                        ICDecompressBegin( capture->hic, &vfmt0, &vfmt1 ) == ICERR_OK )
                    {
                        cvReleaseImage( &capture->rgb_frame );
                        capture->rgb_frame = cvCreateImage(
                            cvSize( vfmt0.biWidth, vfmt0.biHeight ), IPL_DEPTH_8U, 3 );
                        capture->rgb_frame->origin = IPL_ORIGIN_BL;

                        code = ICDecompress( capture->hic, 0,
                                             &vfmt0, capture->hdr->lpData,
                                             &vfmt1, capture->rgb_frame->imageData );
                        frame_data = capture->rgb_frame->imageData;
                    }
                }
            }
        
            if( code == ICERR_OK )
            {
                cvInitImageHeader( &capture->frame,
                                   cvSize(vfmt.bmiHeader.biWidth,
                                          vfmt.bmiHeader.biHeight),
                                   IPL_DEPTH_8U, 3, IPL_ORIGIN_BL, 4 );
                capture->frame.imageData = capture->frame.imageDataOrigin = frame_data;
                return &capture->frame;
            }
        }
    }

    return 0;
}


static double icvGetPropertyCAM_VFW( CvCaptureCAM_VFW* capture, int property_id )
{
    switch( property_id )
    {
    case CV_CAP_PROP_FRAME_WIDTH:
        return capture->frame.width;
    case CV_CAP_PROP_FRAME_HEIGHT:
        return capture->frame.height;
    case CV_CAP_PROP_FOURCC:
        return capture->fourcc;
    }
    return 0;
}



static CvCaptureVTable captureCAM_VFW_vtable = 
{
    6,
    (CvCaptureCloseFunc)icvCloseCAM_VFW,
    (CvCaptureGrabFrameFunc)icvGrabFrameCAM_VFW,
    (CvCaptureRetrieveFrameFunc)icvRetrieveFrameCAM_VFW,
    (CvCaptureGetPropertyFunc)icvGetPropertyCAM_VFW,
    (CvCaptureSetPropertyFunc)0,
    (CvCaptureGetDescriptionFunc)0
};


CvCapture* cvCaptureFromCAM_VFW( int index )
{
	CvCaptureCAM_VFW * capture = (CvCaptureCAM_VFW*)cvAlloc( sizeof(*capture));
	memset( capture, 0, sizeof(*capture));
	capture->vtable = &captureCAM_VFW_vtable;
	
	if( icvOpenCAM_VFW( capture, index ))
		return (CvCapture*)capture;
	
	cvReleaseCapture( (CvCapture**)&capture );
	return 0;
}


/*************************** writing AVIs ******************************/

typedef struct CvAVI_VFW_Writer
{
    PAVIFILE      avifile;
    PAVISTREAM    compressed;
    PAVISTREAM    uncompressed;
    double        fps;
    CvSize        frameSize;
    IplImage    * tempFrame;
    long          pos;
    int           fourcc;
} CvAVI_VFW_Writer;


static void icvCloseAVIWriter( CvAVI_VFW_Writer* writer )
{
    if( writer )
    {
        if( writer->uncompressed )
            AVIStreamRelease( writer->uncompressed );
        if( writer->compressed )
            AVIStreamRelease( writer->compressed );
        if( writer->avifile )
            AVIFileRelease( writer->avifile );
        cvReleaseImage( &writer->tempFrame );
        memset( writer, 0, sizeof(*writer));
    }
}


// philipg.  Made this code capable of writing 8bpp gray scale bitmaps
typedef struct tagBITMAPINFO_8Bit 
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOHEADER_8BIT;

static int icvInitAVIWriter( CvAVI_VFW_Writer* writer, int fourcc,
                             double fps, CvSize frameSize, int is_color )
{
    if( writer && writer->avifile )
    {
        AVICOMPRESSOPTIONS copts, *pcopts = &copts;
        AVISTREAMINFO aviinfo;
		
        assert( frameSize.width > 0 && frameSize.height > 0 );
		
        BITMAPINFOHEADER_8BIT bmih;
        int i;
		
        bmih.bmiHeader = icvBitmapHeader( frameSize.width, frameSize.height, is_color ? 24 : 8 );
        for( i = 0; i < 256; i++ )
        {
            bmih.bmiColors[i].rgbBlue = (BYTE)i;
            bmih.bmiColors[i].rgbGreen = (BYTE)i;
            bmih.bmiColors[i].rgbRed = (BYTE)i;
            bmih.bmiColors[i].rgbReserved = 0;
        }
		
        memset( &aviinfo, 0, sizeof(aviinfo));
        aviinfo.fccType = streamtypeVIDEO;
        aviinfo.fccHandler = 0;
        // use highest possible accuracy for dwRate/dwScale
        aviinfo.dwScale = (DWORD)((double)0x7FFFFFFF / fps);
        aviinfo.dwRate = cvRound(fps * aviinfo.dwScale);
        aviinfo.rcFrame.top = aviinfo.rcFrame.left = 0;
        aviinfo.rcFrame.right = frameSize.width;
        aviinfo.rcFrame.bottom = frameSize.height;
		
        if( AVIFileCreateStream( writer->avifile,
								 &writer->uncompressed, &aviinfo ) == AVIERR_OK )
        {
            copts.fccType = streamtypeVIDEO;
            copts.fccHandler = fourcc != -1 ? fourcc : 0; 
            copts.dwKeyFrameEvery = 1; 
            copts.dwQuality = (DWORD)-1; 
            copts.dwBytesPerSecond = 0; 
            copts.dwFlags = AVICOMPRESSF_VALID; 
            copts.lpFormat = &bmih; 
            copts.cbFormat = (is_color ? sizeof(BITMAPINFOHEADER) : sizeof(bmih));
            copts.lpParms = 0; 
            copts.cbParms = 0; 
            copts.dwInterleaveEvery = 0;
			
            if( fourcc != -1 ||
                AVISaveOptions( 0, 0, 1, &writer->uncompressed, &pcopts ) == TRUE )
            {
                if( AVIMakeCompressedStream( &writer->compressed,
											 writer->uncompressed, pcopts, 0 ) == AVIERR_OK &&
                    // check that the resolution was not changed
                    bmih.bmiHeader.biWidth == frameSize.width &&
                    bmih.bmiHeader.biHeight == frameSize.height &&
                    AVIStreamSetFormat( writer->compressed, 0, &bmih, sizeof(bmih)) == AVIERR_OK )
				{
					writer->fps = fps;
					writer->fourcc = (int)copts.fccHandler;
					writer->frameSize = frameSize;
					writer->tempFrame = cvCreateImage( frameSize, 8, (is_color ? 3 : 1) );
					return 1;
				}
            }
        }
    }
    icvCloseAVIWriter( writer );
    return 0;
}

CV_IMPL CvVideoWriter* cvCreateVideoWriter( const char* filename, int fourcc,
                                            double fps, CvSize frameSize, int is_color )
{
    CvAVI_VFW_Writer* writer = (CvAVI_VFW_Writer*)cvAlloc( sizeof(CvAVI_VFW_Writer));
    memset( writer, 0, sizeof(*writer));
	
    icvInitCapture_VFW();
    
    if( AVIFileOpen( &writer->avifile, filename, OF_CREATE | OF_WRITE, 0 ) == AVIERR_OK )
    {
        if( frameSize.width > 0 && frameSize.height > 0 )
        {
            if( !icvInitAVIWriter( writer, fourcc, fps, frameSize, is_color ))
                cvReleaseVideoWriter( (CvVideoWriter**)&writer );
        }
        else if( fourcc == -1 )
        {
            icvCloseAVIWriter( writer );
        }
        else
        {
            /* postpone initialization until the first frame is written */
            writer->fourcc = fourcc;
            writer->fps = fps;
            writer->frameSize = frameSize;
        }
    }
    
    return (CvVideoWriter*)writer;
}

CV_IMPL int cvWriteFrame( CvVideoWriter* _writer, const IplImage* image )
{
    CvAVI_VFW_Writer* writer = (CvAVI_VFW_Writer*)_writer;
	
    if( writer && (writer->compressed ||
				   icvInitAVIWriter( writer, writer->fourcc, writer->fps,
									 writer->frameSize, image->nChannels > 1 )))
    {
        if (image->nChannels != writer->tempFrame->nChannels)
        {
            cvConvertImage( image, writer->tempFrame,
							image->origin == 0 ? CV_CVTIMG_FLIP : 0 );
            image = (const IplImage*)writer->tempFrame;
        }
        // If only flipping is needed, do not call cvConvertImage because when source and destination are single channel, cvConvertImage fails.
        else if (image->origin == 0)
        {
            cvFlip( image, writer->tempFrame, 0 );
            image = (const IplImage*)writer->tempFrame;
        }
        if( AVIStreamWrite( writer->compressed, writer->pos++, 1, image->imageData,
                            image->imageSize, AVIIF_KEYFRAME, 0, 0 ) == AVIERR_OK )
        {
            return 1;
        }
    }
    return 0;
}

CV_IMPL void cvReleaseVideoWriter( CvVideoWriter** writer )
{
    if( writer && *writer )
    {
        icvCloseAVIWriter( (CvAVI_VFW_Writer*)*writer );
        cvFree( writer );
    }
}


