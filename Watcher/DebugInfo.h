#pragma once

class DebugInfo
{
	DWORD _processId;
	DWORD _threadId;
public:
	//DebugInfo() {}
	DebugInfo(DWORD p, DWORD t) 
		: _processId(p), _threadId(t) {}
	virtual ~DebugInfo() {}
	virtual std::wstring Print() const = 0;
	virtual std::wstring Explain() const = 0;
};

class ProcessInfo : public DebugInfo
{
	DWORD _imageBase;
	DWORD _startAddress;
	std::wstring _imageName;
	WORD _unicode;
	DWORD _exitCode;
	HANDLE _handle;
public:
	ProcessInfo(const DEBUG_EVENT& event);
	void SetExitCode(DWORD code) { _exitCode = code; }
	DWORD GetExitCode() const { return _exitCode; }

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};

class ThreadInfo : public DebugInfo
{
	DWORD _localBase;
	DWORD _startAddress;
	DWORD _exitCode;
public:
	ThreadInfo(const DEBUG_EVENT& event)
		: DebugInfo(event.dwProcessId, event.dwThreadId),
		_localBase((DWORD)event.u.CreateThread.lpThreadLocalBase), 
		_startAddress((DWORD)event.u.CreateThread.lpStartAddress) {}
	void SetExitCode(DWORD code) { _exitCode = code; }
	DWORD GetExitCode() const { return _exitCode; }

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};

class LibraryInfo : public DebugInfo
{
	DWORD _baseAddr;
	std::wstring _imageName;
	WORD _unicode;
public:
	LibraryInfo(const DEBUG_EVENT& event);

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};

class ExceptionInfo : public DebugInfo
{
	EXCEPTION_DEBUG_INFO item;
public:
	ExceptionInfo(const DEBUG_EVENT& event) 
		: DebugInfo(event.dwProcessId, event.dwThreadId), item(event.u.Exception) {}

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};

class DebugStringInfo : public DebugInfo
{
	std::wstring _data;
public:
	DebugStringInfo(const DEBUG_EVENT& event, HANDLE h);

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};

class RIPInfo : public DebugInfo
{
	RIP_INFO _data;
public:
	RIPInfo(const DEBUG_EVENT& event) 
		: DebugInfo(event.dwProcessId, event.dwThreadId), _data(event.u.RipInfo) { }

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};