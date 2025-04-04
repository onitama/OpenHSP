//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   dllmain.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE /*hModule*/,
                      DWORD  ul_reason_for_call,
                      LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
    default:
		break;
	}

	return TRUE;
}
