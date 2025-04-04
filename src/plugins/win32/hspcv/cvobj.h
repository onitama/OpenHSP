
//
//	cvobj structures
//
#ifndef __cvobj_h
#define __cvobj_h

#include <cv.h>
#include <highgui.h>

#define CVOBJ_MAX 64

#define CVOBJ_FLAG_NONE 0
#define CVOBJ_FLAG_USED 1
#define CVOBJ_FLAG_VIDEOCAP 2

#define CVCOPY_SET 0
#define CVCOPY_ADD 1
#define CVCOPY_SUB 2
#define CVCOPY_MUL 3
#define CVCOPY_DIF 4
#define CVCOPY_AND 5

enum {
CVOBJ_INFO_SIZEX = 0,
CVOBJ_INFO_SIZEY,
CVOBJ_INFO_CHANNEL,
CVOBJ_INFO_BIT,
};


// CVOBJèÓïÒ
typedef struct
{
short flag;			// enable flag
short mode;			// mode
IplImage *img;		// OpenCV Image
} CVOBJ;

#endif
