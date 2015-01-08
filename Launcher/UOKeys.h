// UOKeys.h: interface for the CUOKeys class.
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
// Copyright (C) 2001 Wayne A. Hogue II 'Chiphead' (w_hogue@hotmail.com)
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

#if !defined(AFX_UOKEYS_H__D72F0029_4918_4125_B65B_02C395BEA339__INCLUDED_)
#define AFX_UOKEYS_H__D72F0029_4918_4125_B65B_02C395BEA339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_KEY_LENGTH 80

class CUOKeys  
{
public:
	void setEmulationList(CString sKeyPath, CComboBox * cbList);
	CUOKeys();
	virtual ~CUOKeys();

};

void UpdateKeysInRegistry();

#endif // !defined(AFX_UOKEYS_H__D72F0029_4918_4125_B65B_02C395BEA339__INCLUDED_)
