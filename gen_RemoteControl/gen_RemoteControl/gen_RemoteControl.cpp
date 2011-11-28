// gen_RemoteControl.cpp: Hauptprojektdatei.

#include "stdafx.h"

/**
* \brief	init
*
* initializes the plugin: checks winamp version, creates new UI, loads services, gets settings folder, reads settings and creates critical sections.
* if settings are set: shows UI and starts server. must return 0 or plugin is not loaded.
*
* \return	0 if success, 1 if error
*/
int init() {
	
	// winamp version 5.5+
	if(SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETVERSION) >= 0x5050) {

		// initialize GUI
		UIManager::setUI(gcnew gen_RemoteControl::UI());

		// load wasabi services
		WASABI_API_SVC = (api_service*)SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_API_SERVICE);
		if (WASABI_API_SVC == (api_service*)1) 
			WASABI_API_SVC = NULL;

		if(WASABI_API_SVC != NULL) {
			ServiceBuild(WASABI_API_LNG,languageApiGUID);

			WASABI_API_START_LANG(plugin.hDllInstance,GenQueueExampleLangGUID);
		}
		else
			UIManager::addLogText("Could not load wasabi services!\r\n");
		

		// check JTFE version (1.2.4 necessary for QUEUE_X commands)
		UINT jtfeVer = SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GET_JTFE_VERSION);

		if (jtfeVer >= 0x1020040) {
			genjtfe_queue = IPC_GET_JTFE_QUEUE_UPDATE(plugin.hwndParent);
		}
		else
			UIManager::addLogText("JTFE 1.2.4 necessary to recognize \"jump to file\"-commands. Update Winamp or manually install new JTFE version.\r\n\r\n");

		// get settings folder
		TCHAR szPath[200];
		SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szPath );
		USES_CONVERSION;
		settingsPath = string(T2A(szPath));
		settingsPath += string("\\Winamp\\");
		settingsPath += settingsFileName;

		// read current settings
		if (readSettings() == 1)
			UIManager::addLogText("Could not read settings!\r\n");

		// initialize critical sections
		InitializeCriticalSection(&cs_alive);
		InitializeCriticalSection(&cs_winamp);

		// show UI?
		if (showconfigonstartup == 1)
			UIManager::showUI(true);

		// start server?
		if (autostart == 1)
			startServer();
	}
	
  return 0;
}


/**
* \brief	quit
*
* quits the plugin: hides UI, stops server and deletes critical sections
*
*/
void quit() {
	// fast close
	UIManager::showUI(false);


	// stop server
	stopServer(false);


	// delete critical sections
	DeleteCriticalSection(&cs_alive);
	DeleteCriticalSection(&cs_winamp);
}


/**
* \brief	config
*
* executed when plugin information is opened. shows UI.
*
*/
void config() {
	UIManager::showUI(true);
}


/**
* \brief	winampGetGeneralPurposePlugin
*
* exports this plugin for winamp
*
*/
extern "C" __declspec(dllexport) RemoteControlPlugin * winampGetGeneralPurposePlugin() {
  return &plugin;
}

/**
* \brief	winampUninstallPlugin
*
* exports the winampUninstallPlugin function for winamp. when plugin is uninstalled the settings file is deleted, UI is hided and winamp is restarted.
*
*/
extern "C" __declspec( dllexport ) int winampUninstallPlugin(HINSTANCE hDllInst, HWND hwndDlg, int param) {
	// delete config file
	remove(settingsPath.c_str());
	
	UIManager::showUI(false);

	// 0x0: winamp needs to be restarted
	return 0x0;
}


/**
* \brief	MainWndProc
*
* callback hook that is called when client is connected and winamp actions are performed. 
* enqueues the command in the tasklist and returns to original winamp function.
*/
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_WA_IPC) {
        if (lParam == delay_load_ipc) {
            CHECK_QUEUEMGR();
        }  else if (lParam == 636) { // item has just finished playback or next button is pressed
            int QueueLength = WASABI_API_QUEUEMGR->GetNumberOfQueuedItems();

            if (QueueLength != 0)   // this is next element in queue
				tasklist.push("queue_next");	// tell app to update queue
        } else if (lParam == IPC_PLAYING_FILE) {	// begin playing
            CallWindowProc(lpWndProcOld,hwnd,message,wParam,lParam);

			tasklist.push("new_song_");
            
            return 0;
        } else if (lParam == IPC_SETVOLUME && wParam != -666) {	// volume changed
            // CRITICAL
			EnterCriticalSection(&cs_winamp);

			volume = wParam;

			LeaveCriticalSection(&cs_winamp);
			// CRITICAL END

            tasklist.push("volume_");
        } else if (lParam == IPC_JUMPTOTIME) {	// position in track changed
            // CRITICAL
			EnterCriticalSection(&cs_winamp);

			progress = wParam;

			LeaveCriticalSection(&cs_winamp);
			// CRITICAL END
			
            tasklist.push("progress_");
        } else if (lParam == IPC_PLAYLIST_MODIFIED) {	// playlist modified
			UIManager::addLogText("Playlist out of sync!\r\n");
        } else if (lParam == genjtfe_queue) {
			if (wParam == QUEUE_ADD || wParam == QUEUE_CLEAR || wParam == QUEUE_REMOVE || wParam == QUEUE_RANDOMISE || wParam == QUEUE_MOVE || wParam == QUEUE_MISC) {
				// sync queue lists
				tasklist.push("queueList");
			}
		}

    } else if (message == WM_COMMAND || message == WM_SYSCOMMAND) {
        if (wParam == 40048 || wParam == 40044) {	// next or previous button pressed
            // CRITICAL
			EnterCriticalSection(&cs_winamp);

			isPlaying = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

			LeaveCriticalSection(&cs_winamp);
			// CRITICAL END
			
            CallWindowProc(lpWndProcOld,hwnd,message,wParam,lParam);

            // if not playing
            if (isPlaying != 1) {
                tasklist.push("new_song_");

                if (wParam == 40048)	// if next pressed
                    if (WASABI_API_QUEUEMGR->GetNumberOfQueuedItems() != 0) // this is next element in queue
                        tasklist.push("queue_next");	// tell app to update queue
            }

            return 0;
        } else if (wParam == 40045) {	// play button pressed
			isPlaying = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

			if (isPlaying == 3)
                tasklist.push("new_song_");     // NEEDED FOR PAUSE -> PLAY
        } else if (wParam == 40046) {	// pause button pressed
                tasklist.push("pause");
        } else if (wParam == 40047) {	// stop button pressed
                tasklist.push("stop");
        } else if (wParam == 40023) {	// shuffle button pressed
            if (shuffle_status == 1) {
                    // CRITICAL
					EnterCriticalSection(&cs_winamp);

					shuffle_status = 0;

					LeaveCriticalSection(&cs_winamp);
					// CRITICAL END
					
					tasklist.push("shuffle_0");
            } else if (shuffle_status == 0) {
                    // CRITICAL
					EnterCriticalSection(&cs_winamp);

					shuffle_status = 1;

					LeaveCriticalSection(&cs_winamp);
					// CRITICAL END

					tasklist.push("shuffle_1");
            }
        } else if (wParam == 40022) {	// repeat button pressed
            if (repeat_status == 1) {
					// CRITICAL
					EnterCriticalSection(&cs_winamp);

					repeat_status = 0;

					LeaveCriticalSection(&cs_winamp);
					// CRITICAL END

					tasklist.push("repeat_0");
            } else if (repeat_status == 0) {
                    // CRITICAL
					EnterCriticalSection(&cs_winamp);

					repeat_status = 1;

					LeaveCriticalSection(&cs_winamp);
					// CRITICAL END

					tasklist.push("repeat_1");
            } 
        }
    } else if (message == WM_MOUSEWHEEL) {	// volume changed with mouse wheel
        // CRITICAL
		EnterCriticalSection(&cs_winamp);

		volume = SendMessageA(plugin.hwndParent, WM_WA_IPC, -666, IPC_SETVOLUME);

		LeaveCriticalSection(&cs_winamp);
		// CRITICAL END

        tasklist.push("volume_");
    }

    return CallWindowProc(lpWndProcOld,hwnd,message,wParam,lParam);
}
