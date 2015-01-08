////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002 mamaich
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////
//
//  Module Name:
// 
//     WndProcHook.cpp
// 
//  Abstract:
// 
//     Client main window procedure hook.
//     I don't use SetWindowsHook functions, because they cause
//     problems on W2K when Ansi function is used to hook Unicode
//     window. Better hook RegisterClassA/W functions and replace
//	   window proc by our own.
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/17/2002]	- source code cleanup
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Injection.h"
#include "MainWindow.h"
#include "GuiThread.h"
#include "api.h"
#include "common.h"
#include "ImportHook.h"
#include "WndProcHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static HWND ClientHWND=0;

WNDPROC OldWndProc=0;
LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wparam,
    LPARAM lparam)
{
	ClientHWND=hwnd;
	switch(msg)
	{
	case WM_SETTEXT:
		if(GetPlayerSerial()!=0)			// Serial !=0 after logging in
		{
			char *Str=(char*)lparam;
			if(Str[1]==0)	// check - is this window Unicode?
			{
				if(wcsncmp((wchar_t*)Str,L"UO - ",4)!=0)	// disable client to update window caption after logging in
					return TRUE;				// Injection always prepends "UO - " to the caption
			} else	// ansi
				if(strncmp(Str,"UO - ",4)!=0)	// disable client to update window caption after logging in
					return TRUE;				// Injection always prepends "UO - " to the caption
		}
		break;
	case WM_CLOSE:
		if(MainWindow)
		{
			HANDLE hGui = GUIThread->m_hThread;
			MainWindow->PostMessage(WM_CLOSE);
			if(GUIThread)
				GUIThread->PostThreadMessage(WM_QUIT,0,0);
			WaitForSingleObject(hGui,60000);
		}
		ClientHWND = 0;
		break;
	};
	return OldWndProc(hwnd,msg,wparam,lparam);
}

ATOM WINAPI HookRegisterClassA(
    WNDCLASSA *lpWndClass 	// address of structure with class data
   )
{
	OldWndProc = lpWndClass->lpfnWndProc;
	lpWndClass->lpfnWndProc = HookWndProc;
	return RegisterClassA(lpWndClass);
}

ATOM WINAPI HookRegisterClassW(
    WNDCLASSW *lpWndClass 	// address of structure with class data
   )
{
	OldWndProc = lpWndClass->lpfnWndProc;
	lpWndClass->lpfnWndProc = HookWndProc;
	return RegisterClassW(lpWndClass);
}

bool HookClientWindowProc()
{
	bool ret = NULL!=HookImportedFunction("user32.dll","RegisterClassA",0,
		HookRegisterClassA);
	ret |= (NULL!=HookImportedFunction("user32.dll","RegisterClassW",0,
		HookRegisterClassW));
	return ret;
}

HWND INJECTION_API GetClientWindow()
{
	return ClientHWND;
}
