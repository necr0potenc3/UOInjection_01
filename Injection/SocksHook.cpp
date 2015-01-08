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
//     SocksHook.cpp
// 
//  Abstract:
// 
//     winsock.dll hooks & communication functions
//     Classes use static buffers to keep their data. If in log you see
//     errors saying that "value1 > value2", you should increase BUFFER_SIZE.
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

#include "stdafx.h"
#include "afxmt.h"

#include "Injection.h"
#include "GuiThread.h"
#include "Plugins.h"
#include "api.h"
#include "SocksHook.h"
#include "defcmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


SocketHook *FirstSocket=0;
SocketHook *SecondSocket=0;

bool LogCI=true;
bool LogIC=true;
bool LogSI=true;
bool LogIS=true;


static CCriticalSection CommunicationLockVar;
class CommunicationLock
{
public:
	CommunicationLock() {CommunicationLockVar.Lock();}
	~CommunicationLock() {CommunicationLockVar.Unlock();}
};

/*
 *	Stub. Used for not implemented crypting functions
 */
int CryptStub(char *Buff, int Len)
{
	return Len;
}

InitLoginEncryption *InitLogin=0;
InitGameEncryption *InitGame=0;

CryptFunc *LoginEncryptDataToServer=CryptStub;
CryptFunc *LoginEncryptDataToClient=CryptStub;
CryptFunc *LoginDecryptDataFromServer=CryptStub;
CryptFunc *LoginDecryptDataFromClient=CryptStub;

CryptFunc *GameEncryptDataToServer=CryptStub;
CryptFunc *GameEncryptDataToClient=CryptStub;
CryptFunc *GameDecryptDataFromServer=CryptStub;
CryptFunc *GameDecryptDataFromClient=CryptStub;

/*
 *	Find SocketHook object for the given socket
 */
SocketHook *FindSocket(SOCKET s)
{
	if(FirstSocket && FirstSocket->Socket==s)
		return FirstSocket;
	if(SecondSocket && SecondSocket->Socket==s)
		return SecondSocket;
	return 0;
}


/*
 *	I assume that the first socket created is for login encryption,
 *	second - game encryption, third & so on - translation server.
 *	After the second socket is closed all hooks are reset.
 */

SocketHook::SocketHook(SOCKET s) 
{
	Socket=s;
	FirstMessage=true;
}


SocketHook::~SocketHook()
{
	CommunicationLock Lock;

	while(!Queue.IsEmpty())
	{
		BufferElement b=Queue.RemoveHead();
		delete b.Buffer;
	}
}

/*
 *	Empty virtual functions
 */
bool SocketHook::HandleServerMessage(char *Buff, int &Len)
{	
	return false;
}
bool SocketHook::HandleClientMessage(char *Buff, int &Len)
{
	return false;
}

/*
 *	Used to determine is there data to send to client
 */
bool SocketHook::HasData()
{
	return !Queue.IsEmpty();
}

/*
 *	Actual data receiving
 */
void SocketHook::ReceiveData()
{
	char Buff[BUFFER_SIZE];
	int Len=::recv(Socket,Buff,BUFFER_SIZE,0);
	if(Len>0)
	{
		Len=DecryptDataFromServer(Buff,Len);

		// plugin notification goes here
		HandleServerMessage(Buff,Len);
	}
}


int SocketHook::SendToServer(void *Buff, int Len)
{
	char TmpBuff[BUFFER_SIZE];
	if(Len>BUFFER_SIZE)
		error_printf("send() data block size %d > %d\n",Len,BUFFER_SIZE);
	memcpy(TmpBuff,Buff,Len);
	int tmp = EncryptDataToServer(TmpBuff,Len);
	if(tmp>BUFFER_SIZE)
		error_printf("Encrypted block size %d > %d\n",tmp,BUFFER_SIZE);
	return ::send(Socket,TmpBuff,tmp,0);
}

/*
 *	used to put leaving part of partially sent data to the head of queue
 */
void SocketHook::InternalSendToClient(void *Buff, int Len)
{
	CommunicationLock Lock;

	BufferElement e;
	e.Buffer=new char[Len];
	e.Len=Len;
	memcpy(e.Buffer,Buff,Len);
	Queue.AddHead(e);
}

void SocketHook::SendToClient(void *Buff, int Len)
{
	CommunicationLock Lock;

	BufferElement e;
	e.Buffer=new char[Len];
	e.Len=Len;
	memcpy(e.Buffer,Buff,Len);
	Queue.AddTail(e);
}

int SocketHook::send(char *buf, int len, int /*flags*/)
{
	char TmpBuff[BUFFER_SIZE];
	if(len>BUFFER_SIZE)
		error_printf("send() data block size %d > %d\n",len,BUFFER_SIZE);
	memcpy(TmpBuff,buf,len);
	int tmp = DecryptDataFromClient(TmpBuff,len);
	if(tmp>BUFFER_SIZE)
		error_printf("Decrypted block size %d > %d\n",tmp,BUFFER_SIZE);

	if(FirstMessage)	// bypass encryption seeds
	{
		FirstMessage = false;
		SendToServer(buf,len);
		return len;
	}

	HandleClientMessage(TmpBuff,tmp);

	return len;
}


int SocketHook::recv(char *buf, int len, int flags)
{
	ReceiveData();

	CommunicationLock Lock;

	if(!HasData() || len==0 || buf==0)
		return 0;

	int TotalLen=0;

	while(!Queue.IsEmpty() && len>0)
	{
		BufferElement b=Queue.RemoveHead();
		char TmpBuff[BUFFER_SIZE];

		memcpy(TmpBuff,b.Buffer,b.Len);
		delete b.Buffer;

		int ret = EncryptDataToClient(TmpBuff,b.Len);
		if(ret>BUFFER_SIZE)
			error_printf("Encrypted buffer size %d > %d!\n",ret,BUFFER_SIZE);

		if(ret>len)	// partial data
		{
			memcpy(buf,TmpBuff,len);
			InternalSendToClient(TmpBuff+len,ret-len);	// put unsent data to the head of queue
			TotalLen+=len;
			len=0;
		} else
		{
			memcpy(buf,TmpBuff,ret);
			len-=ret;
			buf+=ret;
			TotalLen+=ret;
		}
	}
	return TotalLen;
}

int SocketHook::EncryptDataToServer(char *Buff, int Len)
{
	if(LogIS)
		trace_dump_comment("Injection->Server",(unsigned char*)Buff, Len);
	return Len;
}

int SocketHook::EncryptDataToClient(char *Buff, int Len)
{
	if(LogIC)
		trace_dump_comment("Injection->Client",(unsigned char*)Buff, Len);
	return Len;
}

int SocketHook::DecryptDataFromServer(char *Buff, int Len)
{
	if(LogSI)
		trace_dump_comment("Server->Injection",(unsigned char*)Buff, Len);
	return Len;
}

int SocketHook::DecryptDataFromClient(char *Buff, int Len)
{
	if(LogCI)
		trace_dump_comment("Client->Injection",(unsigned char*)Buff, Len);
	return Len;
}


bool LoginSocket::Redirecting=false;	// when TRUE - the next socket created is a game socket

LoginSocket::LoginSocket(SOCKET s) : SocketHook(s)
{
	Redirecting = false;
	trace_printf("Login socket created.\n");
	if(InitLogin)
		InitLogin();
	else
		error_printf("LoginSocket::LoginSocket: InitLogin == 0!\n");
}


LoginSocket::~LoginSocket()
{
	trace_printf("Login socket closed.\n");
}


int LoginSocket::EncryptDataToServer(char *Buff, int Len)
{
	SocketHook::EncryptDataToServer(Buff, Len);
	return LoginEncryptDataToServer(Buff,Len);
}

int LoginSocket::EncryptDataToClient(char *Buff, int Len)
{
	SocketHook::EncryptDataToClient(Buff, Len);
	return LoginEncryptDataToClient(Buff,Len);
}

int LoginSocket::DecryptDataFromServer(char *Buff, int Len)
{
	int tmp=LoginDecryptDataFromServer(Buff,Len);
	SocketHook::DecryptDataFromServer(Buff, tmp);
	return tmp;
}

int LoginSocket::DecryptDataFromClient(char *Buff, int Len)
{
	int tmp=LoginDecryptDataFromClient(Buff,Len);
	SocketHook::DecryptDataFromClient(Buff, tmp);
	return tmp;
}

bool LoginSocket::HandleClientMessage(char *Buff, int &Len)
{
	if(SocketHook::HandleClientMessage(Buff, Len))
		return true;

	SendToServer(Buff,Len);
	return false;
}

bool LoginSocket::HandleServerMessage(char *Buff, int &Len)
{
	if(SocketHook::HandleServerMessage(Buff, Len))
		return true;

	if(Buff[0]==(char)0x8c && Len>=11)	// 0x8c - redirect to the game server
	{
		Redirecting = true;
		// POL server sends 0xFFFFFFFF as a game encryption seed, but my NewGameCrypt
		// code expects it to be 0x7f000001, so I modify the seed in a buffer...
		// TODO: modify the GameCrypt class, so that it works with all seeds
		memcpy(Buff+7,"\x7f\x00\x00\x01",4);
		trace_printf("Redirected to the game server %d.%d.%d.%d:%d\n",
			Buff[1],Buff[2],Buff[3],Buff[4],Buff[5]*256+Buff[6]);
	}
	SendToClient(Buff,Len);
	return false;
}



GameSocket::GameSocket(SOCKET s) : SocketHook(s)
{
	LoginSocket::Redirecting = false;	// Clear redirection flag after connecting to game server
	trace_printf("Game socket created.\n");
	if(InitGame)
		InitGame();
	else
		error_printf("GameSocket::GameSocket: InitGame == 0!\n");
}


GameSocket::~GameSocket()
{
	CharacterSerial = 0;
	trace_printf("Game socket closed.\n");
}


int GameSocket::EncryptDataToServer(char *Buff, int Len)
{
	SocketHook::EncryptDataToServer(Buff, Len);
	return GameEncryptDataToServer(Buff,Len);
}

int GameSocket::EncryptDataToClient(char *Buff, int Len)
{
	SocketHook::EncryptDataToClient(Buff, Len);
	return GameEncryptDataToClient(Buff,Len);
}

int GameSocket::DecryptDataFromServer(char *Buff, int Len)
{
	int tmp=GameDecryptDataFromServer(Buff,Len);
	SocketHook::DecryptDataFromServer(Buff, tmp);
	return tmp;
}

int GameSocket::DecryptDataFromClient(char *Buff, int Len)
{
	int tmp=GameDecryptDataFromClient(Buff,Len);
	SocketHook::DecryptDataFromClient(Buff, tmp);
	return tmp;
}


bool GameSocket::HandleServerMessage(char *Buff, int &Len)
{	
	if(SocketHook::HandleServerMessage(Buff, Len))
		return true;

	return HandleMessage((uint8*)Buff,Len,true);
}

bool GameSocket::HandleClientMessage(char *Buff, int &Len)
{
	if(SocketHook::HandleClientMessage(Buff, Len))
		return true;

	return HandleMessage((uint8*)Buff,Len,false);
}



////////////////////////////////////////////////////////////////////////////////
//
//  The socket hook functions
//
////////////////////////////////////////////////////////////////////////////////

int WINAPI hook_closesocket(SOCKET s)
{
    int err = closesocket(s);

    //trace_printf(">> closesocket(%d) => %d\n", s, err);
    if(FirstSocket && s==FirstSocket->Socket)
	{
		delete FirstSocket;
		FirstSocket = 0;
	}
    if(SecondSocket && s==SecondSocket->Socket)
	{
		delete SecondSocket;
		SecondSocket = 0;
		// First socket should be already deleted here. But just to be sure...
		delete FirstSocket;
		FirstSocket = 0;
	}
		
    return err;
}

int WINAPI hook_connect(SOCKET s, const struct sockaddr *name,
    int namelen)
{
    const sockaddr_in * inaddr =
        reinterpret_cast<const sockaddr_in *>(name);

    int err = connect(s, name, namelen);

	trace_printf("Connecting to: family:%d  %d.%d.%d.%d:%d\n",
		inaddr->sin_family, inaddr->sin_addr.S_un.S_un_b.s_b1,
		inaddr->sin_addr.S_un.S_un_b.s_b2,
	    inaddr->sin_addr.S_un.S_un_b.s_b3,
		inaddr->sin_addr.S_un.S_un_b.s_b4, ntohs(inaddr->sin_port));

	// ignore translation server and errors
/*
    if(err == SOCKET_ERROR || (ntohs(inaddr->sin_port) == 28888 && 
		inaddr->sin_addr.S_un.S_addr==0x0100007f))
    {
		if(err == SOCKET_ERROR)
			trace_printf("Failed connecting to: family:%d  %d.%d.%d.%d:%d\n",
				inaddr->sin_family, inaddr->sin_addr.S_un.S_un_b.s_b1,
				inaddr->sin_addr.S_un.S_un_b.s_b2,
	            inaddr->sin_addr.S_un.S_un_b.s_b3,
		        inaddr->sin_addr.S_un.S_un_b.s_b4, ntohs(inaddr->sin_port));

		if(FirstSocket && s==FirstSocket->Socket)
		{
			delete FirstSocket;
			FirstSocket = 0;
		}
		if(SecondSocket && s==SecondSocket->Socket)
		{
			delete SecondSocket;
			SecondSocket = 0;
		}
    }
*/
    if(err != SOCKET_ERROR && (ntohs(inaddr->sin_port) != 28888 || 
		inaddr->sin_addr.S_un.S_addr!=0x0100007f))
	{
		if(!FirstSocket && !LoginSocket::Redirecting)
		{
	        FirstSocket = new LoginSocket(s);
			if(SecondSocket)	// old clients "forgot" to close game socket on logout
			{
				delete SecondSocket;
				SecondSocket = 0;
			}
		}
		else if(!SecondSocket && LoginSocket::Redirecting)
	        SecondSocket = new GameSocket(s);
	}
    return err;
}

int WINAPI hook_recv(SOCKET s, char *buf, int len, int flags)
{
//    trace_printf(">> recv(%d, buf:%p, len:%d, f:0x%08X)\n", s, buf, len, flags);
	SocketHook *Sock=FindSocket(s);
    if(Sock)
        return Sock->recv(buf, len, flags);
    else 
        return ::recv(s, buf, len, flags);
}

int WINAPI hook_send(SOCKET s, /*const*/ char *buf, int len,
    int flags)
{
//    trace_printf(">> send(%d, buf:%p, len:%d, f:0x%08X)\n", s, buf, len, flags);

	SocketHook *Sock=FindSocket(s);

// Bug or feature? Sometimes when login and game servers both have the same IP
// 127.0.0.1 and computer is not connected to any network and OS is WinXP,
// client does not close its login socket and simply reuses it as a
// game socket (i.e. it switches encryption without closing socket). Sphere 
// server currently does not understand this "feature". I think that it
// is a client bug
	if(FirstSocket && Sock==FirstSocket && LoginSocket::Redirecting)
	{
		error_printf("Redirecting to the game server reusing the login socket!\n");

// To make this message disappear - install "Microsoft Loopback Adapter",
// specify there any IP address and reboot.
		MessageBox(0,"Unimplemented mode of redirecting to game server is used. Exiting.","Injection error",0);
		ExitProcess(1);
	}

    if(Sock)
        return Sock->send(buf, len, flags);
    else
        return ::send(s, buf, len, flags);
}

int WINAPI hook_select(int nfds, fd_set * readfds,
    fd_set * writefds, fd_set * exceptfds, const struct timeval * timeout)
{
    //trace_printf(">> select(%d, r:%p, w:%p, e:%p, timeout:%p)\n",
    //  nfds, readfds, writefds, exceptfds, timeout);
	int ret=::select(nfds, readfds, writefds, exceptfds,
            timeout); 

	if(FirstSocket)
	{
		if(FD_ISSET(FirstSocket->Socket,readfds))
		{
			FD_CLR(FirstSocket->Socket, readfds);
			FirstSocket->ReceiveData();
			ret--;
		}

		if(FirstSocket->HasData())
		{
			FD_SET(FirstSocket->Socket, readfds);
			ret++;
		}
	}

	if(SecondSocket)
	{
		if(FD_ISSET(SecondSocket->Socket,readfds))
		{
			FD_CLR(SecondSocket->Socket, readfds);
			SecondSocket->ReceiveData();
			ret--;
		}

		if(SecondSocket->HasData())
		{
			FD_SET(SecondSocket->Socket, readfds);
			ret++;
		}
	}

	return ret;
}

SOCKET WINAPI hook_socket(int af, int type, int protocol)
{
    SOCKET s = ::socket(af, type, protocol);

//    trace_printf(">> socket(%d, %d, %d) => %d\n", af, type, protocol, s);
    return s;
}



bool HookWinsockFunctions()
{
	return HookImportedFunction("wsock32.dll", "recv", 16, hook_recv) &&
		HookImportedFunction("wsock32.dll", "send", 19, hook_send) &&
		HookImportedFunction("wsock32.dll", "closesocket", 3, hook_closesocket) &&
		HookImportedFunction("wsock32.dll", "connect", 4, hook_connect) &&
		HookImportedFunction("wsock32.dll", "select", 18, hook_select) &&
		HookImportedFunction("wsock32.dll", "socket", 23, hook_socket);
}

void INJECTION_API SendToServer(const void IN *Buff, int Len)
{
	if(SecondSocket)
		SecondSocket->SendToServer((void*)Buff,Len);
	else if(FirstSocket)
		FirstSocket->SendToServer((void*)Buff,Len);
}

void INJECTION_API SendToClient(const void IN *Buff, int Len)
{
	if(SecondSocket)
		SecondSocket->SendToClient((void*)Buff,Len);
	else if(FirstSocket)
		FirstSocket->SendToClient((void*)Buff,Len);
}


// buggy old clients do not close sockets on exit. So we imitate closing manually
void CloseSockets()
{
	CommunicationLock tmp;
	if(SecondSocket)
		delete SecondSocket;
	if(FirstSocket)
		delete FirstSocket;
	FirstSocket = 0;
	SecondSocket = 0;
}
