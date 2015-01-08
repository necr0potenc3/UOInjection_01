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

#if !defined(AFX_ADVANCEDSETTINGSDLG_H__085D179F_3046_4344_BE9A_F11BCF666E84__INCLUDED_)
#define AFX_ADVANCEDSETTINGSDLG_H__085D179F_3046_4344_BE9A_F11BCF666E84__INCLUDED_

#include ".\Registry.h"	// Added by ClassView
#include "afxwin.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvancedSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedSettingsDlg dialog

class CAdvancedSettingsDlg : public CDialog
{
// Construction
public:
	CAdvancedSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvancedSettingsDlg)
	enum { IDD = IDD_ADVANCE_SETTINGS_DIALOG };
	CEdit	m_ebPatchFile;
	CEdit	m_ebKeyFile;
	CEdit	m_ebInstallDir;
	CString	m_sInstallDir;
	CString	m_sKeyFile;
	CString	m_sPatchFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvancedSettingsDlg)
	afx_msg void OnInstallDirButton();
	afx_msg void OnKeyfileButton();
	afx_msg void OnPatchfileButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRegistry m_rReg;
public:
	CEdit m_ebInjectionDir;
	CString m_sInjectionDir;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVANCEDSETTINGSDLG_H__085D179F_3046_4344_BE9A_F11BCF666E84__INCLUDED_)
