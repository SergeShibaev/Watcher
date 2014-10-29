#include "stdafx.h"
#include "Logger.h"

void Logger::InitLogFile()
{
	_logFile.open(_logFileName.c_str(), std::ios_base::trunc | std::ios_base::out);
	_logFile.imbue(std::locale("Russian"));
}

void Logger::SetLogFile(const std::wstring& logFileName)
{
	_logFileName = logFileName;
	InitLogFile();
}

void Logger::Add(const std::wstring& message)
{
	_log.push_back(message);
}

void Logger::Save()
{
	if (_logFile.is_open())
	{
		for (size_t i = 0; i < _log.size(); ++i)
			_logFile << _log[i].c_str() << L"\n";
	}
}