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
//     MainTab.h
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

#pragma once
#include "afxwin.h"


// CMainTab dialog

class CMainTab : public CDialog
{
	DECLARE_DYNAMIC(CMainTab)

public:
	CMainTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainTab();

// Dialog Data
	enum { IDD = IDD_MAIN_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedVerboseLogging();
	CButton VerbLogging;
	CButton LCI;
	CButton LIS;
	CButton LSI;
	CButton LIC;
	void EnableCheckboxes();
	afx_msg void OnBnClickedCI();
	afx_msg void OnBnClickedSI();
	afx_msg void OnBnClickedIS();
	afx_msg void OnBnClickedIC();
	void UpdateVars();
	BOOL CbAutoCountItems;
	BOOL CMainTab::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	BOOL DisplayHitpoints;
};
