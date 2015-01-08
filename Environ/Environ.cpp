// Environ.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdlib.h"

#include "api.h"

bool FixLight = false;
int FixLightLevel=100;
bool INJECTION_CALLBACK OnLight(const uint8 IN *Msg, int IN Len);
void INJECTION_CALLBACK CmdLight(int IN argc, char IN **argv);

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

void PLUGIN_API GetPluginName(char OUT *Buffer, int IN BuffSize)
{
	strncpy(Buffer,"Light and weather filter",BuffSize);
	Buffer[BuffSize-1]=0;
}

BOOL PLUGIN_API InitPlugin()
{
	bool ret=true;

	GetShardRegistryDword("FixLight",(DWORD*)&FixLight);
	GetShardRegistryDword("FixLightLevel",(DWORD*)&FixLightLevel);

	ret&=RegisterOnServerMessage(0x4f,OnLight);
	ret&=RegisterCommand("light",CmdLight);

	return ret;
}

void PLUGIN_API CleanupPlugin()
{
	SetShardRegistryDword("FixLight",FixLight);
	SetShardRegistryDword("FixLightLevel",FixLightLevel);
}

bool INJECTION_CALLBACK OnLight(const uint8 IN *Msg, int IN Len)
{
	if(FixLight)
	{
		uint8 Buff[2]={0x4f,0};
		// issue a new message to client
		Buff[1]=(uint8)FixLightLevel;
		SendToClient(Buff,2);
		// eat this message
		return true;
	}
	return false;
}

void INJECTION_CALLBACK CmdLight(int IN argc, char IN **argv)
{
	if(argc!=2)
	{
		ClientPrint("Usage: light [value]");
		ClientPrint("Sets the global light level to the specified value.");
		ClientPrint("The value can be:");
		ClientPrint("   -1 - turn light fixing off");
		ClientPrint("   0-31 - set light level to the given value. 0 - max light");
		ClientPrint("Current value: %d",FixLightLevel);
		return;
	}
	int Level = atoi(argv[1]);
	if(Level==-1)
	{
		FixLight = false;
		FixLightLevel = Level;
		ClientPrint("Light fixing is now off");
	}
	else
	{
		if(Level>=0 && Level<=31)
		{
			FixLightLevel = Level;
			FixLight = true;
			uint8 Buff[2]={0x4f,0};
			// issue a new message to client
			Buff[1]=(uint8)FixLightLevel;
			SendToClient(Buff,2);
			ClientPrint("Light level is now set to %d",FixLightLevel);
		} else
			ClientPrintWarning("Invalid value for the light level");
	}
}
