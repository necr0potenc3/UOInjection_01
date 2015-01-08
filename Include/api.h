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
//     api.h
// 
//  Abstract:
// 
//     All injection API prototypes should go into this header. 
//	   Function bodies can be anywhere.
//	   Dumb Borland C++ prepends '_' to the names of all functions provided
//     in this file. To prevent this you need to add a .DEF file to a project.
//     Look into Skills.dll sources for an example.
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

#ifdef INJECTION_DLL
# define INJECTION_API __declspec(dllexport) __cdecl
# define PLUGIN_API	   __cdecl
#else
# ifndef INJECTION_API 
#  define INJECTION_API __declspec(dllimport) __cdecl
# endif
# define PLUGIN_API    __declspec(dllexport) __cdecl
#endif

#define INJECTION_CALLBACK __cdecl

// Function parameter directions
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef INOUT
#define INOUT
#endif

// Load long from address C into LL with bytes swapped.
#define N2L(C, LL) \
    LL  = ((unsigned int)(*((C)++))) << 24, \
    LL |= ((unsigned int)(*((C)++))) << 16, \
    LL |= ((unsigned int)(*((C)++))) << 8, \
    LL |= ((unsigned int)(*((C)++)))
// Store long LL at address C with bytes swapped.
#define L2N(LL, C) \
    *((C)++) = (unsigned char)(((LL) >> 24) & 0xff), \
    *((C)++) = (unsigned char)(((LL) >> 16) & 0xff), \
    *((C)++) = (unsigned char)(((LL) >> 8) & 0xff), \
    *((C)++) = (unsigned char)(((LL)) & 0xff)

// Prefix of all injection commands
#define COMMAND_PREFIX	','

#ifdef __cplusplus
extern "C" {
#endif

//
//					Some typedefs & defines
//

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed long sint32;

const uint32 INVALID_SERIAL  = 0xffffffff;
const uint16 INVALID_GRAPHIC = 0xffff;
const uint16 INVALID_COLOR   = 0xffff;


//////////////////////////////////////////////////////////////
// 
//					Here injection API goes. 
//
// No classes. Only structs, so API can be used from any language.
// 
//////////////////////////////////////////////////////////////

/*
 *	Returns the directory to plugins. Directory contains trailing "\"
 */
void INJECTION_API GetPluginsDirectory(char OUT *DirBuff, int IN BufferSize);

/*
 *	Returns base address of injection.dll
 */
HINSTANCE INJECTION_API GetInjectionDllBase();

/*
 *	Returns HWND of ultima online client's main window
 */
HWND INJECTION_API GetClientWindow();

/*
 *	Writes an error to log file and executes DebugBreak() in debug build
 */
void INJECTION_API UnhandledException();

/*
 *	Reads the registry key data. Function prepends "HKLM\Software\UOInjection"
 *	to the key name automatically. Returns TRUE if read successfully, FALSE
 *	if error.
 */
BOOL INJECTION_API GetRegistryString(const char IN *KeyName, const char IN *SubKey,
									 char OUT *Value, int IN cbValue);
/*
 *	Set registry string
 */
BOOL INJECTION_API SetRegistryString(const char IN *KeyName, const char IN *SubKey,
									 char IN *Value);

/*
 *	Reads the registry DWORD. Read the notes about GetRegistryDword in the end of this file
 */
BOOL INJECTION_API GetRegistryDword(const char IN *KeyName, const char IN *SubKey,
									 DWORD OUT *Value);

/*
 *	Set registry dword
 */
BOOL INJECTION_API SetRegistryDword(const char IN *KeyName, const char IN *SubKey,
									DWORD IN Value);

/*
 *	Read per-server settings from registry
 */
BOOL INJECTION_API GetShardRegistryString(const char IN *SubKey,
									 char OUT *Value, int IN cbValue);
/*
 *	Set registry string
 */
BOOL INJECTION_API SetShardRegistryString(const char IN *SubKey,
									 char IN *Value);

/*
 *	Reads the registry DWORD
 */
BOOL INJECTION_API GetShardRegistryDword(const char IN *SubKey,
									 DWORD OUT *Value);

/*
 *	Set registry dword
 */
BOOL INJECTION_API SetShardRegistryDword(const char IN *SubKey,
									DWORD IN Value);


/*
 *	logging functions
 */
void INJECTION_API log_printf(const char * IN format, ...);
void INJECTION_API error_printf(const char * IN format, ...);
void INJECTION_API warning_printf(const char * IN format, ...);
void INJECTION_API log_dump(unsigned char * IN buf, int IN length);
void INJECTION_API log_flush();
void INJECTION_API trace_printf(const char * IN format, ...);
void INJECTION_API trace_dump_comment(const char * IN comment, unsigned char * IN buf, int IN length);
void INJECTION_API trace_dump(unsigned char * IN buf, int IN length);


// 
//					User interface functions
//
// These functions should be used by plugins which add tabs to injection

/*
 *	Return HWND of injection tab control. It should be a parent HWND to all added windows
 */
HWND INJECTION_API GetInjectionTabHwnd();

/*
 *	Add a new tab to injection. If Resizeable == FALSE than that
 *	window would be centered inside injection window. Otherwize
 *	tab window would be resized to the maximal tab size. In this
 *	case window should realign all its controls itself.
 */
BOOL INJECTION_API AddTab(HWND IN Tab, const char * IN Name, BOOL IN Resizeable);

/*
 *	Remove tab with the given name
 */
void INJECTION_API RemoveTab(const char * IN Name);


// 
//					Import table hooking functions
//
// Used internally to hook send/recv, and may be useful to plugin writers.

/*
 *	Hooks the given function. Returns the previous address, so hooks
 *	may be cascaded. Ordinal parameter is used if FuncName is NULL
 *	or no function with given name is found in imports.
 *	In the case of error return value is NULL.
 */
PVOID INJECTION_API HookImportedFunction(const char * IN Dll, const char * IN FuncName,
										 int IN Ordinal, void * IN Function);



// 
//					Message hooking functions
//

/*
 *	Returns the length in bytes of message contained in Buff. 
 *	If size is unknown - returns Len. Function may return the 
 *	number larger than Len in a case of a partial message.
 */
int INJECTION_API GetMessageLength(const uint8 * IN Buff, int Len);

/*
 *	Message hooking is done through callbacks. Plugin can register 
 *	any number of callbacks. They would be called in the registration
 *	order.
 *	Callbacks can be added and removed at any time, but only from the
 *	context of a main plugin thread.
 */

/*
 *	Data flow handlers. Functions hook the specified message number.
 *	If you want to process all messages in one handler - specify "-1"
 *	as MsgNo. One function may be registered several times to hook 
 *	different messages.
 */
typedef bool INJECTION_CALLBACK FOnMessage(const uint8 IN *Msg, int IN Len);	// return TRUE to "eat" this message
bool INJECTION_API RegisterOnServerMessage(int MsgNo, FOnMessage *Func);	// set the handler for messages coming from server
void INJECTION_API RemoveOnServerMessage(int MsgNo, FOnMessage *Func);		// remove handler
bool INJECTION_API RegisterOnClientMessage(int MsgNo, FOnMessage *Func);	// set the handler for messages coming from client
void INJECTION_API RemoveOnClientMessage(int MsgNo, FOnMessage *Func);		// remove handler

/*
 *	Send messages to server/client. Messages are not checked - so be careful
 */
void INJECTION_API SendToServer(const void IN *Buff, int Len);
void INJECTION_API SendToClient(const void IN *Buff, int Len);

/*
 *	Output to client screen. Functions differ by output color
 */
void INJECTION_API ClientPrint(const char * IN format, ...);		// green
void INJECTION_API ClientPrintWarning(const char * IN format, ...);	// yellow
void INJECTION_API ClientPrintError(const char * IN format, ...);	// red & bold text
void INJECTION_API ClientPrintColor(int color, int font, const char * IN format, ...); // specify color & font

/*
 *	Simulate client saying something
 */
void INJECTION_API ClientTalk(const char * IN format, ...);		
void INJECTION_API ClientTalkColor(int color, const char * IN format, ...); // specify color


/*
 *	Set the OnIdle callback. It is called every 500 ms. Plugin may
 *	do the background processing there. But it should better use
 *	parallel threads.
 */
typedef void INJECTION_CALLBACK FOnIdle();
bool INJECTION_API RegisterOnIdle(FOnIdle *Func);	// set the idle handler
void INJECTION_API RemoveOnIdle(FOnIdle *Func);		// remove handler

/*
 *	OnEnterWorld callback
 */
typedef void INJECTION_CALLBACK FOnEnterWorld();
bool INJECTION_API RegisterOnEnterWorld(FOnEnterWorld *Func);	// set the EnterWorld handler
void INJECTION_API RemoveOnEnterWorld(FOnEnterWorld *Func);		// remove handler

/*
 *	Installable commands callbacks
 */
typedef void INJECTION_CALLBACK FOnCommand(int IN argc, char IN **argv);	// argv[0] - command without prefix, argv[1+] - params
bool INJECTION_API RegisterCommand(const char * IN cmd, FOnCommand *Func);
void INJECTION_API RemoveCommand(const char * IN cmd, FOnCommand *Func);

/*
 *	Execute an installed command
 */
void INJECTION_API ExecuteCommand(const char *Str);	// you can omit ',' from the start of a command

// Put this command into queue and execute it later when Injection will have time.
// Several commands may wait in queue. They would be executed in sequence and client
// would not be hang during their execution
void INJECTION_API ExecuteCommandAsync(const char *Str);	

//////////////////////////////////////////////////////////////
// 
//					Information functions
// 
//////////////////////////////////////////////////////////////

/*
 *	Return the character name
 */
void INJECTION_API GetCharacterName(char IN *Buff, int BuffSize);

/*
 *	Return character serial number
 */
DWORD INJECTION_API GetPlayerSerial();

/*
 *	Return player's backpack
 */
DWORD INJECTION_API GetBackpackSerial();

/*
 *	Get information on object by its serial
 */
struct ObjectInformation	// unknown fields are set to zero
{
	uint32 Serial;	
	uint16 Graphic;
	uint16 Color;
	uint16 X,Y;
	int Z;
	int Layer;
	int Quantity;
	uint32 Container;		// Object which contains this item or 0
};
/*
 *	Fills the given structure with the information about object.
 *	Returns TRUE on success or FALSE if no such object
 */
BOOL INJECTION_API GetObjectInformation(uint32 IN Serial, ObjectInformation * OUT Info);


/*
 *	Skills
 */

int INJECTION_API GetSkill(int Num);		// Return the skill, Num=0..49
int INJECTION_API GetSkillBase(int Num);	// Skill base value
int INJECTION_API GetSkillLock(int Num);	// Skill lock: 0=up, 1=down, 2=locked
void INJECTION_API SetSkillLock(int Num,int Lock);	// Skill lock

/*
 *	Get the item's serial number from the objects list by its name
 *	returns INVALID_SERIAL on error
 */
uint32 INJECTION_API GetObjectSerial(const char * IN Text);

/*
 *	Get the item's graphics number from the object types list by its name
 *	returns INVALID_GRAPHIC on error
 */
uint16 INJECTION_API GetObjectGraphic(const char * IN Text);

/*
 *	Get the item's color from the object types list by its name
 *	returns 0 on error or if the given type does not use color
 */
uint16 INJECTION_API GetObjectColor(const char * IN Text);

/*
 *	Returns the first item of the given type.
 *  TODO: should I return a random item and not the first one?
 *  TODO: should I look for items only in player's backpack or I 
 *  can return serials of the equipped items?
 */
uint32 INJECTION_API FindObjectWithType(uint16 Type, uint16 Color=0, uint32 Container=GetPlayerSerial());


//////////////////////////////////////////////////////////////
// 
//  Low level API functions. Can be called from any thread.
//  they return without waiting for the request to be processed
//  by server
// 
//////////////////////////////////////////////////////////////

/*
 *	Switch to war/peace mode
 */
void INJECTION_API WarMode();
void INJECTION_API PeaceMode();

/*
 *	Simulate client saying something. The same as ClientTalk
 */
void INJECTION_API Say(const char * IN Text);

/*
 *	Use the given object (sumulate doubleclick)
 */
void INJECTION_API UseObject(uint32 obj);

/*
 *	Use the object with the given graphics. Returns FALSE if no such 
 *	object found
 */
BOOL INJECTION_API UseType(uint16 Type, uint16 Color=0);

//////////////////////////////////////////////////////////////
// 
//  High level API functions. Can be called only from parallel 
//  threads
// 
//////////////////////////////////////////////////////////////

/*
 *	Starts the given procedure in a parallel thread, so that it
 *	can use the high level API
 */
typedef unsigned int INJECTION_THREADPROC(void* Param);
BOOL INJECTION_API BeginHighLevelApiThread(INJECTION_THREADPROC *ThreadProc, void *Param = NULL);

/*
 *	Forces the client to bring a target cursor and select an item.
 *	If targetting is cancelled - returned value is INVALID_SERIAL
 */
uint32 INJECTION_API SelectItem();


//////////////////////////////////////////////////////////////
// 
//				Functions exported by plugins
// 
//////////////////////////////////////////////////////////////

#ifdef INJECTION_DLL
# define TYPEDEF typedef
#else
# define TYPEDEF
#endif


/*
 *	Plugin returns its name on this call. All plugins have to implement this function.
 */
TYPEDEF void PLUGIN_API GetPluginName(char OUT *Buffer, int IN BuffSize);

/*
 *	Plugin should initialize itself (read registry & so on) on this call.
 *	If it returns FALSE - it is unloaded.
 *	Plugin should register its callbacks here.
 */
TYPEDEF BOOL PLUGIN_API InitPlugin();

/*
 *	Plugin cleanup.
 *	Providing this function is optional. 
 */
TYPEDEF void PLUGIN_API CleanupPlugin();


// 
//		Remove client encryption plugin functions
//
// These functions should be exported by plugins which are used to
// remove encryption from client via in-memory patching. Other plugins 
// should not provide this function.
// At first Injection loads RemCrypt.dll which tries to patch client
// with data found in ignition.cfg. If plugin fails - next plugins are
// loaded until one of them patches client successfully. If none of
// them succeeded - message is displayed and client terminates.

/*
 *	Plugin should return TRUE if it patched client, or false if it failed
 *	or it is unsure that client is patched correctly.
 */
TYPEDEF BOOL PLUGIN_API PatchClient(const char IN *ClientExeName);



// 
//		Client<->Injection<->Server encryption plugin functions
//
// This is a special-purpose plugin & should be written with care...
// It is executed in the context of various client/injection threads 
// - so synchronization may be needed.

/*
 *	This function is called when first (login) socket created.
 *	It is called on every relogin.
 */
TYPEDEF void PLUGIN_API InitLoginEncryption();

/*
 *	This function is called when second (game) socket created.
 *	It is called on every relogin.
 */
TYPEDEF void PLUGIN_API InitGameEncryption();


#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////
// 
//	    Inline wrappers around APIs using MFC classes
// So if you write plugins in VC you'll be in advantageous 
// position. Later will add the same wrappers for BCB.
// 
//////////////////////////////////////////////////////////////

#ifdef __cplusplus
#ifdef _AFX		

// inline wrapper around GetPluginsDirectory
inline CString GetPluginsDirectory()
{
	char Buff[MAX_PATH];
	GetPluginsDirectory(Buff,MAX_PATH);
	return CString(Buff);
}

// A bit modified wrapper around GetRegistryString. In the case of error
// it returns the DefaultValue or empty string.
inline CString GetRegistryString(const CString IN &KeyName, const CString IN &SubKey,
							const CString IN &DefaultValue="")
{
	char Buff[MAX_PATH];
	if(!GetRegistryString(KeyName,SubKey,Buff,MAX_PATH))
		return DefaultValue;
	return CString(Buff);
}

inline CString GetShardRegistryString(const CString IN &SubKey,
							const CString IN &DefaultValue="")
{
	char Buff[MAX_PATH];
	if(!GetShardRegistryString(SubKey,Buff,MAX_PATH))
		return DefaultValue;
	return CString(Buff);
}

/*
 *	NOTE: VC may call the wrong version of GetRegistryDword in the case if you
 *	pass 0 as a parameter to the following function. It may think that 0 is NULL
 *	pointer and not a default DWORD value.
 *	To prevent this - simply do not pass 0 (or FALSE or any other constants 
 *	which evaluate to 0) to this function as DefaultValue. When 0 is needed - simply
 *	omit the last (DefaultValue) parameter and everything will compile fine.
 */
inline DWORD GetRegistryDword(const CString IN &KeyName, const CString IN &SubKey,
							const DWORD IN DefaultValue=0)
{
	DWORD tmp=0;
	if(!GetRegistryDword(KeyName,SubKey,&tmp))
		return DefaultValue;
	return tmp;
}

inline DWORD GetShardRegistryDword(const CString IN &SubKey,
							const DWORD IN DefaultValue=0)
{
	DWORD tmp=0;
	if(!GetShardRegistryDword(SubKey,&tmp))
		return DefaultValue;
	return tmp;
}

inline CString GetCharacterName()
{
	char Buff[128];
	GetCharacterName(Buff,128);
	return CString(Buff);
}

#endif	// _AFX
#endif	// __cplusplus
