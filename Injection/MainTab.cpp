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
//     MainTab.cpp
// 
//  Abstract:
// 
//     Main Tab implementation
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
#include "MainTab.h"
#include "GuiThread.h"
#include "api.h"
#include "SocksHook.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainTab dialog

IMPLEMENT_DYNAMIC(CMainTab, CDialog)
CMainTab::CMainTab(CWnd* pParent /*=NULL*/)
	: CDialog(CMainTab::IDD, pParent)
	, CbAutoCountItems(FALSE)
	, DisplayHitpoints(FALSE)
{
	CbAutoCountItems = GetRegistryDword("","AutoCountItems",TRUE);
	DisplayHitpoints = GetRegistryDword("","DisplayHitpoints",TRUE);
	if(!Create(CMainTab::IDD,pParent))
		error_printf("Error in CMainTab constructor!\n");
	VerbLogging.SetCheck(GUIThread->Log->get_verbose()?BST_CHECKED:BST_UNCHECKED);
	EnableCheckboxes();

	// TODO: read LogXX values from registry

	LCI.SetCheck(LogCI?BST_CHECKED:BST_UNCHECKED);
	LIC.SetCheck(LogIC?BST_CHECKED:BST_UNCHECKED);
	LSI.SetCheck(LogSI?BST_CHECKED:BST_UNCHECKED);
	LIS.SetCheck(LogIS?BST_CHECKED:BST_UNCHECKED);
}

CMainTab::~CMainTab()
{
	SetRegistryDword("","AutoCountItems",CbAutoCountItems);
	SetRegistryDword("","DisplayHitpoints",DisplayHitpoints);
}

void CMainTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, VerbLogging);
	DDX_Control(pDX, IDC_CHECK3, LCI);
	DDX_Control(pDX, IDC_CHECK4, LIS);
	DDX_Control(pDX, IDC_CHECK5, LSI);
	DDX_Control(pDX, IDC_CHECK6, LIC);
	DDX_Check(pDX, IDC_CHECK7, CbAutoCountItems);
	DDX_Check(pDX, IDC_CHECK1, DisplayHitpoints);
}


BEGIN_MESSAGE_MAP(CMainTab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedVerboseLogging)
	ON_BN_CLICKED(IDC_CHECK3, OnBnClickedCI)
	ON_BN_CLICKED(IDC_CHECK4, OnBnClickedSI)
	ON_BN_CLICKED(IDC_CHECK5, OnBnClickedIS)
	ON_BN_CLICKED(IDC_CHECK6, OnBnClickedIC)
END_MESSAGE_MAP()


// CMainTab message handlers

void CMainTab::OnBnClickedButton2()
{
	// TODO: show a normal dialog and not this dumb MessageBox
	MessageBox("UOInjection - free UOAssist clone.\nVisit http://injection.sourceforge.net for more information.","About",MB_ICONINFORMATION);
}

void CMainTab::OnBnClickedVerboseLogging()
{
	GUIThread->Log->set_verbose(VerbLogging.GetCheck()==BST_CHECKED);
	EnableCheckboxes();
}

void CMainTab::EnableCheckboxes()
{
	bool Ena = VerbLogging.GetCheck()==BST_CHECKED;
	LIC.EnableWindow(Ena);
	LCI.EnableWindow(Ena);
	LIS.EnableWindow(Ena);
	LSI.EnableWindow(Ena);
}

void CMainTab::UpdateVars()
{
	LogCI = LCI.GetCheck()==BST_CHECKED;
	LogSI = LSI.GetCheck()==BST_CHECKED;
	LogIC = LIC.GetCheck()==BST_CHECKED;
	LogIS = LIS.GetCheck()==BST_CHECKED;
}

void CMainTab::OnBnClickedCI()
{
	UpdateVars();
}

void CMainTab::OnBnClickedSI()
{
	UpdateVars();
}

void CMainTab::OnBnClickedIS()
{
	UpdateVars();
}

void CMainTab::OnBnClickedIC()
{
	UpdateVars();
}

/*
 *	Handle all notification messages in one place.
 */
BOOL CMainTab::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// update variables
	UpdateData();
	return CDialog::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}
