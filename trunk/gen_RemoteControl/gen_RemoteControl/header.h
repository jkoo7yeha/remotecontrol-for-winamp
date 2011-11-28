#pragma once

/*
   Copyright Martin Schlodinski

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

// needed for winsock
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <Ws2tcpip.h>


#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <time.h>
#include <sstream>
#include <vcclr.h>
#include <queue>
#include <tchar.h>
#include <list>
#include <wchar.h>
#include <wctype.h>

// ATL INCLUDES
#include <shlobj.h>
#include <atlbase.h>
#include <atlconv.h>

// marshal_as library
#include <msclr/marshal.h>
#include <msclr\marshal_cppstd.h> 

// WINAMP SDK INCLUDES
#include "wa_ipc.h"
#include "gen.h"
#include "api.h"
#include "Winamp SDK\Agave\Queue\wa_jtfe.h"

// TAGLIB INCLUDES
#include <config.h>
#include <taglib_config.h>
#include <audioproperties.h>
#include <fileref.h>
#include <tag.h>
#include <taglib_export.h>
#include <tagunion.h>
#include <apefooter.h>
#include <apeitem.h>
#include <apetag.h>
#include <asfattribute.h>
#include <asffile.h>
#include <asfproperties.h>
#include <asftag.h>
#include <flacfile.h>
#include <flacproperties.h>
#include <mp4atom.h>
#include <mp4coverart.h>
#include <mp4file.h>
#include <mp4item.h>
#include <mp4properties.h>
#include <mp4tag.h>
#include <mpcfile.h>
#include <mpcproperties.h>
#include <mpegfile.h>
#include <mpegheader.h>
#include <mpegproperties.h>
#include <xingheader.h>
#include <id3v1genres.h>
#include <id3v1tag.h>
#include <id3v2extendedheader.h>
#include <id3v2footer.h>
#include <id3v2frame.h>
#include <id3v2framefactory.h>
#include <id3v2header.h>
#include <id3v2synchdata.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <commentsframe.h>
#include <generalencapsulatedobjectframe.h>
#include <popularimeterframe.h>
#include <privateframe.h>
#include <relativevolumeframe.h>
#include <textidentificationframe.h>
#include <uniquefileidentifierframe.h>
#include <unknownframe.h>
#include <unsynchronizedlyricsframe.h>
#include <urllinkframe.h>
#include <oggfile.h>
#include <oggpage.h>
#include <oggpageheader.h>
#include <xiphcomment.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <speexproperties.h>
#include <vorbisfile.h>
#include <vorbisproperties.h>
#include <rifffile.h>
#include <aifffile.h>
#include <aiffproperties.h>
#include <wavfile.h>
#include <wavproperties.h>
#include <taglib.h>
#include <tbytevector.h>
#include <tbytevectorlist.h>
#include <tdebug.h>
#include <tfile.h>
#include <tlist.h>
#include <tmap.h>
#include <tstring.h>
#include <tstringlist.h>
#include <unicode.h>
#include <trueaudiofile.h>
#include <trueaudioproperties.h>
#include <wavpackfile.h>
#include <wavpackproperties.h>


// server variables
extern volatile bool connecting, connected;

// winamp variables
extern volatile int shuffle_status, repeat_status, volume, volume_last, isPlaying, progress, playlistlength;

// settings variables
extern std::string settingsFileName;
extern std::string settingsPath;

extern volatile int port, autostart, showconfigonstartup, autorestart, keepalivemessages; 

// sockets
extern volatile int c, s;



// socket adress
extern struct sockaddr_in addr;

// critical winamp variables section
extern CRITICAL_SECTION cs_winamp;

// critical keep alive section
extern CRITICAL_SECTION cs_alive;

// keep alive message delay
extern volatile int alive_delay;

// old callback hook
extern WNDPROC lpWndProcOld;


// plugin version (don't touch this)
#define GPPHDR_VER 0x10
 

// NOTE: when creating a new plugin this must be changed as it will otherwise cause conflicts!
// {F6FFE370-C7F3-4937-B8E5-C348216CD7D8}
extern const GUID GenQueueExampleLangGUID;


// Wasabi based services for localisation support
extern api_service *WASABI_API_SVC;
extern api_language *WASABI_API_LNG;
// these two must be declared as they're used by the language api's
// when the system is comparing/loading the different resources
extern HINSTANCE WASABI_API_LNG_HINST,
                 WASABI_API_ORIG_HINST;

extern api_queue *WASABI_API_QUEUEMGR;
#define CHECK_QUEUEMGR() \
		if(WASABI_API_QUEUEMGR == NULL){ServiceBuild(WASABI_API_QUEUEMGR,QueueManagerApiGUID);}


extern UINT_PTR delay_load_ipc;

template <class api_T>
void ServiceBuild(api_T *&api_t, GUID factoryGUID_t)
{
	if (WASABI_API_SVC)
	{
		waServiceFactory *factory = WASABI_API_SVC->service_getServiceByGuid(factoryGUID_t);
		if (factory)
			api_t = (api_T *)factory->getInterface();
	}
}


#define IDS_NULLSOFT_LANG_EXAMPLE       1
#define IDS_DO_YOU_ALSO_WANT_TO_REMOVE_SETTINGS 2
#define IDS_ABOUT_MESSAGE               3
#define IDS_QUEUE_ENABLED               3
#define IDS_ABOUT_TITLE                 4
#define IDS_QUEUE_DISABLED              4
#define IDD_DIALOG1                     101
#define IDC_ABOUT                       1000
#define IDC_EXAMPLE                     1001
#define IDC_CLEAR_QUEUE                 1002
#define IDC_BUTTON2                     1003
#define IDC_RANDOMISE_QUEUE             1003
#define IDC_ENABLE_QUEUE                1004
#define IDC_REFRESH                     1005
#define IDC_QUEUE_ENABLED               1006
#define IDC_CLEAR_QUEUE2                1007
#define IDC_QUEUE_FIRST                 1007

// JTFE 1.2.4 commands
#define IPC_GET_JTFE_QUEUE_UPDATE(hwnd_winamp) \
SendMessage(hwnd_winamp,WM_WA_IPC,(WPARAM)&"genjtfe_queue",IPC_REGISTER_WINAMP_IPCMESSAGE);

#define QUEUE_ADD 0 // added item(s) to the queue
#define QUEUE_CLEAR 1 // full clearance of the queue
#define QUEUE_REMOVE 2 // removed item(s) from the queue
#define QUEUE_RANDOMISE 3 // contents of the queue were randomised
#define QUEUE_MOVE 4 // item(s) in the queue changed position
#define QUEUE_MISC 5 // possible multiple changes of the queue e.g. via drag and drop

// pointer to jtfe plugin
extern UINT_PTR genjtfe_queue;


// main structure with plugin information, version, name...
typedef struct {
  int version;                   // version of the plugin structure
  char *description;             // name/title of the plugin 
  int (*init)();                 // function which will be executed on init event
  void (*config)();              // function which will be executed on config event
  void (*quit)();                // function which will be executed on quit event
  HWND hwndParent;               // hwnd of the Winamp client main window (stored by Winamp when dll is loaded)
  HINSTANCE hDllInstance;        // hinstance of this plugin DLL. (stored by Winamp when dll is loaded) 
} RemoteControlPlugin;

extern RemoteControlPlugin plugin;

// definitions to identify UIAction: add log text or change status text in UI?
#define LOG 1
#define STATUS 2
#define BUTTON 3
#define IP 4
#define VERSION 5

// window visibility
extern volatile bool windowVisible;

// namespaces
using namespace System::Threading;
using namespace gen_RemoteControl;