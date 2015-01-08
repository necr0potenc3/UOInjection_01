#pragma once
#include "afxwin.h"


// CObjectsTab dialog

class CObjectsTab : public CDialog
{
	DECLARE_DYNAMIC(CObjectsTab)

public:
	CObjectsTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CObjectsTab();

// Dialog Data
	enum { IDD = IDD_ITEMS_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton FromTargetButton;
	afx_msg void OnBnClickedFromtargetidcFromtarget();
};
