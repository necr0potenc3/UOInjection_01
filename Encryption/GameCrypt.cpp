#include "stdafx.h"

#include "api.h"
#include "crypt.h"
#include "uo_huffman.h"
#include "GameCrypt.h"

const BUFFER_SIZE = 65536;

GameCrypt *ServerCrypt=0;
GameCrypt *ServerCryptNew=0;
GameCrypt *ClientCrypt=0;
GameCrypt *ClientCryptNew=0;
CompressingCopier *Compressor=0;
DecompressingCopier *Decompressor=0;
extern int LoginSeed;	// defined in LoginCrypt.cpp

// TODO: find why my code does not work if servers sends GameSeed != 127.0.0.1
static uint8 GameSeed[]="\x7f\x00\x00\x01";

void PLUGIN_API InitGameEncryption()
{
	if(ServerCrypt)
		delete ServerCrypt;
	if(ServerCryptNew)
		delete ServerCryptNew;

	if(ClientCrypt)
		delete ClientCrypt;
	if(ClientCryptNew)
		delete ClientCryptNew;

	ClientCryptNew = ClientCrypt = 0;

	if(Compressor)
		delete Compressor;

	if(Decompressor)
		delete Decompressor;

	if(OutgoingEncryption==Crypt_203)		// 2.0.3 uses both 2.0.0 & 2.0.4 encryptions
		ServerCryptNew = new NewGameCrypt(GameSeed);

	if(OutgoingEncryption==Crypt_None)
		ServerCrypt = new NoGameCrypt;
	if(OutgoingEncryption==Crypt_204)
		ServerCrypt = new NewGameCrypt(GameSeed);
	if(OutgoingEncryption==Crypt_203 || OutgoingEncryption==Crypt_200  || OutgoingEncryption==Crypt_Client)
		ServerCrypt = new OldGameCrypt;		// in the case of Crypt_Client we recreate ServerCrypt on first message

	Compressor = new CompressingCopier;
	Decompressor = new DecompressingCopier;

	ServerCrypt->init();	
	if(ServerCryptNew)
		ServerCryptNew->init();	
	Compressor->initialise();
	Decompressor->initialise();
}

extern "C" int PLUGIN_API GameEncryptDataToServer(char *Buff, int Len)
{
	if(ServerCrypt->IsFirstMessage())	// first message is crypt seed
	{
		ServerCrypt->FirstMessageProcessed();
		if(ServerCryptNew)
			ServerCryptNew->FirstMessageProcessed();

		if(Len!=4 && Buff[0]!=0x7f && Buff[1]!=0 && Buff[2]!=0 && Buff[3]!=1)
			error_printf("Game encryption seed == %02X%02X%02X%02X (!=0x7F000001)\n",
				Buff[0],Buff[1],Buff[2],Buff[3]);

		return Len;
	}

	// encrypt
	char *Tmp1 = new char[Len];	
	ServerCrypt->EncryptToServer((uint8*)Buff,(uint8*)Tmp1,Len);
	memcpy(Buff,Tmp1,Len);
	if(OutgoingEncryption==Crypt_203 && ServerCryptNew)
	{
		ServerCryptNew->EncryptToServer((uint8*)Buff,(uint8*)Tmp1,Len);
		memcpy(Buff,Tmp1,Len);
	}

	delete Tmp1;

	return Len;
}

extern "C" int PLUGIN_API GameDecryptDataFromServer(char *Buff, int Len)
{
	// decrypt
	char *Tmp = new char[Len];	// TODO: optimize this - allocate large buffer in constructor & use it

	ServerCrypt->DecryptFromServer((uint8*)Buff,(uint8*)Tmp,Len);
	
	// decompress
	int OutBytes=BUFFER_SIZE;
	char Tmp1[BUFFER_SIZE];

	(*Decompressor)(Tmp1,Tmp,OutBytes,Len);
	memcpy(Buff,Tmp1,OutBytes);
	delete Tmp;

	return OutBytes;
}

extern "C" int PLUGIN_API GameEncryptDataToClient(char *Buff, int Len)
{
	// compress
	int OutBytes=BUFFER_SIZE;
	char Tmp[BUFFER_SIZE];

	(*Compressor)(Tmp,Buff,OutBytes,Len);
	int Flush=BUFFER_SIZE-OutBytes;
	Compressor->flush(Tmp+OutBytes,Flush);
	memcpy(Buff,Tmp,OutBytes+Flush);

	Len=OutBytes+Flush;

	ClientCrypt->EncryptToClient((unsigned char*)Tmp,(unsigned char*)Buff,Len);
	
	return Len;
}

extern "C" int PLUGIN_API GameDecryptDataFromClient(char *Buff, int Len)
{
	// this is a special case when login socket is reused for the game
	// encryption. In this case encryption seed is not sent to client.
	// FIXME: is special encryption used in such case? I can't guess it.
	if(ServerCrypt->IsFirstMessage() && Len!=4)
	{
		warning_printf("GameEncryption seed is not sent by client!\n");
		ServerCrypt->FirstMessageProcessed();
		if(ServerCryptNew)
			ServerCryptNew->FirstMessageProcessed();
	}

	if(ServerCrypt->IsFirstMessage())	// first message is crypt seed
	{
		// for sphere server seed is 0x0100007f
		// for POL = 0xFFFFFFFF
		memcpy(GameSeed,Buff,4);
		return Len;
	} else
	{
		if(ClientCrypt == 0)	// Select the client encryption
		{
			// This is the first message sent to server. I use these bytes
			// to determine encryption
			unsigned char ToCheck[]={0x91, 0x7f, 0x00, 0x00, 0x01};
			memcpy(ToCheck+1,GameSeed,4);

			GameCrypt *Tmp=new OldGameCrypt;
			GameCrypt *TmpNew = new NewGameCrypt(GameSeed);
			Tmp->init(); TmpNew->init();

			unsigned char TempBuff[65536];
			unsigned char TempBuff1[65536];

			trace_printf("Client encryption: ");
			if(memcmp(Buff,ToCheck,sizeof(ToCheck))==0)
			{
				trace_printf("none\n");
				IncomingEncryption=Crypt_None;
				ClientCrypt = new NoGameCrypt;
				goto Cont;
			}

			// check 2.0.4+ encryption
			TmpNew->DecryptFromClient((unsigned char*)Buff,TempBuff,Len);
			if(memcmp(TempBuff,ToCheck,sizeof(ToCheck))==0)
			{
				trace_printf("2.0.4+\n");
				IncomingEncryption=Crypt_204;
				ClientCrypt = new NewGameCrypt(GameSeed);
				goto Cont;
			}

			// check 2.0.3 encryption
			Tmp->DecryptFromClient(TempBuff,TempBuff1,Len);
			if(memcmp(TempBuff1,ToCheck,sizeof(ToCheck))==0)
			{
				trace_printf("2.0.3\n");
				IncomingEncryption=Crypt_203;
				ClientCrypt = new OldGameCrypt;
				ClientCryptNew = new NewGameCrypt(GameSeed);
				goto Cont;
			}

			// reset crypt
			delete Tmp;
			Tmp=new OldGameCrypt;
			Tmp->init();

			// check 2.0.0- encryption
			Tmp->DecryptFromClient((unsigned char*)Buff,TempBuff1,Len);
			if(memcmp(TempBuff1,ToCheck,sizeof(ToCheck))==0)
			{
				trace_printf("<=2.0.0\n");
				IncomingEncryption=Crypt_200;
				ClientCrypt = new OldGameCrypt;
				goto Cont;
			}

			error_printf("Unable to determine client's game encryption!\n");
			MessageBox(0,"Unable to determine client's game encryption! Exiting.","UOInjection",0);
			ExitProcess(1);
Cont:
			delete Tmp;
			delete TmpNew;
			ClientCrypt->init();
			if(ClientCryptNew)
				ClientCryptNew->init();

			if(OutgoingEncryption == Crypt_Client)
			{
				OutgoingEncryption = IncomingEncryption;
				if(ServerCrypt)
					delete ServerCrypt;

				if(OutgoingEncryption==Crypt_203)		// 2.0.3 uses both 2.0.0 & 2.0.4 encryptions
					ServerCryptNew = new NewGameCrypt(GameSeed);
				if(OutgoingEncryption==Crypt_None)
					ServerCrypt = new NoGameCrypt;
				if(OutgoingEncryption==Crypt_204)
					ServerCrypt = new NewGameCrypt(GameSeed);
				if(OutgoingEncryption==Crypt_203 || OutgoingEncryption==Crypt_200  || OutgoingEncryption==Crypt_Client)
					ServerCrypt = new OldGameCrypt;		// in the case of Crypt_Client we recreate ServerCrypt on first message

				ServerCrypt->init();	
				if(ServerCryptNew)
				{
					ServerCryptNew->init();	
					ServerCryptNew->FirstMessageProcessed();
				}
				ServerCrypt->FirstMessageProcessed();
			}
		}
	}
	unsigned char *Tmp = new unsigned char[Len];
	if(ClientCryptNew)
	{
		ClientCryptNew->DecryptFromClient((unsigned char*)Buff,Tmp,Len);
		memcpy(Buff,Tmp,Len);
	}

	ClientCrypt->DecryptFromClient((unsigned char*)Buff,Tmp,Len);
	memcpy(Buff,Tmp,Len);
	delete Tmp;

	return Len;
}

void CleanupGame()
{
	delete ServerCrypt;
	delete ServerCryptNew;
	delete ClientCrypt;
	delete ClientCryptNew;
	delete Compressor;
	delete Decompressor;

	ServerCrypt=0;
	ServerCryptNew=0;
	ClientCrypt=0;
	ClientCryptNew=0;
	Compressor=0;
	Decompressor=0;
	ServerCrypt=0;
	ClientCrypt=0;
}