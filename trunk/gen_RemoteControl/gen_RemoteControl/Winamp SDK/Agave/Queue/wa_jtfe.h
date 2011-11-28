#ifndef WA_JTFE
#define WA_JTFE

/*
** wa_jtfe.h by Darren Owen (aka DrO)
**
** This header is for those who want to make use of the features provided by the
** Jump to File Extra (JTFE) plugin
**
** Header and Supported Plugin version:
**
** v0.1 (0.96f+)      Initial release (not really much at all)
**
**
** ----------------------------------------------------------------------------
**
**
** How to use the JTFE api:
**
** To get the plugin callbacks you need to get the callback message identifier.
** This is done using the following code:
**
** genjtfe_ipc = SendMessage(winampWindow,WM_WA_IPC,(WPARAM)&"GenJTFE",IPC_REGISTER_WINAMP_IPCMESSAGE);
**
** Then for the callbacks you will need to subclass the main winamp window and
** do the following to catch the messages:
**
** if(umsg == genjtfe_ipc){
**    switch(lparam){
**       case IPC_XXXXX:
**         // do stuff
**       break;
**    }
** }
**
** Note: this requires Winamp 5.x to be available otherwise it will fail since
**       versions of Winamp below this don't support IPC_REGISTER_WINAMP_IPCMESSAGE
**       and also a supporting version of the JTFE plugin
**
**
** ----------------------------------------------------------------------------
**
**
** IPC Defines:
**
** The format for the message callbacks / messages to be received / sent is as follows
**
** SendMessage(winampWindow,genjtfe_ipc,extra_info,message_id);
*/

#define IPC_GET_JTFE_VERSION        0x000   // (not implemented)
/* (not implemented in v0.1)
** Send this to get the current version of JTFE being used
**
** extra_info = 0 -> returns integer formatted as 0xA0BB0CD (eg 0x00960F0)
**                A == major version     eg  1.yz
**                B == minor version     eg  x.96z
**                C == sub version       eg  x.xFz
**                D == sub sub version   eg  x.xyA (not often used)
**
** extra_info = 1 -> returns a nice formatted string of the plugin version
*/

#define IPC_QUEUED_TRACK_STARTED    0x001
/* (implemented in 0.1+)
** This callback is sent just as JTFE is about to make Winamp play a queued file
**
** extra_info = index of the track in the playlist to be played
*/

#endif