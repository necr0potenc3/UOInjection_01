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
//     ObjectTypesTab.cpp
// 
//  Abstract:
// 
//     Object types tab - like in old injection
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
#include "ObjectTypesTab.h"


// CObjectTypesTab dialog

IMPLEMENT_DYNAMIC(CObjectTypesTab, CDialog)
CObjectTypesTab::CObjectTypesTab(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectTypesTab::IDD, pParent)
{
	// TODO: read all needed values from registry here


	if(!Create(CObjectTypesTab::IDD,pParent))
		error_printf("Error in CObjectTypesTab constructor!\n");
}

CObjectTypesTab::~CObjectTypesTab()
{
	// TODO: Write changes to registry
}

void CObjectTypesTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CObjectTypesTab, CDialog)
END_MESSAGE_MAP()


// CObjectTypesTab message handlers

/*
 *	Get the item's graphics number from the object types list by its name
 *	returns INVALID_GRAPHIC on error
 *	name can be a hex constant in a form "0xabcdef" or a predefined name
 */
uint16 INJECTION_API GetObjectGraphic(const char * IN Text)
{
	uint16 Graph=(uint16)strtol(Text,0,0);
	if(Graph>0)
		return Graph;

	// TODO: look for the given type in list
	return INVALID_GRAPHIC;
}

/*
 *	Get the item's color from the object types list by its name
 *	returns 0 on error or if the given type does not use color
 *	name can be a hex constant in a form "0xabcdef" or a predefined name
 */
uint16 INJECTION_API GetObjectColor(const char * IN Text)
{
	uint16 Color=(uint16)strtol(Text,0,0);
	if(Color>0)
		return Color;

	// TODO: look for the given type in list
	return 0;
}
