#pragma once


// CObjectTypesTab dialog

class CObjectTypesTab : public CDialog
{
	DECLARE_DYNAMIC(CObjectTypesTab)

public:
	CObjectTypesTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CObjectTypesTab();

// Dialog Data
	enum { IDD = IDD_OBJECT_TYPES_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
