
//
//		HSP plugin main
//				onion software/onitama 1999
//

#include <windows.h>
#include <stdio.h>

int WINAPI hspext_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved);

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	return hspext_DllMain ( hInstance, fdwReason, pvReserved);
}


