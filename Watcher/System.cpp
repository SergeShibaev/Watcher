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

std::wstring System::GetOSName()
{
	std::wstring sysName = L"Microsoft Windows ";
	if (IsWindows8Point1OrGreater())
		return sysName + L"8.1";
	if (IsWindows8OrGreater())
		return sysName + L"8";
	if (IsWindows7SP1OrGreater())
		return sysName + L"7 SP1";
	if (IsWindows7OrGreater())
		return sysName + L"7";
	if (IsWindowsVistaSP2OrGreater())
		return sysName + L"Vista SP2";
	if (IsWindowsVistaSP1OrGreater())
		return sysName + L"Vista SP1";
	if (IsWindowsVistaOrGreater())
		return sysName + L"Vista";
	if (IsWindowsXPSP3OrGreater())
		return sysName + L"XP SP3";
	if (IsWindowsXPSP2OrGreater())
		return sysName + L"XP SP2";
	if (IsWindowsXPSP1OrGreater())
		return sysName + L"XP SP1";
	if (IsWindowsXPOrGreater())
		return sysName + L"XP";
	return L"Unknown system";
}
