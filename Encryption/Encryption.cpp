// Encryption.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "api.h"
#include "RegistryConstants.h"

#include "crypt.h"
#include "GameCrypt.h"
#include "LoginCrypt.h"

char UserName[64];	// used to calculate login keys if they were not found
char Password[64];	// in client's memory

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
	strncpy(Buffer,"Encryption plugin",BuffSize);
	Buffer[BuffSize-1]=0;
}

BOOL PLUGIN_API InitPlugin()
{
	// read registry, select encryption mode

	GetShardRegistryDword("LoginKey1",(DWORD*)&LoginKey1);
	GetShardRegistryDword("LoginKey2",(DWORD*)&LoginKey2);

	GetShardRegistryString("Username",UserName,64);
	GetShardRegistryString("Password",Password,64);

	// read OutgoingEncryption from registry
	GetShardRegistryDword("GameCrypt",(DWORD*)&OutgoingEncryption);
	if(OutgoingEncryption<0 || OutgoingEncryption>Crypt_max)
		error_printf("Invalid encryption mode\n");

/*	This code may be used to find login keys in memory. 
	But it is better to calculate them.

	if(IncomingEncryption == Crypt_Client)
	{
		// Look for login keys in client's body
		unsigned char Bytes[2][11]={{	// code signature for T2A
			0xc1, 0xe0, 0x1f, 0xd1, 0xe0, 0xd1, 0xe0,
			0x0b, 0xc0, 0x0b, 0x00
		},{	// UOTD client since 3.0.4 uses different code:
			0xd1, 0xe0, 0x0b, 0xc0, 0xc1, 0xe2, 0x1f,
			0x35, 0x00, 0x00, 0x00, 
		}};
			
		__try {
			for(unsigned char*Ptr=(unsigned char*)0x410000; Ptr<((unsigned char*)0x4FFFFF) ;Ptr++)
			{
				int i,j;
				bool Found;
				for(j=0; j<2; j++)
				{
					Found=true;
					for(i=0; i<sizeof(Bytes[0]); i++)
					{
						if(Bytes[j][i]==0x00)
							continue;
						
						if(Bytes[j][i]==0xe0 || Bytes[j][i]==0xc0)
						{
							if((Ptr[i]&(unsigned char)0xF0)!=Bytes[j][i])
								Found=false;
						} else
							if(Ptr[i]!=Bytes[j][i])
								Found=false;
					}
					if(Found)
						break;
				}
				if(!Found)
					goto Fail;
				
				if(j==0)
				{
					Ptr+=11;
					if(*Ptr==0x81)
						Ptr++;
					ClientLoginKey1=*(DWORD*)(Ptr+1);
					ClientLoginKey2=*(DWORD*)(Ptr+1+4+2);
				} else
				{
					ClientLoginKey1=*(DWORD*)(Ptr+8);
					ClientLoginKey2=*(DWORD*)(Ptr+8+12+7);
				}
				break;

Fail:			;
			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {}

		if(ClientLoginKey1==0 || ClientLoginKey2==0)
		{
			error_printf("Unable to find login keys in client's memory!\n");
			return FALSE;
		}
		trace_printf("Client encryption: Login key1 = %08X, key2 = %08X\n",ClientLoginKey1,ClientLoginKey2);
	} else if(IncomingEncryption == Crypt_None)
	{
		// TODO:
	} else
	{
		error_printf("Invalid client encryption!\n");
		return FALSE;
	}
*/

	return TRUE;
}

void PLUGIN_API CleanupPlugin()
{
	CleanupGame();
	CleanupLogin();
}

