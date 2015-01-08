// Registry.h: interface for the CRegistry class.
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


#if !defined(AFX_REGISTRY_H__D9F07565_363D_46B4_96D4_3B0E5649E7D1__INCLUDED_)
#define AFX_REGISTRY_H__D9F07565_363D_46B4_96D4_3B0E5649E7D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry
{
public:
	// get the current server config stuff
	// these are the routines that should be used after the start of the launching
	// process for the lifetime of the current session.

	// return full path of client to load
	CString getClient();

	// return the checksum of the client
	DWORD getClientChecksum();

	// return the length of the client
	DWORD getClientLength();

	// return TRUE if we should remember the username
	BOOL getRememberUsername();

	// return the username,  empty string if RememberUsername is FALSE
	CString getUsername();

	// return TRUE if we should remember the password
	BOOL getRememberPassword();

	// return the password, empty string if RememberPassword is FALSE
	CString getPassword();

	// are we useing an Alt Data Directory?
	BOOL getUseAltDataDir();

	// should we launch UOAM?  Always FALSE if UseAltDataDirectory is FALSE
	BOOL getLaunchUOAM();

	// return the path to the AltDataDirectory
	CString getAltDataDir();

	// should we load injection?
	BOOL getUseInjection();

	// Emulation string.  This is used to look up keys in the UOKeys.cfg
	CString getEmulation();

	BOOL getFixTalk();
	BOOL getFixWalk();

	// returns a CString Array of addresses for this server
	void getAddresses(CStringArray & saRetuenVal);

	// returns a CString Array of plugins to load for this server
	void getPlugins(CStringArray & saReturnVal);

	// returns the full path of the plugin for loading. Empty if the plugin is not installed.
	CString getPluginPath(CString GUID);

	// return the install dir for the plugin.  Empty if the plugin is not installed.
	CString getPluginDirPath(CString GUID);

	// these can be changed by injection for the current server
	void setEmulation(CString sEmulation);
	void setFixTalk(BOOL b);
	void setFixWalk(BOOL b);

	// get the name of the server we are currently using
	CString getServer();

	// global settings

	// this will be removed and places in the script plugin
	CString getScriptDirPath();

	// return the install path of injection
	CString getInstallDirPath();

	// return the path to the patch file.  Prolly wont be needing this anymore
	CString getPatchFilePath();

	// return the path to the keys file
	CString getKeyFilePath();

	// return the path to the UO installation
	CString getUOInstallDir();

	// set the path to the UO installation
	void setUOInstallDir(CString s);

	// return the path to the UOAM installation
	CString getUOAMExecPath();

	CRegistry();
	virtual ~CRegistry();

//private:
	CString getCurrentServer();
	void setCurrentServer(CString sServerName);
	void setCurrenClientChecksum(DWORD dwChecksum);
	void setCurrentClientLength(DWORD dwLength);

	BOOL serverExists(CString sServerName);
	void addServer(CString sServerName);
	void deleteServer(CString sServer);

	// per server settings
	CString getServerClient(CString sServerName);
	BOOL getServerRememberUsername(CString sServerName);
	CString getServerUsername(CString sServerName);
	BOOL getServerRememberPassword(CString sServerName);
	CString getServerPassword(CString sServerName);
	BOOL getServerUseAltDataDir(CString sServerName);
	BOOL getServerLaunchUOAM(CString sServerName);
	CString getServerAltDataDir(CString sServerName);
	BOOL getServerUseInjection(CString sServerName);
	CString getServerEmulation(CString sServerName);
	BOOL getServerFixTalk(CString sServerName);
	BOOL getServerFixWalk(CString sServerName);
	void getServerAddresses(CString sServerName, CStringArray & saReturnVal);
	void getServerPlugins(CString sServerName, CStringArray & saReturnVal);

	void setServerClient(CString sServerName, CString sClientPath);
	void setServerRememberUsername(CString sServerName, BOOL b);
	void setServerUsername(CString sServerName, CString sUsername);
	void setServerRememberPassword(CString sServerName, BOOL b);
	void setServerPassword(CString sServerName, CString sPassword);
	void setServerUseAltDataDir(CString sServerName, BOOL b);
	void setServerLaunchUOAM(CString sServerName, BOOL b);
	void setServerAltDataDir(CString sServerName, CString sAltDataDirPath);
	void setServerUseInjection(CString sServerName, BOOL b);
	void setServerEmulation(CString sServername, CString sEmulation);
	void setServerFixTalk(CString sServerName, BOOL b);
	void setServerFixWalk(CString sServerName, BOOL b);
	void setServerAddresses(CString sServerName, const CStringArray & saVal);
	void setServerPlugins(CString sServerName, const CStringArray & saVal);

	// global settings
	void setCheckForUpdates(BOOL bCheck);
	BOOL getCheckForUpdates();
	void setScriptDirPath(CString sPath);
	void setPluginDirPath(CString sPath);
	void setPatchFilePath(CString sPath);
	void setKeyFilePath(CString sPath);
	void setInstallDirPath(CString sPath);
	void setServerList(CListBox *lbServers);

	void recursiveDeleteKey(HKEY hkRootKey, CString sKeyToDelete);
	HKEY m_hkInjectionRoot;
	void displayError(CString sCaption);
	BOOL getBool(CString sKeyName, CString sValName);
	DWORD getDW(CString sKeyName, CString sValName);
	CString getString(CString sKeyName, CString sValName);
	void set(CString sKeyName, CString sValName, CString sVal);
	void set(CString sKeyName, CString sValName, BOOL b);
	void set(CString sKeyName, CString sValName, DWORD dwVal);
	void setStringArray(CString sKeyName, CString sValName, const CStringArray & saVal);
	void getStringArray(CString sKeyName, CString sValName, CStringArray & saReturnVal);
	BOOL getRootBool(CString sValName);
	CString getRootString(CString sValName);
	void setRoot(CString sValName, CString sVal);
	void setRoot(CString sValName, BOOL b);

	friend class CAdvancedSettingsDlg;
	friend class CEditAddressDlg;
	friend class CEditServerDgl;
	friend class CUOInjectionApp;
	friend class CUOInjectionDlg;

};


#endif // !defined(AFX_REGISTRY_H__D9F07565_363D_46B4_96D4_3B0E5649E7D1__INCLUDED_)
