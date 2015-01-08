// ILaunch.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClientStarter.h"
#include "LaunchProgressDlg.h"
#include "registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


ClientStartState ClientStarterThreadState=CS_Idle;
CString ErrorDetails="No error.";



bool PrepareToLoadInjection(HANDLE hProcess, HANDLE hThread)
{
// Client is started in a suspended state, so I patch it in memory
// in such a way that it loads LoaderDLL.dll when resumed.
// On NT4+ I allocate needed memory with VirtualAllocEx, and
// on 9x I use free space in PE header, because VirtualAllocEx is
// not implemented there. I can't use PE header space under all OSes
// because XP does not allow writing there even after calling VirtualProtectEx.

	DWORD PatchBase=(DWORD)VirtualAllocEx(hProcess,0,
		4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	if(PatchBase==0)
		PatchBase=0x400400;

	BOOL AllOk=TRUE;	// Flag that everything went ok

	char InjectionDll[MAX_PATH]=".\\error";	// look for injection.dll in the same dir as ILaunch

    CRegistry Reg;

/*
	GetModuleFileName(0,InjectionDll,MAX_PATH);
	char *StartOfFileName=strrchr(InjectionDll,'\\');	// remove file name from path
	if(StartOfFileName) *StartOfFileName=0;
*/
	strcpy(InjectionDll,Reg.getInstallDirPath());

	strcat(InjectionDll,"\\LoaderDLL.dll");

#define pokeb(addr,byte)	\
	{	\
		BYTE b=(BYTE)(byte);	\
		AllOk&=WriteProcessMemory(hProcess,(void*)(addr),&b,1,0);	\
	}

#define poked(addr,dword)	\
	{	\
		DWORD dw=(DWORD)(dword);	\
		AllOk&=WriteProcessMemory(hProcess,(void*)(addr),&dw,4,0);	\
	}

	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	AllOk&=GetThreadContext(hThread,&ctx);
	
	AllOk&=WriteProcessMemory(hProcess,(void*)(PatchBase+0x100),InjectionDll,strlen(InjectionDll)+1,0);

	pokeb(PatchBase  ,0x9c);				// pushfd
	pokeb(PatchBase+1,0x60);				// pushad
	pokeb(PatchBase+2,0x68);				// push "address of dll path"
	poked(PatchBase+3,PatchBase+0x100);

	pokeb(PatchBase+7,0xe8);				// call LoadLibraryA
	poked(PatchBase+8,(char*)(GetProcAddress(GetModuleHandle("kernel32"),"LoadLibraryA"))-
		(PatchBase+7+5));

	pokeb(PatchBase+12,0x61);				// popad
	pokeb(PatchBase+13,0x9d);				// popfd

	pokeb(PatchBase+14,0xe9);				// jmp OldEIP
	poked(PatchBase+15,ctx.Eip-(PatchBase+14+5));

	if(!AllOk)
	{
		ErrorDetails.Format("Unable to patch client in memory. Error number = %08X.", 
			GetLastError());
		return false;
	}

	ctx.Eip=PatchBase;
	AllOk&=SetThreadContext(hThread,&ctx);

	return AllOk!=FALSE;
}


UINT ClientStarterThreadProc( LPVOID pParam )
{
    StartClientParams* params = (StartClientParams*)pParam;
	STARTUPINFO si;
	HANDLE hEvent;
	
	PROCESS_INFORMATION pi;

	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	memset(&pi,0,sizeof(pi));

	// Wait while dialog window is created
	while(CRun->GetSafeHwnd() == 0)
		Sleep(100);	

	Sleep(500);

	ClientStarterThreadState = CS_Starting;
	CRun->m_bClose.EnableWindow(FALSE);

    if (params == NULL)
	{
		ErrorDetails = "Internal error.";
		ClientStarterThreadState = CS_Error;
		goto exit;
	}

//	MessageBox(GetForegroundWindow(),"Thread Started",0,0);

#define START_PHASE(num)\
	CRun->GetDlgItem(IDC_STATIC_PHASE##num)->EnableWindow(TRUE);

#define SET_PHASE_STATE(num,text) \
	CRun->SetDlgItemText(IDC_STATIC_STATE##num,text);	\
	CRun->GetDlgItem(IDC_STATIC_STATE##num)->ShowWindow(SW_SHOW);

	START_PHASE(1);	// Start suspended client

	if(!CreateProcess(NULL, params->Executable, NULL,
		NULL, FALSE, CREATE_SUSPENDED, NULL, params->ClientDir,
		&si, &pi))
	{
		DWORD Error=GetLastError();
		SET_PHASE_STATE(1,"Error");
		ErrorDetails.Format("Unable to start client. Error number = %08X.", 
			Error);
		ClientStarterThreadState = CS_Error;
		goto exit;
	}

	SET_PHASE_STATE(1,"Done");

	START_PHASE(2);		// Load injection to client's address space
	if(!PrepareToLoadInjection(pi.hProcess, pi.hThread))
	{
		SET_PHASE_STATE(4,"Error");
		ClientStarterThreadState = CS_Error;
		goto exit;
	}
	SET_PHASE_STATE(2,"Done");

	START_PHASE(3);	// resume client & check is injection loaded successfully

	hEvent = CreateEvent(NULL,FALSE,FALSE,"Injection_load_dll_event");

	if(hEvent==0 || !ResumeThread(pi.hThread))
	{
		ErrorDetails.Format("Unable to resume client. Error number = %08X.", 
			GetLastError());
		SET_PHASE_STATE(3,"Error");
		ClientStarterThreadState = CS_Error;
		goto exit;
	}

	// Wait for injection.dll to signal that it is loaded
	if(WaitForSingleObject(hEvent,5000)!=WAIT_OBJECT_0)
	{
		ErrorDetails.Format("Unable to load LoaderDLL.dll. Check your client and injection paths.");
		SET_PHASE_STATE(3,"Error");
		ClientStarterThreadState = CS_Error;
		goto exit;
	}

	SET_PHASE_STATE(3,"Done");

	ClientStarterThreadState = CS_Successful;

exit:
	CRun->m_bClose.EnableWindow(TRUE);
	if(CRun && ClientStarterThreadState == CS_Successful)
	{
		Sleep(2000);	// Delay to see the results
		PostMessage(CRun->GetSafeHwnd(),WM_CLOSE,0,0);
	}

	if(CRun && ClientStarterThreadState == CS_Error)
	{
		if(pi.hProcess)	// don't leave hanging client in a cese of error
			TerminateProcess(pi.hProcess,1);

		CRun->m_bDetails.EnableWindow(TRUE);
	}
    return 0;   // thread completed successfully
}
