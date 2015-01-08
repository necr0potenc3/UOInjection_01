// LaunchProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "uoinjection.h"
#include "LaunchProgressDlg.h"
#include "ClientStarter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLaunchProgressDlg dialog
CLaunchProgressDlg *CRun = 0;

CLaunchProgressDlg::CLaunchProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLaunchProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLaunchProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLaunchProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLaunchProgressDlg)
	DDX_Control(pDX, IDCANCEL, m_bClose);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON1, m_bDetails);
}


BEGIN_MESSAGE_MAP(CLaunchProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CLaunchProgressDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLaunchProgressDlg message handlers

void CLaunchProgressDlg::log(CString sLine)
{
/*
	CString sLog;
	m_ebLaunchLog.GetWindowText(sLog);
	sLog += sLine + "\n";
	m_ebLaunchLog.SetWindowText((LPCTSTR)sLog);
*/
}

void CLaunchProgressDlg::OnClose()
{
	// Do not allow this dialog to close while client is starting
	if(ClientStarterThreadState != CS_Starting && 
	   ClientStarterThreadState != CS_Idle)
		CDialog::OnClose();
}

void CLaunchProgressDlg::OnBnClickedCancel()
{
	if(ClientStarterThreadState != CS_Starting && 
	   ClientStarterThreadState != CS_Idle)
	    EndDialog(0);
}

void CLaunchProgressDlg::OnBnClickedButton1()
{
	MessageBox(ErrorDetails,"Error",MB_ICONINFORMATION);
}
