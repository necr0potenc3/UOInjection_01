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
//     HighLevel.cpp
// 
//  Abstract:
// 
//	   Injection high level API
//	   This API can be called only from parallel threads, and all exported 
//     functions in this file should call CheckThread() to check this
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [3/3/2002] - created
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "atlcoll.h"
#include "afxmt.h"

#include "api.h"
#include "common.h"
#include "defcmd.h"
#include "HighLevel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD GUIThreadId=0;
DWORD ClientThreadId=0;

static CCriticalSection StorageLockVar;
class StorageLock	// class used to protect CLists from being modified in parallel threads
{					//  [3/3/2002] - class is never used. Remove it?
public:
	StorageLock() {StorageLockVar.Lock();}
	~StorageLock() {StorageLockVar.Unlock();}
};

/*
 *	This function determines the urrent thread, and if it is main client's thread
 *	or GUI thread it prints an error.
 */
void CheckThread()
{
	DWORD th=GetCurrentThreadId();
	if(th==GUIThreadId || th==ClientThreadId)
	{
		if(IDOK==MessageBox(0,"Calling high level API from the wrong thread. Press OK to debug.",0,MB_ICONSTOP|MB_OKCANCEL))
			DebugBreak();
	}
}


/*
 *	Select item vars
 */

static uint32 TargetSerial;		// INVALID_SERIAL if cancelled
static CEvent *TargetEvent=0;
static CCriticalSection TargetCriticalSection;

/*
 *	Returns the serial of the selected item. Several requests can wait in queue
 */
uint32 INJECTION_API SelectItem()
{
	CheckThread();
	CEvent Evt;

	trace_printf("Selecting a target...\n");
	static uint8 Buff[19]="\x6c\x01\x00\x00\x00\x22";

	TargetCriticalSection.Lock();
	while(TargetEvent)	// Wait until TargetEvent would be NULL, so we can use it
	{	// Of cause I can use locks not to check TargetEvent value in a loop,
		// but it would not save much CPU time
		TargetCriticalSection.Unlock();
		Sleep(100);
		TargetCriticalSection.Lock();
	}
	TargetEvent=&Evt;
	SendToClient(Buff,19);
	TargetCriticalSection.Unlock();

	WaitForSingleObject(Evt,INFINITE);

	TargetCriticalSection.Lock();
	uint32 Serial=TargetSerial;
	TargetEvent=0;
	TargetCriticalSection.Unlock();

	if(Serial==0)
		Serial=INVALID_SERIAL;

	trace_printf("Selected target %08x\n",Serial);
	return Serial;
}

/*
 *	Supplementary functions.
 *
 *  Sets the value of TargetSerial after target is selected or cancelled
 */
bool INJECTION_CALLBACK OnTarget(const uint8 IN *Msg, int IN Len)
{
	TargetCriticalSection.Lock();
	TargetSerial=unpack_big_uint32(Msg+7);
	if (TargetEvent)
	{
		TargetEvent->SetEvent();
		TargetCriticalSection.Unlock();
		return true;
	}
	TargetCriticalSection.Unlock();

	return false;
}

/*
 *	If server asks for a target while we are waiting for it in SelectItem() - 
 *  cancel our wait
 */
bool INJECTION_CALLBACK OnServerTarget(const uint8 IN *Msg, int IN Len)
{
	TargetCriticalSection.Lock();
	TargetSerial=INVALID_SERIAL;
	if (TargetEvent)
		TargetEvent->SetEvent();
	TargetCriticalSection.Unlock();

	return false;
}

BOOL INJECTION_API BeginHighLevelApiThread(INJECTION_THREADPROC ThreadProc, void *Param)
{
	// Don't allow to start threads before logging in
	if(GetPlayerSerial()==0 || GetPlayerSerial()==INVALID_SERIAL)
		return FALSE;

	return 0!=AfxBeginThread((AFX_THREADPROC)ThreadProc,Param);
}
