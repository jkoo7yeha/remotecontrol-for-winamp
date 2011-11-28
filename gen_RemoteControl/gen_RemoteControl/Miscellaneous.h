#pragma once

// returns current time
extern std::string const time();

// Convert a wide Unicode string to an UTF8 string
extern std::string const utf8_encode(const std::wstring &wstr);

// gets the extension of a file
extern std::string const GetFileExtension(const std::string& FileName);

// Gets IP addresses of the local computer
extern System::String^ GetLocalIP();



// starts a new thread that checks for a new version
extern void checkForNewVersionInvoker();

// downloads version information and notifies user about new version if necessary
extern DWORD WINAPI checkForNewVersion(LPVOID parameter);