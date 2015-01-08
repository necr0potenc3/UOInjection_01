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
//     MainWindow.h
// 
//  Abstract:
// 
//     Main window implementation.
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
#include "plugins.h"

// CMainWindow
#define WM_NOTIFY_PLUGINS		(WM_USER + 123)
#define WM_NOTIFY_PLUGINS_ASYNC (WM_NOTIFY_PLUGINS + 1)

class CMainWindow : public CWnd
{
	DECLARE_DYNAMIC(CMainWindow)

public:
	CMainWindow();
	virtual ~CMainWindow();

protected:
	DECLARE_MESSAGE_MAP()
	void OnSize(UINT, int, int);
	void OnTimer(UINT);
	void OnGetMinMaxInfo(MINMAXINFO*);
	LRESULT OnNotifyPlugins(WPARAM,LPARAM);
	LRESULT OnNotifyPluginsAsync(WPARAM,LPARAM);
	afx_msg void OnTabChanged( NMHDR * pNotifyStruct, LRESULT * result );

public:
	CTabCtrl *TabControl;

	bool Create(void);		// Called to create and display this window on screen
	void Log(CString Line);	// Called internally by warning_printf and error_printf to add lines to on-screen log

	HWND GetTabHwnd();		// return HWND of a tab control
	CWnd *GetTabCWnd();		// returns CWnd*
	bool AddTab(HWND Tab, CString Name, bool Resizeable=false);	// add new tab. If !Resizeable - tab would be centered inside tab control
	void RemoveTab(CString Name);				// remove tab by its name

private:
	CEdit *LogControl;
	CFont *dlgFont;

	int MaxTabWidth;	// the width/height of the biggest tab inside tab control
	int MaxTabHeight;

	struct AddedTab
	{
		CWnd *Wnd;
		CString Name;
		bool Resizeable;
	};

	CList<AddedTab,AddedTab&> Tabs;

	RECT CalculateMinimalWindowSize();	// find the min. size of client area injection window can be sized to
	void ResizeWindowAfterAddingNewTab();	// Increase size of a window after adding new tab
	void ActivateTab(int TabNo);		// activate the given tab and show the corresponding window
};


extern CMainWindow *MainWindow;	// pointer to the injection main window
