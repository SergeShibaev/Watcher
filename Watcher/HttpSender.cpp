#include "stdafx.h"
#include "HttpSender.h"


HttpSender::HttpSender(const std::wstring& host)
	: _lastError(0)
{
	_hInternet = InternetOpen(L"Watcher", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	_hConnect = InternetConnect(_hInternet, host.c_str(),
		INTERNET_INVALID_PORT_NUMBER, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
}

BOOL HttpSender::SendRequest(const std::wstring& script, const std::string& request)
{
	static LPCWSTR accept[2] = { L"*/*", NULL };
	HINTERNET hReq = HttpOpenRequest(_hConnect, L"POST", script.c_str(),
		NULL, NULL, accept, 0, 1);
	if (hReq == NULL)
		_lastError = GetLastError();
	TCHAR postHeader[] = L"Content-Type: application/x-www-form-urlencoded\n";

	//boost::replace_all(request, "+", "%2B");
	size_t pos = 0;
	std::string reqStr(request);
	while ((pos = reqStr.find('+', pos)) != std::string::npos)
		reqStr.replace(pos, 1, "%2B");
	BOOL req = HttpSendRequest(hReq, postHeader, wcslen(postHeader), (LPVOID)(request.c_str()), request.size() * sizeof(request[0]));
	if (!req)
		_lastError = GetLastError();
	InternetCloseHandle(hReq);

	return req;
}


HttpSender::~HttpSender()
{
	InternetCloseHandle(_hConnect);
	InternetCloseHandle(_hInternet);

}
