#pragma once
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

class HttpSender
{
	DWORD _lastError;
	HINTERNET _hInternet;
	HINTERNET _hConnect;

public:
	HttpSender(const std::wstring& host);
	BOOL SendRequest(const std::wstring& script, const std::string& request);
	~HttpSender();
};

