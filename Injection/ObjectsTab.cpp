////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002 mamaich
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
//
//  Module Name:
// 
//     ObjectsTab.cpp
// 
//  Abstract:
// 
//     Objects tab - like in old injection
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [3/3/2002] - created
//
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Injection.h"
#include "api.h"
#include "common.h"
#include "ObjectsTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CObjectsTab dialog

IMPLEMENT_DYNAMIC(CObjectsTab, CDialog)
CObjectsTab::CObjectsTab(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectsTab::IDD, pParent)
{
	// TODO: read all needed values from registry here


	if(!Create(CObjectsTab::IDD,pParent))
		error_printf("Error in CObjectsTab constructor!\n");
}

CObjectsTab::~CObjectsTab()
{
	// TODO: Write changes to registry
}

void CObjectsTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FROMTARGETIDC_FROMTARGET, FromTargetButton);
}


BEGIN_MESSAGE_MAP(CObjectsTab, CDialog)
	ON_BN_CLICKED(IDC_FROMTARGETIDC_FROMTARGET, OnBnClickedFromtargetidcFromtarget)
END_MESSAGE_MAP()


// CObjectsTab message handlers

/*
 *	This procedure is executed after "From Target" button is pessed.
 *  It requests a target's serial and its info.
 *  (this code is run in a parallel thread)
 */
unsigned int FromTargetProc(void* Param)
{
	CObjectsTab *Tab=(CObjectsTab*)Param;

	ClientPrint("Select an object...");

	// TODO: bring the UO client's window to front

	uint32 Serial=SelectItem();
	Tab->FromTargetButton.EnableWindow(TRUE);

	// TODO: bring Injection window to front

	if(Serial!=INVALID_SERIAL)
	{
		ObjectInformation oi;
		if(GetObjectInformation(Serial,&oi))
		{
			ClientPrint("Selected object %08X, graphic %04X, color %04X",
				oi.Serial,oi.Graphic,oi.Color);

			// Process this data as needed
			return 0;
		}
	}

	ClientPrintWarning("Object selection canceled");
	return 0;
}

// the "from target" button
void CObjectsTab::OnBnClickedFromtargetidcFromtarget()
{
	FromTargetButton.EnableWindow(FALSE);
	if(!BeginHighLevelApiThread(FromTargetProc,this))
		FromTargetButton.EnableWindow(TRUE);	// reenable window if the thread was not created for some reason (for example not logged in yet)
}


/*
 *	Get the item's serial number from the objects list by its name.
 *	returns INVALID_SERIAL on error
 *	name can be a hex constant in a form "0xabcdef" or a predefined name
 */
uint32 INJECTION_API GetObjectSerial(const char * IN Text)
{
	uint32 Serial=strtol(Text,0,0);
	if(Serial>0)
		return Serial;

	// TODO: look for the given item in list
	return INVALID_SERIAL;
}

