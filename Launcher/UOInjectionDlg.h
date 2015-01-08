// UOInjectionDlg.h : header file
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
// Copyright (C) 2002 Wayne A. Hogue II 'Chiphead' (w_hogue@hotmail.com)
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


#if !defined(AFX_UOINJECTIONDLG_H__8AF0A5AF_9870_4918_BDF4_1991B1B0F7A0__INCLUDED_)
#define AFX_UOINJECTIONDLG_H__8AF0A5AF_9870_4918_BDF4_1991B1B0F7A0__INCLUDED_

#include ".\Registry.h"	// Added by ClassView
#include "afxwin.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUOInjectionDlg dialog

class CEditServerDgl;

class CUOInjectionDlg : public CDialog
{
// Construction
public:
	CUOInjectionDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CUOInjectionDlg)
	enum { IDD = IDD_UOINJECTION_DIALOG };
	CStatic	m_stClient;
	CStatic	m_stVersion;
	CStatic	m_stUseInjection;
	CStatic	m_stLaunchUOAM;
	CStatic	m_stEmulation;
	CStatic	m_stAltDataDir;
	CStatic	m_stAddress;
	CButton	m_cbCheckForUpdates;
	CButton	m_bOk;
	CButton	m_bEdit;
	CButton	m_bDelete;
	CListBox	m_lbServerList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUOInjectionDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUOInjectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNewButton();
	afx_msg void OnSelchangeServersList();
	afx_msg void OnEditButton();
	afx_msg void OnDeleteButton();
	virtual void OnOK();
	afx_msg void OnCheckForUpdatesCheck();
	afx_msg void OnAdvanceButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRegistry m_rReg;
	void saveServerConfig(CString sName, const CEditServerDgl & dlg);
	// refresh the server list and reset buttons and display
	void RefreshDisplay(void);
public:
	CButton m_bAdvanced;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UOINJECTIONDLG_H__8AF0A5AF_9870_4918_BDF4_1991B1B0F7A0__INCLUDED_)
