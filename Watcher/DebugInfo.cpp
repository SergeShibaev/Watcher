#include "stdafx.h"
#include "DebugInfo.h"
#include "System.h"

std::wstring DebugInfo::GetIDs() const
{
	TCHAR str[32];
	StringCchPrintf(str, sizeof(str), L"P:0x%X, T:0x%X", _processId, _threadId);

	return str;
}

ProcessInfo::ProcessInfo(const DEBUG_EVENT& event) 
	: DebugInfo(event.dwProcessId, event.dwThreadId),
	_imageBase((DWORD)event.u.CreateProcessInfo.lpBaseOfImage),
	_startAddress((DWORD)event.u.CreateProcessInfo.lpStartAddress),
	_unicode(event.u.CreateProcessInfo.fUnicode),
	_handle(event.u.CreateProcessInfo.hProcess),
	_exitCode(0)
{
	System::GetFileName(event.u.CreateProcessInfo.hFile, _imageName);
}

std::wstring ProcessInfo::Print() const
{
	std::wstring str(_imageName.size() + 50, 0);
	StringCchPrintf(&str[0], str.size(), L"%s  B:0x%X, A:0x%X", _imageName.c_str(), _imageBase, _startAddress);
	if (_exitCode)
		StringCchPrintf(&str[0], str.size(), L"%s EC:0x%X", str.c_str(), _exitCode);

	return str;
}

LibraryInfo::LibraryInfo(const DEBUG_EVENT& event) 
	: DebugInfo(event.dwProcessId, event.dwThreadId),
	_baseAddr((DWORD)event.u.LoadDll.lpBaseOfDll),
	_unicode(event.u.LoadDll.fUnicode)
{
	System::GetFileName(event.u.LoadDll.hFile, _imageName);
}

std::wstring LibraryInfo::Print() const
{
	std::wstring str(_imageName.size() + 20, 0);
	StringCchPrintf(&str[0], str.size(), L"%s  B:0x%X", _imageName.c_str(), _baseAddr);

	return str;
}

DebugStringInfo::DebugStringInfo(const DEBUG_EVENT& event, HANDLE h) : DebugInfo(event.dwProcessId, event.dwThreadId)
{
	SIZE_T bytes;
	if (event.u.DebugString.fUnicode)
	{
		_data.resize(event.u.DebugString.nDebugStringLength);
		ReadProcessMemory(h, event.u.DebugString.lpDebugStringData, &_data[0],
			event.u.DebugString.nDebugStringLength, &bytes);
	}
	else
	{
		std::string data(event.u.DebugString.nDebugStringLength, 0);
		ReadProcessMemory(h, event.u.DebugString.lpDebugStringData, &data[0],
			event.u.DebugString.nDebugStringLength, &bytes);
		_data = std::wstring(data.begin(), data.end());
	}
}

std::wstring ThreadInfo::Print() const
{
	std::wstring str(32, 0);
	StringCchPrintf(&str[0], str.size(), L"B:0x%X, A:0x%X", _localBase, _startAddress);
	
	if (_exitCode)
		StringCchPrintf(&str[0], str.size(), L"%s EC:0x%X", str.c_str(), _exitCode);
	return str;
}

std::wstring ExceptionInfo::Print() const
{
	std::wstring str(1000, 0);
	try
	{
		StringCchPrintf(&str[0], str.size(), L"%s", _excInfo.at(_item.ExceptionRecord.ExceptionCode));
	}
	catch (std::out_of_range)
	{
		return L"Unknown exception";
	}
	
	return str;
}