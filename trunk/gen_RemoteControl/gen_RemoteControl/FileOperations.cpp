#include "stdafx.h"

/**
* \brief	saveSettings
*
* saves the current settings in the settings file in local appdata path with the file name set in stdafx.cpp
*
* \return	1 if error, 0 if success
*/
int const saveSettings() {

	fstream file;
	file.open (settingsPath.c_str(), ios::out);
	

	/////////////// PORT //////////////
	System::String^ str = UIManager::getUI()->portEditBox->Text;

	pin_ptr<const wchar_t> wch = PtrToStringChars(str);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	errno_t err = 0;
	char *port_char = (char *)malloc(sizeInBytes);

	err = wcstombs_s(&convertedChars, 
                    port_char, sizeInBytes,
                    wch, sizeInBytes);
	if (err != 0) {
		file.close();
		return 1;
	}

	file << port_char << endl;	// PORT
	
	/////////////// AUTOSTART //////////////

	if (UIManager::getUI()->autostartCheckBox->Checked) {
		file << 1 << endl;
		autostart = 1;
	}
	else {
		file << 0 << endl;
		autostart = 0;
	}

	/////////////// SHOWCONFIGONSTARTUP //////////////

	if (UIManager::getUI()->showConfigOnStartupCheckBox->Checked) {
		file << 1 << endl;
		showconfigonstartup = 1;
	}
	else {
		file << 0 << endl;
		showconfigonstartup = 0;
	}

	/////////////// AUTORESTART //////////////

	if (UIManager::getUI()->autoRestartCheckBox->Checked) {
		file << 1 << endl;
		autorestart = 1;
	}
	else {
		file << 0 << endl;
		autorestart = 0;
	}

	/////////////// KEEP ALIVE MESSAGES //////////////

	if (UIManager::getUI()->keepAliveCheckBox->Checked) {
		file << 1 << endl;
		keepalivemessages = 1;
	}
	else {
		file << 0 << endl;
		keepalivemessages = 0;
	}


	// check
	if (file.fail()) {
		file.close();
		return 1;
	}

	file.close();
	
	return 0;
}

/**
* \brief	fillSettingValues
*
* fills the UI fields with the read settings
*/
void fillSettingValues() {
	// FILL PORT FIELD
	stringstream portStream;
	portStream << port;

	System::String^ portString = gcnew String(portStream.str().c_str());

	UIManager::getUI()->portEditBox->Text = portString;

	// FILL CHECKBOXES
				 
	if (autostart == 1)
		UIManager::getUI()->autostartCheckBox->Checked = true;
				 
	if (showconfigonstartup == 1)
		UIManager::getUI()->showConfigOnStartupCheckBox->Checked = true;

	if (autorestart == 1)
		UIManager::getUI()->autoRestartCheckBox->Checked = true;

	if (keepalivemessages == 1)
		UIManager::getUI()->keepAliveCheckBox->Checked = true;

	UIManager::getUI()->saveButton->Enabled = false;
}

/** \brief	createNewSettingsFile
*
* deletes an old settings file and creates a new one
*
* \return	1 if error, 0 if success
*/
int const createNewSettingsFile() {
	
	// delete config file

	ifstream oldFile;
	oldFile.open(settingsPath.c_str());

	if (oldFile)
		remove(settingsPath.c_str());


	// new settings
	port = 50000;
	autostart = 1;
	showconfigonstartup = 1;
	autorestart = 1;
	keepalivemessages = 1;


	// create new file
	ofstream outFile;
	outFile.open(settingsPath.c_str());
		
	outFile << "50000" << endl;	// PORT
	outFile << "1" << endl;		// AUTOSTART
	outFile << "1" << endl;		// SHOWCONFIGONSTARTUP
	outFile << "1" << endl;		// AUTORESTART
	outFile << "1" << endl;		// KEEP ALIVE MESSAGES

	// check
	if (outFile.fail()) {
		outFile.close();

		UIManager::getUI()->addLogText("creating new settings file failed. please delete this file: " + gcnew String(settingsPath.c_str()) + "\r\n");

		return 1;
	}

	outFile.close();

	// fill UI
	fillSettingValues();

	UIManager::getUI()->addLogText("settings file restored\r\n");

	return 0;
}


/**
* \brief	readSettings
*
* reads settings from the settings file from local appdata path with the file name set in stdafx.cpp
*
* \return	1 if error, 0 if success
*/
int const readSettings() {
	ifstream inFile;
	inFile.open(settingsPath.c_str());

	if (!inFile) {
		inFile.close();

		createNewSettingsFile();
		
		return 0;
	}
	
	// start reading settings

	char *buf;

	// READ PORT
	buf = new char[6];
	inFile.getline(buf,6);
	port = atoi(buf);
	delete []buf;

	// check
	if (inFile.eof()) {
		inFile.close();
		
		createNewSettingsFile();
		
		return 0;
	}

	// READ AUTOSTART
	buf = new char[2];
	inFile.getline(buf,2);
	autostart = atoi(buf);
	delete []buf;

	// check
	if (inFile.eof()) {
		inFile.close();
		
		createNewSettingsFile();
		
		return 0;
	}

	// READ SHOWCONFIGONSTARTUP
	buf = new char[2];
	inFile.getline(buf,2);
	showconfigonstartup = atoi(buf);
	delete []buf;

	// check
	if (inFile.eof()) {
		inFile.close();
		
		createNewSettingsFile();
		
		return 0;
	}

	// READ AUTORESTART
	buf = new char[2];
	inFile.getline(buf,2);
	autorestart = atoi(buf);
	delete []buf;

	// check
	if (inFile.eof()) {
		inFile.close();
		
		createNewSettingsFile();
		
		return 0;
	}

	// READ KEEPALIVEMESSAGES
	buf = new char[2];
	inFile.getline(buf,2);
	keepalivemessages = atoi(buf);
	delete []buf;
	

	// last check if whole operation failed
	if (inFile.fail()) {
		if (createNewSettingsFile() == 1)
			return 1;
		
		return 0;
	}

	inFile.close();

	return 0;
}