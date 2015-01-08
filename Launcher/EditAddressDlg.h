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

#if !defined(AFX_EDITADDRESSDLG_H__6B48DCF2_24B6_4844_A158_B8429CF87A11__INCLUDED_)
#define AFX_EDITADDRESSDLG_H__6B48DCF2_24B6_4844_A158_B8429CF87A11__INCLUDED_

#include ".\Registry.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditAddressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditAddressDlg dialog

class CEditAddressDlg : public CDialog
{
// Construction
public:
	CEditAddressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditAddressDlg)
	enum { IDD = IDD_EDIT_ADDRESS_DIALOG };
	CEdit	m_ebPort;
	CEdit	m_ebAddress;
	CEdit	m_ebLoginCfg;
	UINT	m_nPort;
	CString	m_sAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditAddressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditAddressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRegistry rReg;
public:
	CString m_sOldPort;
	CString m_sOldAddress;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITADDRESSDLG_H__6B48DCF2_24B6_4844_A158_B8429CF87A11__INCLUDED_)
