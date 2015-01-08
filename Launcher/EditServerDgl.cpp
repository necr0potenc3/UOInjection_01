// EditServerDgl.cpp : implementation file
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
#include "UOInjection.h"
#include "EditServerDgl.h"
#include "EditAddressDlg.h"
#include "UOKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditServerDgl dialog


CEditServerDgl::CEditServerDgl(CWnd* pParent /*=NULL*/)
	: CDialog(CEditServerDgl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditServerDgl)
	m_sAltDataDir = _T("");
	m_sClient = _T("");
	m_sClientEmulation = _T("");
	m_boolFixTalk = FALSE;
	m_boolFixWalk = FALSE;
	m_boolLaunchUOAM = FALSE;
	m_sName = _T("");
	m_boolRememberPassword = TRUE;
	m_boolRememberUsername = TRUE;
	m_boolUseAltDataDir = FALSE;
	m_boolUseInjection = TRUE;
	m_sPassword = _T("");
	m_sUsername = _T("");
	//}}AFX_DATA_INIT
}


void CEditServerDgl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditServerDgl)
	DDX_Control(pDX, IDC_NAME_EDIT, m_ebName);
	DDX_Control(pDX, IDC_CLIENT_EMULATION_STATIC, m_slClientEmulation);
	DDX_Control(pDX, IDC_FIX_WALK_CHECK, m_cbFixWalk);
	DDX_Control(pDX, IDC_FIX_TALK_CHECK, m_cbFixTalk);
	DDX_Control(pDX, IDC_USE_INJECTION_CHECK, m_cbUseInjection);
	DDX_Control(pDX, IDC_ALT_DATA_DIR_BUTTON, m_bAltDataDir);
	DDX_Control(pDX, IDC_ALT_DATA_DIR_STATIC, m_slAltDataDir);
	DDX_Control(pDX, IDC_ALT_DATA_DIR_EDIT, m_ebAltDataDir);
	DDX_Control(pDX, IDC_LAUNCH_UOAM_CHECK, m_cbLaunchUOAM);
	DDX_Control(pDX, IDC_USE_ALT_DIR_CHECK, m_cbUseAltDataDir);
	DDX_Control(pDX, IDC_CLIENT_EDIT, m_ebClient);
	DDX_Control(pDX, IDC_CLIENT_EMULATION_COMBO, m_cbClientEmulation);
	DDX_Control(pDX, IDC_REMEMBER_USERNAME_CHECK, m_cbRememberUsername);
	DDX_Control(pDX, IDC_REMEMBER_PASSWORD_CHECK, m_cbRememberPassword);
	DDX_Control(pDX, IDC_PASSWORD_EDIT, m_ebPassword);
	DDX_Control(pDX, IDC_USERNAME_EDIT, m_ebUsername);
	DDX_Control(pDX, IDC_EDIT_ADDRESS_BUTTON, m_bEdit);
	DDX_Control(pDX, IDC_DELETE_ADDRESS_BUTTON, m_bDelete);
	DDX_Control(pDX, IDC_ADDRESSES_LIST, m_lbAddresses);
	DDX_Text(pDX, IDC_ALT_DATA_DIR_EDIT, m_sAltDataDir);
	DDX_Text(pDX, IDC_CLIENT_EDIT, m_sClient);
	DDX_CBString(pDX, IDC_CLIENT_EMULATION_COMBO, m_sClientEmulation);
	DDX_Check(pDX, IDC_FIX_TALK_CHECK, m_boolFixTalk);
	DDX_Check(pDX, IDC_FIX_WALK_CHECK, m_boolFixWalk);
	DDX_Check(pDX, IDC_LAUNCH_UOAM_CHECK, m_boolLaunchUOAM);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sName);
	DDX_Check(pDX, IDC_REMEMBER_PASSWORD_CHECK, m_boolRememberPassword);
	DDX_Check(pDX, IDC_REMEMBER_USERNAME_CHECK, m_boolRememberUsername);
	DDX_Check(pDX, IDC_USE_ALT_DIR_CHECK, m_boolUseAltDataDir);
	DDX_Check(pDX, IDC_USE_INJECTION_CHECK, m_boolUseInjection);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_sPassword);
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_sUsername);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditServerDgl, CDialog)
	//{{AFX_MSG_MAP(CEditServerDgl)
	ON_LBN_SELCHANGE(IDC_ADDRESSES_LIST, OnSelchangeAddressesList)
	ON_BN_CLICKED(IDC_REMEMBER_USERNAME_CHECK, OnRememberUsernameCheck)
	ON_BN_CLICKED(IDC_REMEMBER_PASSWORD_CHECK, OnRememberPasswordCheck)
	ON_BN_CLICKED(IDC_USE_ALT_DIR_CHECK, OnUseAltDirCheck)
	ON_BN_CLICKED(IDC_USE_INJECTION_CHECK, OnUseInjectionCheck)
	ON_BN_CLICKED(IDC_GET_CLIENT_BUTTON, OnGetClientButton)
	ON_BN_CLICKED(IDC_NEW_ADDRESS_BUTTON, OnNewAddressButton)
	ON_BN_CLICKED(IDC_ALT_DATA_DIR_BUTTON, OnAltDataDirButton)
	ON_BN_CLICKED(IDC_DELETE_ADDRESS_BUTTON, OnDeleteAddressButton)
	ON_BN_CLICKED(IDC_EDIT_ADDRESS_BUTTON, OnEditAddressButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditServerDgl message handlers

void CEditServerDgl::OnSelchangeAddressesList() 
{
	m_bEdit.EnableWindow(m_lbAddresses.GetSelCount());	
	m_bDelete.EnableWindow(m_lbAddresses.GetSelCount());	
}

void CEditServerDgl::OnRememberUsernameCheck() 
{
	m_ebUsername.EnableWindow(m_cbRememberUsername.GetCheck());
	m_cbRememberPassword.EnableWindow(m_cbRememberUsername.GetCheck());
	if(!m_cbRememberUsername.GetCheck())
	{
		m_ebUsername.SetWindowText("");
		m_ebPassword.SetWindowText("");
		m_ebPassword.EnableWindow(FALSE);
		m_cbRememberPassword.SetCheck(FALSE);
	}
}

void CEditServerDgl::OnRememberPasswordCheck() 
{
	m_ebPassword.EnableWindow(m_cbRememberPassword.GetCheck());
	
	if(!m_cbRememberPassword.GetCheck())
	{
		m_ebPassword.SetWindowText("");
	}
}

void CEditServerDgl::OnUseAltDirCheck() 
{
	m_cbLaunchUOAM.EnableWindow(m_cbUseAltDataDir.GetCheck());
	m_slAltDataDir.EnableWindow(m_cbUseAltDataDir.GetCheck());
	m_ebAltDataDir.EnableWindow(m_cbUseAltDataDir.GetCheck());
	m_bAltDataDir.EnableWindow(m_cbUseAltDataDir.GetCheck());
	if(!m_cbUseAltDataDir.GetCheck())
	{
		m_cbLaunchUOAM.SetCheck(FALSE);
		m_ebAltDataDir.SetWindowText("");
	}
}

void CEditServerDgl::OnUseInjectionCheck() 
{
	m_slClientEmulation.EnableWindow(m_cbUseInjection.GetCheck());
	m_cbClientEmulation.EnableWindow(m_cbUseInjection.GetCheck());
	m_cbFixTalk.EnableWindow(m_cbUseInjection.GetCheck());
	m_cbFixWalk.EnableWindow(m_cbUseInjection.GetCheck());
	if(!m_cbUseInjection.GetCheck())
	{
		m_cbClientEmulation.SetCurSel(0);
		m_cbFixTalk.SetCheck(FALSE);
		m_cbFixWalk.SetCheck(FALSE);
	}
}

BOOL CEditServerDgl::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CUOKeys uoKeys;
	uoKeys.setEmulationList(m_rReg.getKeyFilePath(),&m_cbClientEmulation);
	m_cbClientEmulation.SetCurSel(0);
	m_ebName.SetFocus();

	if(m_sOldName.GetLength()>0)
	{
		// edit existing
		m_ebName.SetWindowText((LPCTSTR)m_sOldName);
		m_ebClient.SetWindowText((LPCTSTR)m_rReg.getServerClient(m_sOldName));
		m_cbRememberUsername.SetCheck(m_rReg.getServerRememberUsername(m_sOldName));
		CStringArray saAddresses;
		m_rReg.getServerAddresses(m_sOldName, saAddresses);
		for(int i = 0; i<saAddresses.GetSize(); i++)
		{
			m_lbAddresses.AddString((LPCTSTR)(saAddresses.GetAt(i)));
		}
		if(m_cbRememberUsername.GetCheck())
		{
			m_ebUsername.EnableWindow();
			m_ebUsername.SetWindowText((LPCTSTR)m_rReg.getServerUsername(m_sOldName));
//  [3/3/2002]			m_cbRememberPassword.EnableWindow();
			m_cbRememberPassword.SetCheck(m_rReg.getServerRememberPassword(m_sOldName));
			if(m_cbRememberPassword.GetCheck())
			{
				m_ebPassword.EnableWindow();
				m_ebPassword.SetWindowText((LPCTSTR)m_rReg.getServerPassword(m_sOldName));
			}
		}
		m_cbUseAltDataDir.SetCheck(m_rReg.getServerUseAltDataDir(m_sOldName));
		if(m_cbUseAltDataDir.GetCheck())
		{
			m_cbLaunchUOAM.EnableWindow();
			m_cbLaunchUOAM.SetCheck(m_rReg.getServerLaunchUOAM(m_sOldName));
			m_ebAltDataDir.EnableWindow();
			m_ebAltDataDir.SetWindowText((LPCTSTR)m_rReg.getServerAltDataDir(m_sOldName));
		}
		m_cbUseInjection.SetCheck(m_rReg.getServerUseInjection(m_sOldName));
		if(m_cbUseInjection.GetCheck())
		{
			m_cbClientEmulation.EnableWindow();
			int nIndex = m_cbClientEmulation.FindString(0,(LPCTSTR)m_rReg.getServerEmulation(m_sOldName));
			m_cbClientEmulation.SetCurSel(nIndex);
			m_cbFixTalk.EnableWindow();
			m_cbFixTalk.SetCheck(m_rReg.getServerFixTalk(m_sOldName));
			m_cbFixWalk.EnableWindow();
			m_cbFixWalk.SetCheck(m_rReg.getServerFixWalk(m_sOldName));
		}
		m_ebName.EnableWindow(FALSE);
		SetWindowText("Edit Existing Server");
		m_ebClient.SetFocus();
	}
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditServerDgl::OnGetClientButton() 
{
	char path[MAX_PATH + 1];
	strcpy(path, (LPCTSTR)m_sClient);
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = "Executable Fles (*.exe)\0*.exe\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = (LPCTSTR)m_sClient;
	ofn.lpstrTitle = "Select File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	if(GetOpenFileName(&ofn) == TRUE)
	{
		m_ebClient.SetWindowText(path);
	}
}

void CEditServerDgl::OnNewAddressButton() 
{
	CEditAddressDlg dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		CString sAddressLine = dlg.m_sAddress;
		sAddressLine.Format("%s,%d", sAddressLine, dlg.m_nPort);
		m_lbAddresses.AddString((LPCTSTR)sAddressLine);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CEditServerDgl::OnAltDataDirButton() 
{
	char path[MAX_PATH + 1];
	BROWSEINFO bi;

	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;	// This is just for display: not useful
	bi.lpszTitle = "Choose Alternate Data Directory";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl != NULL && SHGetPathFromIDList(pidl, path))
	{
		m_ebAltDataDir.SetWindowText(path);
	}

}

void CEditServerDgl::OnDeleteAddressButton() 
{
	m_lbAddresses.DeleteString(m_lbAddresses.GetCurSel());
	m_bDelete.EnableWindow(FALSE);
	m_bEdit.EnableWindow(FALSE);
}

void CEditServerDgl::OnEditAddressButton() 
{
	int nIndex = m_lbAddresses.GetCurSel();
	CString sLine;
	m_lbAddresses.GetText(nIndex, sLine);
	int nSeperator = sLine.Find(",");
	CEditAddressDlg dlg;
	dlg.m_sOldAddress = sLine.Mid(0,nSeperator);
	dlg.m_sOldPort = sLine.Mid(nSeperator+1);
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		CString sAddressLine = dlg.m_sAddress;
		sAddressLine.Format("%s,%d", sAddressLine, dlg.m_nPort);
		m_lbAddresses.DeleteString(nIndex);
		m_lbAddresses.InsertString(nIndex, (LPCTSTR)sAddressLine);
		m_lbAddresses.SetCurSel(nIndex);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CEditServerDgl::OnOK() 
{
	if(m_ebName.GetWindowTextLength()< 1)
	{
		MessageBox("You must provide a Name", "Blank Name Box", MB_OK | MB_ICONINFORMATION );
		m_ebName.SetFocus();
		return;
	}
	else
	{
		// does name already exist?
		if(m_sOldName.GetLength() == 0)
		{
			CString sNewName;
			m_ebName.GetWindowText(sNewName);
			if(m_rReg.serverExists(sNewName))
			{
				MessageBox("The server name already exists! Please choose a different name.", "Server Name Exists", MB_OK | MB_ICONINFORMATION );
				m_ebName.SetFocus();
				return;
			}
		}
	}

	if(m_ebClient.GetWindowTextLength()< 1)
	{
		MessageBox("You must provide a Client", "Blank Client Box", MB_OK | MB_ICONINFORMATION );
		m_ebClient.SetFocus();
		return;
	}
	else
	{
		// check for file existance
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		CString sFile;
		m_ebClient.GetWindowText(sFile);
		hFind = FindFirstFile((LPCTSTR)sFile, &FindFileData);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			MessageBox("The Client specified is not found", "Client Not Found", MB_OK | MB_ICONINFORMATION );
			m_ebClient.SetFocus();
			return;
		}
//		else
//		{
//			if(!(GetFileType(hFind) && VFT_APP))
//			{
//				MessageBox("The Client specified is not an Application", "Client Not Executable", MB_OK | MB_ICONINFORMATION );
//				m_ebClient.SetFocus();
//				FindClose(hFind);
//				return;
//			}
//		}
		FindClose(hFind);
	}
	if(m_lbAddresses.GetCount()< 1)
	{
		MessageBox("You must provide at least 1 address", "Blank Address List", MB_OK | MB_ICONINFORMATION );
		m_lbAddresses.SetFocus();
		return;
	}
	if(m_cbRememberUsername.GetCheck())
	{
		if(m_ebUsername.GetWindowTextLength() < 1)
		{
			MessageBox("You must provide a Username or uncheck the Remember Username checkbox", "Blank Username", MB_OK | MB_ICONINFORMATION );
			m_ebUsername.SetFocus();
			return;
		}
		if(m_cbRememberPassword.GetCheck())
		{
			if(m_ebUsername.GetWindowTextLength() < 1)
			{
				MessageBox("You must provide a Password or uncheck the Remember Password checkbox", "Blank Password", MB_OK | MB_ICONINFORMATION );
				m_ebPassword.SetFocus();
				return;
			}
		}
	}
	if(m_cbUseAltDataDir.GetCheck())
	{
		if(m_ebAltDataDir.GetWindowTextLength()<1)
		{
			MessageBox("You must provide a an Alternate Data Directory or uncheck the Use Alt Dir checkbox", "Blank Alternate Data Directory", MB_OK | MB_ICONINFORMATION );
			m_ebAltDataDir.SetFocus();
			return;
		}
		else
		{
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			CString sFile;
			m_ebAltDataDir.GetWindowText(sFile);
			hFind = FindFirstFile((LPCTSTR)sFile, &FindFileData);
			if(hFind == INVALID_HANDLE_VALUE)
			{
				MessageBox("The Alt Data Directory specified is not found", "Alt Data Dir Not Found", MB_OK | MB_ICONINFORMATION );
				m_ebAltDataDir.SetFocus();
				return;
			}
//			else
//			{
//				if(!(FindFileData.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY))
//				{
//					MessageBox("The Alt Data Dir specified is not a Directory", "Alt Data Dir not a Directory", MB_OK | MB_ICONINFORMATION );
//					m_ebAltDataDir.SetFocus();
//					FindClose(hFind);
//					return;
//				}
//			}
			FindClose(hFind);
		}

	}
	m_saAddresses.RemoveAll();
	int i = 0;
	for(; i< m_lbAddresses.GetCount(); i++)
	{
		CString sAddress;
		m_lbAddresses.GetText(i, sAddress);
		m_saAddresses.Add(sAddress);
	}

	CDialog::OnOK();
}
