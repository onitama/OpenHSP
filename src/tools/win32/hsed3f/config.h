/*------------------------------------------------------------------------------
	Header of editor configration routines
------------------------------------------------------------------------------*/
#ifndef __config_h
#define __config_h

#include <windows.h>
#include "classify.h"

void LoadConfig();
void SaveConfig();

void DefaultColor(MYCOLOR *);
void DefaultFont(LOGFONT *, LOGFONT *);

#endif // #ifndef __tabmanager_h