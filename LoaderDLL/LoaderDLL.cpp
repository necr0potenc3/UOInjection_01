// LoaderDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "crash.h"

static char OepBytes[5];	// 5 bytes at client's original entry point overwritten by hook
static DWORD OEP=0;			// client's original entry point address

HINSTANCE DllBase;

void InitDlls();

void __declspec(naked) EntryPointHook()
{
// This function is called just before client starts its work,
// but after Windows had finished application initialization.
// So we can safely create all needed windows, hooks, load plugins, etc.
	InitDlls();
	__asm jmp dword ptr [OEP];
}

#pragma warning(disable:4311)
#pragma warning(disable:4312)

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllBase = hModule;
		DisableThreadLibraryCalls(hModule);
		{
			// Windows does not allow to load DLLs & create threads in DllMain.
			// So I hook client's entry point & perform initialization
			// in that hook.

			DWORD image_base = (DWORD)GetModuleHandle(NULL);
			IMAGE_DOS_HEADER *idh = (IMAGE_DOS_HEADER *)image_base;
			IMAGE_FILE_HEADER *ifh = (IMAGE_FILE_HEADER *)(image_base +
				idh->e_lfanew + sizeof(DWORD));
			IMAGE_OPTIONAL_HEADER *ioh = (IMAGE_OPTIONAL_HEADER *)((DWORD)(ifh) +
				sizeof(IMAGE_FILE_HEADER));
			OEP = ioh->AddressOfEntryPoint + image_base;

			memcpy(OepBytes,(void*)OEP,5);	// remember bytes overwritten by my hook

		#include <pshpack1.h>	// select 1-byte structure packing
			struct NewOepBytes
			{
				BYTE  JMP;
				DWORD Addr;
			} Patch = {0xe9, (DWORD)EntryPointHook - (OEP+5)};
		#include <poppack.h>

			WriteProcessMemory(GetCurrentProcess(),(void*)OEP,
				&Patch,sizeof(Patch),0);
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		delete g_logger;
		g_logger = 0;
		break;
	}
    return TRUE;
}

// Use import table editor (for exaple PEditor by yoda & M.o.D.)
// to add this function to client.exe import table, so this DLL
// would be loaded without need of a launcher (and we can debug
// it in VC IDE, use SocksCap, BoundsChecker & so on).
extern "C" void __declspec(dllexport) __cdecl Init()
{
}

// This function loads all DLLs specified in registry.
// They should be: Injection.dll, MUL path hooker, UOAM hooker
// This function should not load plugin DLLs
void InitDlls()
{
// restore bytes at OEP overwritten by our hook
	WriteProcessMemory(GetCurrentProcess(),(void*)OEP,
		&OepBytes,sizeof(OepBytes),0);

// Now all exceptions would be handled in my filter
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    WORD FPUcw=0x27f;
    __asm {
		fstcw [FPUcw]
	}

	// TODO: Read DLL list from registry and load all of them

	char Buff[MAX_PATH];
	GetModuleFileName(DllBase,Buff,MAX_PATH);
	char *Tmp=strrchr(Buff,'\\');
	if(Tmp)
		*Tmp=0;
	strcat(Buff,"\\injection.dll");

	HMODULE InjDLL = LoadLibrary(Buff);

	if(InjDLL == 0)
		error_printf("Unable to load Injection.dll\n");
	else
	{
		typedef void __cdecl InitProc();
		InitProc *Init=(InitProc*)GetProcAddress(InjDLL,"Init");
		if(Init)
			Init();
		else
			error_printf("Init() function not found in Injection.dll\n");
	}

    __asm {
        fldcw [FPUcw]
    }
}
