// stdafx.cpp : Quelldatei, die nur die Standard-Includes einbindet.
// gen_RemoteControl.pch ist der vorkompilierte Header.
// stdafx.obj enthält die vorkompilierten Typinformationen.

#include "stdafx.h"


// server variables
volatile bool connecting = false, connected = false;

// winamp variables
volatile int shuffle_status, repeat_status, volume, volume_last, isPlaying, progress, playlistlength;

// settings variables
std::string settingsFileName = "RemoteControl.dat";
std::string settingsPath;

volatile int port = 50000, autostart = 1, showconfigonstartup = 1, autorestart = 1, keepalivemessages = 1; 

// sockets
volatile int c, s;

// socket adress
struct sockaddr_in addr;

// keep alive message delay
volatile int alive_delay = 0;


// critical sections
CRITICAL_SECTION cs_alive;
CRITICAL_SECTION cs_winamp;


volatile HANDLE refreshQueueListThread;

volatile HANDLE checkForNewVersionThread;

// NOTE: when creating a new plugin this must be changed as it will otherwise cause conflicts!
// {F6FFE370-C7F3-4937-B8E5-C348216CD7D8}
const GUID GenQueueExampleLangGUID = 
{ 0xf6ffe370, 0xc7f3, 0x4937, { 0xb8, 0xe5, 0xc3, 0x48, 0x21, 0x6c, 0xd7, 0xd8 } };


// Wasabi based services for localisation support
api_queue *WASABI_API_QUEUEMGR = 0;

UINT_PTR delay_load_ipc = -1;

api_service *WASABI_API_SVC = 0;
api_language *WASABI_API_LNG = 0;

WNDPROC lpWndProcOld = NULL;

HINSTANCE WASABI_API_LNG_HINST = 0,
          WASABI_API_ORIG_HINST = 0;

// pointer to jtfe plugin
UINT_PTR genjtfe_queue = NULL;

// plugin instance
RemoteControlPlugin plugin = {
  GPPHDR_VER,  // version of the plugin, defined in "gen_myplugin.h"
  PLUGIN_NAME, // name of the plugin, defined in "gen_myplugin.h"
  init,        // function name which will be executed on init event
  config,      // function name which will be executed on config event
  quit,        // function name which will be executed on quit event
  0,           // handle to Winamp main window, loaded by winamp when this dll is loaded
  0            // hinstance to this dll, loaded by winamp when this dll is loaded
};

// Tasklist object
TaskList tasklist;

// window visibility
bool volatile windowVisible;