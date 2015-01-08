//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include "..\include\api.h"
#include "SDLLUnitF.h"

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    if(reason==DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls(hinst);
        
    return 1;
}
//---------------------------------------------------------------------------
void PLUGIN_API GetPluginName(char OUT *Buffer, int IN BuffSize)
{
	strncpy(Buffer,"Skills control plugin",BuffSize);
	Buffer[BuffSize-1]=0;
}

BOOL PLUGIN_API InitPlugin()
{
    if(SkillsForm==0)              
    {
        SkillsForm = new TSkillsForm(0);
        SkillsForm->ParentWindow = GetInjectionTabHwnd();
        AddTab(SkillsForm->Handle,"Skills",TRUE);
        SkillsForm->Visible=true;
        SkillsForm->Left=0; SkillsForm->Top=0;
    }
    return TRUE;
}

void PLUGIN_API CleanupPlugin()
{
    if(SkillsForm)
    {
        SkillsForm->Close();
        delete SkillsForm;
        SkillsForm=0;
    }
}
