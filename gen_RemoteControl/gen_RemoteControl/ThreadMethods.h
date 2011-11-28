#pragma once
#include "stdafx.h"

extern DWORD WINAPI waitingForClientFunction(LPVOID parameter);
extern DWORD WINAPI sendCommandFunction(LPVOID parameter);
extern DWORD WINAPI waitingForCommandFunction(LPVOID parameter);
extern DWORD WINAPI keepAliveMessagesFunction(LPVOID parameter);

extern volatile HANDLE waitingForClientThread;
extern volatile HANDLE keepAliveMessagesThread;
extern volatile HANDLE waitingForCommandThread;
extern volatile HANDLE sendCommandThread;

extern void refreshQueueListFunction();

extern volatile HANDLE checkForNewVersionThread;