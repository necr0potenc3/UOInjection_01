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
//     MsgSize.cpp
// 
//  Abstract:
// 
//     Determine the message size.
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
#include "Injection.h"
#include "api.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
 *	Table is taken from client 3.0.6a. If 0x8000 - variable len
 */
int MessageLength[] = {
	/*0x0000*/	0x0068,   //XCMD_Create           "Create Character" 
	/*0x0001*/	0x0005,   //                      "Disconnect"
	/*0x0002*/	0x0007,   //XCMD_Walk             "Walk Request" 
	/*0x0003*/	0x8000,   //XCMD_Talk             "Client Talk"
	/*0x0004*/	0x0002,   //                      
	/*0x0005*/	0x0005,   //XCMD_Attack           "Attack"
	/*0x0006*/	0x0005,   //XCMD_DClick           "Double Click"
	/*0x0007*/	0x0007,   //XCMD_ItemPickupReq    "Pick Up Item"
	/*0x0008*/	0x000E,   //XCMD_ItemDropReq      "Drop Item"
	/*0x0009*/	0x0005,   //XCMD_Click            "Single Click"
	/*0x000A*/	0x000B,   //                      
	/*0x000B*/	0x010A,   //                      
	/*0x000C*/	0x8000,   //                      
	/*0x000D*/	0x0003,   //                      
	/*0x000E*/	0x8000,   //                      
	/*0x000F*/	0x003D,   //                      
	/*0x0010*/	0x00D7,   // 0x10                 
	/*0x0011*/	0x8000,   //XCMD_Status           "Character Status"
	/*0x0012*/	0x8000,   //XCMD_ExtCmd           "Perform Action"
	/*0x0013*/	0x000A,   //XCMD_ItemEquipReq     "Client Equip Item"
	/*0x0014*/	0x0006,   //XCMD_GMToolMsg        
	/*0x0015*/	0x0009,   //XCMD_Follow           
	/*0x0016*/	0x0001,   //                      
	/*0x0017*/	0x8000,   //                      
	/*0x0018*/	0x8000,   //                      
	/*0x0019*/	0x8000,   //                      
	/*0x001A*/	0x8000,   //XCMD_Put              "Update Item"
	/*0x001B*/	0x0025,   //XCMD_Start            "Enter World"
	/*0x001C*/	0x8000,   //XCMD_Speak            "Server Talk"
	/*0x001D*/	0x0005,   //XCMD_Remove           "Delete Object"
	/*0x001E*/	0x0004,   //                      
	/*0x001F*/	0x0008,   //                      
	/*0x0020*/	0x0013,   //XCMD_View             "Update Player"
	/*0x0021*/	0x0008,   //XCMD_WalkCancel       "Deny Walk"
	/*0x0022*/	0x0003,   //XCMD_WalkAck          "Confirm Walk"
	/*0x0023*/	0x001A,   //XCMD_DragAnim         "Drag Animation"
	/*0x0024*/	0x0007,   //XCMD_ContOpen         "Open Container"
	/*0x0025*/	0x0014,   //XCMD_ContAdd          "Update Contained Item"
	/*0x0026*/	0x0005,   //XCMD_Kick             
	/*0x0027*/	0x0002,   //XCMD_DragCancel       "Deny Move Item"
	/*0x0028*/	0x0005,   //XCMD_ClearSquare      
	/*0x0029*/	0x0001,   //XCMD_Unk29            
	/*0x002A*/	0x0005,   //                      
	/*0x002B*/	0x0002,   //                      
	/*0x002C*/	0x0002,   //XCMD_DeathMenu        "Death Dialog"
	/*0x002D*/	0x0011,   //                      
	/*0x002E*/	0x000F,   //XCMD_ItemEquip        "Server Equip Item"
	/*0x002F*/	0x000A,   //XCMD_Fight            "Combat Notification"
	/*0x0030*/	0x0005,   // 0x30                 
	/*0x0031*/	0x0001,   //                      
	/*0x0032*/	0x0002,   //                      
	/*0x0033*/	0x0002,   //XCMD_Pause            "Pause Control"
	/*0x0034*/	0x000A,   //XCMD_CharStatReq      "Status Request"
	/*0x0035*/	0x028D,   //                      
	/*0x0036*/	0x8000,   //                      
	/*0x0037*/	0x0008,   //                      
	/*0x0038*/	0x0007,   //XCMD_PathFind         
	/*0x0039*/	0x0009,   //XCMD_ChangeGroup      
	/*0x003A*/	0x8000,   //XCMD_Skill            "Update Skills"
	/*0x003B*/	0x8000,   //XCMD_VendorBuy        "Vendor Buy Reply"
	/*0x003C*/	0x8000,   //XCMD_Content          "Update Contained Items"
	/*0x003D*/	0x0002,   //XCMD_Unk3d            
	/*0x003E*/	0x0025,   //                      
	/*0x003F*/	0x8000,   //                      
	/*0x0040*/	0x00C9,   // 0x40                 
	/*0x0041*/	0x8000,   //                      
	/*0x0042*/	0x8000,   //                      
	/*0x0043*/	0x0229,   //                      
	/*0x0044*/	0x02C9,   //                      
	/*0x0045*/	0x0005,   //                      
	/*0x0046*/	0x8000,   //                      
	/*0x0047*/	0x000B,   //                      
	/*0x0048*/	0x0049,   //                      
	/*0x0049*/	0x005D,   //                      
	/*0x004A*/	0x0005,   //                      
	/*0x004B*/	0x0009,   //                      
	/*0x004C*/	0x8000,   //                      
	/*0x004D*/	0x8000,   //                      
	/*0x004E*/	0x0006,   //XCMD_LightPoint       "Object Light Level"
	/*0x004F*/	0x0002,   //XCMD_Light            "Global Light Level"
	/*0x0050*/	0x8000,   // 0x50                 
	/*0x0051*/	0x8000,   //                      
	/*0x0052*/	0x8000,   //                      
	/*0x0053*/	0x0002,   //XCMD_IdleWarn         "Error Code"
	/*0x0054*/	0x000C,   //XCMD_Sound            "Sound Effect"
	/*0x0055*/	0x0001,   //XCMD_ReDrawAll        "Login Complete"
	/*0x0056*/	0x000B,   //XCMD_MapEdit          "Map Data"
	/*0x0057*/	0x006E,   //                      
	/*0x0058*/	0x006A,   //                      
	/*0x0059*/	0x8000,   //                      
	/*0x005A*/	0x8000,   //                      
	/*0x005B*/	0x0004,   //XCMD_Time             "Set Time?"
	/*0x005C*/	0x0002,   //                      
	/*0x005D*/	0x0049,   //XCMD_CharPlay         "Select Character"
	/*0x005E*/	0x8000,   //                      
	/*0x005F*/	0x0031,   //                      
	/*0x0060*/	0x0005,   // 0x60                 
	/*0x0061*/	0x0009,   //                      
	/*0x0062*/	0x000F,   //                      
	/*0x0063*/	0x000D,   //                      
	/*0x0064*/	0x0001,   //                      
	/*0x0065*/	0x0004,   //XCMD_Weather          "Set Weather"
	/*0x0066*/	0x8000,   //XCMD_BookPage         "Book Page Data"
	/*0x0067*/	0x0015,   //                      
	/*0x0068*/	0x8000,   //                      
	/*0x0069*/	0x8000,   //XCMD_Options          
	/*0x006A*/	0x0003,   //                      
	/*0x006B*/	0x0009,   //                      
	/*0x006C*/	0x0013,   //XCMD_Target           "Target Data"
	/*0x006D*/	0x0003,   //XCMD_PlayMusic        "Play Music"
	/*0x006E*/	0x000E,   //XCMD_CharAction       "Character Animation"
	/*0x006F*/	0x8000,   //XCMD_SecureTrade      "Secure Trading"
	/*0x0070*/	0x001C,   //XCMD_Effect           "Graphic Effect"
	/*0x0071*/	0x8000,   //XCMD_BBoard           "Message Board Data"
	/*0x0072*/	0x0005,   //XCMD_War              "War Mode"
	/*0x0073*/	0x0002,   //XCMD_Ping             "Ping"
	/*0x0074*/	0x8000,   //XCMD_VendOpenBuy      "Vendor Buy List"
	/*0x0075*/	0x0023,   //XCMD_CharName         "Rename Character"
	/*0x0076*/	0x0010,   //XCMD_ZoneChange       
	/*0x0077*/	0x0011,   //XCMD_CharMove         "Update Character"
	/*0x0078*/	0x8000,   //XCMD_Char             "Update Object"
	/*0x0079*/	0x0009,   //                      
	/*0x007A*/	0x8000,   //                      
	/*0x007B*/	0x0002,   //                      
	/*0x007C*/	0x8000,   //XCMD_MenuItems        "Open Menu Gump"
	/*0x007D*/	0x000D,   //XCMD_MenuChoice       "Menu Choice"
	/*0x007E*/	0x0002,   //                      
	/*0x007F*/	0x8000,   //                      
	/*0x0080*/	0x003E,   //XCMD_ServersReq       "First Login"
	/*0x0081*/	0x8000,   //XCMD_CharList3        
	/*0x0082*/	0x0002,   //XCMD_LogBad           "Login Error"
	/*0x0083*/	0x0027,   //XCMD_CharDelete       "Delete Character"
	/*0x0084*/	0x0045,   //                      
	/*0x0085*/	0x0002,   //XCMD_DeleteBad        
	/*0x0086*/	0x8000,   //XCMD_CharList2        "Character List 2"
	/*0x0087*/	0x8000,   //                      
	/*0x0088*/	0x0042,   //XCMD_PaperDoll        "Open Paperdoll"
	/*0x0089*/	0x8000,   //XCMD_CorpEquip        "Corpse Equipment"
	/*0x008A*/	0x8000,   //                      
	/*0x008B*/	0x8000,   //XCMD_GumpTextDisp     
	/*0x008C*/	0x000B,   //XCMD_Relay			  "Relay Server"
	/*0x008D*/	0x8000,   //                      
	/*0x008E*/	0x8000,   //                      
	/*0x008F*/	0x8000,   //                      
	/*0x0090*/	0x0013,   //XCMD_MapDisplay       "Display Map"
	/*0x0091*/	0x0041,   //XCMD_CharListReq      "Second Login"
	/*0x0092*/	0x8000,   //                      
	/*0x0093*/	0x0063,   //XCMD_BookOpen         "Open Book"
	/*0x0094*/	0x8000,   //                      
	/*0x0095*/	0x0009,   //XCMD_DyeVat           "Dye Data"
	/*0x0096*/	0x8000,   //                      
	/*0x0097*/	0x0002,   //XCMD_WalkForce        
	/*0x0098*/	0x8000,   //                      
	/*0x0099*/	0x001A,   //XCMD_TargetMulti      "Multi Placement"
	/*0x009A*/	0x8000,   //XCMD_Prompt           
	/*0x009B*/	0x0102,   //XCMD_HelpPage         "Help Request"
	/*0x009C*/	0x0135,   //                      
	/*0x009D*/	0x0033,   //                      
	/*0x009E*/	0x8000,   //XCMD_VendOpenSell     "Vendor Sell List"
	/*0x009F*/	0x8000,   //XCMD_VendorSell       "Vendor Sell Reply"
	/*0x00A0*/	0x0003,   //XCMD_ServerSelect     "Select Server"
	/*0x00A1*/	0x0009,   //XCMD_StatChngStr      "Update Hitpoints"
	/*0x00A2*/	0x0009,   //XCMD_StatChngInt      "Update Mana"
	/*0x00A3*/	0x0009,   //XCMD_StatChngDex      "Update Stamina"
	/*0x00A4*/	0x0095,   //XCMD_Spy              "System Information"
	/*0x00A5*/	0x8000,   //XCMD_Web              "Open URL"
	/*0x00A6*/	0x8000,   //XCMD_Scroll           "Tip Window"
	/*0x00A7*/	0x0004,   //XCMD_TipReq           "Request Tip"
	/*0x00A8*/	0x8000,   //XCMD_ServerList       "Server List"
	/*0x00A9*/	0x8000,   //XCMD_CharList         "Character List"
	/*0x00AA*/	0x0005,   //XCMD_AttackOK         "Attack Reply"
	/*0x00AB*/	0x8000,   //XCMD_GumpInputBox     "Text Input Dialog"
	/*0x00AC*/	0x8000,   //XCMD_GumpText         "Text Input Reply"
	/*0x00AD*/	0x8000,   //XCMD_TalkUNICODE      "Unicode Client Talk"
	/*0x00AE*/	0x8000,   //XCMD_SpeakUNICODE     "Unicode Server Talk"
	/*0x00AF*/	0x000D,   //XCMD_CharDeath        
	/*0x00B0*/	0x8000,   //XCMD_GumpDialog       "Open Dialog Gump"
	/*0x00B1*/	0x8000,   //XCMD_GumpButton       "Dialog Choice"
	/*0x00B2*/	0x8000,   //XCMD_ChatReq          "Chat Data"
	/*0x00B3*/	0x8000,   //XCMD_ChatText         "Chat Text ?"
	/*0x00B4*/	0x8000,   //XCMD_TargetItems      
	/*0x00B5*/	0x0040,   //XCMD_Chat             "Open Chat Window"
	/*0x00B6*/	0x0009,   //XCMD_ToolTipReq       "Popup Help Request"
	/*0x00B7*/	0x8000,   //XCMD_ToolTip          "Popup Help Data"
	/*0x00B8*/	0x8000,   //XCMD_CharProfile      "Character Profile"
	/*0x00B9*/	0x0003,   //XCMD_ChatEnable       "Chat Enable"
	/*0x00BA*/	0x0006,   //XCMD_Arrow            "Display Guidance Arrow"
	/*0x00BB*/	0x0009,   //XCMD_MailMsg          "Account ID ?"
	/*0x00BC*/	0x0003,   //XCMD_Unk_bc           "Season ?"
	/*0x00BD*/	0x8000,   //XCMD_ClientVersion    "Client Version"
	/*0x00BE*/	0x8000,   //be                    
	/*0x00BF*/	0x8000,   //XCMD_ExtData          "New Commands"
	/*0x00C0*/	0x0024,                           
	/*0x00C1*/	0x8000,   //XCMD_SpeakTable       "Display cliloc String"
	/*0x00C2*/	0x8000,                           
	/*0x00C3*/	0x8000,                           
	/*0x00C4*/	0x0006,   //c4                   
	/*0x00C5*/	0x00CB,
	/*0x00C6*/	0x0001,
	/*0x00C7*/	0x0031,
	/*0x00C8*/	0x0002,
	/*0x00C9*/	0x0006,
	/*0x00CA*/	0x0006,
	/*0x00CB*/	0x0007,
	/*0x00CC*/	0x8000,
	/*0x00CD*/	0x0001,
	/*0x00CE*/	0x8000,
	/*0x00CF*/	0x004E,
	/*0x00D0*/	0x8000,
	/*0x00D1*/	0x0002,
	/*0x00D2*/	0x0019,
	/*0x00D3*/	0x8000,
	/*0x00D4*/	0x8000
};

/*
 *	Return the length of a message contained at the start of a Buff
 *	Len can be less than the message length in the case of a partial packet
 */
int INJECTION_API GetMessageLength(const uint8 * IN Buff, int Len)
{
	int Msg=Buff[0]&255;
	if(Msg>sizeof(MessageLength)/sizeof(MessageLength[0]))
		return Len;
	int ML=MessageLength[Msg];
	if(ML>=0x8000)
	{
		// Determine sizes of all other messages
		if(Len<3)	// if message is too small - return minimal length to determine its size
			return 3;
		return Buff[2]+Buff[1]*256;
	}
	return ML;
}
