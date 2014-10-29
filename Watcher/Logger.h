#pragma once
#include <fstream>

class Logger
{
	std::wstring _logFileName;
	std::wofstream _logFile;
	std::vector<std::wstring> _log;

	void InitLogFile();

public:
	Logger(const std::wstring& logFileName) : _logFileName(logFileName) 
	{
		InitLogFile();
	}
	~Logger(void)
	{
		if (_logFile.is_open())
			_logFile.close();
	}

	void SetLogFile(const std::wstring& logFileName);
	void Add(const std::wstring& message);
	void Add(const std::wstring& message, const std::wstring& param);
	void Save();
};

