#include "stdafx.h"


/**
* \brief	TaskList
*
* constructor, creates non_empty_list event
*/
TaskList::TaskList() {
	non_empty_list = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	InitializeCriticalSection(&cs_tasklist);
	InitializeCriticalSection(&cs_parameter);
}

/**
* \brief	~TaskList
*
* destructor
*/
TaskList::~TaskList() {
	DeleteCriticalSection(&cs_tasklist);
	DeleteCriticalSection(&cs_parameter);
}

const int TaskList::getParameter() {
	// CRITICAL
	EnterCriticalSection(&cs_parameter);
	
	int param = parameter;

	LeaveCriticalSection(&cs_parameter);
	// CRITICAL END

	return param;
}


void TaskList::setParameter(const int & param) {
	// CRITICAL
	EnterCriticalSection(&cs_parameter);
	
	parameter = param;

	LeaveCriticalSection(&cs_parameter);
	// CRITICAL END
}

/**
* \brief	pop
*
* takes the first inserted element out of queue (BLOCKING!)
*
* \return	taken element
*/
std::string const TaskList::pop() {
	WaitForSingleObject(non_empty_list, INFINITE);

	// CRITICAL
	EnterCriticalSection(&cs_tasklist);

	std::string element = list.front();
	list.pop();

	if (!list.empty())
		SetEvent(non_empty_list);

	LeaveCriticalSection(&cs_tasklist);
	// CRITICAL END

	return element;
}


/**
* \brief	push
*
* inserts new element into queue relating to input parameter
*
* \param	value for element to be added
*/
void TaskList::push(const std::string & element, const int & number) {

	if (element.compare("new_song_") == 0) {

		/////////////////////////////// read file information ////////////////////////////////////
		const char* file = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC,SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS),IPC_GETPLAYLISTFILE);

		TagLib::FileRef f(file);

		/////////////////////////////// isPlaying ////////////////////////////////////

		int isplaying = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

		stringstream isplayingStream;
		isplayingStream << "isplaying_";
		isplayingStream << isplaying;
		
		// CRITICAL
		EnterCriticalSection(&cs_tasklist);
		
		list.push(isplayingStream.str().c_str());
		
		LeaveCriticalSection(&cs_tasklist);
		// CRITICAL END


		SetEvent(non_empty_list);


		/////////////////////////////////// PLAYLIST POSITION ///////////////////////////////////////

		int playlistPosition = SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS);

		stringstream playlistPositionStream;
		playlistPositionStream << "playlistPosition_";
		playlistPositionStream << playlistPosition;
		
		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(playlistPositionStream.str().c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);

		/////////////////////////////// SAMPLERATE ////////////////////////////////////
		int samplerate;

		try {
			samplerate = f.audioProperties()->sampleRate();
		} catch (...) {
			samplerate = SendMessage(plugin.hwndParent,WM_WA_IPC,5,IPC_GETINFO);
		}

		stringstream samplerateStream;
		samplerateStream << "samplerate_";
		samplerateStream << samplerate;

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(samplerateStream.str().c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);


		/////////////////////////////// BITRATE ////////////////////////////////////
		int bitrate;

		try {
			bitrate = f.audioProperties()->bitrate();
		} catch (...) {
			bitrate = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETINFO);
		}

		stringstream bitrateStream;
		bitrateStream << "bitrate_";
		bitrateStream << bitrate;

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(bitrateStream.str().c_str());
		
		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);

		/////////////////////////////// LENGTH ////////////////////////////////////
		int length;

		try {
			length = f.audioProperties()->length();
		} catch (...) {
			length = SendMessage(plugin.hwndParent,WM_WA_IPC,1,IPC_GETOUTPUTTIME);
		}

		stringstream lengthStream;
		lengthStream << "length_";
		lengthStream << length;

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(lengthStream.str().c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);

		/////////////////////////////// TITLE ////////////////////////////////////

		wchar_t *title_wchar_t;
		title_wchar_t = (wchar_t*)SendMessageA(plugin.hwndParent,WM_USER, playlistPosition, IPC_GETPLAYLISTTITLEW);

		std::string title_str = "title_";
		title_str += utf8_encode(title_wchar_t);

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(title_str.c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);

		/////////////////////////////// COVER ////////////////////////////////////

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push("cover");

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);

		//////////////////////////////////////////////////////////////////////////

	}
	else if (element.compare("volume_") == 0) {
		
		// volume is read in MainWndProc hook!

		// VOLUME
		stringstream information;
		information << "volume_";
		information << volume;
		
		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(information.str().c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);
	}
	else if (element.compare("progress_") == 0) {
		
		// GET PROGRESS
		stringstream information;
		information << "progress_";
		information << progress;

		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(information.str().c_str());

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);
	} else if (element.compare("track_info") == 0) {
		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		setParameter(number);
		list.push("track_info");

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);
	} else {
		// CRITICAL
		EnterCriticalSection(&cs_tasklist);

		list.push(element);

		// CRITICAL END
		LeaveCriticalSection(&cs_tasklist);

		SetEvent(non_empty_list);
	}
}