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
//     Injection.cpp
// 
//  Abstract:
// 
//     Injection.dll initialization routines.
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
#include "GuiThread.h"
#include "HighLevel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CInjectionApp

BEGIN_MESSAGE_MAP(CInjectionApp, CWinApp)
END_MESSAGE_MAP()


// CInjectionApp construction

CInjectionApp::CInjectionApp()
{
}


// The one and only CInjectionApp object

CInjectionApp theApp;

// CInjectionApp initialization

BOOL CInjectionApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

/*
 *	This function creates GUI thread and forces it to load plugins
 */ 
extern "C" void __declspec(dllexport) __cdecl Init()
{
	if(afxCurrentInstanceHandle==0)
		MessageBox(0,"afxCurrentInstanceHandle==0",0,0);

	ClientThreadId = GetCurrentThreadId();

	// Create event which would be signaled after GUI thread
	// finishes its initialization, so it is safe to continue
	// client execution
	hResumeClient = CreateEvent(0,FALSE,FALSE,NULL);

	// Run GUI thread
	AfxBeginThread(RUNTIME_CLASS(CGuiThread),THREAD_PRIORITY_HIGHEST); // FIXME: THREAD_PRIORITY_HIGHEST may slow things too much
	if(GUIThread==0)	// this var is set in GUIThread constructor
		MessageBox(0,"Error starting thread",0,0);

	// Wait for GUI thread to finish its initialization
	WaitForSingleObject(hResumeClient,INFINITE);	
	CloseHandle(hResumeClient);
	hResumeClient=0;
}
