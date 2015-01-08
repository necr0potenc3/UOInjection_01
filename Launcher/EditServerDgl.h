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

#if !defined(AFX_EDITSERVERDGL_H__16564CEC_AB7A_4B0F_A0EB_EBDD4CBD3DAE__INCLUDED_)
#define AFX_EDITSERVERDGL_H__16564CEC_AB7A_4B0F_A0EB_EBDD4CBD3DAE__INCLUDED_

#include ".\Registry.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditServerDgl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditServerDgl dialog

class CEditServerDgl : public CDialog
{
// Construction
public:
	CStringArray m_saAddresses;
	CString m_sOldName;
	CEditServerDgl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditServerDgl)
	enum { IDD = IDD_EDITSERVERDLG_DIALOG };
	CEdit	m_ebName;
	CStatic	m_slClientEmulation;
	CButton	m_cbFixWalk;
	CButton	m_cbFixTalk;
	CButton	m_cbUseInjection;
	CButton	m_bAltDataDir;
	CStatic	m_slAltDataDir;
	CEdit	m_ebAltDataDir;
	CButton	m_cbLaunchUOAM;
	CButton	m_cbUseAltDataDir;
	CEdit	m_ebClient;
	CComboBox	m_cbClientEmulation;
	CButton	m_cbRememberUsername;
	CButton	m_cbRememberPassword;
	CEdit	m_ebPassword;
	CEdit	m_ebUsername;
	CButton	m_bEdit;
	CButton	m_bDelete;
	CListBox	m_lbAddresses;
	CString	m_sAltDataDir;
	CString	m_sClient;
	CString	m_sClientEmulation;
	BOOL	m_boolFixTalk;
	BOOL	m_boolFixWalk;
	BOOL	m_boolLaunchUOAM;
	CString	m_sName;
	BOOL	m_boolRememberPassword;
	BOOL	m_boolRememberUsername;
	BOOL	m_boolUseAltDataDir;
	BOOL	m_boolUseInjection;
	CString	m_sPassword;
	CString	m_sUsername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditServerDgl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditServerDgl)
	afx_msg void OnSelchangeAddressesList();
	afx_msg void OnRememberUsernameCheck();
	afx_msg void OnRememberPasswordCheck();
	afx_msg void OnUseAltDirCheck();
	afx_msg void OnUseInjectionCheck();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetClientButton();
	afx_msg void OnNewAddressButton();
	afx_msg void OnAltDataDirButton();
	afx_msg void OnDeleteAddressButton();
	afx_msg void OnEditAddressButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRegistry m_rReg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSERVERDGL_H__16564CEC_AB7A_4B0F_A0EB_EBDD4CBD3DAE__INCLUDED_)
