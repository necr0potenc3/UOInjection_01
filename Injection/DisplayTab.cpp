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
//     DisplayTab.cpp
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

#include "stdafx.h"
#include "Injection.h"
#include "DisplayTab.h"
#include "api.h"
#include "WndProcHook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDisplayTab *DisplayTab=0;

// CDisplayTab dialog

IMPLEMENT_DYNAMIC(CDisplayTab, CDialog)
CDisplayTab::CDisplayTab(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayTab::IDD, pParent)
	, ShowHitpoints(TRUE)
	, ShowMana(TRUE)
	, ShowStamina(TRUE)
	, ShowAR(FALSE)
	, ShowWeight(FALSE)
	, ShowGold(FALSE)
	, ShowBandages(FALSE)
	, ShowArrows(FALSE)
	, ShowBolts(FALSE)
	, ShowHealPotions(FALSE)
	, ShowManaPotions(FALSE)
	, ShowCurePotions(FALSE)
	, ShowBM(FALSE)
	, ShowBP(FALSE)
	, ShowGL(FALSE)
	, ShowGS(FALSE)
	, ShowMR(FALSE)
	, ShowNS(FALSE)
	, ShowSA(FALSE)
	, ShowSS(FALSE)
	, ShowVAsh(FALSE)
	, ShowEon(FALSE)
	, ShowWHeart(FALSE)
	, ShowFDirt(FALSE)
	, ShowBMoor(FALSE)
{
	DisplayTab = this;
	ShowHitpoints = GetRegistryDword("","ShowHitpoints",TRUE);
	ShowMana = GetRegistryDword("","ShowMana",TRUE);
	ShowStamina = GetRegistryDword("","ShowStamina",TRUE);
	ShowAR = GetRegistryDword("","ShowAR",TRUE);
	ShowWeight = GetRegistryDword("","ShowWeight",TRUE);
	ShowGold = GetRegistryDword("","ShowGold",TRUE);
	ShowBandages = GetRegistryDword("","ShowBandages");
	ShowArrows = GetRegistryDword("","ShowArrows",TRUE);
	ShowBolts = GetRegistryDword("","ShowBolts",TRUE);
	ShowHealPotions = GetRegistryDword("","ShowHealPotions");
	ShowManaPotions = GetRegistryDword("","ShowManaPotions");
	ShowCurePotions = GetRegistryDword("","ShowCurePotions");
	ShowBM = GetRegistryDword("","ShowBM");
	ShowBP = GetRegistryDword("","ShowBP");
	ShowGL = GetRegistryDword("","ShowGL");
	ShowGS = GetRegistryDword("","ShowGS");
	ShowMR = GetRegistryDword("","ShowMR");
	ShowNS = GetRegistryDword("","ShowNS");
	ShowSA = GetRegistryDword("","ShowSA");
	ShowSS = GetRegistryDword("","ShowSS");
	ShowVAsh = GetRegistryDword("","ShowVAsh");
	ShowEon = GetRegistryDword("","ShowEon");
	ShowWHeart = GetRegistryDword("","ShowWHeart");
	ShowFDirt = GetRegistryDword("","ShowFDirt");
	ShowBMoor = GetRegistryDword("","ShowBMoor");

	if(!Create(CDisplayTab::IDD,pParent))
		error_printf("Error in CDisplayTab constructor!\n");
}

CDisplayTab::~CDisplayTab()
{
	SetRegistryDword("","ShowHitpoints",ShowHitpoints);
	SetRegistryDword("","ShowMana",ShowMana);
	SetRegistryDword("","ShowStamina",ShowStamina);
	SetRegistryDword("","ShowAR",ShowAR);
	SetRegistryDword("","ShowWeight",ShowWeight);
	SetRegistryDword("","ShowGold",ShowGold);
	SetRegistryDword("","ShowBandages",ShowBandages);
	SetRegistryDword("","ShowArrows",ShowArrows);
	SetRegistryDword("","ShowBolts",ShowBolts);
	SetRegistryDword("","ShowHealPotions",ShowHealPotions);
	SetRegistryDword("","ShowManaPotions",ShowManaPotions);
	SetRegistryDword("","ShowCurePotions",ShowCurePotions);
	SetRegistryDword("","ShowBM",ShowBM);
	SetRegistryDword("","ShowBP",ShowBP);
	SetRegistryDword("","ShowGL",ShowGL);
	SetRegistryDword("","ShowGS",ShowGS);
	SetRegistryDword("","ShowMR",ShowMR);
	SetRegistryDword("","ShowNS",ShowNS);
	SetRegistryDword("","ShowSA",ShowSA);
	SetRegistryDword("","ShowSS",ShowSS);
	SetRegistryDword("","ShowVAsh",ShowVAsh);
	SetRegistryDword("","ShowEon",ShowEon);
	SetRegistryDword("","ShowWHeart",ShowWHeart);
	SetRegistryDword("","ShowFDirt",ShowFDirt);
	SetRegistryDword("","ShowBMoor",ShowBMoor);
	DisplayTab = 0;
}

void CDisplayTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, ShowHitpoints);
	DDX_Check(pDX, IDC_CHECK2, ShowMana);
	DDX_Check(pDX, IDC_CHECK3, ShowStamina);
	DDX_Check(pDX, IDC_CHECK5, ShowAR);
	DDX_Check(pDX, IDC_CHECK6, ShowWeight);
	DDX_Check(pDX, IDC_CHECK7, ShowGold);
	DDX_Check(pDX, IDC_CHECK8, ShowBandages);
	DDX_Check(pDX, IDC_CHECK9, ShowArrows);
	DDX_Check(pDX, IDC_CHECK10, ShowBolts);
	DDX_Check(pDX, IDC_CHECK11, ShowHealPotions);
	DDX_Check(pDX, IDC_CHECK12, ShowManaPotions);
	DDX_Check(pDX, IDC_CHECK13, ShowCurePotions);
	DDX_Check(pDX, IDC_CHECK14, ShowBM);
	DDX_Check(pDX, IDC_CHECK15, ShowBP);
	DDX_Check(pDX, IDC_CHECK16, ShowGL);
	DDX_Check(pDX, IDC_CHECK17, ShowGS);
	DDX_Check(pDX, IDC_CHECK18, ShowMR);
	DDX_Check(pDX, IDC_CHECK19, ShowNS);
	DDX_Check(pDX, IDC_CHECK20, ShowSA);
	DDX_Check(pDX, IDC_CHECK21, ShowSS);
	DDX_Check(pDX, IDC_CHECK22, ShowVAsh);
	DDX_Check(pDX, IDC_CHECK23, ShowEon);
	DDX_Check(pDX, IDC_CHECK24, ShowWHeart);
	DDX_Check(pDX, IDC_CHECK25, ShowFDirt);
	DDX_Check(pDX, IDC_CHECK26, ShowBMoor);
}


BEGIN_MESSAGE_MAP(CDisplayTab, CDialog)
END_MESSAGE_MAP()


// CDisplayTab message handlers

/*
 *	Handle all notification messages in one place.
 */
BOOL CDisplayTab::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// update variables
	UpdateData();
	UpdateClientCaption();
	return CDialog::OnCmdMsg(nID,nCode,pExtra,pHandlerInfo);
}
