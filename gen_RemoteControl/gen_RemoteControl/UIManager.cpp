#include "stdafx.h"

using namespace gen_RemoteControl;
using namespace System::Threading;

/**
* \brief	getUI
*
* returns the used UI object or creates a new one if it's not yet created.
*
* \return UI^
*/
UI^ UIManager::getUI() {
	if (UIform == nullptr)
		UIform = gcnew gen_RemoteControl::UI();

	return UIform;
}

/**
* \brief	setUI
*
* sets a new UI object
*
* \param	UIform_orig	the new UI object to use
*/
void UIManager::setUI(UI^ UIform_orig) {
	UIform = UIform_orig;
}


/**
* \brief	addLogText
*
* starts a thread to create a new log entry
*
* \param	text	the new entry to add
*/
void UIManager::addLogText(System::String^ text) {
	UIAction^ la = gcnew UIAction();

	la->setParameter(text);
	la->setAction(LOG);

	Thread^ t = gcnew Thread(gcnew ThreadStart(la, &UIAction::Start));
	t->Start();
}

/**
* \brief	setStatusText
*
* starts a thread to set a new status text
*
* \param	text	the new text
*/
void UIManager::setStatusText(System::String^ text) {
	UIAction^ la = gcnew UIAction();

	la->setParameter(text);
	la->setAction(STATUS);

	Thread^ t = gcnew Thread(gcnew ThreadStart(la, &UIAction::Start));
	t->Start(); 
}

/**
* \brief	setButtonText
*
* starts a thread to set a new button text
*
* \param	text	the new text
*/
void UIManager::setButtonText(System::String^ text) {
	UIAction^ la = gcnew UIAction();

	la->setParameter(text);
	la->setAction(BUTTON);

	Thread^ t = gcnew Thread(gcnew ThreadStart(la, &UIAction::Start));
	t->Start(); 
}

/**
* \brief	setIP
*
* starts a thread to set a new IP text
*
* \param	text	the new text
*/
void UIManager::setIP(System::String^ text) {
	UIAction^ la = gcnew UIAction();

	la->setParameter(text);
	la->setAction(IP);

	Thread^ t = gcnew Thread(gcnew ThreadStart(la, &UIAction::Start));
	t->Start(); 
}

/**
* \brief	newVersionFound
*
* starts a thread to notice about a new available version
*/
void UIManager::newVersionFound() {
	UIAction^ la = gcnew UIAction();

	la->setParameter(gcnew String(""));
	la->setAction(VERSION);

	Thread^ t = gcnew Thread(gcnew ThreadStart(la, &UIAction::Start));
	t->Start(); 
}

/**
* \brief	showUI
*
* starts the backgroundWorker that shows or hides the UI window
*
* \param	value	shows window if true, hides window if false
*/
void UIManager::showUI(const bool value) {
	windowVisible = value;

	getUI()->backgroundWorker->RunWorkerAsync();
}