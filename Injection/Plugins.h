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
//     Plugins.h
// 
//  Abstract:
// 
//     Plugin control finctions: load, init, cleanup, notify...
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/17/2002]	- source code cleanup
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "api.h"
#include "atlcoll.h"
#include "afxmt.h"
#include "sockshook.h"

class PluginInfo
{
public:
	CString		DllName;	// DLL name without path
	CString		PluginName;	// Short plugin description
	HINSTANCE	hInstance;	// hInstance to use in GetProcAddress
};

extern CList<PluginInfo,PluginInfo&> PluginList;

void LoadPluginDLLs();
bool RemoveEncryptionFromClientWithPlugins();
void UnloadPlugins();
bool InitEncryptionPlugin();

bool HandleMessage(uint8 *Buff, int &Len, bool FromServer);	// FromServer=true when message is coming from server

PluginInfo *GetPluginInfo();
void IdleMessage();

/*
 *	Callbacks structures
 */
enum CallbackType
{
	CB_Idle,			// OnIdle
	CB_EnterWorld,		// OnEnterWorld
	CB_ServerMessage,	// OnServerMessage
	CB_ClientMessage,	// OnClientMesage
	CB_Command			// installed command
};

// I use union to keep callbacks data to minimize memory use
union CallbackParameters
{
	struct DataFlow	// CB_ServerMessage/CB_ClientMessage callback parameters:
	{
		uint8 MessageBuffer[BUFFER_SIZE];	// Buffer to hold message
		int MessageLength;					// message len
		bool EatMessage;					// set to TRUE to suppress this message
	} Msg;
	struct CommandHandler
	{
		char Command[256];					// command name without ','
		char CommandLine[MAX_PATH];			// full command line with args
	} Cmd;
};

LRESULT SignalCallbacks(CallbackType Callback,CallbackParameters *CBParams);
LRESULT SignalCallbacksAsync(CallbackType Callback,CallbackParameters *CBParams);
