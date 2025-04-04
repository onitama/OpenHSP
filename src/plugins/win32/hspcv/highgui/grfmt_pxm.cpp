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
#include "grfmt_pxm.h"

// P?M filter factory

GrFmtPxM::GrFmtPxM()
{
    m_sign_len = 3;
    m_signature = "";
    m_description = "Portable image format (*.pbm;*.pgm;*.ppm)";
}


GrFmtPxM::~GrFmtPxM()
{
}


bool GrFmtPxM::CheckSignature( const char* signature )
{
    return signature[0] == 'P' &&
           '1' <= signature[1] && signature[1] <= '6' &&
           isspace(signature[2]);
}


GrFmtReader* GrFmtPxM::NewReader( const char* filename )
{
    return new GrFmtPxMReader( filename );
}


GrFmtWriter* GrFmtPxM::NewWriter( const char* filename )
{
    return new GrFmtPxMWriter( filename );
}


///////////////////////// P?M reader //////////////////////////////

static int ReadNumber( RLByteStream& strm, int maxdigits )
{
    int code;
    int val = 0;
    int digits = 0;

    code = strm.GetByte();

    if( !isdigit(code))
    {
        do
        {
            if( code == '#' )
            {
                do
                {
                    code = strm.GetByte();
                }
                while( code != '\n' && code != '\r' );
            }
            
            code = strm.GetByte();

            while( isspace(code))
                code = strm.GetByte();
        }
        while( !isdigit( code ));
    }

    do
    {
        val = val*10 + code - '0';
        if( ++digits >= maxdigits ) break;
        code = strm.GetByte();
    }
    while( isdigit(code));

    return val;
}


GrFmtPxMReader::GrFmtPxMReader( const char* filename ) : GrFmtReader( filename )
{
    m_offset = -1;
}


GrFmtPxMReader::~GrFmtPxMReader()
{
}


void  GrFmtPxMReader::Close()
{
    m_strm.Close();
}


bool  GrFmtPxMReader::ReadHeader()
{
    bool result = false;
    
    assert( strlen(m_filename) != 0 );
    if( !m_strm.Open( m_filename )) return false;

    if( setjmp( m_strm.JmpBuf()) == 0 )
    {
        int code = m_strm.GetByte();
        if( code != 'P' )
            BAD_HEADER_ERR();

        code = m_strm.GetByte();
        switch( code )
        {
        case '1': case '4': m_bpp = 1; break;
        case '2': case '5': m_bpp = 8; break;
        case '3': case '6': m_bpp = 24; break;
        default: BAD_HEADER_ERR();
        }
        
        m_binary = code >= '4';
        m_iscolor = m_bpp > 8;

        m_width = ReadNumber( m_strm, INT_MAX );
        m_height = ReadNumber( m_strm, INT_MAX );
        
        m_maxval = m_bpp == 1 ? 1 : ReadNumber( m_strm, INT_MAX );

        if( m_maxval > 255 ) m_binary = false;

        if( m_width > 0 && m_height > 0 && m_maxval > 0 && m_maxval < (1 << 16)) 
        {
            m_offset = m_strm.GetPos();
            result = true;
        }
bad_header_exit:
        ;
    }

    if( !result )
    {
        m_offset = -1;
        m_width = m_height = -1;
        m_strm.Close();
    }
    return result;
}


bool  GrFmtPxMReader::ReadData( uchar* data, int step, int color )
{
    const  int buffer_size = 1 << 12;
    uchar  buffer[buffer_size];
    uchar  pal_buffer[buffer_size];
    uchar  bgr_buffer[buffer_size];
    PaletteEntry palette[256];
    bool   result = false;
    uchar* src = buffer;
    uchar* gray_palette = pal_buffer;
    uchar* bgr = bgr_buffer;
    int  src_pitch = (m_width*m_bpp + 7)/8;
    int  nch = m_iscolor ? 3 : 1;
    int  width3 = m_width*nch;
    int  i, x, y;

    if( m_offset < 0 || !m_strm.IsOpened())
        return false;
    
    if( src_pitch+32 > buffer_size )
        src = new uchar[width3 + 32];

    if( m_maxval + 1 > buffer_size )
        gray_palette = new uchar[m_maxval + 1];

    if( m_width*3 + 32 > buffer_size )
        bgr = new uchar[m_width*3 + 32];

    // create LUT for converting colors
    for( i = 0; i <= m_maxval; i++ )
    {
        gray_palette[i] = (uchar)((i*255/m_maxval)^(m_bpp == 1 ? 255 : 0));
    }

    FillGrayPalette( palette, m_bpp==1 ? 1 : 8 , m_bpp == 1 );
    
    if( setjmp( m_strm.JmpBuf()) == 0 )
    {
        m_strm.SetPos( m_offset );
        
        switch( m_bpp )
        {
        ////////////////////////// 1 BPP /////////////////////////
        case 1:
            if( !m_binary )
            {
                for( y = 0; y < m_height; y++, data += step )
                {
                    for( x = 0; x < m_width; x++ )
                        src[x] = ReadNumber( m_strm, 1 ) != 0;

                    if( color )
                        FillColorRow8( data, src, m_width, palette );
                    else
                        FillGrayRow8( data, src, m_width, gray_palette );
                }
            }
            else
            {
                for( y = 0; y < m_height; y++, data += step )
                {
                    m_strm.GetBytes( src, src_pitch );
                    
                    if( color )
                        FillColorRow1( data, src, m_width, palette );
                    else
                        FillGrayRow1( data, src, m_width, gray_palette );
                }
            }
            result = true;
            break;

        ////////////////////////// 8 BPP /////////////////////////
        case 8:
        case 24:
            for( y = 0; y < m_height; y++, data += step )
            {
                if( !m_binary )
                {
                    for( x = 0; x < width3; x++ )
                    {
                        int code = ReadNumber( m_strm, INT_MAX );
                        if( (unsigned)code > (unsigned)m_maxval ) code = m_maxval;
                        src[x] = gray_palette[code];
                    }
                }
                else
                {
                    m_strm.GetBytes( src, src_pitch );
                }

                if( m_bpp == 8 )
                {
                    if( color )
                        FillColorRow8( data, src, m_width, palette );
                    else
                        memcpy( data, src, m_width );
                }
                else
                {
                    if( color )
                        icvCvt_RGB2BGR_8u_C3R( src, 0, data, 0, cvSize(m_width,1) );
                    else
                        icvCvt_BGR2Gray_8u_C3C1R( src, 0, data, 0, cvSize(m_width,1), 2 );
                }
            }
            result = true;
            break;
        default:
            assert(0);
        }
    }

    if( src != buffer )
        delete[] src; 

    if( bgr != bgr_buffer )
        delete[] bgr;

    if( gray_palette != pal_buffer )
        delete[] gray_palette;

    return result;
}


//////////////////////////////////////////////////////////////////////////////////////////

GrFmtPxMWriter::GrFmtPxMWriter( const char* filename ) : GrFmtWriter( filename )
{
}


GrFmtPxMWriter::~GrFmtPxMWriter()
{
}


static char PxMLUT[256][5];
static bool isPxMLUTInitialized = false;

bool  GrFmtPxMWriter::WriteImage( const uchar* data, int step,
                                  int width, int height, int /*depth*/, int _channels )
{
    bool isBinary = false;
    bool result = false;

    int  channels = _channels > 1 ? 3 : 1;
    int  fileStep = width*channels;
    int  x, y;

    assert( data && width > 0 && height > 0 && step >= fileStep );
    
    if( m_strm.Open( m_filename ) )
    {
        int  lineLength = ((isBinary ? 1 : 4)*channels +
                           (channels > 1 ? 2 : 0))*width + 32;
        int  bufferSize = 128; // buffer that should fit a header
        char* buffer = 0;
                
        if( bufferSize < lineLength )
            bufferSize = lineLength;

        buffer = new char[bufferSize];
        if( !buffer )
        {
            m_strm.Close();
            return false;
        }

        if( !isPxMLUTInitialized )
        {
            for( int i = 0; i < 256; i++ )
                sprintf( PxMLUT[i], "%4d", i );

            isPxMLUTInitialized = 1;
        }

        // write header;
        sprintf( buffer, "P%c\n%d %d\n255\n",
                 '2' + (channels > 1 ? 1 : 0) + (isBinary ? 3 : 0),
                 width, height );

        m_strm.PutBytes( buffer, (int)strlen(buffer) );

        for( y = 0; y < height; y++, data += step )
        {
            if( isBinary )
            {
                if( _channels == 3 )
                    icvCvt_BGR2RGB_8u_C3R( (uchar*)data, 0,
                        (uchar*)buffer, 0, cvSize(width,1) );
                else if( _channels == 4 )
                    icvCvt_BGRA2BGR_8u_C4C3R( (uchar*)data, 0,
                        (uchar*)buffer, 0, cvSize(width,1), 1 );
                m_strm.PutBytes( channels > 1 ? buffer : (char*)data, fileStep );
            }
            else
            {
                char* ptr = buffer;
                
                if( channels > 1 )
                    for( x = 0; x < width*channels; x += channels )
                    {
                        strcpy( ptr, PxMLUT[data[x+2]] );
                        ptr += 4;
                        strcpy( ptr, PxMLUT[data[x+1]] );
                        ptr += 4;
                        strcpy( ptr, PxMLUT[data[x]] );
                        ptr += 4;
                        *ptr++ = ' ';
                        *ptr++ = ' ';
                    }
                else
                    for( x = 0; x < width; x++ )
                    {
                        strcpy( ptr, PxMLUT[data[x]] );
                        ptr += 4;
                    }

                *ptr++ = '\n';

                m_strm.PutBytes( buffer, (int)(ptr - buffer) );
            }
        }
        delete[] buffer;
        m_strm.Close();
        result = true;
    }
    
    return result;
}

