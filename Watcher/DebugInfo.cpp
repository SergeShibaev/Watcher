#include "stdafx.h"
#include "DebugInfo.h"
#include "System.h"

ProcessInfo::ProcessInfo(const DEBUG_EVENT& event) : DebugInfo(event.dwProcessId, event.dwThreadId)
{
	_imageBase = (DWORD)event.u.CreateProcessInfo.lpBaseOfImage;
	_startAddress = (DWORD)event.u.CreateProcessInfo.lpStartAddress;
	_unicode = event.u.CreateProcessInfo.fUnicode;
	_handle = event.u.CreateProcessInfo.hProcess;

	System::GetFileName(event.u.CreateProcessInfo.hFile, _imageName);
}

LibraryInfo::LibraryInfo(const DEBUG_EVENT& event) : DebugInfo(event.dwProcessId, event.dwThreadId)
{
	_baseAddr = (DWORD)event.u.LoadDll.lpBaseOfDll;
	_unicode = event.u.LoadDll.fUnicode;

	System::GetFileName(event.u.LoadDll.hFile, _imageName);

}

DebugStringInfo::DebugStringInfo(const DEBUG_EVENT& event, HANDLE h) : DebugInfo(event.dwProcessId, event.dwThreadId)
{
	size_t bytes;
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