#include "stdafx.h"


// thread handles
volatile HANDLE waitingForClientThread;
volatile HANDLE keepAliveMessagesThread;
volatile HANDLE waitingForCommandThread;
volatile HANDLE sendCommandThread;


/**
* \brief	waitingForClient
*
* waits for a client to connect. is successful, starts synchronization and waitingForCommand, keepAliveMessages threads and enables MainWndProc hook
*
*/
DWORD WINAPI waitingForClientFunction(LPVOID parameter) {
	if ((c = accept(s, NULL, NULL)) == -1 && connecting == true) {	// if "connecting" is false then waiting for client has been aborted
		
		////////////////////////////////////////// UI ///////////////////////////////////////////////////////
		UIManager::addLogText("Could not accept client\r\n");
		UIManager::setStatusText("Disconnected");
		UIManager::setButtonText("Start server");
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		connecting = false;
		
		return -1;
	}
	
	// synchronize, then start waiting for commands

	UIManager::addLogText("Connected\r\n");


	if (synchronize() == 0) {
		////////////////////////////////////////// UI ///////////////////////////////////////////////////////
		UIManager::setStatusText("Connected");
		UIManager::setButtonText("Disconnect");
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if (keepalivemessages == 1)
			keepAliveMessagesThread = CreateThread(NULL, 0, keepAliveMessagesFunction, 0, 0, NULL);

        waitingForCommandThread = CreateThread(NULL, 0, waitingForCommandFunction, 0, 0, NULL);
        sendCommandThread = CreateThread(NULL, 0, sendCommandFunction, 0, 0, NULL);

		// enable MainWndProc callback hook
		if (IsWindowUnicode(plugin.hwndParent)) {
			lpWndProcOld = (WNDPROC)SetWindowLongPtrW(plugin.hwndParent,GWLP_WNDPROC,(LONG)MainWndProc);
		} else {
			lpWndProcOld = (WNDPROC)SetWindowLongPtrA(plugin.hwndParent,GWLP_WNDPROC,(LONG)MainWndProc);
		}

		connected = true;

	} else {
		UIManager::setStatusText("Disconnected");
		UIManager::setButtonText("Start server");
	}

	connecting = false;

	return 0;
}

/**
* \brief	sendCommand
*
* waits for elements to be inserted into tasklist queue, extracts them and sends these commands to the app. every call to rawSend must come from this thread!
*
*/
DWORD WINAPI sendCommandFunction(LPVOID parameter)
{
	std::string element;
	element = tasklist.pop();

	while (1) {
		if (element.compare("") != 0) {
			if (element.compare("cover") == 0)
				sendCover("", -1);
			else if (element.compare("track_info") == 0)
				sendTrackInfo(tasklist.getParameter());
			else if (element.compare("queueList") == 0)
				refreshQueueListFunction();
			else {
				int result;

				element += "\n";

				// send
				result = send(c,element.c_str(),element.length(),0);

				if (result == -1)
					UIManager::addLogText("Could not send data\r\n");
			}

		}

		// next element
		element = tasklist.pop();
	}
}


/**
* \brief	waitingForCommand
*
* waits commands from the client and performs corresponding winamp-actions. if client has disconnected, 
* stops the server and restars server if setting is set
*
*/
DWORD WINAPI waitingForCommandFunction(LPVOID parameter) {
	char *buf = new char[256];
    int bytes=1;

	// wait for first command
	bytes = recv(c, buf, 256, 0);

	while (bytes > 0) {
		if (bytes == SOCKET_ERROR) {
			UIManager::addLogText("Receipt error\r\n");

			delete [] buf;

			////////////////////////////////////////// UI ///////////////////////////////////////////////////////
			UIManager::setStatusText("Disconnected");
			UIManager::setButtonText("Start server");
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			if (connected == true)
				stopServer(true);

			if (autorestart == 1)
				startServer();

			return 0;
		} else {
			buf[bytes] = '\0'; // securely terminate char*

			if (strcmp(buf, "alive") == 0) {	// heartbeat received
				// CRITICAL
				EnterCriticalSection(&cs_alive);

				alive_delay = 0;

				// CRITICAL END
				LeaveCriticalSection(&cs_alive);
			} else if (strcmp(buf, "destroy") == 0) {
				delete[] buf;

				// disable title sending function
				if( lpWndProcOld )
					SetWindowLongPtr(plugin.hwndParent, GWL_WNDPROC, (LONG)lpWndProcOld); 

				break;
			} else if (strcmp(buf, "previous") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40044, 0);
			else if (strcmp(buf, "play") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40045, 0); 
			else if (strcmp(buf, "pause") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40046, 0);
			else if (strcmp(buf, "stop") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40047, 0);
			else if (strcmp(buf, "next") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40048, 0);
			else if (strcmp(buf, "shuffle") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40023, 0);
			else if (strcmp(buf, "repeat") == 0)
				SendMessageA(plugin.hwndParent, WM_COMMAND, 40022, 0);
			else if (strcmp(buf, "mute") == 0) {
				if (SendMessageA(plugin.hwndParent, WM_WA_IPC, -666, IPC_SETVOLUME) == 0)	// was already muted
					SendMessageA(plugin.hwndParent, WM_WA_IPC, volume_last, IPC_SETVOLUME);	// reset volume
				else {
					// CRITICAL
					EnterCriticalSection(&cs_winamp);

					volume_last = SendMessageA(plugin.hwndParent, WM_WA_IPC, -666, IPC_SETVOLUME);

					// CRITICAL END
					LeaveCriticalSection(&cs_winamp);

					SendMessageA(plugin.hwndParent, WM_WA_IPC, 0, IPC_SETVOLUME);	// mute winamp
				}
			} else if (strstr (buf,"volume_") != NULL) {
				char *anfang = strstr (buf,"_") + sizeof(char);

				SendMessageA(plugin.hwndParent, WM_WA_IPC, atoi(anfang), IPC_SETVOLUME);
			} else if (strstr (buf,"progress_") != NULL) {	// change position within track
				char *position = strstr (buf,"_") + sizeof(char);

				SendMessage(plugin.hwndParent, WM_WA_IPC, atoi(position), IPC_JUMPTOTIME);
			} else if (strstr (buf,"playlistitem_") != NULL) {	// change played title
				char *position = strstr (buf,"_") + sizeof(char);

				if (SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_ISPLAYING) == 3)	// if paused make sure to continue with selected track
					SendMessageA(plugin.hwndParent, WM_COMMAND, 40047, 0);

				// set position in playlist
				SendMessage(plugin.hwndParent, WM_WA_IPC, atoi(position), IPC_SETPLAYLISTPOS);
				// play selected title
				SendMessage(plugin.hwndParent,WM_COMMAND,MAKEWPARAM(40045,0),0);
			} else if (strstr (buf,"enqueue_") != NULL) {	// add title to queue
				char *position = strstr (buf,"_") + sizeof(char);

				WASABI_API_QUEUEMGR->AddItemToQueue(atoi(position),1,0);
			} else if (strstr (buf,"remqueue_") != NULL) {	// remove title from queue
				char *position = strstr (buf,"_") + sizeof(char);

				WASABI_API_QUEUEMGR->RemoveQueuedItem(atoi(position));
			} else if (strstr (buf,"trackInfo_") != NULL) {	// show track information
				char *position = strstr (buf,"_") + sizeof(char);

				int track = atoi(position);

				tasklist.push("track_info", track);
			}

		}

		if (buf != NULL)
			delete [] buf;
		buf = new char[256];

		// get new command
		bytes = recv(c, buf, 256, 0);
	}

	// stream closed if bytes == 0
	
	delete[] buf;
	
	if(connected == true) {
		stopServer(true);

	if (autorestart == 1)
		startServer();
	}

	return 0;
}

/**
* \brief	keepAliveMessages
*
* sends keep alive messages to the client to see if it has still connection
*
*/
DWORD WINAPI keepAliveMessagesFunction(LPVOID parameter) {
	alive_delay = 0;

	while (connected == true) {
		Sleep(5000);	// wait 5 seconds

		// CRITICAL
		EnterCriticalSection(&cs_alive);

		if (alive_delay == 3) {	// if 3 not arrived messages stop server
			// CRITICAL END
			LeaveCriticalSection(&cs_alive);

			UIManager::addLogText("Connection lost\r\n\r\n");

			stopServer(false);

			if (autorestart == 1)
				startServer();

			return 0;
		}


		tasklist.push("alive");

		alive_delay++;

		// CRITICAL END
		LeaveCriticalSection(&cs_alive);
	}

	return 0;
}


/**
* \brief	refreshQueueListsFunction
*
* refreshs the whole queue list on the client
*/
void refreshQueueListFunction() {
	/////////////////////////////// QUEUE LIST COUNT ////////////////////////////////////

	int queueCount = WASABI_API_QUEUEMGR->GetNumberOfQueuedItems();
	
	stringstream queueCountStream;
	queueCountStream << "queueRefresh_";
	queueCountStream << queueCount;

	if (rawSend(queueCountStream.str().c_str()) != 0) {
		UIManager::addLogText("synchronizing queue list failed!\r\n");

		return;
	}
	
	/////////////////////////////// QUEUE LIST ELEMENTS ////////////////////////////////////
	
	int k, tmp;
	stringstream queueElementStream;

	for (k=0; k < queueCount; k++) {
	
		tmp = WASABI_API_QUEUEMGR->GetQueuedItemFromIndex(k);

		queueElementStream << tmp;

		if (rawSend(queueElementStream.str().c_str()) != 0) {
			UIManager::addLogText("Synchronizing queue list failed!\r\n");

			return;
		}
	
		// clear stringstream
		queueElementStream.str("");
	}
}