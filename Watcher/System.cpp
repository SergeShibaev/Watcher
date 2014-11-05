#include "stdafx.h"
#include "System.h"
#include <Psapi.h>
#include <VersionHelpers.h>

void System::ErrorExit(const std::wstring& reason)
{
	LPVOID lpMsgBuf;
	std::wstring message;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	message.resize(lstrlen((LPCTSTR)lpMsgBuf) + reason.size() + 40);
	StringCchPrintf(&message[0], message.size(), L"%s.\nОшибка 0x%X: %s", reason.c_str(), dw, lpMsgBuf);
	MessageBox(NULL, message.c_str(), L"Ошибка", MB_ICONERROR);

	LocalFree(lpMsgBuf);

	ExitProcess(dw);
}

bool System::SetDebugPrivilegies(BOOL enable)
{
	HANDLE hToken;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
			ErrorExit(L"Не удалось получить системное разрешение на отладку приложения");
		CloseHandle(hToken);
	}

	return true;
}

bool System::GetFileName(HANDLE hFile, std::wstring& filename)
{
	bool bSuccess = FALSE;
	HANDLE hFileMap;

	// Get the file size.
	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
		return FALSE;

	// Create a file mapping object.
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL);

	if (hFileMap)
	{
		// Create a file mapping to get the file name.
		LPVOID pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (pMem)
		{
			filename.resize(MAX_PATH);
			if (GetMappedFileName(GetCurrentProcess(), pMem, &filename[0], MAX_PATH))
			{

				// Translate path with device name to drive letters.
				WCHAR szTemp[MAX_PATH] = { 0 };

				if (GetLogicalDriveStrings(MAX_PATH - 1, szTemp))
				{
					WCHAR szName[MAX_PATH];
					WCHAR szDrive[3] = L" :";
					WCHAR* p = szTemp;

					while (*p)
					{
						*szDrive = *p;
						if (QueryDosDevice(szDrive, szName, MAX_PATH))
						{
							std::size_t pos = filename.find(szName);
							if (pos != std::wstring::npos)
							{
								filename.replace(pos, wcslen(szName), szDrive);
								break;
							}
						}
						while (*p++);
					}
				}
			}
			bSuccess = TRUE;
			UnmapViewOfFile(pMem);
		}

		CloseHandle(hFileMap);
	}

	return bSuccess;
}

void System::SysInfo::ReadOSName()
{
	_OSName = L"Microsoft Windows ";
	if (IsWindows8Point1OrGreater())
		_OSName += L"8.1";
	else if (IsWindows8OrGreater())
		_OSName += L"8";
	else if (IsWindows7SP1OrGreater())
		_OSName += L"7 SP1";
	else if (IsWindows7OrGreater())
		_OSName += L"7";
	else if (IsWindowsVistaSP2OrGreater())
		_OSName += L"Vista SP2";
	else if (IsWindowsVistaSP1OrGreater())
		_OSName += L"Vista SP1";
	else if (IsWindowsVistaOrGreater())
		_OSName += L"Vista";
	else if (IsWindowsXPSP3OrGreater())
		_OSName += L"XP SP3";
	else if (IsWindowsXPSP2OrGreater())
		_OSName += L"XP SP2";
	else if (IsWindowsXPSP1OrGreater())
		_OSName += L"XP SP1";
	else if (IsWindowsXPOrGreater())
		_OSName += L"XP";
	else _OSName = L"Unknown system";
}

void System::SysInfo::ReadProductName()
{
	_OSProductName = Registry::GetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName");
}

void System::SysInfo::ReadOSBuildLab()
{
	_OSBuildLab = Registry::GetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLabEx");
}

void System::SysInfo::ReadOSVersion()
{
	_OSVersion = Registry::GetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentVersion") + L"." +
		   Registry::GetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuild");
}

void System::SysInfo::ReadAppInitDLLs()
{
	_AppInitDlls = Registry::GetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", L"AppInit_DLLs");
}

void System::SysInfo::ReadProcessorID()
{
	_processorID = Registry::GetValue(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 
		L"PROCESSOR_IDENTIFIER");
}

BOOL ReplaceSubString(std::wstring& source, const std::wstring& fromStr, const std::wstring& toStr)
{
	size_t pos = source.find(fromStr);
	if (pos != std::wstring::npos)
	{
		source.replace(pos, fromStr.length(), toStr);
		return TRUE;
	}

	return FALSE;
}

void System::SysInfo::ReadKnownDLLs()
{
	TCHAR keyName[] = L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\KnownDLLs";
	_KnownDlls = Registry::GetValues(HKEY_LOCAL_MACHINE, keyName);

	TCHAR envVar[MAX_PATH] = { 0 };
	if (!FAILED(GetEnvironmentVariable(L"SystemRoot", envVar, MAX_PATH)))
	{
		ReplaceSubString(_KnownDlls[L"DllDirectory"], L"%SystemRoot%", envVar);
		ReplaceSubString(_KnownDlls[L"DllDirectory32"], L"%SystemRoot%", envVar);
	}
		
	/*for (auto item : values)
	{
		if (item.first == L"DllDirectory" || item.first == L"DllDirectory32")
			continue;

		WIN32_FIND_DATA fd;
		std::wstring fileName = dir + L"\\" + item.second;
		if (FindFirstFile(fileName.c_str(), &fd) != INVALID_HANDLE_VALUE)
			dlls.push_back(fileName);
	}*/
}