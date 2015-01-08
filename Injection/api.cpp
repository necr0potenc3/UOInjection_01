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
//     api.cpp
// 
//  Abstract:
// 
//     Exported API functions not going to any categories
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
#include "api.h"
#include "common.h"
#include "RegistryConstants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void INJECTION_API UnhandledException()
{
	error_printf("Unhandled exception in injection!\n");
#ifdef _DEBUG
	if(IDOK==MessageBox(0,"Unhandled exception. Press OK to debug.",0,MB_ICONSTOP|MB_OKCANCEL))
		DebugBreak();
#endif
}


HINSTANCE INJECTION_API GetInjectionDllBase()
{
	return afxCurrentInstanceHandle;
}


BOOL INJECTION_API GetRegistryString(const char IN *KeyName, const char IN *SubKey,
									 char OUT *Value, int IN cbValue)
{
	BOOL Ret=FALSE;
	FUNCTION_PROLOGUE 

	HKEY InjectionRoot;
    int retCode;
    DWORD   dwDisposition;
    retCode = RegCreateKeyEx(HKEY_CURRENT_USER, CString(HKEY_INJECTION_ROOT)+"\\"+KeyName, 0,
		NULL, 0, KEY_ALL_ACCESS, NULL, &InjectionRoot, &dwDisposition);
		
	Ret=(ERROR_SUCCESS==(RegQueryValueEx(InjectionRoot,SubKey,0,0,
		(LPBYTE)Value,(LPDWORD)&cbValue)));

	RegCloseKey(InjectionRoot);

	FUNCTION_EPILOGUE 

	return Ret;
}

BOOL INJECTION_API GetRegistryDword(const char IN *KeyName, const char IN *SubKey,
									 DWORD OUT *Value)
{
	BOOL Ret=FALSE;
	FUNCTION_PROLOGUE 
	DWORD cbValue=4;

	HKEY InjectionRoot;
    int retCode;
    DWORD   dwDisposition;
    retCode = RegCreateKeyEx(HKEY_CURRENT_USER, CString(HKEY_INJECTION_ROOT)+"\\"+KeyName, 0,
		NULL, 0, KEY_ALL_ACCESS, NULL, &InjectionRoot, &dwDisposition);
		
	Ret=(ERROR_SUCCESS==RegQueryValueEx(InjectionRoot,SubKey,0,0,
		(LPBYTE)Value,&cbValue));

	RegCloseKey(InjectionRoot);

	FUNCTION_EPILOGUE 

	return Ret;
}

void ClientPrintVCF(int color, int font, const char * IN format, va_list ap)
{
	char Buff[1024];
	memset(Buff,0,1024);
	vsprintf(Buff+0x2c,format,ap);
	int Len=strlen(Buff+0x2c)+1;
	Buff[0]=0x1c;
	Buff[1]=(Len+0x2c)/256;
	Buff[2]=(Len+0x2c)%256;
	Buff[0xa]=color/256;
	Buff[0xb]=color%256;
	Buff[0xd]=font;
	strcpy(Buff+0xe,"Injection");
	SendToClient(Buff,Len+0x2c);
}

void INJECTION_API ClientPrintColor(int color, int font, const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientPrintVCF(color, font, format, arg);
    va_end(arg);
}

void INJECTION_API ClientPrint(const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientPrintVCF(0x3e, 3, format, arg);
    va_end(arg);
}

void INJECTION_API ClientPrintWarning(const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientPrintVCF(0x9b, 3, format, arg);
    va_end(arg);
}

void INJECTION_API ClientPrintError(const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientPrintVCF(0x25, 0, format, arg);
    va_end(arg);
}

void ClientTalkVCF(int color, const char * IN format, va_list ap)
{
	bool UnicodeTalk=false;
	char Buff[1024];
	vsprintf(Buff,format,ap);

// If text contains not only latin chars - send it as unicode
	for(unsigned int i=0; i<strlen(Buff); i++)
	{
		if(Buff[i]&0x80)
			UnicodeTalk=true;
	}

	if(UnicodeTalk)
	{
		WCHAR Text[1024];
		int StrLen=strlen(Buff);
		MultiByteToWideChar(CP_ACP,0,Buff,StrLen,Text,1024);
		for(int i=0; i<StrLen; i++)
			Text[i]=htons(Text[i]);

		Buff[0]=(char)0xad;
		Buff[1]=(StrLen*2+14)/256;
		Buff[2]=(StrLen*2+14)%256;
		Buff[3]=0;
		Buff[4]=color/256;
		Buff[5]=color%256;
		Buff[6]=0;
		Buff[7]=3;
		strcpy(Buff+8,"HEZ");	// dummy code page :)
		memcpy(Buff+12,Text,StrLen*2);
		Buff[StrLen*2+12]=0;
		Buff[StrLen*2+13]=0;

		SendToServer(Buff,StrLen*2+14);
		return;
	}
// Send latin text as normal, so old dumb servers will understand it

	int Len=strlen(Buff);
	memmove(Buff+8,Buff,Len+1);
	Buff[0]=0x3;
	Buff[1]=(Len+8)/256;
	Buff[2]=(Len+8)%256;
	Buff[0x3]=0;
	Buff[0x4]=color/256;
	Buff[0x5]=color%256;
	Buff[0x6]=0;
	Buff[0x7]=3;
	SendToServer(Buff,Len+8+1);
}


void INJECTION_API ClientTalk(const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientTalkVCF(0x2b2, format, arg);
    va_end(arg);
}

void INJECTION_API ClientTalkColor(int color, const char * IN format, ...)
{
    va_list arg;
    va_start(arg, format);
    ClientTalkVCF(color, format, arg);
    va_end(arg);
}


BOOL INJECTION_API SetRegistryString(const char IN *KeyName, const char IN *SubKey,
									 char IN *Value)
{
	BOOL Ret=FALSE;
	FUNCTION_PROLOGUE 

	HKEY InjectionRoot;
    int retCode;
    DWORD   dwDisposition;
    retCode = RegCreateKeyEx(HKEY_CURRENT_USER, CString(HKEY_INJECTION_ROOT)+"\\"+KeyName, 0,
		NULL, 0, KEY_ALL_ACCESS, NULL, &InjectionRoot, &dwDisposition);
		
	Ret=(ERROR_SUCCESS==(RegSetValueEx(InjectionRoot,SubKey,0,REG_SZ,
		(LPBYTE)Value,strlen(Value))));

	RegCloseKey(InjectionRoot);

	FUNCTION_EPILOGUE 

	return Ret;
}

BOOL INJECTION_API SetRegistryDword(const char IN *KeyName, const char IN *SubKey,
									 DWORD IN Value)
{
	BOOL Ret=FALSE;
	FUNCTION_PROLOGUE 
	DWORD cbValue=4;

	HKEY InjectionRoot;
    int retCode;
    DWORD   dwDisposition;
    retCode = RegCreateKeyEx(HKEY_CURRENT_USER, CString(HKEY_INJECTION_ROOT)+"\\"+KeyName, 0,
		NULL, 0, KEY_ALL_ACCESS, NULL, &InjectionRoot, &dwDisposition);
		
	Ret=(ERROR_SUCCESS==RegSetValueEx(InjectionRoot,SubKey,0,REG_DWORD,
		(LPBYTE)&Value,4));

	RegCloseKey(InjectionRoot);

	FUNCTION_EPILOGUE 

	return Ret;
}

BOOL INJECTION_API GetShardRegistryString(const char IN *SubKey,
									 char OUT *Value, int IN cbValue)
{
	char Shard[128]=HKEY_SERVERS_KEY "\\";
	GetRegistryString(HKEY_RUNTIME_KEY,HVAL_CURRENT_SERVER,Shard+strlen(Shard),128);
	return GetRegistryString(Shard,SubKey,Value,cbValue);
}

BOOL INJECTION_API GetShardRegistryDword(const char IN *SubKey,
									 DWORD OUT *Value)
{
	char Shard[128]=HKEY_SERVERS_KEY "\\";
	GetRegistryString(HKEY_RUNTIME_KEY,HVAL_CURRENT_SERVER,Shard+strlen(Shard),128);
	return GetRegistryDword(Shard,SubKey,Value);
}

BOOL INJECTION_API SetShardRegistryString(const char IN *SubKey,
									 char IN *Value)
{
	char Shard[128]=HKEY_SERVERS_KEY "\\";
	GetRegistryString(HKEY_RUNTIME_KEY,HVAL_CURRENT_SERVER,Shard+strlen(Shard),128);
	return SetRegistryString(Shard,SubKey,Value);
}

BOOL INJECTION_API SetShardRegistryDword(const char IN *SubKey,
									DWORD IN Value)
{
	char Shard[128]=HKEY_SERVERS_KEY "\\";
	GetRegistryString(HKEY_RUNTIME_KEY,HVAL_CURRENT_SERVER,Shard+strlen(Shard),128);
	return SetRegistryDword(Shard,SubKey,Value);
}

void INJECTION_API WarMode()
{
	SendToServer("\x72\x01\x00\x32\x00",5);
}

void INJECTION_API PeaceMode()
{
	SendToServer("\x72\x00\x00\x32\x00",5);
}

void INJECTION_API Say(const char * IN Text)
{
	ClientTalk(Text);
}

void INJECTION_API UseObject(uint32 Serial)
{
	if(Serial==0 || Serial==INVALID_SERIAL)
		return;
	uint8 Buff[5]="\x06";
	*((DWORD*)(Buff+1))=htonl(Serial);
	SendToServer(Buff,5);
}

BOOL INJECTION_API UseType(uint16 Type, uint16 Color)
{
	if(Type==INVALID_GRAPHIC)
		return false;
	uint32 Ser=FindObjectWithType(Type,Color);
	if(Ser!=INVALID_SERIAL)
		UseObject(Ser);
	return Ser!=INVALID_SERIAL;
}
