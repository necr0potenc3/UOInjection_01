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
//     DisplayTab.h
// 
//  Abstract:
// 
//     Display counters on caption
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/27/2002] - created
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Injection.h"


// CDisplayTab dialog

class CDisplayTab : public CDialog
{
	DECLARE_DYNAMIC(CDisplayTab)

public:
	CDisplayTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplayTab();

// Dialog Data
	enum { IDD = IDD_DISPLAY_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	DECLARE_MESSAGE_MAP()
public:
	BOOL ShowHitpoints;
	BOOL ShowMana;
	BOOL ShowStamina;
	BOOL ShowAR;
	BOOL ShowWeight;
	BOOL ShowGold;
	BOOL ShowBandages;
	BOOL ShowArrows;
	BOOL ShowBolts;
	BOOL ShowHealPotions;
	BOOL ShowManaPotions;
	BOOL ShowCurePotions;
	BOOL ShowBM;
	BOOL ShowBP;
	BOOL ShowGL;
	BOOL ShowGS;
	BOOL ShowMR;
	BOOL ShowNS;
	BOOL ShowSA;
	BOOL ShowSS;
	BOOL ShowVAsh;
	BOOL ShowEon;
	BOOL ShowWHeart;
	BOOL ShowFDirt;
	BOOL ShowBMoor;
};

extern CDisplayTab *DisplayTab;
