#include "stdafx.h"


/**
* \brief	time()
* returns current time with format MM/DD/YY HH:MM:SS
*
* \return	time string
*/
std::string const time()
{
	char tmpbuf[128];
	_strtime_s( tmpbuf, 128 );

	char datebuf[128];
	_strdate_s( datebuf, 128 );

	std::string ret;
	ret.append(datebuf);
	ret.append(" - ");
	ret.append(tmpbuf);

	return ret;
}


/**
* \brief	utf8_encode
*
* converts a wide unicode string to an UTF8 string
*
* \return	utf8 encoded string
*/
std::string const utf8_encode(const std::wstring & wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

/**
* \brief	GetFileExtension
*
* gets the extension of a file
*
* \return	string with file extension
*/
std::string const GetFileExtension(const std::string & FileName) {
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}


// Gets IP addresses of the local computer
System::String^ GetLocalIP()
{
    System::String ^_IP = gcnew String("");

    // Resolves a host name or IP address to an IPHostEntry instance.
    // IPHostEntry - Provides a container class for Internet host address information. 
    System::Net::IPHostEntry ^_IPHostEntry = System::Net::Dns::GetHostEntry(System::Net::Dns::GetHostName());

    // IPAddress class contains the address of a computer on an IP network. 
    for each (System::Net::IPAddress ^_IPAddress in _IPHostEntry->AddressList)
    {
        // InterNetwork indicates that an IP version 4 address is expected 
        // when a Socket connects to an endpoint
        if (_IPAddress->AddressFamily.ToString() == "InterNetwork")
        {
            _IP += _IPAddress->ToString() + "\r\n";
        }
    }
    return _IP;
}


/**
* \brief	checkForNewVersionInvoker
*
* starts a new thread that checks for a new version
*/
void checkForNewVersionInvoker() {
	// check for new version
	CreateThread(NULL, 0, checkForNewVersion, 0, 0, NULL);
}

/**
* \brief	checkForNewVersion
*
* downloads version information and notifies user about new version if necessary
*/
DWORD WINAPI checkForNewVersion(LPVOID parameter) {
	// new WebClient
	System::Net::WebClient^ wClient = gcnew System::Net::WebClient();
	String^ strSource = gcnew String("");

	// download version info

	try {

		strSource = wClient->DownloadString("http://remotecontrol-for-winamp.googlecode.com/svn/trunk/gen_RemoteControl/gen_RemoteControl/version.h");
		
		// check disabled status
		if (strSource->EndsWith("DISABLED"))
			return 0;

		// notify user if this version is not up to date
		if (!strSource->EndsWith("\"" + PLUGIN_VERSION + "\""))
			UIManager::newVersionFound();

	} catch (Exception^) {}


	return 0;
}