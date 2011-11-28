#pragma once
#include "stdafx.h"


ref class UIManager
{
	private: static gen_RemoteControl::UI^ UIform = nullptr;

	public: static gen_RemoteControl::UI^ getUI();
            static void setUI(gen_RemoteControl::UI^ UIform_orig);

			static void addLogText(System::String^ text);
			static void setStatusText(System::String^ text);
			static void setButtonText(System::String^ text);
			static void setIP(System::String^ text);
			static void	newVersionFound();
			static void showUI(const bool value);
};