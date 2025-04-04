
//
//		High performance Graphic Image access Plugin for HSP
//				onion software/onitama 2000
//

#include <windows.h>
#include <stdio.h>

int WINAPI hgimg_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved);

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	return hgimg_DllMain ( hInstance, fdwReason, pvReserved);
}


