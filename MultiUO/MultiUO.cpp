// MultiUO.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "api.h"


BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

void PLUGIN_API GetPluginName(char OUT *Buffer, int IN BuffSize)
{
	strncpy(Buffer,"Run multiple clients simultaneously",BuffSize);
	Buffer[BuffSize-1]=0;
}

DWORD WINAPI MyGetLastError(VOID)
{
	DWORD err=GetLastError();
	if(err==ERROR_ALREADY_EXISTS)
		return 0;

	return err;
}

HWND WINAPI MyFindWindow(
  LPCTSTR lpClassName,  // class name
  LPCTSTR lpWindowName  // window name
)
{
	if(strcmp(lpClassName,"Ultima Online")==0 ||
		strcmp(lpClassName,"Ultima Online Third Dawn")==0)
		return 0;

	return FindWindow(lpClassName,lpWindowName);;
}

BOOL PLUGIN_API InitPlugin()
{
	return NULL!=HookImportedFunction("kernel32.dll","GetLastError",0, MyGetLastError) && 
		NULL!=HookImportedFunction("User32.dll","FindWindowA",0, MyFindWindow);
}

void PLUGIN_API CleanupPlugin()
{
}
