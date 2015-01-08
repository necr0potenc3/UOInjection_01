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
//     SocksHook.h
// 
//  Abstract:
// 
//     winsock.dll hook classes
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
#include "api.h"

bool HookWinsockFunctions();

const BUFFER_SIZE = 65536;

/*
 *	An base class for socket hooks. Login & game processing
 *	would be added in its descendants
 */
class SocketHook
{
	struct BufferElement
	{
		char *Buffer;
		int Len;
	};
	CList<BufferElement,BufferElement&> Queue;

	void InternalSendToClient(void *Buff, int Len);	// like SendToClient, but puts Buff to the list head
	bool FirstMessage;
public:
	SOCKET Socket;

	SocketHook(SOCKET s);
	virtual ~SocketHook();

    int send(char *buf, int size, int flags);	// called on WinSock send function
    int recv(char *buf, int len, int flags);	// called on WinSock recv function

	bool HasData();		// returns true if there is data waiting to be received by client
	void ReceiveData();	// handles data sent from server

	int SendToServer(void *Buff, int Len);	// Send data to server &
	void SendToClient(void *Buff, int Len);	// to client. Would be encrypted/compressed automatically


	// functions return the size of encrypted block. They should
	// overwrite data in Buff. Buff is allocated BUFFER_SIZE in size, and
	// it should be enough for all needs...
	// Compression is also processed by these functions
	virtual int EncryptDataToServer(char *Buff, int Len);		// Injection -> Server
	virtual int EncryptDataToClient(char *Buff, int Len);		// Injection -> Client
	virtual int DecryptDataFromServer(char *Buff, int Len);		// Server -> Injection
	virtual int DecryptDataFromClient(char *Buff, int Len);		// Client -> Injection

	virtual bool HandleServerMessage(char *Buff, int &Len);	// return TRUE to suppress this message
	virtual bool HandleClientMessage(char *Buff, int &Len);	// return TRUE to suppress this message
};

class LoginSocket : public SocketHook
{
public:
	static bool Redirecting;

	LoginSocket(SOCKET s);
	~LoginSocket();

	virtual int EncryptDataToServer(char *Buff, int Len);
	virtual int EncryptDataToClient(char *Buff, int Len);
	virtual int DecryptDataFromServer(char *Buff, int Len);
	virtual int DecryptDataFromClient(char *Buff, int Len);

	virtual bool HandleServerMessage(char *Buff, int &Len);
	virtual bool HandleClientMessage(char *Buff, int &Len);
};


class GameSocket : public SocketHook
{
public:
	GameSocket(SOCKET s);
	~GameSocket();

	virtual int EncryptDataToServer(char *Buff, int Len);
	virtual int EncryptDataToClient(char *Buff, int Len);
	virtual int DecryptDataFromServer(char *Buff, int Len);
	virtual int DecryptDataFromClient(char *Buff, int Len);

	virtual bool HandleServerMessage(char *Buff, int &Len);	
	virtual bool HandleClientMessage(char *Buff, int &Len);	
};

typedef int PLUGIN_API CryptFunc(char * INOUT Buff, int IN Length);

extern CryptFunc *LoginEncryptDataToServer;
extern CryptFunc *LoginEncryptDataToClient;
extern CryptFunc *LoginDecryptDataFromServer;
extern CryptFunc *LoginDecryptDataFromClient;

extern CryptFunc *GameEncryptDataToServer;
extern CryptFunc *GameEncryptDataToClient;
extern CryptFunc *GameDecryptDataFromServer;
extern CryptFunc *GameDecryptDataFromClient;

extern InitLoginEncryption *InitLogin;
extern InitGameEncryption *InitGame;

extern bool LogCI;
extern bool LogIC;
extern bool LogSI;
extern bool LogIS;

void CloseSockets();
