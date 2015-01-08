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
//     defcmd.cpp
// 
//  Abstract:
// 
//     Injection default message hooks and default comands implementation.
//	   Not all headers can be included in this file due to the API.H tweak 
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

#include "stdafx.h"

#undef INJECTION_DLL
#define INJECTION_API __declspec(dllexport) __cdecl
#include "api.h"

#include "defcmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
 *	Global variables
 */

static char CharacterName[128];	// char name
DWORD CharacterSerial;	// char serial No


/*
 *	Functions
 */

void CleanupWorld();

void INJECTION_CALLBACK OnIdle();
bool INJECTION_CALLBACK OnCharSelect(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnTalk(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnTalkUnicode(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnEnterWorld(const uint8 IN *Msg, int IN Len);

bool INJECTION_CALLBACK OnCharStatus(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateItem(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnDeleteItem(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateCreature(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnOpenContainer(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnAddToContainer(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnClearSquare(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnEquipItem(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateSkills(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateContainer(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdatePlayerPos(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdatePlayer(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateHealth(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateMana(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnUpdateStamina(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnWarPeace(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnPaperdoll(const uint8 IN *Msg, int IN Len);

bool INJECTION_CALLBACK OnTarget(const uint8 IN *Msg, int IN Len);
bool INJECTION_CALLBACK OnServerTarget(const uint8 IN *Msg, int IN Len);

void INJECTION_CALLBACK PerformActionsOnEnteringWorld();

void INJECTION_CALLBACK CmdVersion(int IN argc, char IN **argv);
void INJECTION_CALLBACK CmdHelp(int IN argc, char IN **argv);
void INJECTION_CALLBACK CmdDebug(int IN argc, char IN **argv);
void INJECTION_CALLBACK CmdInfo(int IN argc, char IN **argv);
void INJECTION_CALLBACK CmdUseObject(int IN argc, char IN **argv);
void INJECTION_CALLBACK CmdUseType(int IN argc, char IN **argv);

void PLUGIN_API GetPluginName(char OUT *Buffer, int IN BuffSize)
{
	strncpy(Buffer,"Injection default commands",BuffSize);
	Buffer[BuffSize-1]=0;
}

BOOL PLUGIN_API InitPlugin()
{
	// Hook events:

	RegisterOnIdle(OnIdle);	// idle

	RegisterOnServerMessage(0x1b,OnEnterWorld);	// Collect necessary information on char

	RegisterOnClientMessage(0x5d,OnCharSelect);	// Store the character name

	RegisterOnClientMessage(0x3,OnTalk);
	RegisterOnClientMessage(0xad,OnTalkUnicode);

// in-world updates handlers
	RegisterOnServerMessage(0x11,OnCharStatus);
	RegisterOnServerMessage(0x1A,OnUpdateItem);
	RegisterOnServerMessage(0x1D,OnDeleteItem);
	RegisterOnServerMessage(0x20,OnUpdateCreature);
	RegisterOnServerMessage(0x24,OnOpenContainer);
	RegisterOnServerMessage(0x25,OnAddToContainer);
	RegisterOnServerMessage(0x28,OnClearSquare);
	RegisterOnServerMessage(0x2e,OnEquipItem);
	RegisterOnServerMessage(0x3a,OnUpdateSkills);
	RegisterOnServerMessage(0x3c,OnUpdateContainer);
	RegisterOnServerMessage(0x77,OnUpdatePlayerPos);
	RegisterOnServerMessage(0x78,OnUpdatePlayer);
	RegisterOnServerMessage(0xa1,OnUpdateHealth);
	RegisterOnServerMessage(0xa2,OnUpdateMana);
	RegisterOnServerMessage(0xa3,OnUpdateStamina);
	RegisterOnServerMessage(0x72,OnWarPeace);
	RegisterOnServerMessage(0x88,OnPaperdoll);

// high level API support
	RegisterOnClientMessage(0x6c,OnTarget);
	RegisterOnServerMessage(0x6c,OnServerTarget);

//
	RegisterOnEnterWorld(PerformActionsOnEnteringWorld);

	RegisterCommand("version",CmdVersion);
	RegisterCommand("help",CmdHelp);
	RegisterCommand("debug",CmdDebug);

	RegisterCommand("useobject",CmdUseObject);
	RegisterCommand("usetype",CmdUseType);

	RegisterCommand("info",CmdInfo);
	return TRUE;	
}

void PLUGIN_API CleanupPlugin()
{
}

void INJECTION_CALLBACK OnIdle()
{
	static LastCleanupTime=0;
	LastCleanupTime++;
	if(LastCleanupTime>60*5*15)
	{
		// cleanup the world from the 'old' items once per 15 min
		CleanupWorld();	
	}
}

bool INJECTION_CALLBACK OnTalk(const uint8 IN *Msg, int IN Len)
{
	if(Msg[8]==COMMAND_PREFIX)
	{
		ExecuteCommandAsync((char*)Msg+9);
		return true;
	}
	return false;
}

bool INJECTION_CALLBACK OnTalkUnicode(const uint8 IN *Msg, int IN Len)
{
	if((Msg[3]&0xc0)==0xc0)
	{
		if(Msg[15]==COMMAND_PREFIX)
		{
			ExecuteCommandAsync((char*)Msg+15);
			return true;
		}
	} else 
	{
		WCHAR Buff[1024];
		char Text[512];
		if(Len>1024-12)
			Len=1024-12;
		memcpy(Buff,Msg+12,Len-12);
		int StrLen=(Len-12)/2;
		// Swap bytes
		for(int i=0; i<StrLen; i++)
			Buff[i]=ntohs(Buff[i]);
		WideCharToMultiByte(CP_ACP,0,Buff,StrLen,Text,512,0,0);

		if(Text[0]==COMMAND_PREFIX)
		{
			ExecuteCommandAsync(Text+1);
			return true;
		}
	}
	return false;
}

bool INJECTION_CALLBACK OnCharSelect(const uint8 IN *Msg, int IN Len)
{
	strncpy(CharacterName,(char*)Msg+5,Len-10);
	CharacterName[Len-11]=0;
	trace_printf("Selected character '%s'\n",CharacterName);
	return false;
}

void INJECTION_API GetCharacterName(char IN *Buff, int BuffSize)
{
	if(BuffSize>128)
		BuffSize=128;
	strncpy(Buff,CharacterName,BuffSize);
	Buff[BuffSize-1]=0;
}

void INJECTION_CALLBACK CmdVersion(int IN argc, char IN **argv)
{
	ClientPrint("UOInjection version blablabla...");
}

void INJECTION_CALLBACK CmdHelp(int IN argc, char IN **argv)
{
	ClientPrint("UOInjection help...");
}


/*
 *	place here predefined actions that should be taken on entering the world
 */
void INJECTION_CALLBACK PerformActionsOnEnteringWorld()
{
	InitWorld();	// initialize the world

	// request the character status
	uint8 Buff[]="\x34\xed\xed\xed\xed\x04\x00\x00\x8c\xd9";
	*((DWORD*)(Buff+6))=htonl(GetPlayerSerial());
	SendToServer(Buff,10);
}

void INJECTION_CALLBACK CmdUseObject(int IN argc, char IN **argv)
{
	if(argc!=2)
	{
		ClientPrint("Usage: useobject [object]");
		ClientPrint("Where object can be a serial in a form '0x1234' or a registered object name");
		return;
	}
	uint32 Ser=GetObjectSerial(argv[1]);
	if(Ser==INVALID_SERIAL)
		ClientPrintWarning("Unrecognized object '%s'",argv[1]);
	else
		UseObject(Ser);
}

void INJECTION_CALLBACK CmdUseType(int IN argc, char IN **argv)
{
	if(argc==1)
	{
		ClientPrint("Usage: usetype [object_type [object_color]]");
		ClientPrint("Where object_type can be a constant in a form '0x1234' or a registered type name");
		ClientPrint("and optional object_color is an object color");
		return;
	}
	uint16 Typ=GetObjectGraphic(argv[1]);
	uint16 Color=0;
	if(argc>2)
		Color=GetObjectColor(argv[2]);

	if(!UseType(Typ,Color))
		ClientPrintWarning("Unrecognized type '%s'",argv[1]);
}
