#pragma once

struct StartClientParams
{
	char ClientDir[MAX_PATH];
	char Executable[MAX_PATH];
	char MulsDir[MAX_PATH];
	char ServerAddress[MAX_PATH];
};

UINT ClientStarterThreadProc( LPVOID pParam );

enum ClientStartState {
	CS_Idle,		// Client starter thread not created
	CS_Starting,	// Starting client
	CS_Successful,	// Started client successfully
	CS_Error		// Got error
};

extern ClientStartState ClientStarterThreadState;

extern CString ErrorDetails;
