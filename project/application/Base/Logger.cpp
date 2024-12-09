#include "Logger.h"
#include<Windows.h>

namespace Logger {

	void Logger::Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());

	}


}

