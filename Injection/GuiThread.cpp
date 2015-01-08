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
//     GuiThread.cpp
// 
//  Abstract:
// 
//     User interface thread and main window implementation
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
#include "MainWindow.h"
#include "Plugins.h"
#include "common.h"
#include "ImportHook.h"
#include "SocksHook.h"
#include "defcmd.h"
#include "HighLevel.h"
#include "ObjectsTab.h"
#include "ObjectTypesTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE hResumeClient=0;
CGuiThread *GUIThread=0;

void ReadRegistry()
{
	GUIThread->Log->set_verbose(0!=GetRegistryDword("","VerboseLogging"));
	LogCI = 0!=GetRegistryDword("","LogCI");
	LogIC = 0!=GetRegistryDword("","LogIC");
	LogSI = 0!=GetRegistryDword("","LogSI");
	LogIS = 0!=GetRegistryDword("","LogIS");
}

void UpdateRegistry()
{
	SetRegistryDword("","VerboseLogging",GUIThread->Log->get_verbose());
	SetRegistryDword("","LogCI",LogCI);
	SetRegistryDword("","LogSI",LogSI);
	SetRegistryDword("","LogIS",LogIS);
	SetRegistryDword("","LogIC",LogIC);
}

// CGuiThread

IMPLEMENT_DYNCREATE(CGuiThread, CWinThread)

CGuiThread::CGuiThread()
{
	m_bAutoDelete = true;
	Log = 0;
	MainTab = 0;
	DisplayTab = 0;
	ObjectsTab = 0;
	ObjectTypesTab = 0;
	GUIThread = this;
}

CGuiThread::~CGuiThread()
{
	delete Log;
	delete MainTab;
	delete DisplayTab;
	delete ObjectsTab;
	delete MainWindow;
	delete ObjectTypesTab;
	MainWindow = 0;
	GUIThread = 0;
}

/*
 *	CGuiThread initialization: Load plugins, create windows & so on	
 */
BOOL CGuiThread::InitInstance()
{
	GUIThreadId = GetCurrentThreadId();

	HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE,FALSE,"Injection_load_dll_event");
	if(hEvent == 0)
	{
		// It is possible to load injection not with ILaunch. For example by
		// manually modifying client's import table
		OutputDebugString("Injection: DLL loaded not from ILaunch.");
	} else
		SetEvent(hEvent);

	// Initialize main window
	MainWindow = new CMainWindow();
	if(!MainWindow->Create())
	{
		char Buff[111];
		sprintf(Buff,"Error creating main window %08X",GetLastError());
		MessageBox(0,Buff,LoadString(IDS_ERROR),0);
	}

	// Enable error logging
	Log=new Logger;
/*
// DEBUG version always use verbose logging
#ifdef _DEBUG
	Log->set_verbose(true);
#else
	Log->set_verbose(false);
#endif
*/

	ReadRegistry();

//////////////////////////////////////////////////////////
//
//		 Here goes Injection initialization
//
//////////////////////////////////////////////////////////

	// Add predefined tabs
	MainTab = new CMainTab(MainWindow->GetTabCWnd());
	MainWindow->AddTab(MainTab->m_hWnd,"Main");

	DisplayTab = new CDisplayTab(MainWindow->GetTabCWnd());
	MainWindow->AddTab(DisplayTab->m_hWnd,"Display");

	ObjectsTab = new CObjectsTab(MainWindow->GetTabCWnd());
	MainWindow->AddTab(ObjectsTab->m_hWnd,"Objects");

	ObjectTypesTab = new CObjectTypesTab(MainWindow->GetTabCWnd());
	MainWindow->AddTab(ObjectTypesTab->m_hWnd,"Object Types");

	if(!HookClientWindowProc())
		error_printf("Unable to hook CreateWindow function!\n");

	if(!HookWinsockFunctions())
		error_printf("Unable to hook winsock.dll!\n");

	// Load plugins
	LoadPluginDLLs();

// This code was previously used to patch client using ignition.
// It is no longer necessary, but is left here as it may be needed
// in the future.
	if(!RemoveEncryptionFromClientWithPlugins())
	{
		trace_printf("All plugins refused to patch client\n");
	}

	// Find the encryption plugin in plugin list
	if(!InitEncryptionPlugin())
	{
		error_printf("Unable to load encryption plugin.");
		if(IDNO==MessageBox(0,LoadString(IDS_NOENCRYPTIONPLUGIN),LoadString(IDS_ERROR),
			MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			ExitProcess(1);
		}
	}

	// Make main window visible
	MainWindow->ShowWindow(SW_SHOW);

	// resume client execution
	SetEvent(hResumeClient);

	return TRUE;
}

/*
 *	Cleanup
 */
int CGuiThread::ExitInstance()
{
	UpdateRegistry();
	CloseSockets();	// buggy old clients do not close sockets on exit. So we imitate closing manually
	UnloadPlugins();
	DeleteWorld();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGuiThread, CWinThread)
END_MESSAGE_MAP()


// CGuiThread message handlers
