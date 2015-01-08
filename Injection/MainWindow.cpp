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
//     MainWindow.cpp
// 
//  Abstract:
// 
//     Main window implementation.
//     Unlike UOAssist or previous Injection version, this window is
//     resizeable.
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
#include "api.h"
#include "common.h"
#include "plugins.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMainWindow *MainWindow=0;

// CMainWindow

IMPLEMENT_DYNAMIC(CMainWindow, CWnd)
CMainWindow::CMainWindow()
{
	LogControl = 0;
	TabControl = 0;
	dlgFont = 0;
	MaxTabWidth = 50;	// dummy values. Would be calculated after adding tabs to control
	MaxTabHeight = 50;
}

CMainWindow::~CMainWindow()
{
	delete TabControl;
	delete LogControl;
	delete dlgFont;
}

void CMainWindow::OnTimer(UINT id)
{ 
	OnNotifyPlugins(CB_Idle, 0);
	return CWnd::OnTimer(id);
}

BEGIN_MESSAGE_MAP(CMainWindow, CWnd)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(TCN_SELCHANGE,IDC_TABCONTROL,OnTabChanged)
	ON_MESSAGE(WM_NOTIFY_PLUGINS,OnNotifyPlugins)
	ON_MESSAGE(WM_NOTIFY_PLUGINS_ASYNC,OnNotifyPluginsAsync)
END_MESSAGE_MAP()



// CMainWindow message handlers


bool CMainWindow::Create(void)
{
	if(!CWnd::CreateEx(WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT | WS_EX_APPWINDOW,
		 AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE, LoadCursor(NULL,IDC_ARROW),
		 (HBRUSH)GetStockObject(LTGRAY_BRUSH), theApp.LoadIcon(IDI_MAINICON)),
		"Injection", WS_OVERLAPPED | WS_THICKFRAME | WS_SYSMENU |
		WS_MINIMIZEBOX | WS_CLIPCHILDREN,CW_USEDEFAULT,CW_USEDEFAULT,
		50,80,NULL,NULL))
		 return false;

	dlgFont=new CFont();
	dlgFont->CreatePointFont(90,"MS Sans Serif");

	TabControl = new CTabCtrl();

	if(!TabControl)
		return false;

	RECT rect;
	GetClientRect(&rect);

	int H = GetDialogBaseUnits()>>16;	// default char height

	RECT tcrect=rect;
	RECT ecrect=rect;
	tcrect.bottom-=H*2+2*GetSystemMetrics(SM_CYBORDER)+1;	// reserve space for log wnd
	ecrect.top=tcrect.bottom+1;

	if(!TabControl->Create(TCS_MULTILINE | TCS_FOCUSONBUTTONDOWN |
		WS_CHILD | WS_VISIBLE, tcrect, this, IDC_TABCONTROL))
		return false;

	LogControl = new CEdit();
	if(!LogControl)
		return false;
	if(!LogControl->CreateEx(WS_EX_CLIENTEDGE,"EDIT","Injection window created.\r\n",WS_VSCROLL | WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
		ecrect, this, IDC_EDITCONTROL))
		return false;

	SendMessage(WM_SETFONT,(WPARAM)(HFONT)*dlgFont,TRUE);
	TabControl->SendMessage(WM_SETFONT,(WPARAM)(HFONT)*dlgFont,TRUE);
	LogControl->SendMessage(WM_SETFONT,(WPARAM)(HFONT)*dlgFont,TRUE);

	// TODO: Read previous window size & position from registry
	// and reposition window on screen

	SetTimer(1, 200, 0);	// idle timer

	return true;
}

/*
 *	Make sure that all child windows are resized correctly after
 *	main injection window changes its size
 */
void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
	if(LogControl)
	{
		RECT ecrect;
		LogControl->GetWindowRect(&ecrect);
		int EH = ecrect.bottom-ecrect.top;	// log window height

		TabControl->MoveWindow(0,0,cx,cy-EH-1);
		LogControl->MoveWindow(0,cy-EH,cx,EH);

		ActivateTab(TabControl->GetCurSel());	// resize active tab
	}
	CWnd::OnSize(nType,cx,cy);
}

void CMainWindow::Log(CString Line)
{
	if(LogControl->GetSafeHwnd())
	{
		CString Str;
		LogControl->GetWindowText(Str);
		Line.Replace("\n","\r\n");
		Str+=Line;

		if(strlen(Str)>2048)		// Keep the log size small enough
			Str=(const char*)Str+4096-strlen(Str);

		LogControl->SetWindowText(Str);

		// scroll edit so that the last not empty line is visible
		int CharsToSkipAtEnd=0;
		while(CharsToSkipAtEnd<(int)strlen(Str) && Str[(int)strlen(Str)-1-CharsToSkipAtEnd]<32)
			CharsToSkipAtEnd++;
		LogControl->LineScroll(LogControl->GetLineCount()-CharsToSkipAtEnd/2-2);
	}
}

/*
 *	calculate the minimal size of injection client area
 */
RECT CMainWindow::CalculateMinimalWindowSize()
{
	RECT rc;
	rc.left=rc.top=0;
	rc.bottom=MaxTabHeight; 
	rc.right=MaxTabWidth;

	if(!TabControl)
		return rc;

	TabControl->AdjustRect(TRUE,&rc);

	RECT ecrect;
	LogControl->GetWindowRect(&ecrect);
	int EH = ecrect.bottom-ecrect.top;	// log window height

	rc.bottom+=EH;

	return rc;
}

/*
 *	Do not allow window to be sized smaller than its minimal tab size
 */
void CMainWindow::OnGetMinMaxInfo(MINMAXINFO*mmi)
{
	RECT r=CalculateMinimalWindowSize();
	mmi->ptMinTrackSize.x=r.right-r.left+2*GetSystemMetrics(SM_CXFRAME);
	mmi->ptMinTrackSize.y=r.bottom-r.top+2*GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION);
}

HWND CMainWindow::GetTabHwnd()
{
	return m_hWnd;
}

/*
 *	Add new tab and resize window if needed
 */
bool CMainWindow::AddTab(HWND TabH, CString name, bool Resizeable)
{
	AddedTab tab;
	tab.Wnd=FromHandle(TabH);
	tab.Name=name;
	tab.Resizeable=Resizeable;
	RECT r;
	tab.Wnd->GetWindowRect(&r);
	if(MaxTabWidth<r.right-r.left)
		MaxTabWidth=r.right-r.left;

	if(MaxTabHeight<r.bottom-r.top)
		MaxTabHeight=r.bottom-r.top;
	
	tab.Wnd->ShowWindow(SW_HIDE);

	TabControl->InsertItem(Tabs.GetCount(),tab.Name);
	Tabs.AddTail(tab);

	ResizeWindowAfterAddingNewTab();

	if(Tabs.GetCount()==1)	// activate the first added tab
		ActivateTab(0);

	return true;
}


void CMainWindow::RemoveTab(CString TabCaption)
{
	// TODO: remove tab
	error_printf("MainWindow::RemoveTab() is not implemented!\n");
}


CWnd *CMainWindow::GetTabCWnd()
{
	return TabControl;
}

/*
 *	If added tab is larger than all other tabs - resize main window.
 *	All tabs would be asked to resize themselves automatically
 */
void CMainWindow::ResizeWindowAfterAddingNewTab()
{
	RECT r=CalculateMinimalWindowSize();
	int width=r.right-r.left+2*GetSystemMetrics(SM_CXFRAME);
	int height=r.bottom-r.top+2*GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION);
	GetWindowRect(&r);
	r.right=r.left+width;
	r.bottom=r.top+height;
// TODO: adjust window position that it is always is fully visible on screen
	MoveWindow(&r);
}

/*
 *	Make the given tab active and resize it/center inside main window
 *	if necerssary
 */
void CMainWindow::ActivateTab(int TabNo)
{
	if(TabControl->GetCurSel()!=TabNo)
		TabControl->SetCurSel(TabNo);

	POSITION pos = Tabs.GetHeadPosition();

	CWnd *wnd=0;
	bool Resizeable;
	// Find the TabNo tab in list
	for (int i=0; i < Tabs.GetCount(); i++)
	{
		AddedTab &tab=Tabs.GetNext(pos);
		if(i==TabNo)
		{
			wnd=tab.Wnd;
			Resizeable=tab.Resizeable;
		} else
			tab.Wnd->ShowWindow(SW_HIDE);
	}

	if(!wnd)
	{
		error_printf("internal error in ActivateTab\n");
		return;
	}

	RECT dr;	// display rect
	TabControl->GetWindowRect(&dr);
	::MapWindowPoints(HWND_DESKTOP,m_hWnd,(LPPOINT)&dr,2);
	TabControl->AdjustRect(FALSE,&dr);

	// There are 2 different tab types: that can resize themselves
	// and that are of fixed size. The first should process resizing
	// themselves, and the second would be centered inside tab control
	// automatically
	if(Resizeable)
	{	// if window is resizeable - let it resize itself
		wnd->MoveWindow(&dr);
	} else
	{	// if not - center it inside tab
		RECT r;
		wnd->GetWindowRect(&r);
		int dw=dr.right-dr.left;
		int dh=dr.bottom-dr.top;
		int w=r.right-r.left;
		int h=r.bottom-r.top;
		dr.left+=(dw-w)/2;
		dr.top+=(dh-h)/2;
		dr.right=dr.left+w;
		dr.bottom=dr.top+h;
		wnd->MoveWindow(&dr);
	}
	wnd->ShowWindow(SW_SHOW);
}

void CMainWindow::OnTabChanged( NMHDR * pNotifyStruct, LRESULT * result )
{
	ActivateTab(TabControl->GetCurSel());
}


HWND INJECTION_API GetInjectionTabHwnd()
{
	if(MainWindow)
		return MainWindow->TabControl->GetSafeHwnd();
	else
		return 0;
}

/*
 *	exported API. Look into API.H for description
 */
BOOL INJECTION_API AddTab(HWND Tab, const char *Name, BOOL Resizeable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL ret=FALSE;

	FUNCTION_PROLOGUE

	ret=MainWindow->AddTab(Tab,Name,Resizeable!=FALSE);

	FUNCTION_EPILOGUE

	return ret;
}

void INJECTION_API RemoveTab(const char *Name)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	FUNCTION_PROLOGUE

	MainWindow->RemoveTab(Name);

	FUNCTION_EPILOGUE
}
