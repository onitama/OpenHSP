
//
//		HSP plugin main
//				onion software/onitama 1999
//

#include <windows.h>
#include <stdio.h>

int WINAPI hspsock_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved);

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	return hspsock_DllMain ( hInstance, fdwReason, pvReserved);
}


