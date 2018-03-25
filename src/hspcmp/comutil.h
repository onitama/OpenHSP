
//
//	comutil.cpp header
//
#ifndef __comutil_h
#define __comutil_h

typedef struct _COM_GUID {
	int		Data1;      // 4バイト
	short   Data2;      // 2バイト
	short   Data3;      // 2バイト
	char	Data4[8];   // 1バイト×8
} COM_GUID;

int ConvertIID( COM_GUID *guid, char *name );

#endif
