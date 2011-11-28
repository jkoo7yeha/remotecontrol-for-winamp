#include "stdafx.h"

/**
* \brief	startServer
*	
* starts the RemoteControl server: initializes playlist queue, reads settings from settings file, starts socket+waitingForClient thread
*/
void startServer() {
	connecting = true;


	// initialize playlist queue
	if(!WASABI_API_QUEUEMGR) 
		ServiceBuild(WASABI_API_QUEUEMGR,QueueManagerApiGUID);

	//if(WASABI_API_QUEUEMGR)
	//	WASABI_API_QUEUEMGR->ClearQueue();	// Clear the queue (if one exists)

	
	////////////////////////////////////////// UI ///////////////////////////////////////////////////////
	UIManager::setButtonText("Abort");
	UIManager::addLogText("Starting server on port " + port + "\r\n");
	
	UIManager::setStatusText("Starting socket...");
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	// get current settings
	if (readSettings() == 1) {
		UIManager::addLogText("Could not read settings!\r\n");
		return;
	}


	// start socket
	if (startSocket() == 1) {
		// ERROR

		connecting = false;

		////////////////////////////////////////// UI ///////////////////////////////////////////////////////
		UIManager::setStatusText("Disconnected");
		UIManager::setButtonText("Start server");
		/////////////////////////////////////////////////////////////////////////////////////////////////////
	} else {
		////////////////////////////////////////// UI ///////////////////////////////////////////////////////
		UIManager::setStatusText("Waiting for client...");
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// wait for client
		waitingForClientThread = CreateThread(NULL, 0, waitingForClientFunction, 0, 0, NULL);
	}

}

/**
* \brief	stopServer
*
* stops the server, kills threads, disables MainWndProc callback hook
*
* \return showLogMessage shows log message "disconnected" if true
*/
void stopServer(bool showLogMessage) {
	connected = false;
	
	// kill threads if running. DON'T KILL keepAliveMessagesThread OR CLIENT CONNECTION LOSE WON'T BE RECOGNIZED
	TerminateThread(waitingForClientThread,0);
	TerminateThread(sendCommandThread,0);
	
	// shutdown socket
	shutdownSocket();
		
	////////////////////////////////////////// UI ///////////////////////////////////////////////////////
	UIManager::setStatusText("Disconnected");
	UIManager::setButtonText("Start server");

	if (showLogMessage == true)
		UIManager::addLogText("Disconnected\r\n\r\n");
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	// disable MainWndProc callback hook
	if( lpWndProcOld )
		SetWindowLongPtr(plugin.hwndParent, GWL_WNDPROC, (LONG)lpWndProcOld); 
}

/**
* \brief	startSocket
*
* start winsock (WSA), create+bind+listen socket and displays local IP address
*
*
* \return	1 if error, 0 if success
*/
int const startSocket() {

	int err = startWinsock();

	if(err != 0) {
		UIManager::addLogText("Could not start Winsock. Error code " + err + "\r\n");

		return 1;
	}

	// create socket

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		UIManager::addLogText("Could not create socket\r\n");

		stopServer(true);

		return 1;
	}


	// bind socket

	addr.sin_addr.s_addr = htonl(INADDR_ANY);      /* e.g. inet_addr("127.0.0.1"); */
	addr.sin_port = htons(port);             /* e.g. htons(80);              */
	addr.sin_family = AF_INET;


	if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		UIManager::addLogText("Port already used\r\n");

		stopServer(true);

		return 1;
	}


	// listen for incomming connection

	if (listen(s, SOMAXCONN) == -1) {
		UIManager::addLogText("Too many pending connection requests\r\n");

		stopServer(true);

		return 1;
	}


	// check and display local IP address
	System::String^ address = GetLocalIP();
	
	if(address->Equals("127.0.0.1\r\n") || address->Equals(""))
		UIManager::setIP("No network connection");
	else
		UIManager::setIP(address);

	return 0;
}


/**
* \brief	shutdownSocket
*
* shuts down the socket and cleans up WSA
*
*/
void shutdownSocket() {

	// close connection
	shutdown(c, 2);
	shutdown(s, 2);

	closesocket(c);
	closesocket(s);

	// clean up WSA
	WSACleanup();
}


/**
* \brief	startWinsock
*
* initializes WSA
*
* \return	0 if success, else error code
*/
int const startWinsock() {
  WSADATA wsa;

  return WSAStartup(MAKEWORD(2,0),&wsa);
}



/**
* \brief	rawSend
*
* sends a raw std::string and adds \n
*
* \param	const char *parameter		
*
* \return	1 if error, 0 if success
*/
int const rawSend(const char *parameter) {
	int result;

	string parameterFull(parameter);
	parameterFull.append("\n");

	// send
	result = send(c,parameterFull.c_str(),parameterFull.length(),0);

	// check if successfully sent
	if (result == -1) {
		UIManager::addLogText("Could not send data\r\n");

		return 1;
	}

	return 0;
}

/**
* \brief	sendCover
*
* sends the cover length (bits) and the cover data. to keep socket thread safe only access via TaskList!
*
* \param prefix prefix for every coverlength string
* \param number track number of file to read. currently playing track if -1
*
* \return	0 if success, 1 if error
*/
int const sendCover(const char* prefix, const int & number) {
	string prefixStr(prefix);

	unsigned int coverLength = 0;
	char* data = NULL;

	string fullPath;

	// if -1 then currently playing track. else select track number from playlist
	if (number == -1)
		fullPath = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC,SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS),IPC_GETPLAYLISTFILE);
	else
		fullPath = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC,number,IPC_GETPLAYLISTFILE);

	// get file extension
	string extension(GetFileExtension(fullPath));

	// low character file extension
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower); 
	
	if (extension.compare("mp3") == 0) {
		TagLib::MPEG::File file(fullPath.c_str());

		TagLib::ID3v2::Tag *id3v2tag;

		if(file.isValid()) {
			try {
				id3v2tag = file.ID3v2Tag();
			} catch (...) {
				prefixStr += "coverLength_0";

				if (rawSend(prefixStr.c_str()) != 0)
					UIManager::addLogText("Sending cover info failed!\r\n");

				return 1;
			}

			if(id3v2tag) {
				if(!file.ID3v2Tag()) {	// file open+readable, has id3v2 tag
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}

				TagLib::ID3v2::FrameList l = file.ID3v2Tag()->frameList("APIC");	// APIC: attached picture frame

				if(l.isEmpty()) {
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}

				TagLib::ID3v2::AttachedPictureFrame *f3 = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());	// front: first item

				try {
					coverLength = f3->picture().size(); // size in bytes
				} catch (...) {
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}

				if (coverLength > 0) {	// PICTURE DATA FOUND: copy picture data
					data = new char[coverLength];
					memcpy(data, f3->picture().data(), coverLength);
				} else {
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}
			} else {
				prefixStr += "coverLength_0";

				if (rawSend(prefixStr.c_str()) != 0)
					UIManager::addLogText("Sending cover info failed!\r\n");

				return 1;
			}
		} else {
			prefixStr += "coverLength_0";

			if (rawSend(prefixStr.c_str()) != 0)
				UIManager::addLogText("Sending cover info failed!\r\n");

			return 1;
		}
	} else if (extension.compare("flac") == 0) {
		TagLib::FLAC::File file(fullPath.c_str());

		if(file.isValid()) {
			
			TagLib::List<TagLib::FLAC::Picture *> pictureList = file.pictureList();

			if (!pictureList.isEmpty()) {
				TagLib::FLAC::Picture *f3 = static_cast<TagLib::FLAC::Picture *>(pictureList.front());
				
				try {
					coverLength = f3->data().size(); // size in bytes
				} catch (...) {
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}

				if (coverLength > 0) {	// PICTURE DATA FOUND: copy picture data
					data = new char[coverLength];
					memcpy(data, f3->data().data(), coverLength);
				} else {
					prefixStr += "coverLength_0";

					if (rawSend(prefixStr.c_str()) != 0)
						UIManager::addLogText("Sending cover info failed!\r\n");

					return 1;
				}
			} else {
				prefixStr += "coverLength_0";

				if (rawSend(prefixStr.c_str()) != 0)
					UIManager::addLogText("Sending cover info failed!\r\n");

				return 1;
			}
		} else {
			prefixStr += "coverLength_0";

			if (rawSend(prefixStr.c_str()) != 0)
				UIManager::addLogText("Sending cover info failed!\r\n");

			return 1;
		}
	}
	else
	{
		prefixStr += "coverLength_0";

		if (rawSend(prefixStr.c_str()) != 0)
			UIManager::addLogText("Sending cover info failed!\r\n");

		return 1;
	}

	
	// LENGTH
	stringstream coverlengthStream;
	coverlengthStream << prefixStr;
	coverlengthStream << "coverLength_";
	coverlengthStream << coverLength;

	// send
	if (rawSend(coverlengthStream.str().c_str()) != 0)
	{
		UIManager::addLogText("Sending cover info failed!\r\n");
		
		if (data != NULL)
			delete [] data;

		return 1;
	}



	if (coverLength > 0)
	{
		// COVER

		bool carry = false;
		int result, count, maxCount = coverLength / 1024;	// 1024 byte segments
		if((coverLength % 1024) != 0) {	// count needed steps
			maxCount++;
			carry = true;
		}



		for(count = 0; count < maxCount; count++) {
			if(count == maxCount-1 && carry == true) {
				// send with offset
				result = send(c, data+count*1024, (coverLength % 1024), 0);
			
				if (result == -1) {
					UIManager::addLogText("Sending cover info failed!\r\n");

					if (data != NULL)
						delete [] data;

					return 1;
				}
			} else {
				result = send(c, data+count*1024, 1024, 0);
			
				if (result == -1) {
					UIManager::addLogText("Sending cover info failed!\r\n");

					if (data != NULL)
						delete [] data;

					return 1;
				}
			}
		}

		delete []data;
	}

	return 0;
}



/**
* \brief	synchronize
*
* synchronizes RemoteControl server and app
*
* \return	1 if error, 0 if success
*/
int const synchronize() {
	////////////////////////////////////////// UI ///////////////////////////////////////////////////////
	UIManager::setStatusText("Synchronizing...");
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	
	int bytes=1;

	int result=0, j=0;
	TagLib::FileRef f;

	/////////////////////////////////// INITIALIZE CURRENT FILE  ///////////////////////////////////////

	char *file = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC,SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS),IPC_GETPLAYLISTFILE);
	
	try {
		if (file != NULL) {	// catch empty playlist
			TagLib::FileRef tmp(file);
			f = tmp;
		}
	} catch (...) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		return 1;
	}


	/////////////////////////////////// CRITICAL  BEGIN ////////////////////////////////////////
	EnterCriticalSection(&cs_winamp);

	/////////////////////////////////// PLAYLISTLENGTH  ///////////////////////////////////////

	playlistlength = SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTLENGTH);

	stringstream playlistlengthStream;
	playlistlengthStream << playlistlength;
	
	// send
	if (rawSend(playlistlengthStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}


	/////////////////////////////////// PLAYLIST  ///////////////////////////////////////
	
	for (int i = 0; i < playlistlength; i++) {
		wchar_t *title_wchar_t;
		title_wchar_t = (wchar_t*)SendMessageA(plugin.hwndParent,WM_USER, i, IPC_GETPLAYLISTTITLEW);

		std::string title_str = utf8_encode(title_wchar_t);


		if (title_wchar_t != NULL)
		{
			if (rawSend(title_str.c_str()) != 0) {
				UIManager::addLogText("Synchronizing failed!\r\n");

				LeaveCriticalSection(&cs_winamp);

				return 1;
			}
		} else {
			if (rawSend("") != 0) {
				UIManager::addLogText("Synchronizing failed!\r\n");

				LeaveCriticalSection(&cs_winamp);

				return 1;
			}
		}
	}


	
	/////////////////////////////////// REPEAT  ///////////////////////////////////////
	// 1 if on
	
	repeat_status = SendMessageA(plugin.hwndParent, WM_USER, 0, 251);

	stringstream repeatStream;
	repeatStream << repeat_status;

	// send
	if (rawSend(repeatStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}


	/////////////////////////////////// SHUFFLE ///////////////////////////////////////
	// 1 if on

	shuffle_status = SendMessageA(plugin.hwndParent, WM_USER, 0, 250); 

	stringstream shuffleStream;
	shuffleStream << shuffle_status;

	if (rawSend(shuffleStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}
	
	/////////////////////////////////// VOLUME ///////////////////////////////////////

	volume = SendMessageA(plugin.hwndParent, WM_WA_IPC, -666, IPC_SETVOLUME);
	
	stringstream volumeStream;
	volumeStream << volume;

	if (rawSend(volumeStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}
	
	/////////////////////////////// QUEUE LIST COUNT ////////////////////////////////////

	int queueCount = WASABI_API_QUEUEMGR->GetNumberOfQueuedItems();
	
	stringstream queueCountStream;
	queueCountStream << queueCount;

	if (rawSend(queueCountStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}
	
	/////////////////////////////// QUEUE LIST ELEMENTS ////////////////////////////////////
	
	int k, tmp;
	stringstream queueElementStream;

	for (k=0; k < queueCount; k++) {
	
		tmp = WASABI_API_QUEUEMGR->GetQueuedItemFromIndex(k);

		queueElementStream << tmp;

		if (rawSend(queueElementStream.str().c_str()) != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	
		// clear stringstream
		queueElementStream.str("");
	}

	/////////////////////////////// SAMPLERATE ////////////////////////////////////
	int samplerate;

	if (file != NULL) {

		try {
			samplerate = f.audioProperties()->sampleRate();
		} catch (...) {
			samplerate = SendMessage(plugin.hwndParent,WM_WA_IPC,5,IPC_GETINFO);
		}

		stringstream samplerateStream;
		samplerateStream << samplerate;

		if (rawSend(samplerateStream.str().c_str()) != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}

	}
	else
	{
		if (rawSend("0") != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}

	/////////////////////////////// BITRATE ////////////////////////////////////
	int bitrate;

	if (file != NULL) {

		try {
			bitrate = f.audioProperties()->bitrate();
		} catch (...) {
			bitrate = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETINFO);
		}

		stringstream bitrateStream;
		bitrateStream << bitrate;

		if (rawSend(bitrateStream.str().c_str()) != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}

	}
	else
	{
		if (rawSend("0") != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}


	/////////////////////////////////// LENGTH ///////////////////////////////////////
	int length;

	if (file != NULL) {

		try {
			length = f.audioProperties()->length();
		} catch (...) {
			length = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETOUTPUTTIME);
		}

		stringstream lengthStream;
		lengthStream << length;

		if (rawSend(lengthStream.str().c_str()) != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}
	else
	{
		if (rawSend("0") != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}


	/////////////////////////////////// PLAYLIST POSITION ///////////////////////////////////////

	int playlistPosition = SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS);

	stringstream playlistPositionStream;

	playlistPositionStream << playlistPosition;

	if (rawSend(playlistPositionStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}


	/////////////////////////////////// TITLE ///////////////////////////////////////

	wchar_t *title_wchar_t;
	title_wchar_t = (wchar_t*)SendMessageA(plugin.hwndParent,WM_USER, playlistPosition, IPC_GETPLAYLISTTITLEW);

	if (title_wchar_t != NULL) {

		std::string title_str = utf8_encode(title_wchar_t);

		if (rawSend(title_str.c_str()) != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	} else {
		if (rawSend("") != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}

	/////////////////////////////////// PLAYBACK POSITION ///////////////////////////////////////
	
	int position = SendMessageA(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);

	stringstream positionStream;
	positionStream << position;

	if (rawSend(positionStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}
	

	/////////////////////////////////// PLAYBACK STATUS ///////////////////////////////////////
	
	isPlaying = SendMessageA(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

	stringstream isPlayingStream;
	isPlayingStream << isPlaying;

	if (rawSend(isPlayingStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return 1;
	}

	
	/////////////////////////////////// COVER ///////////////////////////////////////
	
	if (file != NULL) {
		result = sendCover("", -1);

		if (result == -1) {
			UIManager::addLogText("Synchronizing failed!\r\n");
			
			LeaveCriticalSection(&cs_winamp);

			return 1;
		}

	} else {
		if (rawSend("coverLength_0") != 0) {
			UIManager::addLogText("Synchronizing failed!\r\n");

			LeaveCriticalSection(&cs_winamp);

			return 1;
		}
	}


	////////////////////////////////////// CRITICAL END ///////////////////////////////////////////
	LeaveCriticalSection(&cs_winamp);

	return 0;
}



/**
* \brief	sendTrackInfo
*
* reads the tag info from a file and sends the results to the client
*
* \param number track number
*/
void sendTrackInfo(const int & number) {
	int bytes=1;

	int result=0, j=0;
	TagLib::FileRef f;

	/////////////////////////////////// INITIALIZE CURRENT FILE  ///////////////////////////////////////

	char *file = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC,number,IPC_GETPLAYLISTFILE);
	
	try {
		if (file != NULL) {	// catch empty playlist
			TagLib::FileRef tmp(file);
			f = tmp;
		}
	} catch (...) {
		UIManager::addLogText("Could not read TAG data!\r\n");

		return;
	}

	// check
	if (file == NULL)
		return;

	/////////////////////////////////// CRITICAL  BEGIN ////////////////////////////////////////
	EnterCriticalSection(&cs_winamp);

	/////////////////////////////////// TITLE ////////////////////////////////////////////

	TagLib::String title = f.tag()->title();

	string titleString("track_title_");
	titleString.append(title.toCString());

	if (rawSend(titleString.c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// INTERPRET ////////////////////////////////////////////

	TagLib::String artist = f.tag()->artist();

	string artistString("track_artist_");
	artistString.append(artist.toCString());

	if (rawSend(artistString.c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// ALBUM ////////////////////////////////////////////

	TagLib::String album = f.tag()->album();

	string albumString("track_album_");
	albumString.append(album.toCString());

	if (rawSend(albumString.c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// YEAR ////////////////////////////////////////////

	unsigned int year = f.tag()->year();

	stringstream yearStream;
	yearStream << "track_year_";
	yearStream << year;

	if (rawSend(yearStream.str().c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// TRACK ////////////////////////////////////////////

	unsigned int track = f.tag()->track();

	stringstream trackStream;
	trackStream << "track_track_";
	trackStream << track;

	if (rawSend(trackStream.str().c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// GENRE ////////////////////////////////////////////

	TagLib::String genre = f.tag()->genre();

	string genreString("track_genre_");
	genreString.append(genre.toCString());

	if (rawSend(genreString.c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}


	/////////////////////////////// SAMPLERATE ////////////////////////////////////
	int samplerate;

	stringstream samplerateStream;
	samplerateStream << "track_samplerate_";
	

	try {
		samplerate = f.audioProperties()->sampleRate();
	} catch (...) {
		samplerate = SendMessage(plugin.hwndParent,WM_WA_IPC,5,IPC_GETINFO);
	}

	samplerateStream << samplerate;

	if (rawSend(samplerateStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}


	/////////////////////////////// BITRATE ////////////////////////////////////
	int bitrate;

	stringstream bitrateStream;
	bitrateStream << "track_bitrate_";

	try {
		bitrate = f.audioProperties()->bitrate();
	} catch (...) {
		bitrate = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETINFO);
	}

	bitrateStream << bitrate;

	if (rawSend(bitrateStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}



	/////////////////////////////////// LENGTH ///////////////////////////////////////
	int length;

	
	try {
		length = f.audioProperties()->length();
	} catch (...) {
		length = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETOUTPUTTIME);
	}

	stringstream lengthStream;
	lengthStream << "track_length_";
	lengthStream << length;

	if (rawSend(lengthStream.str().c_str()) != 0) {
		UIManager::addLogText("Synchronizing failed!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}
	

	
	/////////////////////////////////// COMMENT ////////////////////////////////////////////

	TagLib::String comment = f.tag()->comment();

	string commentString("track_comment_");
	commentString.append(comment.toCString());

	if (rawSend(commentString.c_str()) != 0) {
		UIManager::addLogText("Could not read TAG info!\r\n");

		LeaveCriticalSection(&cs_winamp);

		return;
	}

	/////////////////////////////////// COVER ///////////////////////////////////////
	
	
	result = sendCover("track_", number);

	if (result == -1) {
		UIManager::addLogText("Synchronizing failed!\r\n");
			
		LeaveCriticalSection(&cs_winamp);

		return;
	}

	

	////////////////////////////////////// CRITICAL END ///////////////////////////////////////////
	LeaveCriticalSection(&cs_winamp);
}