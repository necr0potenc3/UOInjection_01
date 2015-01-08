#include "afxwin.h"
#if !defined(AFX_LAUNCHPROGRESSDLG_H__5DDC993D_0A6F_42A6_B819_1D7B3C22A09E__INCLUDED_)
#define AFX_LAUNCHPROGRESSDLG_H__5DDC993D_0A6F_42A6_B819_1D7B3C22A09E__INCLUDED_

#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LaunchProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLaunchProgressDlg dialog

class CLaunchProgressDlg : public CDialog
{
// Construction
public:
	void log(CString sLine);
	CLaunchProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLaunchProgressDlg)
	enum { IDD = IDD_LAUNCH_PROGRESS_DIALOG };
	CButton	m_bClose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLaunchProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLaunchProgressDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	CButton m_bDetails;
	afx_msg void OnBnClickedButton1();
};

extern CLaunchProgressDlg *CRun;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAUNCHPROGRESSDLG_H__5DDC993D_0A6F_42A6_B819_1D7B3C22A09E__INCLUDED_)
