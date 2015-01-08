// EditAddressDlg.cpp : implementation file
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


#include "stdafx.h"
#include "uoinjection.h"
#include "EditAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditAddressDlg dialog


CEditAddressDlg::CEditAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditAddressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditAddressDlg)
	m_nPort = 0;
	m_sAddress = _T("");
	//}}AFX_DATA_INIT
}


void CEditAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditAddressDlg)
	DDX_Control(pDX, IDC_PORT_EDIT, m_ebPort);
	DDX_Control(pDX, IDC_ADDRESS_EDIT, m_ebAddress);
	DDX_Control(pDX, IDC_LOGIN_FILE_EDIT, m_ebLoginCfg);
	DDX_Text(pDX, IDC_PORT_EDIT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 1, 65535);
	DDX_Text(pDX, IDC_ADDRESS_EDIT, m_sAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CEditAddressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditAddressDlg message handlers

BOOL CEditAddressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// put the login file in the dialog for reference
	CString sLoginFilePath = rReg.getUOInstallDir();
	sLoginFilePath += "\\login.cfg";
	CString sLoginFile;
	FILE *fLoginFile;
	if((fLoginFile = fopen((LPCTSTR)sLoginFilePath, "rb")) != 0)
	{
		CString sLine;
		while (fgets(sLine.GetBuffer(256), 256, fLoginFile))
		{
			sLine.ReleaseBuffer();
			sLoginFile += sLine;
		}
		sLine.ReleaseBuffer();
		fclose(fLoginFile);

		m_ebLoginCfg.SetWindowText((LPCTSTR)sLoginFile);
	}
	else
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// Display the string.
		MessageBox( (LPCTSTR)lpMsgBuf, "Error opening Login File", MB_OK | MB_ICONINFORMATION );
		MessageBox( (LPCTSTR)sLoginFilePath, "Error opening Login File", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	if(m_sOldAddress.GetLength()>0)
	{
		m_ebAddress.SetWindowText((LPCTSTR)m_sOldAddress);
	}
	if(m_sOldPort.GetLength() > 0)
	{
		m_ebPort.SetWindowText((LPCTSTR)m_sOldPort);
	}
	m_ebAddress.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditAddressDlg::OnOK()
{
	CString sAddress;
	m_ebAddress.GetWindowText(sAddress);
	if(sAddress.GetLength() < 1)
	{
		MessageBox("You must provide an address", "Blank Address Box", MB_OK | MB_ICONINFORMATION );
		m_ebAddress.SetFocus();
		return;
	}
	
	CDialog::OnOK();
}
