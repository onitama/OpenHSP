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


extern "C" {
#include <ffmpeg/avformat.h>
}

#ifdef NDEBUG
#define CV_WARN(message) 
#else
#define CV_WARN(message) fprintf(stderr, "warning: %s (%s:%d)\n", message, __FILE__, __LINE__)
#endif

typedef struct CvCaptureAVI_FFMPEG
{
    CvCaptureVTable   * vtable;

    AVFormatContext   * ic;
    int                 video_stream;
    AVStream          * video_st;
    AVFrame           * picture;
    int64_t             picture_pts;
    AVFrame             rgb_picture;

    IplImage            frame;
} CvCaptureAVI_FFMPEG;

static void icvCloseAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture )
{
    //cvFree( (void**)&(capture->entries) );
        
    if( capture->picture )
    av_free(capture->picture);

    if( capture->video_st )
    {
#if LIBAVFORMAT_BUILD > 4628
        avcodec_close( capture->video_st->codec );
#else
        avcodec_close( &capture->video_st->codec );
#endif
        capture->video_st = NULL;
    }

    if( capture->ic )
    {
        av_close_input_file(capture->ic);
        capture->ic = NULL;
    }

    if( capture->rgb_picture.data[0] )
        cvFree( &capture->rgb_picture.data[0] );

    memset( &capture->frame, 0, sizeof(capture->frame));
}


static int icvOpenAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture, const char* filename )
{
    int err, valid = 0, video_index = -1, i;
    AVFormatContext *ic;

    capture->ic = NULL;
    capture->video_stream = -1;
    capture->video_st = NULL;
    /* register all codecs, demux and protocols */
    av_register_all();

    err = av_open_input_file(&ic, filename, NULL, 0, NULL);
    if (err < 0) {
	    CV_WARN("Error opening file");
	    goto exit_func;
    }
    capture->ic = ic;
    err = av_find_stream_info(ic);
    if (err < 0) {
	    CV_WARN("Could not find codec parameters");
	    goto exit_func;
    }
    for(i = 0; i < ic->nb_streams; i++) {
#if LIBAVFORMAT_BUILD > 4628
        AVCodecContext *enc = ic->streams[i]->codec;
#else
        AVCodecContext *enc = &ic->streams[i]->codec;
#endif
        AVCodec *codec;
    if( CODEC_TYPE_VIDEO == enc->codec_type && video_index < 0) {
        video_index = i;
        codec = avcodec_find_decoder(enc->codec_id);
        if (!codec ||
        avcodec_open(enc, codec) < 0)
        goto exit_func;
        capture->video_stream = i;
        capture->video_st = ic->streams[i];
        capture->picture = avcodec_alloc_frame();

        capture->rgb_picture.data[0] = (uchar*)cvAlloc(
                                avpicture_get_size( PIX_FMT_BGR24,
                                enc->width, enc->height ));
        avpicture_fill( (AVPicture*)&capture->rgb_picture, capture->rgb_picture.data[0],
                PIX_FMT_BGR24, enc->width, enc->height );

        cvInitImageHeader( &capture->frame, cvSize( enc->width,
                                   enc->height ), 8, 3, 0, 4 );
        cvSetData( &capture->frame, capture->rgb_picture.data[0],
                           capture->rgb_picture.linesize[0] );
        break;
    }
    }


    if(video_index >= 0)
    valid = 1;

exit_func:

    if( !valid )
        icvCloseAVI_FFMPEG( capture );

    return valid;
}


static int icvGrabFrameAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture )
{
    int valid=0;
    static bool bFirstTime = true;
    static AVPacket pkt;
    int got_picture;

    // First time we're called, set packet.data to NULL to indicate it
    // doesn't have to be freed
    if (bFirstTime) {
        bFirstTime = false;
        pkt.data = NULL;
    }

    if( !capture || !capture->ic || !capture->video_st )
        return 0;

    // free last packet if exist
    if (pkt.data != NULL) {
        av_free_packet (&pkt);
    }

    // get the next frame
    while ((0 == valid) && (av_read_frame(capture->ic, &pkt) >= 0)) {
		if( pkt.stream_index != capture->video_stream ) continue;

#if LIBAVFORMAT_BUILD > 4628
        avcodec_decode_video(capture->video_st->codec, 
                             capture->picture, &got_picture, 
                             pkt.data, pkt.size);
#else
        avcodec_decode_video(&capture->video_st->codec, 
                             capture->picture, &got_picture, 
                             pkt.data, pkt.size);
#endif

        if (got_picture) {
            // we have a new picture, so memorize it
            capture->picture_pts = pkt.pts;
            valid = 1;
        }
    }
    
    // return if we have a new picture or not
    return valid;
}


static const IplImage* icvRetrieveFrameAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture )
{
    if( !capture || !capture->video_st || !capture->picture->data[0] )
    return 0;
#if LIBAVFORMAT_BUILD > 4628
    img_convert( (AVPicture*)&capture->rgb_picture, PIX_FMT_BGR24,
                 (AVPicture*)capture->picture,
                 capture->video_st->codec->pix_fmt,
                 capture->video_st->codec->width,
                 capture->video_st->codec->height );
#else
    img_convert( (AVPicture*)&capture->rgb_picture, PIX_FMT_BGR24,
                 (AVPicture*)capture->picture,
                 capture->video_st->codec.pix_fmt,
                 capture->video_st->codec.width,
                 capture->video_st->codec.height );
#endif
    return &capture->frame;
}


static int icvSetPropertyAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture,
                                     int property_id, double value );

static double icvGetPropertyAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture, int property_id )
{
    if( !capture || !capture->video_st || !capture->picture->data[0] )
    return 0;

    int64_t timestamp;
    timestamp = capture->picture_pts;

    switch( property_id )
    {
    case CV_CAP_PROP_POS_MSEC:
        if(capture->ic->start_time != static_cast<double>(AV_NOPTS_VALUE))
        return (double)(timestamp - capture->ic->start_time)*1000/(double)AV_TIME_BASE;
        break;
    case CV_CAP_PROP_POS_FRAMES:
    if(capture->video_st->cur_dts != static_cast<double>(AV_NOPTS_VALUE))
        return (double)capture->video_st->cur_dts-1;
    break;
    case CV_CAP_PROP_POS_AVI_RATIO:
    if(capture->ic->start_time != static_cast<double>(AV_NOPTS_VALUE) && capture->ic->duration != static_cast<double>(AV_NOPTS_VALUE))
        return (double)(timestamp-capture->ic->start_time)/(double)capture->ic->duration;
    break;
    case CV_CAP_PROP_FRAME_WIDTH:
        return capture->frame.width;
    break;
    case CV_CAP_PROP_FRAME_HEIGHT:
        return capture->frame.height;
    break;
    case CV_CAP_PROP_FPS:
#if LIBAVCODEC_BUILD > 4753
        return av_q2d (capture->video_st->r_frame_rate);
#else
        return (double)capture->video_st->codec.frame_rate
            / (double)capture->video_st->codec.frame_rate_base;
#endif
    break;
    case CV_CAP_PROP_FOURCC:
#if LIBAVFORMAT_BUILD > 4628
        return (double)capture->video_st->codec->codec_tag;
#else
        return (double)capture->video_st->codec.codec_tag;
#endif
    break;
    }
    return 0;
}


static int icvSetPropertyAVI_FFMPEG( CvCaptureAVI_FFMPEG* capture,
                                     int property_id, double value )
{
    if( !capture || !capture->video_st || !capture->picture->data[0] )
    return 0;
    switch( property_id )
    {
#if 0    
    case CV_CAP_PROP_POS_MSEC:
    case CV_CAP_PROP_POS_FRAMES:
    case CV_CAP_PROP_POS_AVI_RATIO:
        {
        int64_t timestamp = AV_NOPTS_VALUE;
        switch( property_id )
            {
        case CV_CAP_PROP_POS_FRAMES:
        if(capture->ic->start_time != AV_NOPTS_VALUE) {
            value *= (double)capture->video_st->codec.frame_rate_base
            / (double)capture->video_st->codec.frame_rate;
            timestamp = capture->ic->start_time+(int64_t)(value*AV_TIME_BASE);
        }
        break;
        case CV_CAP_PROP_POS_MSEC:
        if(capture->ic->start_time != AV_NOPTS_VALUE)
            timestamp = capture->ic->start_time+(int64_t)(value*AV_TIME_BASE/1000);
        break;
        case CV_CAP_PROP_POS_AVI_RATIO:
        if(capture->ic->start_time != AV_NOPTS_VALUE && capture->ic->duration != AV_NOPTS_VALUE)
            timestamp = capture->ic->start_time+(int64_t)(value*capture->ic->duration);
        break;
        }
        if(timestamp != AV_NOPTS_VALUE) {
        //printf("timestamp=%g\n",(double)timestamp);
        int ret = av_seek_frame(capture->ic, -1, timestamp, 0);
        if (ret < 0) {
            fprintf(stderr, "HIGHGUI ERROR: AVI: could not seek to position %0.3f\n", 
                (double)timestamp / AV_TIME_BASE);
            return 0;
        }
        }
    }
        break;
#endif  
    default:
        return 0;
    }

    return 1;
}

static CvCaptureVTable captureAVI_FFMPEG_vtable = 
{
    6,
    (CvCaptureCloseFunc)icvCloseAVI_FFMPEG,
    (CvCaptureGrabFrameFunc)icvGrabFrameAVI_FFMPEG,
    (CvCaptureRetrieveFrameFunc)icvRetrieveFrameAVI_FFMPEG,
    (CvCaptureGetPropertyFunc)icvGetPropertyAVI_FFMPEG,
    (CvCaptureSetPropertyFunc)icvSetPropertyAVI_FFMPEG,
    (CvCaptureGetDescriptionFunc)0
};


CvCapture* cvCaptureFromFile_FFMPEG( const char* filename )
{
    CvCaptureAVI_FFMPEG* capture = 0;

    if( filename )
    {
        capture = (CvCaptureAVI_FFMPEG*)cvAlloc( sizeof(*capture));
        memset( capture, 0, sizeof(*capture));

        capture->vtable = &captureAVI_FFMPEG_vtable;

        if( !icvOpenAVI_FFMPEG( capture, filename ))
            cvReleaseCapture( (CvCapture**)&capture );
    }

    return (CvCapture*)capture;
}

///////////////// FFMPEG CvVideoWriter implementation //////////////////////////
typedef struct CvAVI_FFMPEG_Writer
{
	AVOutputFormat *fmt;
	AVFormatContext *oc;
    uint8_t         * outbuf;
    uint32_t          outbuf_size;
    FILE            * outfile;
    AVFrame         * picture;
    AVFrame         * rgb_picture;
    uint8_t         * picbuf;
	AVStream        * video_st;
} CvAVI_FFMPEG_Writer;

/**
 * the following function is a modified version of code
 * found in ffmpeg-0.4.9-pre1/output_example.c
 */
static AVFrame * icv_alloc_picture_FFMPEG(int pix_fmt, int width, int height, bool alloc)
{
	AVFrame * picture;
	uint8_t * picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size(pix_fmt, width, height);
	if(alloc){
		picture_buf = (uint8_t *) cvAlloc(size);
		if (!picture_buf) 
		{
			av_free(picture);
			return NULL;
		}
		avpicture_fill((AVPicture *)picture, picture_buf, 
				pix_fmt, width, height);
	}
	else {
	}
	return picture;
}

/* add a video output stream */
static AVStream *icv_add_video_stream_FFMPEG(AVFormatContext *oc, int codec_tag, int w, int h, int bitrate, double fps, int pixel_format)
{
	AVCodecContext *c;
	AVStream *st;
    int codec_id;
	int frame_rate, frame_rate_base;
	AVCodec *codec;
	

	st = av_new_stream(oc, 0);
	if (!st) {
		CV_WARN("Could not allocate stream");
		return NULL;
	}

#if LIBAVFORMAT_BUILD > 4628
	c = st->codec;
#else
	c = &(st->codec);
#endif
#if LIBAVFORMAT_BUILD > 4621 
	codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, CODEC_TYPE_VIDEO);
#else
	codec_id = oc->oformat->video_codec;
#endif

    if(codec_tag) c->codec_tag=codec_tag;

	c->codec_id = (CodecID) codec_id;
	codec = avcodec_find_encoder(c->codec_id);

	c->codec_type = CODEC_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = bitrate;

	/* resolution must be a multiple of two */
	c->width = w;
	c->height = h;

	/* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
	frame_rate=cvRound(fps);
	frame_rate_base=1;
	while (fabs((double)frame_rate/frame_rate_base) - fps > 0.001){
		frame_rate_base*=10;
		frame_rate=cvRound(fps*frame_rate_base);
	}
#if LIBAVFORMAT_BUILD > 4752
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
	/* adjust time base for supported framerates */
	if(codec && codec->supported_framerates){
		const AVRational *p= codec->supported_framerates;
		AVRational req= (AVRational){frame_rate, frame_rate_base};
		const AVRational *best=NULL;
		AVRational best_error= (AVRational){INT_MAX, 1};
		for(; p->den!=0; p++){
			AVRational error= av_sub_q(req, *p);
			if(error.num <0) error.num *= -1;
			if(av_cmp_q(error, best_error) < 0){
				best_error= error;
				best= p;
			}
		}
		c->time_base.den= best->num;
		c->time_base.num= best->den;
	}
#else
	c->frame_rate = frame_rate;
	c->frame_rate_base = frame_rate_base;
#endif

	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = (PixelFormat) pixel_format;
	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO){
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        c->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name,  "3gp"))
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

/// Create a video writer object that uses FFMPEG
CV_IMPL CvVideoWriter* cvCreateVideoWriter( const char * filename, int fourcc,
		double fps, CvSize frameSize, int /*is_color*/ )
{
	CV_FUNCNAME("cvCreateVideoWriter");

	CvAVI_FFMPEG_Writer * writer = NULL;
	
	__BEGIN__;

	// check arguments
	assert (filename);
	assert (fps > 0);
	assert (frameSize.width > 0  &&  frameSize.height > 0);

	// allocate memory for structure...
	writer = (CvAVI_FFMPEG_Writer *) cvAlloc( sizeof(CvAVI_FFMPEG_Writer));
	memset (writer, 0, sizeof (*writer));

	// tell FFMPEG to register codecs
	av_register_all ();

	/* auto detect the output format from the name. default is mpeg. */
	writer->fmt = guess_format(NULL, filename, NULL);
	if (!writer->fmt) {
		CV_ERROR( CV_StsUnsupportedFormat, "Could not deduce output format from file extension");
		//writer->fmt = guess_format("mpeg", NULL, NULL);
	}

	// alloc memory for context 
	writer->oc = av_alloc_format_context();
	assert (writer->oc);

	/* set file name */
	writer->oc->oformat = writer->fmt;
	snprintf(writer->oc->filename, sizeof(writer->oc->filename), "%s", filename);

	// TODO -- safe to ignore output audio stream?
	writer->video_st = icv_add_video_stream_FFMPEG(writer->oc, fourcc, frameSize.width, frameSize.height, 800000, fps, PIX_FMT_YUV420P);


	/* set the output parameters (must be done even if no
       parameters). */
    if (av_set_parameters(writer->oc, NULL) < 0) {
		CV_ERROR(CV_StsBadArg, "Invalid output format parameters");
    }

    dump_format(writer->oc, 0, filename, 1);

    /* now that all the parameters are set, we can open the audio and
       video codecs and allocate the necessary encode buffers */
    if (!writer->video_st){
		CV_ERROR(CV_StsBadArg, "Couldn't open video stream");
	}

    AVCodec *codec;
    AVCodecContext *c;

#if LIBAVFORMAT_BUILD > 4628
    c = (writer->video_st->codec);
#else
    c = &(writer->video_st->codec);
#endif

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
		CV_ERROR(CV_StsBadArg, "codec not found");
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
		char errtext[256];
		sprintf(errtext, "Could not open codec '%s'", codec->name);
		CV_ERROR(CV_StsBadArg, errtext);
    }

	//	printf("Using codec %s\n", codec->name);
    writer->outbuf = NULL;

    if (!(writer->oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        writer->outbuf_size = 200000;
        writer->outbuf = (uint8_t *) malloc(writer->outbuf_size);
    }

	bool need_color_convert;
        need_color_convert = c->pix_fmt != PIX_FMT_BGR24;

    /* allocate the encoded raw picture */
    writer->picture = icv_alloc_picture_FFMPEG(c->pix_fmt, c->width, c->height, need_color_convert);
    if (!writer->picture) {
		CV_ERROR(CV_StsNoMem, "Could not allocate picture");
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    writer->rgb_picture = NULL;
    if ( need_color_convert ) {
        writer->rgb_picture = icv_alloc_picture_FFMPEG(PIX_FMT_BGR24, c->width, c->height, false);
        if (!writer->rgb_picture) {
			CV_ERROR(CV_StsNoMem, "Could not allocate picture");
        }
    }

	/* open the output file, if needed */
    if (!(writer->fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&writer->oc->pb, filename, URL_WRONLY) < 0) {
			CV_ERROR(CV_StsBadArg, "Couldn't open output file for writing");
        }
    }

    /* write the stream header, if any */
    av_write_header( writer->oc );
	

	__END__;

	// return what we got
	return (CvVideoWriter *) writer;
}

int icv_av_write_frame_FFMPEG( AVFormatContext * oc, AVStream * video_st, uint8_t * outbuf, uint32_t outbuf_size, AVFrame * picture ){
	CV_FUNCNAME("icv_av_write_frame_FFMPEG");

#if LIBAVFORMAT_BUILD > 4628
	AVCodecContext * c = video_st->codec;
#else
	AVCodecContext * c = &(video_st->codec);
#endif
	int out_size;
	int ret;

	__BEGIN__;

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= video_st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);

        ret = av_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);
#if LIBAVFORMAT_BUILD > 4752 
            pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);
#else 
			pkt.pts = c->coded_frame->pts;
#endif
            if(c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= video_st->index;
            pkt.data= outbuf;
            pkt.size= out_size;

            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
		CV_ERROR(CV_StsError, "Error while writing video frame");
	}

	__END__;
	return CV_StsOk;
}

/// write a frame with FFMPEG
CV_IMPL int cvWriteFrame( CvVideoWriter * writer, const IplImage * image )
{
	int ret = 0;

	CV_FUNCNAME("cvWriteFrame");

	__BEGIN__;

	// typecast from opaque data type to implemented struct
	CvAVI_FFMPEG_Writer * mywriter = (CvAVI_FFMPEG_Writer*) writer;
#if LIBAVFORMAT_BUILD > 4628
    AVCodecContext *c = mywriter->video_st->codec;
#else
	AVCodecContext *c = &(mywriter->video_st->codec);
#endif
	// check parameters
	assert ( image );
	assert ( image->nChannels == 3 );
	assert ( image->depth == IPL_DEPTH_8U );


	// check if buffer sizes match, i.e. image has expected format (size, channels, bitdepth, alignment)
	assert (image->imageSize == avpicture_get_size (PIX_FMT_BGR24, image->width, image->height));

	if (c->pix_fmt != PIX_FMT_BGR24 ) {
		assert( mywriter->rgb_picture );
		// let rgb_picture point to the raw data buffer of 'image'
		avpicture_fill((AVPicture *)mywriter->rgb_picture, (uint8_t *) image->imageData, 
				PIX_FMT_BGR24, image->width, image->height);

		// convert to the color format needed by the codec
		if( img_convert((AVPicture *)mywriter->picture, c->pix_fmt,
					(AVPicture *)mywriter->rgb_picture, PIX_FMT_BGR24, 
					image->width, image->height) < 0){
			CV_ERROR(CV_StsUnsupportedFormat, "FFMPEG::img_convert pixel format conversion from BGR24 not handled");
		}
	}
	else{
		avpicture_fill((AVPicture *)mywriter->picture, (uint8_t *) image->imageData,
				PIX_FMT_BGR24, image->width, image->height);
	}

	ret = icv_av_write_frame_FFMPEG( mywriter->oc, mywriter->video_st, mywriter->outbuf, mywriter->outbuf_size, mywriter->picture);

	__END__;
	return ret;
}

/// close video output stream and free associated memory
CV_IMPL void cvReleaseVideoWriter( CvVideoWriter ** writer )
{
	int i;

	// nothing to do if already released
	if ( !(*writer) )
		return;
	
	// release data structures in reverse order
	CvAVI_FFMPEG_Writer * mywriter = (CvAVI_FFMPEG_Writer*)(*writer);

	/* no more frame to compress. The codec has a latency of a few
	   frames if using B frames, so we get the last frames by
	   passing the same picture again */
	// TODO -- do we need to account for latency here? 

	/* write the trailer, if any */
	av_write_trailer(mywriter->oc);

	// free pictures
#if LIBAVFORMAT_BUILD > 4628
	if( mywriter->video_st->codec->pix_fmt != PIX_FMT_BGR24){
#else
	if( mywriter->video_st->codec.pix_fmt != PIX_FMT_BGR24){
#endif
		cvFree(&(mywriter->picture->data[0]));
	}
	av_free(mywriter->picture);

    if (mywriter->rgb_picture) {
        av_free(mywriter->rgb_picture);
    }

	/* close codec */
#if LIBAVFORMAT_BUILD > 4628
	avcodec_close(mywriter->video_st->codec);
#else
	avcodec_close(&(mywriter->video_st->codec));
#endif
    
	av_free(mywriter->outbuf);

	/* free the streams */
	for(i = 0; i < mywriter->oc->nb_streams; i++) {
		av_freep(&mywriter->oc->streams[i]->codec);
		av_freep(&mywriter->oc->streams[i]);
	}

	if (!(mywriter->fmt->flags & AVFMT_NOFILE)) {
		/* close the output file */
		url_fclose(&mywriter->oc->pb);
	}

	/* free the stream */
	av_free(mywriter->oc);

	/* free cvVideoWriter */
	cvFree ( writer );

	// mark as released
	(*writer) = 0;
}
