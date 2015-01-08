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
//     Plugins.cpp
// 
//  Abstract:
// 
//     Plugin control finctions: load, init, cleanup, notify...
//	   There are 2 types of plugin functions: low level callbacks,
//     always called from the context of GUI thread, and high level
//     functions called from parallel threads
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/20/2002] mamaich	- source code cleanup. Removed all global
//						  variables, so several callbacks can
//						  be dispatched simultaneously
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Injection.h"
#include "MainWindow.h"
#include "GuiThread.h"
#include "Plugins.h"
#include "SocksHook.h"
#include "api.h"
#include "defcmd.h"
#include "world.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CList<PluginInfo,PluginInfo&> PluginList;

/*
 *	Function prototypes
 */

UINT PluginThreadProc( LPVOID pParam );

void INJECTION_API GetPluginsDirectory(char OUT *DirBuff, int IN BufferSize)
{
	FUNCTION_PROLOGUE

	char Buff[MAX_PATH];
	GetModuleFileName(GetInjectionDllBase(),Buff,MAX_PATH);
	char *Tmp=strrchr(Buff,'\\');
	if(Tmp)
		*Tmp=0;

	strcat(Buff,"\\Plugins\\");

	strncpy(DirBuff,Buff,BufferSize);
	DirBuff[BufferSize-1]=0;

	FUNCTION_EPILOGUE
}


/*
 *	Scan 'plugins' directory, load all DLLs and call their InitPlugin functions.
 */
void LoadPluginDLLs()
{
// TODO: instead of scanning directory I should read registry

	FUNCTION_PROLOGUE

	char Buff[MAX_PATH];
	GetPluginsDirectory(Buff,MAX_PATH);
	CString PluginsDir=Buff;
	strcat(Buff,"*.dll");

	PluginList.RemoveAll();

	WIN32_FIND_DATA fd;

	bool AddInjection=true;	// a flag indicating to add injection.dll to list
	HANDLE hf=FindFirstFile(Buff,&fd);
	if(hf == INVALID_HANDLE_VALUE)
	{
		error_printf("No plugins found!\n");
		return;
	}

	do {
		PluginInfo pi;
Restart:
		if(AddInjection)	// add injection.dll to the head of a list
		{
			trace_printf("Loading Injection.dll as a plugin\n",fd.cFileName);
			char Inj[MAX_PATH];
			GetModuleFileName(GetInjectionDllBase(),Inj,MAX_PATH);
			pi.hInstance=LoadLibrary(Inj);	// increment injection.dll reference count
			pi.DllName="Injection.dll";
		} else
		{
			trace_printf("Trying to load %s\n",fd.cFileName);
			pi.hInstance=LoadLibrary(PluginsDir+fd.cFileName);
			pi.DllName=fd.cFileName;
		}
		if(pi.hInstance)
		{
			GetPluginName *gpn=(GetPluginName*)GetProcAddress(pi.hInstance,"GetPluginName");
			if(!gpn)
			{
				warning_printf("Failed to find 'GetPluginName' or 'InitPlugin' in %s\n",fd.cFileName);
				FreeLibrary(pi.hInstance);
			}
			else
			{
				FUNCTION_PROLOGUE

				char Buff[MAX_PATH]="Invalid plugin";
				trace_printf("Calling 'GetPluginName'...\n");
				gpn(Buff,MAX_PATH);
				pi.PluginName=Buff;

				InitPlugin *ipl=(InitPlugin*)GetProcAddress(pi.hInstance,"InitPlugin");

				bool Keep=true;
				if(ipl)
					Keep = (ipl()!=0);

				if(Keep)
				{
					trace_printf("Plugin '%s' loaded\n",pi.PluginName);
					PluginList.AddTail(pi);
				}
				else
				{
					warning_printf("Plugin '%s' refused to load\n",pi.PluginName);
					FreeLibrary(pi.hInstance);
				}

				FUNCTION_EPILOGUE
			}
		} else
			warning_printf("%s failed to load\n",fd.cFileName);

		if(AddInjection)
		{
			AddInjection=false;
			goto Restart;
		}
	} while(FindNextFile(hf,&fd));

	FUNCTION_EPILOGUE
}


/*
 *	Ask all plugins to patch client in memory.
 *	This function was previously used to remove client encryption
 *	using ignition.cfg
 */
bool RemoveEncryptionFromClientWithPlugins()
{
	BOOL Success=FALSE;

	FUNCTION_PROLOGUE

	if(PluginList.IsEmpty())
	{
		error_printf("No plugins loaded!\n");
		return FALSE;
	}

	char ClientExeName[MAX_PATH];
	GetModuleFileName(NULL,ClientExeName,MAX_PATH);

	// First ask RemCrypt.dll to remove client encryption

	HINSTANCE hi = GetModuleHandle("RemCrypt.dll");
	if(hi)
	{
		PatchClient *renc=(PatchClient*)GetProcAddress(hi,"PatchClient");
		if(renc)
			Success=renc(ClientExeName);
	} else
		trace_printf("RemCrypt.dll is not loaded\n");

	POSITION pos = PluginList.GetHeadPosition();
	for (int i=0; i < PluginList.GetCount() && !Success; i++)
	{
		PluginInfo &pi=PluginList.GetNext(pos);

		// do not call RemCrypt.dll for the second time
		if(stricmp(pi.DllName,"RemCrypt.dll") == 0)
			continue;

		PatchClient *renc=(PatchClient*)GetProcAddress(pi.hInstance,"PatchClient");
		if(renc)
			Success=renc(ClientExeName);
	}

	FUNCTION_EPILOGUE
	return Success!=FALSE;
}


/*
 *	Look through the list of loaded plugins & select the first that 
 *	exports "InitLoginEncryption" & "InitGameEncryption" functions. 
 *	There should be only one such plugin loaded.
 */
bool InitEncryptionPlugin()
{
	BOOL Success=FALSE;

	FUNCTION_PROLOGUE

	if(PluginList.IsEmpty())
	{
		error_printf("No plugins loaded!\n");
		return FALSE;
	}

	POSITION pos = PluginList.GetHeadPosition();
	for (int i=0; i < PluginList.GetCount() && !Success; i++)
	{
		PluginInfo &pi=PluginList.GetNext(pos);


		InitLoginEncryption *Le=(InitLoginEncryption*)GetProcAddress(pi.hInstance,"InitLoginEncryption");
		InitGameEncryption *Ge=(InitGameEncryption*)GetProcAddress(pi.hInstance,"InitGameEncryption");

		if(Le==0 || Ge==0)
			continue;

		Success=TRUE;

		InitLogin = Le;
		InitGame = Ge;

		CryptFunc *Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"LoginEncryptDataToServer");
		if(Tmp)
			LoginEncryptDataToServer=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"LoginEncryptDataToClient");
		if(Tmp)
			LoginEncryptDataToClient=Tmp;
		
		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"LoginDecryptDataFromServer");
		if(Tmp)
			LoginDecryptDataFromServer=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"LoginDecryptDataFromClient");
		if(Tmp)
			LoginDecryptDataFromClient=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"GameEncryptDataToServer");
		if(Tmp)
			GameEncryptDataToServer=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"GameEncryptDataToClient");
		if(Tmp)
			GameEncryptDataToClient=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"GameDecryptDataFromServer");
		if(Tmp)
			GameDecryptDataFromServer=Tmp;

		Tmp=(CryptFunc*)GetProcAddress(pi.hInstance,"GameDecryptDataFromClient");
		if(Tmp)
			GameDecryptDataFromClient=Tmp;
	}

	FUNCTION_EPILOGUE
	return Success!=FALSE;
}

void UnloadPlugins()
{
	FUNCTION_PROLOGUE

	if(PluginList.IsEmpty())
	{
		error_printf("No plugins loaded!\n");
		return;
	}

	char ClientExeName[MAX_PATH];
	GetModuleFileName(NULL,ClientExeName,MAX_PATH);

	POSITION pos = PluginList.GetHeadPosition();
	for (int i=0; i < PluginList.GetCount(); i++)
	{
		PluginInfo &pi=PluginList.GetNext(pos);

		CleanupPlugin *renc=(CleanupPlugin*)GetProcAddress(pi.hInstance,"CleanupPlugin");
		if(renc)
			renc();
	}

	FUNCTION_EPILOGUE
}

// Buffers to hold partially received client/server messages
static uint8 ClientPart[BUFFER_SIZE];	// buffers should be large enough
static uint8 ServerPart[BUFFER_SIZE];	// TODO: make them dynamic size?
static int ClientPartLen=0;
static int ServerPartLen=0;
bool HandleMessage(uint8 *Buff, int &Length, bool FromServer)
{
	uint8 *Part;
	int *PartLen;
	if(FromServer)
	{
		Part=ServerPart;
		PartLen=&ServerPartLen;
	} else
	{
		Part=ClientPart;
		PartLen=&ClientPartLen;
	}

	// process partial messages:
	if(*PartLen)	// prepend message part to the start of buffer
	{
		if(Length+*PartLen>BUFFER_SIZE)
			error_printf("Len+*PartLen (%d)>%d!\n",Length+*PartLen,BUFFER_SIZE);
		memmove(Buff+*PartLen,Buff,Length);
		memcpy(Buff,Part,*PartLen);
		Length+=*PartLen;
		*PartLen=0;
	}

	// trim the partial message from the end
	int Pos=0;
	while(Pos<Length && Pos+GetMessageLength(Buff+Pos,Length-Pos)<=Length)
		Pos+=GetMessageLength(Buff+Pos,Length-Pos);

	*PartLen=Length-Pos;
	if(*PartLen)
	{
		memcpy(Part,Buff+Pos,*PartLen);
		Length=Pos;
	}

	CallbackParameters CBParams;
	CallbackType Callback;
	Callback = (FromServer ? CB_ServerMessage:CB_ClientMessage);

	int Len=Length;
	while(Len>0)
	{
		int ML=GetMessageLength(Buff,Len);

		// callback data
		memcpy(CBParams.Msg.MessageBuffer,Buff,ML);
		CBParams.Msg.MessageLength = ML;
		CBParams.Msg.EatMessage = false;

		SignalCallbacks(Callback,&CBParams);

		if(!CBParams.Msg.EatMessage)
		{
			if(FromServer)
				SendToClient(Buff,ML);
			else
				SendToServer(Buff,ML);
		}

		Buff+=ML;
		Len-=ML;
	}

	return Length==0;	//  [2/20/2002] - function return value is no longer used
}

// macros to automatically generate callback registering functions
#define CALLBACK_REGISTRAR(CallbackName,Param)	\
bool inline IRegister##CallbackName(Param Parm)	\
{	\
	if(C##CallbackName.Find(Parm)==NULL)	\
	{	\
		C##CallbackName.AddTail(Parm);	\
		return true;	\
	} else	\
		return false;	\
}

// macro for unregistering callbacks 
#define CALLBACK_UNREGISTRAR(CallbackName,Param)	\
void inline IRemove##CallbackName(Param Parm)	\
{	\
	POSITION p=C##CallbackName.Find(Parm);	\
	if(p!=NULL)	\
		C##CallbackName.RemoveAt(p);	\
}

/*
 *	Callback lists
 */
CAtlList<FOnIdle*> COnIdle;			// list of OnIdle callbacks
CAtlList<FOnEnterWorld*> COnEnterWorld;	// list of OnEnterWorld callbacks

struct MessageHandler
{
	int MessageNumber;
	FOnMessage *Handler;
	MessageHandler(int MN, FOnMessage* OM) : MessageNumber(MN),
		Handler(OM) {}
	bool operator==(const MessageHandler&M) const {return memcmp(this,&M,sizeof(MessageHandler))==0;}
};
CAtlList<MessageHandler> COnServerMessage;	// list of message handlers
CAtlList<MessageHandler> COnClientMessage;

struct InstalledCommand
{
	CString Command;
	FOnCommand *Handler;
	InstalledCommand(const CString &C, FOnCommand *H) : Command(C),
		Handler(H) {}
	bool operator==(const InstalledCommand &C) const {return Command==C.Command;}
	InstalledCommand& operator=(const InstalledCommand&C) 
	{
		Command=C.Command; Handler=C.Handler;
	}
};
CAtlList<InstalledCommand> COnCommand;

/*
 *	These macros would simplify adding new callback types
 */
CALLBACK_REGISTRAR(OnIdle,FOnIdle*)
CALLBACK_UNREGISTRAR(OnIdle,FOnIdle*)

CALLBACK_REGISTRAR(OnEnterWorld,FOnEnterWorld*)
CALLBACK_UNREGISTRAR(OnEnterWorld,FOnEnterWorld*)

CALLBACK_REGISTRAR(OnServerMessage,MessageHandler)
CALLBACK_UNREGISTRAR(OnServerMessage,MessageHandler)

CALLBACK_REGISTRAR(OnClientMessage,MessageHandler)
CALLBACK_UNREGISTRAR(OnClientMessage,MessageHandler)

CALLBACK_REGISTRAR(OnCommand,InstalledCommand)
CALLBACK_UNREGISTRAR(OnCommand,InstalledCommand)


/*
 *	Here exported functions go
 */
bool INJECTION_API RegisterOnIdle(FOnIdle *Func)
{
	return IRegisterOnIdle(Func);
}
void INJECTION_API RemoveOnIdle(FOnIdle *Func)
{
	IRemoveOnIdle(Func);
}

bool INJECTION_API RegisterOnEnterWorld(FOnEnterWorld *Func)
{
	return IRegisterOnEnterWorld(Func);
}
void INJECTION_API RemoveOnEnterWorld(FOnEnterWorld *Func)
{
	IRemoveOnEnterWorld(Func);
}

bool INJECTION_API RegisterOnServerMessage(int MsgNo, FOnMessage *Func)
{
	return IRegisterOnServerMessage(MessageHandler(MsgNo,Func));
}
void INJECTION_API RemoveOnServerMessage(int MsgNo, FOnMessage *Func)
{
	return IRemoveOnServerMessage(MessageHandler(MsgNo,Func));
}

bool INJECTION_API RegisterOnClientMessage(int MsgNo, FOnMessage *Func)
{
	return IRegisterOnClientMessage(MessageHandler(MsgNo,Func));
}
void INJECTION_API RemoveOnClientMessage(int MsgNo, FOnMessage *Func)
{
	return IRemoveOnClientMessage(MessageHandler(MsgNo,Func));
}

bool INJECTION_API RegisterCommand(const char * IN cmd, FOnCommand *Func)
{
	if(cmd[0]==COMMAND_PREFIX)	cmd++;
	return IRegisterOnCommand(InstalledCommand(cmd,Func));
}
void INJECTION_API RemoveCommand(const char * IN cmd, FOnCommand *Func)
{
	if(cmd[0]==COMMAND_PREFIX)	cmd++;
	return IRemoveOnCommand(InstalledCommand(cmd,Func));
}


/*
 *	Callback dispatching functions
 */

// Signals callbacks immediately
LRESULT SignalCallbacks(CallbackType Callback,CallbackParameters *CBParams)
{
	return MainWindow->SendMessage(WM_NOTIFY_PLUGINS,(WPARAM)Callback,(LPARAM)CBParams);
}
// Signals callbacks after the current processing will finish
LRESULT SignalCallbacksAsync(CallbackType Callback,CallbackParameters *CBParams)
{
	CallbackParameters *tmp=0;
	if(CBParams)
		tmp=new CallbackParameters(*CBParams);	// would be deleted in WM_NOTIFY_PLUGINS_ASYNC handler
	return MainWindow->PostMessage(WM_NOTIFY_PLUGINS_ASYNC,(WPARAM)Callback,(LPARAM)tmp);
}


// Calls the specified function for each list member and passes that 
// member as an argument
#define FOR_EACH(P,List,Function) \
{	\
	bool StopProcessing=false;	\
	POSITION pos = (List).GetHeadPosition();	\
	for (unsigned int i=0; i < (List).GetCount() && !StopProcessing; i++)	\
	{	\
		StopProcessing=(Function)(P,(List).GetNext(pos));	\
	}	\
}

// these functions return TRUE to stop firther processing

// Function to call plugin's OnIdle handlers
bool inline CallIdle(CallbackParameters*,FOnIdle *F)
{
	F();
	return false;
}

// Function to call plugin's OnEnterWorld handlers
bool inline CallEnterWorld(CallbackParameters*,FOnEnterWorld *F)
{
	F();
	return false;
}

// function that calls OnServer/ClientMessage handlers
bool inline CallMessage(CallbackParameters *P,MessageHandler M)
{
	if(!P->Msg.EatMessage && (M.MessageNumber==P->Msg.MessageBuffer[0] ||
		M.MessageNumber==-1))
 		P->Msg.EatMessage = M.Handler(P->Msg.MessageBuffer,P->Msg.MessageLength);
	return P->Msg.EatMessage;
}

// calls installable command handler
bool inline CallCommand(CallbackParameters *P,InstalledCommand M)
{
	if(strcmpi(M.Command,P->Cmd.Command)==0)
	{
		#define _MAX_CMD_LINE_ARGS  128
		char * _ppszArgv[_MAX_CMD_LINE_ARGS+1];		
		int cbCmdLine;
		int argc;
		PSTR pszCmdLine;
	    
		/*
		 *	The code is taken from TinyCRT library by Matt Pietrek
		 */

		// Set to no argv elements, in case we have to bail out
		_ppszArgv[0] = 0;

		pszCmdLine = P->Cmd.CommandLine;
		cbCmdLine = lstrlen( pszCmdLine );

		_ppszArgv[0] = pszCmdLine;  // argv[0] == executable name

		while ( *pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine) )
			pszCmdLine++;

		if ( *pszCmdLine )
			*pszCmdLine++ = 0;  // Null terminate and advance to next char

		// Done processing argv[0] (i.e., the executable name).  Now do th
		// actual arguments

		argc = 1;

		while ( 1 )
		{
			// Skip over any whitespace
			while ( *pszCmdLine && (' ' == *pszCmdLine) || ('\t' == *pszCmdLine) )
				pszCmdLine++;

			if ( 0 == *pszCmdLine ) // End of command line???
				break;

			if ( '"' == *pszCmdLine /*|| '\'' == *pszCmdLine*/ )   // Argument starting with a quote???
			{
				pszCmdLine++;   // Advance past quote character

				_ppszArgv[ argc++ ] = pszCmdLine;
				_ppszArgv[ argc ] = 0;

				// Scan to end quote, or NULL terminator
				while ( *pszCmdLine && (*pszCmdLine != '"') /*&& (*pszCmdLine != '\'')*/ )
					pszCmdLine++;
	                
				if ( 0 == *pszCmdLine )
					break;
	            
				if ( *pszCmdLine )
					*pszCmdLine++ = 0;  // Null terminate and advance to next char
			}
			else                        // Non-quoted argument
			{
				_ppszArgv[ argc++ ] = pszCmdLine;
				_ppszArgv[ argc ] = 0;

				// Skip till whitespace or NULL terminator
				while ( *pszCmdLine && (' '!=*pszCmdLine) && ('\t'!=*pszCmdLine) )
					pszCmdLine++;
	            
				if ( 0 == *pszCmdLine )
					break;
	            
				if ( *pszCmdLine )
					*pszCmdLine++ = 0;  // Null terminate and advance to next char
			}

			if ( argc >= (_MAX_CMD_LINE_ARGS) )
				break;
		}
		M.Handler(argc,_ppszArgv);
		return true;
	}
	return false;
}

// Plugin notification dispatcher
LRESULT CMainWindow::OnNotifyPlugins(WPARAM wParam, LPARAM lParam)
{
	CallbackType CB=(CallbackType)wParam;
	CallbackParameters *P=(CallbackParameters*)lParam;

#ifndef _DEBUG
	// debug build does not need try/catch, or VC debugger would not stop on exception
	try 
	{
#endif
		switch(CB)
		{
		case CB_Idle:
			FOR_EACH(P,COnIdle,CallIdle);
			break;
		case CB_EnterWorld:
			FOR_EACH(P,COnEnterWorld,CallEnterWorld);
			break;
		case CB_ServerMessage:
			FOR_EACH(P,COnServerMessage,CallMessage);
			break;
		case CB_ClientMessage:
			FOR_EACH(P,COnClientMessage,CallMessage);
			break;
		case CB_Command:
			{	
				bool StopProcessing=false;	
				POSITION pos = COnCommand.GetHeadPosition();	
				for (unsigned int i=0; i < COnCommand.GetCount() && !StopProcessing; i++)	
				{	
					StopProcessing=CallCommand(P,COnCommand.GetNext(pos));	
				}	
				if(!StopProcessing)
					ClientPrintWarning("Unrecognized command '%s'",P->Cmd.Command);
			}
			break;
		default:
			warning_printf("Unknown callback type %d!\n",CB);
			break;
		};
#ifndef _DEBUG
	} catch(...)
	{
		error_printf("Unhandled exception in callback dispatcher\n");
	}
#endif
	return FALSE;
}

LRESULT CMainWindow::OnNotifyPluginsAsync(WPARAM wParam, LPARAM lParam)
{
	CallbackParameters *P=(CallbackParameters*)lParam;

	OnNotifyPlugins(wParam, lParam);
	if(P)
		delete P;
	return FALSE;
}

bool INJECTION_CALLBACK OnEnterWorld(const uint8 IN *Msg, int IN Len)
{
	Msg++;
	N2L(Msg,CharacterSerial);
	SignalCallbacksAsync(CB_EnterWorld,NULL);
	return false;
}

DWORD INJECTION_API GetPlayerSerial()
{
	return CharacterSerial;
}

void INJECTION_API ExecuteCommand(const char *Str)
{
	if(Str[0]==COMMAND_PREFIX)
		Str++;
	CallbackParameters CP;
	strcpy(CP.Cmd.CommandLine,Str);
	strncpy(CP.Cmd.Command,Str,255);
	CP.Cmd.Command[255]=0;
	char* t=strchr(CP.Cmd.Command,' ');
	if(t)
		*t=0;

	SignalCallbacks(CB_Command,&CP);
}

void INJECTION_API ExecuteCommandAsync(const char *Str)
{
	if(Str[0]==COMMAND_PREFIX)
		Str++;
	CallbackParameters CP;
	strcpy(CP.Cmd.CommandLine,Str);
	strncpy(CP.Cmd.Command,Str,255);
	CP.Cmd.Command[255]=0;
	char* t=strchr(CP.Cmd.Command,' ');
	if(t)
		*t=0;

	SignalCallbacksAsync(CB_Command,&CP);
}

void INJECTION_CALLBACK CmdDebug(int IN argc, char IN **argv)
{
	if(argc==1)
	{
		ClientPrint("Usage: debug [info_number]");
		ClientPrint("Where info_number can be:");
		ClientPrint("   1 - list of loaded plugins");
		ClientPrint("   2 - list of installed commands");
		ClientPrint("   3 - dump world to the log");
		return;
	}
	int Info=atol(argv[1]);
	switch(Info)
	{
	case 1:
		{
			ClientPrint("List of loaded plugins:");
			POSITION pos = PluginList.GetHeadPosition();
			for (int i=0; i < PluginList.GetCount(); i++)
			{
				PluginInfo &pi=PluginList.GetNext(pos);
				ClientPrint("%s - %s",pi.DllName,pi.PluginName);
			}
		}
		break;
	case 2:
		{
			ClientPrint("List of installed commands:");
			POSITION pos = COnCommand.GetHeadPosition();	
			for (unsigned int i=0; i < COnCommand.GetCount(); i++)	
			{	
				ClientPrint("   %s",COnCommand.GetNext(pos).Command);
			}	
		}
		break;
	case 3:
		DumpWorldToLog();
		break;
	default:
		ClientPrintWarning("Invalid parameter");
	}
}
