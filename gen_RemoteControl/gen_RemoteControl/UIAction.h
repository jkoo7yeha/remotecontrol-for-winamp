#pragma once
#include "stdafx.h"

ref class UIAction {
	private: System::String^ parameter;
			 int action;

	public:  void Start();

			 void setParameter(System::String^ text);
			 void setAction(const int & action);
};