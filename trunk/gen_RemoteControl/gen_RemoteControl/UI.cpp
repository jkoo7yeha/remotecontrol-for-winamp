#include "stdafx.h"

using namespace gen_RemoteControl;

/**
* \brief	UI
*
* constructor for UI object
*/
UI::UI() {
	this->Text = PLUGIN_NAME;

	InitializeComponent();
}


/**
* \brief	~UI
*
* destructor for UI object
*/
UI::~UI() {
	if (components)
		delete components;
}


/**
* \brief	linkLabel_LinkClicked
*
* executed when link label is clicked. opens RemoteControl project on google code.
*
* \param sender	System::Object^
* \param e System::Windows::Forms::LinkLabelLinkClickedEventArgs^
*/
System::Void UI::linkLabel_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
	ShellExecute(NULL, L"open", L"http://code.google.com/p/remotecontrol-for-winamp/", NULL, NULL, SW_SHOWNORMAL);
}


/**
* \brief	UI_Load
*
* fills the UI elements with settings from settings file
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::UI_Load(System::Object^  sender, System::EventArgs^  e) {
	//Image image = Image::FromFile("donate.png");
	//donatePictureBox->Image = image;

	fillSettingValues();

	checkForNewVersionInvoker();
}


/**
* \brief	connectButton_Click
*
* executed when connect button is clicked. if client is connected, server is stopped. 
* if waiting for client, thread is aborted. else server is started.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::connectButton_Click(System::Object^  sender, System::EventArgs^  e) {
	if (connecting == false) {
		if(connected == false)
			startServer();
		else
			stopServer(true);
	}
	else {
		// first set connecting = false so accept() can can notice that waiting for client was aborted
		connecting = false;

		TerminateThread(waitingForClientThread,0);

		shutdownSocket();

		UIManager::setStatusText("Disconnected");
		UIManager::setButtonText("Start server");

		UIManager::addLogText("Aborted\r\n\r\n");

	}
			
}

/**
* \brief	saveButton_Click
*
* executed when save button is clicked. saves settings.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::saveButton_Click(System::Object^  sender, System::EventArgs^  e) {
	if (saveSettings() == 1){
		UIManager::addLogText("Could not save settings!\r\n");
		return;
	}

	saveButton->Enabled = false;
}


/**
* \brief	portEditBox_TextChanged
*
* executed when port editbox is changed. activates save button.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::portEditBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	saveButton->Enabled = true;
}

/**
* \brief	autostartCheckBox_CheckedChanged
*
* executed when autostart checkbox is changed. activates save button.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::autostartCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	saveButton->Enabled = true;
}

/**
* \brief	showConfigOnStartupCheckBox_CheckedChanged
*
* executed when showConfigOnStartup checkbox is changed. activates save button.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::showConfigOnStartupCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	saveButton->Enabled = true;
}


/**
* \brief	autoRestartCheckBox_CheckedChanged
*
* executed when autoRestart checkbox is changed. activates save button.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::autoRestartCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	saveButton->Enabled = true;
}


/**
* \brief	keepAliveCheckBox_CheckedChanged
*
* executed when keepAlive checkbox is changed. activates save button.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::keepAliveCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	saveButton->Enabled = true;
}


/**
* \brief	logTextBox_TextChanged
*
* executed when log text has been changed. scrolls down to the bottom.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::logTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	logTextBox->SelectionLength = 0;

	logTextBox->SelectionStart = logTextBox->Text->Length;

	logTextBox->ScrollToCaret();
}


/**
* \brief	UI_FormClosing
*
* executed when UI is closed. if user has closed UI, UI just hides.
*
* \param sender	System::Object^
* \param e System::Windows::Forms::FormClosingEventArgs^
*/
System::Void UI::UI_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	if (e->CloseReason == System::Windows::Forms::CloseReason::UserClosing) {
			this->Hide();
			e->Cancel = true;	// prevent from destroying UI
	}
	else
		quit();
}


/**
* \brief	donatePictureBox_Click
*
* executed when UI is closed. opens paypal website.
*
* \param sender	System::Object^
* \param e System::EventArgs^
*/
System::Void UI::donatePictureBox_Click(System::Object^  sender, System::EventArgs^  e) {
	ShellExecute(NULL, L"open", L"https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4ZME6LEXD23GW", NULL, NULL, SW_SHOWNORMAL);
}


/**
* \brief	addLogText
*
* thread save log text change. invokes UI thread to perform log text change if necessary.
*
* \param text System::String^
*/
System::Void UI::addLogText(System::String^ text) {
	if (logTextBox->InvokeRequired) {    
		SetTextDelegate^ d = gcnew SetTextDelegate(this, &UI::addLogText);
		this->Invoke(d, gcnew array<Object^> { text });
	} else {
		string sb;
		sb.append(time()).append(": ").append(msclr::interop::marshal_as<std::string>(text));
		logTextBox->Text += gcnew String(sb.c_str());
	}
}

/**
* \brief	setStatusText
*
* thread save status text change. invokes UI thread to perform status text change if necessary.
*
* \param text System::String^
*/
System::Void UI::setStatusText(System::String^ text) {
	if (statusLabel->InvokeRequired) {    
		SetTextDelegate^ d = gcnew SetTextDelegate(this, &UI::setStatusText);
        this->Invoke(d, gcnew array<Object^> { text });
	} else
		statusLabel->Text = text;
}


/**
* \brief	setButtonText
*
* thread save button text change. invokes UI thread to perform button text change if necessary.
*
* \param text System::String^
*/
System::Void UI::setButtonText(System::String^ text) {
	if (connectButton->InvokeRequired) {    
		SetTextDelegate^ d = gcnew SetTextDelegate(this, &UI::setButtonText);
        this->Invoke(d, gcnew array<Object^> { text });
	} else
		connectButton->Text = text;
}


/**
* \brief	setIP
*
* thread save IP text change. invokes UI thread to perform IP text change if necessary.
*
* \param text System::String^
*/
System::Void UI::setIP(System::String^ text) {
	if (ipLabel->InvokeRequired) {    
		SetTextDelegate^ d = gcnew SetTextDelegate(this, &UI::setIP);
        this->Invoke(d, gcnew array<Object^> { text });
	} else
		ipLabel->Text = text;
}


/**
* \brief	newVersionFound
*
* thread inform about a new available version. invokes UI thread to perform notification if necessary.
*/
System::Void UI::newVersionFound(System::String^ text) {
	if (ipLabel->InvokeRequired) {    
		SetTextDelegate^ d = gcnew SetTextDelegate(this, &UI::newVersionFound);
        this->Invoke(d, gcnew array<Object^> { text });
	} else
		newVersionLabel->Visible = true;
}

/**
* \brief	backgroundWorker_RunWorkerCompleted
*
* thread save UI Show() or Hide(). Backgroundworker needed because Invoke leads to deadlock.
*
* \param sender System::Object^
* \param e System::ComponentModel::RunWorkerCompletedEventArgs^
*/
System::Void UI::backgroundWorker_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
	if (windowVisible)
		this->Show();
	else
		this->Hide();
}