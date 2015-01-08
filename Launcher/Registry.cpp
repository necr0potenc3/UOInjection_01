// Registry.cpp: implementation of the CRegistry class.
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
#include "RegistryConstants.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
	int retCode;
	DWORD	dwDisposition;
	retCode = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT(HKEY_INJECTION_ROOT), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hkInjectionRoot, &dwDisposition);
	if(retCode != (DWORD)ERROR_SUCCESS)
		displayError("Error Opening Root Key");
}

CRegistry::~CRegistry()
{
	RegCloseKey(m_hkInjectionRoot);
}

////////////////////////////////////////////////////////////////////////
// Publics 
////////////////////////////////////////////////////////////////////////

CString CRegistry::getUOAMExecPath()
{
	CString sVal = "";
	HKEY hkUOAMRoot = 0;
	DWORD	dwDisposition;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT(HKEY_UOAM_ROOT), 0, NULL, 0, KEY_READ, NULL, &hkUOAMRoot, &dwDisposition);
	if(hkUOAMRoot != 0)
	{
		DWORD	dwDataSize = 0;
		DWORD	dwType = REG_SZ;
		RegQueryValueEx(hkUOAMRoot, TEXT(HVAL_UOAM_INSTALL_DIR), 0, &dwType, (PBYTE)NULL, &dwDataSize);
		if(RegQueryValueEx(hkUOAMRoot, TEXT(HVAL_UOAM_INSTALL_DIR), 0, &dwType, (PBYTE)(LPTSTR)sVal.GetBuffer(dwDataSize + 1), &dwDataSize) == ERROR_SUCCESS)
		{
			sVal.ReleaseBuffer();
			RegCloseKey(hkUOAMRoot);
			return sVal;
		}
		sVal.ReleaseBuffer();
		RegCloseKey(hkUOAMRoot);
	}
	RegCloseKey(hkUOAMRoot);
	return("");
}

void CRegistry::setUOInstallDir(CString sPath)
{
	setRoot(HVAL_CLIENT_INSTALL_DIR_PATH, sPath);
}

CString CRegistry::getUOInstallDir()
{
/*
	CString sVal = "";
	HKEY hkUORoot = 0;
	DWORD	dwDisposition;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT(HKEY_UO_ROOT), 0, NULL, 0, KEY_READ, NULL, &hkUORoot, &dwDisposition);
	if(hkUORoot != 0)
	{
		DWORD	dwDataSize = 0;
		DWORD	dwType = REG_SZ;
		RegQueryValueEx(hkUORoot, TEXT(HVAL_UO_INSTALL_DIR), 0, &dwType, (PBYTE)NULL, &dwDataSize);
		if(RegQueryValueEx(hkUORoot, TEXT(HVAL_UO_INSTALL_DIR), 0, &dwType, (PBYTE)(LPTSTR)sVal.GetBuffer(dwDataSize + 1), &dwDataSize) == ERROR_SUCCESS)
		{
			sVal.ReleaseBuffer();
			RegCloseKey(hkUORoot);
			int end = sVal.Find("\\client.exe");
			sVal.Delete(end, sVal.GetLength()-end);
			return sVal;
		}
		sVal.ReleaseBuffer();
		RegCloseKey(hkUORoot);
	}
	RegCloseKey(hkUORoot);
	return("");
*/
	return getRootString(HVAL_CLIENT_INSTALL_DIR_PATH);
}

CString CRegistry::getKeyFilePath()
{
	return getRootString(HVAL_KEYFILE_PATH);
}

CString CRegistry::getPatchFilePath()
{
	return getRootString(HVAL_PATCHFILE_PATH);
}

CString CRegistry::getInstallDirPath()
{
	return getRootString(HVAL_INSTALL_DIR_PATH);
}

CString CRegistry::getPluginDirPath(CString GUID)
{
	return getRootString(HVAL_PLUGIN_DIR_PATH);
}

CString CRegistry::getScriptDirPath()
{
	return getRootString(HVAL_SCRIPT_DIR_PATH);
}

CString CRegistry::getServer()
{
	return getCurrentServer();
}

CString CRegistry::getClient()
{
	return getServerClient(getCurrentServer());
}

DWORD CRegistry::getClientChecksum()
{
	return getDW(HKEY_RUNTIME_KEY, HVAL_CURRENT_CLIENT_CHECKSUM);
}

DWORD CRegistry::getClientLength()
{
	return getDW(HKEY_RUNTIME_KEY, HVAL_CURRENT_CLIENT_LENGTH);
}

BOOL CRegistry::getRememberUsername()
{
	return getServerRememberUsername(getCurrentServer());
}

CString CRegistry::getUsername()
{
	return getServerUsername(getCurrentServer());
}

BOOL CRegistry::getRememberPassword()
{
	return getServerRememberPassword(getCurrentServer());
}

CString CRegistry::getPassword()
{
	return getServerPassword(getCurrentServer());
}

BOOL CRegistry::getUseAltDataDir()
{
	return getServerUseAltDataDir(getCurrentServer());
}

BOOL CRegistry::getLaunchUOAM()
{
	return getServerLaunchUOAM(getCurrentServer());
}

CString CRegistry::getAltDataDir()
{
	return getServerAltDataDir(getCurrentServer());
}

BOOL CRegistry::getUseInjection()
{
	return getServerUseInjection(getCurrentServer());
}

CString CRegistry::getEmulation()
{
	return getServerEmulation(getCurrentServer());
}

BOOL CRegistry::getFixTalk()
{
	return getServerFixTalk(getCurrentServer());
}

BOOL CRegistry::getFixWalk()
{
	return getServerFixWalk(getCurrentServer());
}

void CRegistry::getAddresses(CStringArray & saReturnVal)
{
	getServerAddresses(getCurrentServer(), saReturnVal);
}

void CRegistry::setEmulation(CString sEmulation)
{
	setServerEmulation(getCurrentServer(), sEmulation);
}

void CRegistry::setFixTalk(BOOL b)
{
	setServerFixTalk(getCurrentServer(), b);
}

void CRegistry::setFixWalk(BOOL b)
{
	setServerFixWalk(getCurrentServer(), b);
}


////////////////////////////////////////////////////////////////////////
// privates
////////////////////////////////////////////////////////////////////////

CString CRegistry::getRootString(CString sValName)
{
	CString sVal = "";
	DWORD	dwDataSize = 0;
	DWORD	dwType = REG_SZ;
	RegQueryValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, &dwType, (PBYTE)NULL, &dwDataSize);
	RegQueryValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, &dwType, (PBYTE)(LPTSTR)sVal.GetBuffer(dwDataSize + 1), &dwDataSize);
	sVal.ReleaseBuffer();
	return(sVal);
}

void CRegistry::setRoot(CString sValName, CString sVal)
{
	DWORD	dwDataSize = sVal.GetLength()+1;
	DWORD	dwType = REG_SZ;
	RegSetValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, dwType, (PBYTE)((LPCTSTR)sVal), dwDataSize);
}

BOOL CRegistry::getRootBool(CString sValName)
{
	DWORD dwVal = FALSE;
	DWORD dwDataSize = sizeof(dwVal);
	DWORD dwType = REG_DWORD;
	RegQueryValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, &dwType, 0, &dwDataSize);
	RegQueryValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, &dwType, (LPBYTE)&dwVal, &dwDataSize);
	return dwVal;
}

void CRegistry::setRoot(CString sValName, BOOL b)
{
	DWORD dwVal = b;
	DWORD dwDataSize = sizeof(dwVal);
	DWORD dwType = REG_DWORD;
	RegSetValueEx(m_hkInjectionRoot, (LPCTSTR)sValName, 0, dwType, (LPBYTE)&dwVal, dwDataSize);
}

BOOL CRegistry::getBool(CString sKeyName, CString sValName)
{
	return getDW(sKeyName, sValName);
}

DWORD CRegistry::getDW(CString sKeyName, CString sValName)
{
	DWORD dwVal = 0;
	DWORD dwDataSize = sizeof(dwVal);
	DWORD dwType = REG_DWORD;
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, 0, &dwDataSize);
	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, (LPBYTE)&dwVal, &dwDataSize);
	RegCloseKey(hkKey);
	return dwVal;
}

void CRegistry::set(CString sKeyName, CString sValName, BOOL b)
{
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	DWORD dwVal = b;
	DWORD dwDataSize = sizeof(dwVal);
	DWORD dwType = REG_DWORD;
	RegSetValueEx(hkKey, (LPCTSTR)sValName, 0, dwType, (LPBYTE)&dwVal, dwDataSize);
	RegCloseKey(hkKey);
}

CString CRegistry::getString(CString sKeyName, CString sValName)
{
	CString sVal = "";
	DWORD	dwDataSize = 0;
	DWORD	dwType = REG_SZ;
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, (PBYTE)NULL, &dwDataSize);
	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, (PBYTE)(LPTSTR)sVal.GetBuffer(dwDataSize + 1), &dwDataSize);
	sVal.ReleaseBuffer();
	RegCloseKey(hkKey);
	return(sVal);
}

void CRegistry::setStringArray(CString sKeyName, CString sValName, const CStringArray & saVal)
{
	DWORD dwBufSize = 0;

	int nIndex = 0;
	int nNumStrings = saVal.GetSize();

	CString sTemp;

	while( nIndex < nNumStrings )
	{
		sTemp = saVal.GetAt( nIndex );
		dwBufSize += (( sTemp.GetLength() + 1 ) * sizeof( TCHAR ));
		nIndex++;
	}

	dwBufSize += sizeof( TCHAR );

	LPBYTE lpbBuffer = NULL;
   
	try
	{
		lpbBuffer = new BYTE[ dwBufSize ];
	}
	catch( ... )
	{
		lpbBuffer = NULL;
	}

	if ( lpbBuffer == NULL )
	{
		return;
	}
	ZeroMemory( lpbBuffer, dwBufSize );

	LPTSTR szString = (LPTSTR) lpbBuffer;

	nIndex = 0;
	int nStringLength = 0;

	while(nIndex < nNumStrings)
	{
		sTemp = saVal.GetAt( nIndex );
		_tcscpy(&szString[nStringLength], sTemp);
		nStringLength += ((sTemp.GetLength() + 1) * sizeof( TCHAR ));
		nIndex++;
	}

	nStringLength += sizeof( TCHAR );

	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	DWORD	dwType = REG_SZ;
	RegSetValueEx(hkKey, (LPCTSTR)sValName, 0, dwType, (PBYTE)lpbBuffer, dwBufSize);
	RegCloseKey(hkKey);

	delete [] lpbBuffer;

	return;
}

void CRegistry::getStringArray(CString sKeyName, CString sValName, CStringArray &saReturnVal)
{
	DWORD	dwDataSize = 0;
	DWORD	dwType = REG_MULTI_SZ;
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, (PBYTE)NULL, &dwDataSize);

	LPBYTE lpbBuffer = NULL;
 	try
	{
		lpbBuffer = new BYTE[dwDataSize];
	}
	catch( ... )
	{
		lpbBuffer = NULL;
	}

	if ( lpbBuffer == NULL )
	{
		return;
	}
	ZeroMemory( lpbBuffer, dwDataSize );

	RegQueryValueEx(hkKey, (LPCTSTR)sValName, 0, &dwType, (PBYTE)lpbBuffer, &dwDataSize);

	LPTSTR szString = (LPTSTR) lpbBuffer;

	saReturnVal.RemoveAll();
	while( szString[ 0 ] != 0x00 )
	{
		saReturnVal.Add((LPCTSTR)szString);
		szString += ( _tcslen((LPCTSTR)szString)+1); 
	}

	delete [] lpbBuffer;
	RegCloseKey(hkKey);
	return;
}

void CRegistry::set(CString sKeyName, CString sValName, CString sVal)
{
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	DWORD	dwDataSize = sVal.GetLength()+1;
	DWORD	dwType = REG_SZ;
	RegSetValueEx(hkKey, (LPCTSTR)sValName, 0, dwType, (PBYTE)((LPCTSTR)sVal), dwDataSize);
	RegCloseKey(hkKey);
}

void CRegistry::set(CString sKeyName, CString sValName, DWORD dwVal)
{
	DWORD dwDisposition;
	HKEY hkKey;
	RegCreateKeyEx(m_hkInjectionRoot, (LPCTSTR)sKeyName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkKey, &dwDisposition);
	DWORD	dwDataSize = sizeof(REG_DWORD);
	DWORD	dwType = REG_DWORD;
	RegSetValueEx(hkKey, (LPCTSTR)sValName, 0, dwType, (LPBYTE)&dwVal, dwDataSize);
	RegCloseKey(hkKey);
}

void CRegistry::displayError(CString sCaption)
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
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, sCaption, MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );

}

void CRegistry::recursiveDeleteKey(HKEY hkRootKey, CString sKeyToDelete)
{
	DWORD retCode;
	HKEY hkSubKey = 0;
	DWORD dwDisposition;
	retCode = RegCreateKeyEx(hkRootKey, (LPCTSTR)sKeyToDelete, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkSubKey, &dwDisposition);
	if(retCode == (DWORD)ERROR_SUCCESS)
	{
		int i;
		FILETIME ftLastWriteTime; 
		for (i = 0; retCode == (DWORD)ERROR_SUCCESS; i++) 
		{ 
			char achKey[MAX_PATH];
			DWORD dwKeyLen = MAX_PATH;
			retCode = RegEnumKeyEx(hkSubKey, i, achKey, &dwKeyLen, NULL, NULL, NULL, &ftLastWriteTime); 
			if (retCode == (DWORD)ERROR_SUCCESS) 
			{
				recursiveDeleteKey(hkSubKey, achKey);
			}
		}
	}
	RegCloseKey(hkSubKey);
	RegDeleteKey(hkRootKey, (LPCTSTR)sKeyToDelete);
}

void CRegistry::setServerList(CListBox *lbServers)
{
	lbServers->ResetContent();
	DWORD retCode;
	HKEY hkServers = 0;
	DWORD dwDisposition;
	retCode = RegCreateKeyEx(m_hkInjectionRoot, TEXT(HKEY_SERVERS_KEY), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkServers, &dwDisposition);
	if(retCode == (DWORD)ERROR_SUCCESS)
	{
		int i;
		FILETIME ftLastWriteTime; 
		for (i = 0; retCode == (DWORD)ERROR_SUCCESS; i++) 
		{ 
			char achKey[MAX_PATH];
			DWORD dwKeyLen = MAX_PATH;
			retCode = RegEnumKeyEx(hkServers, i, achKey, &dwKeyLen, NULL, NULL, NULL, &ftLastWriteTime); 
			if (retCode == (DWORD)ERROR_SUCCESS) 
			{
				lbServers->AddString(achKey);
			}
		}
	}
	else 
	{
		displayError("Error Opening Servers Key");
	}
	RegCloseKey(hkServers);
}


BOOL CRegistry::getCheckForUpdates()
{
	return getRootBool(HVAL_CHECK_FOR_UPDATES);
}

void CRegistry::setInstallDirPath(CString sPath)
{
	setRoot(HVAL_INSTALL_DIR_PATH, sPath);
}

void CRegistry::setKeyFilePath(CString sPath)
{
	setRoot(HVAL_KEYFILE_PATH, sPath);
}

void CRegistry::setPatchFilePath(CString sPath)
{
	setRoot(HVAL_PATCHFILE_PATH, sPath);
}

void CRegistry::setScriptDirPath(CString sPath)
{
	setRoot(HVAL_SCRIPT_DIR_PATH, sPath);
}

void CRegistry::setPluginDirPath(CString sPath)
{
	setRoot(HVAL_PLUGIN_DIR_PATH, sPath);
}


void CRegistry::setCheckForUpdates(BOOL bCheck)
{
	setRoot(HVAL_CHECK_FOR_UPDATES, bCheck);
}

CString CRegistry::getServerClient(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getString(sKey, HVAL_SERVER_CLIENT);
}

BOOL CRegistry::getServerRememberUsername(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_REMEMBER_USERNAME);
}


CString CRegistry::getServerUsername(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getString(sKey, HVAL_SERVER_USERNAME);
}


BOOL CRegistry::getServerRememberPassword(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_REMEMBER_PASSWORD);
}


CString CRegistry::getServerPassword(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getString(sKey, HVAL_SERVER_PASSWORD);
}


BOOL CRegistry::getServerUseAltDataDir(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_USE_ALT_DATA_DIR);
}


BOOL CRegistry::getServerLaunchUOAM(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_LAUNCH_UOAM);
}


CString CRegistry::getServerAltDataDir(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getString(sKey, HVAL_SERVER_ALT_DATA_DIR_PATH);
}


BOOL CRegistry::getServerUseInjection(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_USE_INJECTION);
}


CString CRegistry::getServerEmulation(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getString(sKey, HVAL_SERVER_EMULATION);
}


BOOL CRegistry::getServerFixTalk(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_FIX_TALK);
}


BOOL CRegistry::getServerFixWalk(CString sServerName)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	return getBool(sKey, HVAL_SERVER_FIX_WALK);
}



BOOL CRegistry::serverExists(CString sServerName)
{
	BOOL bExists = FALSE;
	DWORD retCode;
	HKEY hkServers = 0;
	DWORD dwDisposition;
	retCode = RegCreateKeyEx(m_hkInjectionRoot, TEXT(HKEY_SERVERS_KEY), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkServers, &dwDisposition);
	if(retCode == (DWORD)ERROR_SUCCESS)
	{
		int i;
		FILETIME ftLastWriteTime; 
		for (i = 0; retCode == (DWORD)ERROR_SUCCESS; i++) 
		{ 
			char achKey[MAX_PATH];
			DWORD dwKeyLen = MAX_PATH;
			retCode = RegEnumKeyEx(hkServers, i, achKey, &dwKeyLen, NULL, NULL, NULL, &ftLastWriteTime); 
			if (retCode == (DWORD)ERROR_SUCCESS) 
			{
				if(sServerName.CompareNoCase(achKey) == 0)
				{
					bExists = TRUE;
					RegCloseKey(hkServers);
					return bExists;
			
				}
			}
		}
	}
	RegCloseKey(hkServers);
	return bExists;
}

void CRegistry::addServer(CString sServerName)
{
	HKEY hkServers = 0;
	HKEY hkServer = 0;
	DWORD dwDisposition;
	RegCreateKeyEx(m_hkInjectionRoot, TEXT(HKEY_SERVERS_KEY), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkServers, &dwDisposition);
	RegCreateKeyEx(hkServers, (LPCTSTR)sServerName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkServer, &dwDisposition);
	RegCloseKey(hkServer);
	RegCloseKey(hkServers);
}

void CRegistry::setServerClient(CString sServerName, CString sClientPath)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_CLIENT, sClientPath);
}


void CRegistry::setServerRememberUsername(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_REMEMBER_USERNAME, b);
}


void CRegistry::setServerUsername(CString sServerName, CString sUsername)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_USERNAME, sUsername);
}


void CRegistry::setServerRememberPassword(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_REMEMBER_PASSWORD, b);
}


void CRegistry::setServerPassword(CString sServerName, CString sPassword)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_PASSWORD, sPassword);
}


void CRegistry::setServerUseAltDataDir(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_USE_ALT_DATA_DIR, b);
}


void CRegistry::setServerLaunchUOAM(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_LAUNCH_UOAM, b);
}


void CRegistry::setServerAltDataDir(CString sServerName, CString sAltDataDirPath)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_ALT_DATA_DIR_PATH, sAltDataDirPath);
}


void CRegistry::setServerUseInjection(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_USE_INJECTION, b);
}


void CRegistry::setServerEmulation(CString sServerName, CString sEmulation)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_EMULATION, sEmulation);
}


void CRegistry::setServerFixTalk(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_FIX_TALK, b);
}


void CRegistry::setServerFixWalk(CString sServerName, BOOL b)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	set(sKey, HVAL_SERVER_FIX_WALK, b);
}

void CRegistry::setServerAddresses(CString sServerName, const CStringArray & saVal)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	setStringArray(sKey, HVAL_SERVER_ADDRESSES, saVal);
}

void CRegistry::getServerAddresses(CString sServerName, CStringArray & saReturnVal)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	getStringArray(sKey, HVAL_SERVER_ADDRESSES, saReturnVal);
}

void CRegistry::setServerPlugins(CString sServerName, const CStringArray & saVal)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	setStringArray(sKey, HVAL_SERVER_PLUGINS, saVal);
}

void CRegistry::getServerPlugins(CString sServerName, CStringArray & saReturnVal)
{
	CString sKey = TEXT(HKEY_SERVERS_KEY);
	sKey += "\\" + sServerName;
	getStringArray(sKey, HVAL_SERVER_PLUGINS, saReturnVal);
}

void CRegistry::deleteServer(CString sServer)
{
	DWORD dwDisposition;
	HKEY hkRootKey;
	RegCreateKeyEx(m_hkInjectionRoot, TEXT(HKEY_SERVERS_KEY), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkRootKey, &dwDisposition);
	if(hkRootKey != 0)
	{
		recursiveDeleteKey(hkRootKey, sServer);
	}
}

CString CRegistry::getCurrentServer()
{
	return getString(HKEY_RUNTIME_KEY, HVAL_CURRENT_SERVER);
}

void CRegistry::setCurrentServer(CString sServerName)
{
	set(HKEY_RUNTIME_KEY, HVAL_CURRENT_SERVER, sServerName);
}

void CRegistry::setCurrenClientChecksum(DWORD dwChecksum)
{
	set(HKEY_RUNTIME_KEY, HVAL_CURRENT_CLIENT_CHECKSUM, dwChecksum);
}

void CRegistry::setCurrentClientLength(DWORD dwLength)
{
	set(HKEY_RUNTIME_KEY, HVAL_CURRENT_CLIENT_LENGTH, dwLength);
}

