// UOKeys.cpp: implementation of the CUOKeys class.
//
//////////////////////////////////////////////////////////////////////
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
#include "UOKeys.h"
#include "registry.h"
#include "RegistryConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUOKeys::CUOKeys()
{

}

CUOKeys::~CUOKeys()
{

}

void CUOKeys::setEmulationList(CString sKeyPath, CComboBox *cbList)
{
	cbList->ResetContent();
//	cbList->AddString("None");	- not needed. It is present in keys.cfg
	// open key file
	FILE *fKeyFile;
	if((fKeyFile = fopen((LPCTSTR)sKeyPath, "rb")) == 0)
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
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error opening Key File", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );

		return;
	}
	CString sLine;
	while (fgets(sLine.GetBuffer(MAX_KEY_LENGTH+1), MAX_KEY_LENGTH, fKeyFile))
	{
		sLine.ReleaseBuffer();
		sLine.TrimLeft();
		if(sLine.Find('#')!=0)
		{
			int nStartOfLabel = sLine.Find('"');
			if(nStartOfLabel >= 0 )
			{
				int nEndOfLabel = sLine.Find('"', nStartOfLabel+1);
				if(nEndOfLabel > nStartOfLabel)
				{
					CString sLabel = sLine.Mid(nStartOfLabel+1, nEndOfLabel-nStartOfLabel-1);
					cbList->AddString((LPCTSTR)sLabel);
				}
		
			}
		}
	}
	sLine.ReleaseBuffer();
	fclose(fKeyFile);
}

void UpdateKeysInRegistry()
{
	CRegistry Reg;
	CString sKeyFile = Reg.getKeyFilePath();
	CString Server = Reg.getCurrentServer();
	CString Encryption=Reg.getServerEmulation(Server);

	// open key file
	FILE *fKeyFile;
	if((fKeyFile = fopen((LPCTSTR)sKeyFile, "rb")) == 0)
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
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error opening Key File", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );

		return;
	}
	CString sLine;
	while (fgets(sLine.GetBuffer(MAX_KEY_LENGTH+1), MAX_KEY_LENGTH, fKeyFile))
	{
		sLine.ReleaseBuffer();
		sLine.TrimLeft();
		if(sLine.Find('#')!=0)
		{
			int nStartOfLabel = sLine.Find('"');
			if(nStartOfLabel >= 0 )
			{
				int nEndOfLabel = sLine.Find('"', nStartOfLabel+1);
				if(nEndOfLabel > nStartOfLabel)
				{
					CString sLabel = sLine.Mid(nStartOfLabel+1, nEndOfLabel-nStartOfLabel-1);
					if(sLabel==Encryption)
					{
						const char *Str=((const char*)sLine)+nEndOfLabel+1;
						while(*Str && (*Str==' ' || *Str=='\t'))
							Str++;

						DWORD Key1, Key2, mode;
						sscanf(Str, "%8X %8X %d",&Key1,&Key2,&mode);
						Reg.set(CString(HKEY_SERVERS_KEY)+"\\"+Server,"LoginKey1",Key1);
						Reg.set(CString(HKEY_SERVERS_KEY)+"\\"+Server,"LoginKey2",Key2);
						Reg.set(CString(HKEY_SERVERS_KEY)+"\\"+Server,"GameCrypt",mode);
					}
				}
		
			}
		}
	}
	sLine.ReleaseBuffer();
	fclose(fKeyFile);
}
