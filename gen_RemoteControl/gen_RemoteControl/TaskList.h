#pragma once
#include "stdafx.h"



class TaskList {
	private: 
		std::queue<std::string> list;
		volatile int parameter;

		HANDLE non_empty_list;

		// critical tasklist section
		CRITICAL_SECTION cs_tasklist;
		CRITICAL_SECTION cs_parameter;
	public:	
		TaskList();

		~TaskList();


		std::string const pop();
		void push(const std::string & element, const int & number = -1);

		const int getParameter();
		void setParameter(const int & param);
};