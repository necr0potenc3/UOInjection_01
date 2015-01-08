// AdvancedSettingsDlg.cpp : implementation file
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
#include "AdvancedSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvancedSettingsDlg dialog


CAdvancedSettingsDlg::CAdvancedSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedSettingsDlg::IDD, pParent)
	, m_sInjectionDir(_T(""))
{
	//{{AFX_DATA_INIT(CAdvancedSettingsDlg)
	m_sInstallDir = _T("");
	m_sKeyFile = _T("");
	m_sPatchFile = _T("");
	m_sInjectionDir = _T("");
	//}}AFX_DATA_INIT
}


void CAdvancedSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedSettingsDlg)
	//	DDX_Control(pDX, IDC_PATCHFILE_EDIT, m_ebPatchFile);
	DDX_Control(pDX, IDC_KEYFILE_EDIT, m_ebKeyFile);
	DDX_Control(pDX, IDC_INSTALL_DIR_EDIT, m_ebInstallDir);
	DDX_Text(pDX, IDC_INSTALL_DIR_EDIT, m_sInstallDir);
	DDX_Text(pDX, IDC_KEYFILE_EDIT, m_sKeyFile);
	//	DDX_Text(pDX, IDC_PATCHFILE_EDIT, m_sPatchFile);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_INJECTIONDIR_EDIT, m_ebInjectionDir);
	DDX_Text(pDX, IDC_INJECTIONDIR_EDIT, m_sInjectionDir);
}


BEGIN_MESSAGE_MAP(CAdvancedSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvancedSettingsDlg)
	ON_BN_CLICKED(IDC_INSTALL_DIR_BUTTON, OnInstallDirButton)
	ON_BN_CLICKED(IDC_KEYFILE_BUTTON, OnKeyfileButton)
	ON_BN_CLICKED(IDC_PATCHFILE_BUTTON, OnPatchfileButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedSettingsDlg message handlers

void CAdvancedSettingsDlg::OnKeyfileButton() 
{
	CString sOrigPath;
	m_ebKeyFile.GetWindowText(sOrigPath);
	char path[MAX_PATH + 1];
	strcpy(path, (LPCTSTR)sOrigPath);
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = "Config Fles (*.cfg)\0*.cfg\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = (LPCTSTR)sOrigPath;
	ofn.lpstrTitle = "Select Key Config File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	if(GetOpenFileName(&ofn) == TRUE)
	{
		m_ebKeyFile.SetWindowText(path);
	}
	
}

void CAdvancedSettingsDlg::OnPatchfileButton() 
{
	char path[MAX_PATH + 1];
	BROWSEINFO bi;

	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;	// This is just for display: not useful
	bi.lpszTitle = "Choose Directory With Injection.dll";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl != NULL && SHGetPathFromIDList(pidl, path))
	{
		m_ebInjectionDir.SetWindowText(path);
	}
/*	
 	CString sOrigPath;
	m_ebPatchFile.GetWindowText(sOrigPath);
	char path[MAX_PATH + 1];
	strcpy(path, (LPCTSTR)sOrigPath);
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = "Config Fles (*.cfg)\0*.cfg\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = (LPCTSTR)sOrigPath;
	ofn.lpstrTitle = "Select Patch Config File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	if(GetOpenFileName(&ofn) == TRUE)
	{
		m_ebPatchFile.SetWindowText(path);
	}
*/
}

void CAdvancedSettingsDlg::OnInstallDirButton() 
{
	char path[MAX_PATH + 1];
	BROWSEINFO bi;

	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;	// This is just for display: not useful
	bi.lpszTitle = "Choose Client Installation Directory";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl != NULL && SHGetPathFromIDList(pidl, path))
	{
		m_ebInstallDir.SetWindowText(path);
	}

}

BOOL CAdvancedSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ebInstallDir.SetWindowText(m_rReg.getUOInstallDir());
//	m_ebPatchFile.SetWindowText(m_rReg.getPatchFilePath());
	m_ebInjectionDir.SetWindowText(m_rReg.getInstallDirPath());
	m_ebKeyFile.SetWindowText(m_rReg.getKeyFilePath());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


