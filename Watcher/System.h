#pragma once
class System
{
public:
	System();
	~System();

	static void ErrorExit(const std::wstring& reason);
	static bool SetDebugPrivilegies(BOOL enable);
	static bool GetFileName(HANDLE hFile, std::wstring& filename);
};

