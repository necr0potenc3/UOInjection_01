////////////////////////////////////////////////////////////////////////////////
//
// RegistryConstants.h
//
// Copyright (C) 2001 Wayne A.Hogue II 'Chiphead', (w_hogue@hotmail.com)
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

#ifndef REGISTRY_CONSTANTS_H
#define REGISTRY_CONSTANTS_H


////////////////////////////////////////////////////////////////////////////////
//
// This file contains all the constants used to access the configuration in the
// registry.
//
////////////////////////////////////////////////////////////////////////////////

// injection root key
#define HKEY_INJECTION_ROOT			"Software\\UOInjection"
#define HVAL_INSTALL_DIR_PATH		"InstallDir"			//STRING
#define HVAL_CLIENT_INSTALL_DIR_PATH	"ClientInstallDir"	//STRING
#define HVAL_KEYFILE_PATH			"KeyfilePath"		    //STRING
#define HVAL_PATCHFILE_PATH			"PatchfilePath"		    //STRING
#define HVAL_PLUGIN_DIR_PATH		"PluginDirPath"		    //STRING
#define HVAL_SCRIPT_DIR_PATH		"ScriptDirPath"		    //STRING
#define HVAL_CHECK_FOR_UPDATES		"CheckForUpdates"	    //BOOL
#define HVAL_HOME_URL				"HomeURL"				//STRING
#define HVAL_NEWS_URL				"NewsURL"				//STRING
#define HVAL_UPDATE_URL				"UpdateURL"				//STRING
#define HVAL_LAST_SERVER			"LastServer"			//STRING

#define HKEY_PLUGINS_KEY			"Plugins"
#define HVAL_PLUGIN_INSTALL_DIR		"InstallDir"			//STRING
#define HVAL_PLUGIN_DLL_NAME		"DLL"					//STRING
#define HVAL_PLUGIN_NAME			"Name"					//STRING
#define HVAL_PLUGIN_AUTHOR			"Author"				//STRING
#define HVAL_PLUGIN_DESCRIPTION		"Description"			//STRING
#define HVAL_PLUGIN_VERSION			"Version"				//STRING
#define HVAL_PLUGIN_DEPENDANCIES	"Dependacies"			//STRING

#define HKEY_RUNTIME_KEY				"Runtime"
#define HVAL_CURRENT_SERVER 			"CurrentServer"			//STRING
#define HVAL_CURRENT_ACCOUNT			"CurrentAccount"		//STRING
#define HVAL_CURRENT_SHARD 				"CurrentShard"			//STRING
#define HVAL_CURRENT_CHARACTER 			"CurrentCharacter"		//STRING
#define HVAL_CURRENT_CLIENT_CHECKSUM	"CurrentClientChecksum"	//DWORD
#define HVAL_CURRENT_CLIENT_LENGTH		"CurrentClientLength"	//DWORD

#define HKEY_SERVERS_KEY				"Servers"
#define HVAL_SERVER_CLIENT				"Client"				//STRING
#define HVAL_SERVER_ADDRESSES			"Addresses"				//MULTI STRING
#define HVAL_SERVER_REMEMBER_USERNAME	"RememberUsername"		//BOOL
#define HVAL_SERVER_USERNAME			"Username"				//STRING
#define HVAL_SERVER_REMEMBER_PASSWORD	"RememberPassword"		//BOOL
#define HVAL_SERVER_PASSWORD			"Password"				//STRING
#define HVAL_SERVER_USE_ALT_DATA_DIR	"UseAltDataDir"			//BOOL
#define HVAL_SERVER_LAUNCH_UOAM			"LaunchUOAM"			//BOOL
#define HVAL_SERVER_ALT_DATA_DIR_PATH	"AltDataDirPath"		//STRING
#define HVAL_SERVER_USE_INJECTION		"UseInjection"			//BOOL
#define HVAL_SERVER_EMULATION			"Emulation"				//STRING
#define HVAL_SERVER_FIX_TALK			"FixTalk"				//BOOL
#define HVAL_SERVER_FIX_WALK			"FixWalk"				//BOOL
#define HVAL_SERVER_PLUGINS				"Plugins"				//MULTI STRING

// UO root key
#define HKEY_UO_ROOT				"Software\\Origin Worlds Online\\Ultima Online\\1.0"
#define HVAL_UO_INSTALL_DIR			"ExePath"			//STRING

// UOAM keys to launch UOAM
#define HKEY_UOAM_ROOT				"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\UOAM"
#define HVAL_UOAM_INSTALL_DIR		"UninstallString"

#endif // REGISTRY_CONSTANTS_H