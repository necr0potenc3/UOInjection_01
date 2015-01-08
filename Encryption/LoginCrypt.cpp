#include "stdafx.h"

#include "api.h"
#include "LoginCrypt.h"
#include "crypt.h"
#include "getkeys.h"

/*
 *	TODO: Code is too ugly - encryption classes are recreated 
 *	after the first message is sent from client.
 *	Optimize that.
 */

LoginCrypt *ServerCrypt=0;
LoginCrypt *ClientCrypt=0;

unsigned ClientLoginKey1=0;
unsigned ClientLoginKey2=0;

unsigned int LoginKey1 = 0;	
unsigned int LoginKey2 = 0;

ClientEncryptionSet IncomingEncryption=Crypt_Client;
ClientEncryptionSet OutgoingEncryption=Crypt_Client;

int LoginSeed = 0;

void PLUGIN_API InitLoginEncryption()
{
	if(ServerCrypt)
		delete ServerCrypt;
	if(ClientCrypt)
		delete ClientCrypt;

	if(OutgoingEncryption == Crypt_None)
		ServerCrypt = new NoLoginCrypt;
	else
		ServerCrypt=new LoginCrypt;

	if(IncomingEncryption == Crypt_None)
		ClientCrypt = new NoLoginCrypt;
	else
		ClientCrypt = new LoginCrypt;
}

extern "C" int PLUGIN_API LoginEncryptDataToServer(char *Buff, int Len)
{
	// First message sent to server is a key
	if(ServerCrypt->IsFirstMessage())
	{
		if(Len!=4)
			error_printf("Login seed is not 4 bytes in length!\n");

		trace_printf("Login seed=%02X%02X%02X%02X\n",Buff[0]&255,Buff[1]&255,Buff[2]&255,Buff[3]&255);
		memcpy(&LoginSeed, Buff, 4);
		if(OutgoingEncryption == Crypt_Client)
			ServerCrypt->init((unsigned char*)Buff,ClientLoginKey1,ClientLoginKey2);
		else
			ServerCrypt->init((unsigned char*)Buff,LoginKey1,LoginKey2);
		ServerCrypt->MessageProcessed();

		ClientCrypt->init((unsigned char*)Buff,ClientLoginKey1,ClientLoginKey2);
		ClientCrypt->MessageProcessed();
	} else
		ServerCrypt->encrypt((unsigned char*)Buff,(unsigned char*)Buff,Len);

	return Len;
}

extern "C" int PLUGIN_API LoginDecryptDataFromServer(char *Buff, int Len)
{
	return Len;
}

// Client does not require incoming encryption
extern "C" int PLUGIN_API LoginEncryptDataToClient(char *Buff, int Len)
{
	return Len;
}

extern "C" int PLUGIN_API LoginDecryptDataFromClient(char *Buff, int Len)
{
	if(!ClientCrypt->IsFirstMessage())	// 1st message - is login seed. It does not need encryption
	{
		if(ClientCrypt->IsSecondMessage())	// 2nd message is used to calculate login keys
		{
			char Message[62];
			char Out[62];
			if(Len==62)
			{
				memset(Message,0,61);	// First data packet sent from server is known
				Message[0]=(char)0x80;
				strcpy(Message+1,UserName);
				strcpy(Message+31,Password);
				Message[61]=(char)0xff;

				ClientCrypt->encrypt((unsigned char*)Buff,(unsigned char*)Out,Len);
				if(memcmp(Out,Message,62)==0)
				{
					memcpy(Buff,Message,62);
					ClientCrypt->MessageProcessed();
					return Len;
				} else
				{
//					trace_printf("Calculating login keys using UserName and Password from registry\n");
					CalculateKeys((BYTE*)Message,(BYTE*)Buff);
					delete ClientCrypt;
					ClientCrypt = new LoginCrypt;
					ClientCrypt->init((unsigned char*)&LoginSeed,ClientLoginKey1,ClientLoginKey2);
					ClientCrypt->MessageProcessed();	// skip login seed check
					if(OutgoingEncryption==Crypt_Client)
					{
						delete ServerCrypt;
						ServerCrypt = new LoginCrypt;
						ServerCrypt->init((unsigned char*)&LoginSeed,ClientLoginKey1,ClientLoginKey2);
						ServerCrypt->MessageProcessed();	// skip login seed check
					}
				}
			} else
				warning_printf("First login message is not 62 bytes in length!\n");
		} 
		ClientCrypt->MessageProcessed();
		ClientCrypt->encrypt((unsigned char*)Buff,(unsigned char*)Buff,Len);
	}

	return Len;
}

void CleanupLogin()
{
	delete ServerCrypt;
	delete ClientCrypt;
	ServerCrypt = ClientCrypt = 0;
}
