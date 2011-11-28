#include "stdafx.h"

/**
* \brief	setParameter
*
* sets the text parameter used by the UI delegate
*
* \param text System::String^
*/
void UIAction::setParameter(System::String^ text) {
	parameter = text;
}

/**
* \brief	setAction
*
* sets the action parameter used by the UI delegate. parameter definitions are made in header.h
*
* \param action int
*/
void UIAction::setAction(const int & action) {
	this->action = action;
}


/**
* \brief	Start
*
* starts UI action specified by setAction with the text parameter specified by setParamter
*/
void UIAction::Start() {
	// check if UI is set
	if (UIManager::getUI() == nullptr)
		return;

	// perform action
	if (action == LOG)
		UIManager::getUI()->addLogText(parameter);
	else if (action == STATUS)
		UIManager::getUI()->setStatusText(parameter);
	else if (action == BUTTON)
		UIManager::getUI()->setButtonText(parameter);
	else if (action == IP)
		UIManager::getUI()->setIP(parameter);
	else if (action == VERSION)
		UIManager::getUI()->newVersionFound(gcnew String(""));
}
