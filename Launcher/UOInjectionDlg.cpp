// UOInjectionDlg.cpp : implementation file
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
#include "AdvancedSettingsDlg.h"
#include "LaunchProgressDlg.h"
#include "UOInjectionDlg.h"
#include "crc.h"
#include "UOKeys.h"
#include "LaunchProgressDlg.h"
#include "ClientStarter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUOInjectionDlg dialog

CUOInjectionDlg::CUOInjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUOInjectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUOInjectionDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUOInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUOInjectionDlg)
	DDX_Control(pDX, IDC_CLIENT_STATIC, m_stClient);
	DDX_Control(pDX, IDC_VERSION_STATIC, m_stVersion);
	DDX_Control(pDX, IDC_USE_INJECTION_STATIC, m_stUseInjection);
	DDX_Control(pDX, IDC_LAUNCH_UOAM_STATIC, m_stLaunchUOAM);
	DDX_Control(pDX, IDC_CLIENT_EMULATION_STATIC, m_stEmulation);
	DDX_Control(pDX, IDC_ALT_DATA_DIR_STATIC, m_stAltDataDir);
	DDX_Control(pDX, IDC_ADDRESS_STATIC, m_stAddress);
	DDX_Control(pDX, IDC_CHECK_FOR_UPDATES_CHECK, m_cbCheckForUpdates);
	DDX_Control(pDX, IDOK, m_bOk);
	DDX_Control(pDX, IDC_EDIT_BUTTON, m_bEdit);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_bDelete);
	DDX_Control(pDX, IDC_SERVERS_LIST, m_lbServerList);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ADVANCE_BUTTON, m_bAdvanced);
}

BEGIN_MESSAGE_MAP(CUOInjectionDlg, CDialog)
	//{{AFX_MSG_MAP(CUOInjectionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NEW_BUTTON, OnNewButton)
	ON_LBN_SELCHANGE(IDC_SERVERS_LIST, OnSelchangeServersList)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, OnEditButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_CHECK_FOR_UPDATES_CHECK, OnCheckForUpdatesCheck)
	ON_BN_CLICKED(IDC_ADVANCE_BUTTON, OnAdvanceButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUOInjectionDlg message handlers

BOOL CUOInjectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// add servers to dialog
	m_rReg.setServerList(&m_lbServerList);
	m_cbCheckForUpdates.SetCheck(m_rReg.getCheckForUpdates());
	//set version
	CString strVersion;
	strVersion.LoadString(IDS_VERSION);
	m_stVersion.SetWindowText((LPCTSTR)strVersion);
	// clear runtime

	m_lbServerList.PostMessage(WM_LBUTTONDOWN);	// click the server list to select the topmost server
	m_lbServerList.PostMessage(WM_LBUTTONUP);	// if there is anything in that list

	if(strlen(m_rReg.getUOInstallDir())==0)		// force user to select client dir
	{
		m_bAdvanced.PostMessage(WM_LBUTTONDOWN);
		m_bAdvanced.PostMessage(WM_LBUTTONUP);
	}

	return TRUE;
}

void CUOInjectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUOInjectionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUOInjectionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUOInjectionDlg::OnNewButton() 
{
	CEditServerDgl 	dlg;
	dlg.m_sOldName = "";
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		m_rReg.addServer(dlg.m_sName);
		saveServerConfig(dlg.m_sName, dlg);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}



void CUOInjectionDlg::OnEditButton() 
{
	CEditServerDgl 	dlg;
	int nIndex = m_lbServerList.GetCurSel();
	m_lbServerList.GetText(nIndex, dlg.m_sOldName);
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		saveServerConfig(dlg.m_sName, dlg);
		RefreshDisplay();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

}

void CUOInjectionDlg::saveServerConfig(CString sName, const CEditServerDgl & dlg)
{
	m_rReg.setServerClient(sName, dlg.m_sClient);
	m_rReg.setServerAddresses(sName, dlg.m_saAddresses);
	m_rReg.setServerRememberUsername(sName, dlg.m_boolRememberUsername);
	m_rReg.setServerUsername(sName, dlg.m_sUsername);
	m_rReg.setServerRememberPassword(sName, dlg.m_boolRememberPassword);
	m_rReg.setServerPassword(sName, dlg.m_sPassword);
	m_rReg.setServerUseAltDataDir(sName, dlg.m_boolUseAltDataDir);
	m_rReg.setServerLaunchUOAM(sName, dlg.m_boolLaunchUOAM);
	m_rReg.setServerAltDataDir(sName, dlg.m_sAltDataDir);
	m_rReg.setServerUseInjection(sName, dlg.m_boolUseInjection);
	m_rReg.setServerEmulation(sName, dlg.m_sClientEmulation);
	m_rReg.setServerFixTalk(sName, dlg.m_boolFixTalk);
	m_rReg.setServerFixWalk(sName, dlg.m_boolFixWalk);
	m_rReg.setServerList(&m_lbServerList);
}

void CUOInjectionDlg::OnSelchangeServersList() 
{
	m_bOk.EnableWindow(m_lbServerList.GetSelCount());
	m_bEdit.EnableWindow(m_lbServerList.GetSelCount());
	m_bDelete.EnableWindow(m_lbServerList.GetSelCount());
	if(m_lbServerList.GetSelCount())
	{
		CStringArray saAddresses;
		CString sServer;
		m_lbServerList.GetText(m_lbServerList.GetCurSel(), sServer);
		m_rReg.getServerAddresses(sServer, saAddresses);
		m_stAddress.SetWindowText((LPCTSTR)(saAddresses.GetAt(0)));
		m_stClient.SetWindowText((LPCTSTR)m_rReg.getServerClient(sServer));
		if(m_rReg.getServerUseAltDataDir(sServer))
		{
			CString sTmp = m_rReg.getServerAltDataDir(sServer);
			m_stAltDataDir.SetWindowText(sTmp);
			if(m_rReg.getServerLaunchUOAM(sServer))
				m_stLaunchUOAM.SetWindowText("Yes");
			else
				m_stLaunchUOAM.SetWindowText("No");
		}
		else
		{
			m_stAltDataDir.SetWindowText("None");
			m_stLaunchUOAM.SetWindowText("No");
		}
		if(m_rReg.getServerUseInjection(sServer))
		{
			CString sTmp = m_rReg.getServerEmulation(sServer);
			m_stEmulation.SetWindowText(sTmp);
			m_stUseInjection.SetWindowText("Yes");
		}
		else
		{
			m_stEmulation.SetWindowText("None");
			m_stUseInjection.SetWindowText("No");
		}
	}
}

void CUOInjectionDlg::OnDeleteButton() 
{
	
	int nResponse = MessageBox(TEXT("Are you sure you want to delete this server entry?"), TEXT("Delete Server Entry"), MB_OKCANCEL | MB_ICONWARNING );	
	if(nResponse == IDOK)
	{
		CString sTemp;
		m_lbServerList.GetText(m_lbServerList.GetCurSel(), sTemp);
		m_rReg.deleteServer(sTemp);
		RefreshDisplay();
	}
}

void CUOInjectionDlg::OnOK() 
{
	// set the runtime current server
	CString sCurrentServer;
	if(m_lbServerList.GetCurSel()<0)
		return;

	m_lbServerList.GetText(m_lbServerList.GetCurSel(), sCurrentServer);

/*	client patching is no longer needed
	// set current client checksum and length
	UINT nChecksum = 0;
	UINT nLength = 0;
	CCrc crc;
	if(!crc.GetCrc(m_rReg.getClient(), &nChecksum, &nLength))
	{
		MessageBox(TEXT("Client specified in configuration does not exist.  Edit config!"), TEXT("Client does not exist"), MB_OKCANCEL | MB_ICONWARNING );	
		return;
	}
	m_rReg.setCurrenClientChecksum(nChecksum);
	m_rReg.setCurrentClientLength(nLength);
*/

	m_rReg.setCurrentServer(sCurrentServer);
	UpdateKeysInRegistry();	// set the LoginKey1/2 values in registry

	// create login.cfg file in UO dir
	{
		// TODO: when I'll finish the "AltDataDir" stuff - create this file in alternate directory
		FILE *Login;
		if((Login = fopen((LPCTSTR)(m_rReg.getUOInstallDir()+"\\login.cfg"), "wt")) == 0)
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
			MessageBox( (LPCTSTR)lpMsgBuf, "Error creating login.cfg", MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );

			return;
		}
		fputs("; Generated by UOInjection\n; do not edit this file\n",Login);
		CStringArray s;
		m_rReg.getAddresses(s);
		for(int i=0; i<s.GetCount(); i++)
			fprintf(Login, "LoginServer=%s\n",(const char*)s.GetAt(i));

		fclose(Login);
	}

	CRun = new CLaunchProgressDlg;
	StartClientParams *pParams = new StartClientParams;

	strcpy(pParams->ClientDir, m_rReg.getUOInstallDir());	
	m_stClient.GetWindowText(pParams->Executable,MAX_PATH);
	m_stAltDataDir.GetWindowText(pParams->MulsDir,MAX_PATH);
	m_stAddress.GetWindowText(pParams->ServerAddress,MAX_PATH);

	if(pParams->Executable[0]==0 || pParams->ClientDir[0]==0)
	{
		MessageBox("You should select correct directories in Advanced settings.");
		return;
	}

	ClientStarterThreadState = CS_Idle;

	AfxBeginThread(ClientStarterThreadProc, pParams);	
	// pParams is freed in thread

	CRun->DoModal();

	while(ClientStarterThreadState == CS_Starting)
		;	// Should not be necessary, because dialog is closed just after setting ClientStarterThreadState

	if(ClientStarterThreadState == CS_Successful)	
		PostQuitMessage(0);	// Exit if started successfully

	delete CRun;
	CRun=0;

	delete pParams;

//	CDialog::OnOK();
}

void CUOInjectionDlg::OnCheckForUpdatesCheck() 
{
	m_rReg.setCheckForUpdates(m_cbCheckForUpdates.GetCheck());
}

void CUOInjectionDlg::OnAdvanceButton() 
{
	CAdvancedSettingsDlg dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		m_rReg.setInstallDirPath(dlg.m_sInjectionDir);
		m_rReg.setKeyFilePath(dlg.m_sKeyFile);
//		m_rReg.setPatchFilePath(dlg.m_sPatchFile);
		m_rReg.setUOInstallDir(dlg.m_sInstallDir);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

// refresh the server list and reset buttons and display
void CUOInjectionDlg::RefreshDisplay(void)
{
	m_rReg.setServerList(&m_lbServerList);
	m_bDelete.EnableWindow(FALSE);
	m_bEdit.EnableWindow(FALSE);
	m_bOk.EnableWindow(FALSE);
	m_stAddress.SetWindowText("");
	m_stAltDataDir.SetWindowText("");
	m_stEmulation.SetWindowText("");
	m_stLaunchUOAM.SetWindowText("");
	m_stUseInjection.SetWindowText("");
	m_stClient.SetWindowText("");
}
