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
//     GuiThread.h
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
#pragma once

#include "common.h" 
#include "MainTab.h"
#include "DisplayTab.h"
#include "ObjectsTab.h"
#include "ObjectTypesTab.h"

// CGuiThread

class CGuiThread : public CWinThread
{
	DECLARE_DYNCREATE(CGuiThread)

protected:
	CGuiThread();           // protected constructor used by dynamic creation
	virtual ~CGuiThread();

public:
	CDisplayTab *DisplayTab;
	CObjectsTab *ObjectsTab;
	CObjectTypesTab *ObjectTypesTab;
	CMainTab *MainTab;
	Logger *Log;

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DWORD GetThreadId() {return m_nThreadID;}

protected:
	DECLARE_MESSAGE_MAP()
};

extern HANDLE hResumeClient;	// Event that is signaled after the user pressed a button "Continue client execution"
extern CGuiThread *GUIThread;	// GUI thread object
