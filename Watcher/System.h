#pragma once
namespace System
{
	void ErrorExit(const std::wstring& reason);
	bool SetDebugPrivilegies(BOOL enable);
	bool GetFileName(HANDLE hFile, std::wstring& filename);
	std::wstring GetOSName();
};

