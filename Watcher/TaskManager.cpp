#include "stdafx.h"
#include "TaskManager.h"
#include <winternl.h>

TaskManager::TaskManager(DWORD flags, DWORD pId)
	: _snapshot(INVALID_HANDLE_VALUE)
{
	System::SetDebugPrivilegies(TRUE);
	CreateSnapshot(flags, pId);
}

TaskManager::~TaskManager()
{
	System::SetDebugPrivilegies(FALSE);
}

void TaskManager::CreateSnapshot(DWORD flags, DWORD pId)
{
	if (_snapshot != INVALID_HANDLE_VALUE)
		CloseHandle(_snapshot);
	_snapshot = (flags == 0) ? INVALID_HANDLE_VALUE : CreateToolhelp32Snapshot(flags, pId);
}

BOOL TaskManager::ProcessFirst(LPPROCESSENTRY32 pe) const
{
	BOOL done = Process32First(_snapshot, pe);
	if (done && pe->th32ProcessID == 0)
		done = ProcessNext(pe);

	return done;
}

BOOL TaskManager::ProcessNext(LPPROCESSENTRY32 pe) const
{
	BOOL done = Process32Next(_snapshot, pe);
	if (done && pe->th32ProcessID == 0)
		done = ProcessNext(pe);
	
	return done;
}

void TaskManager::GetActiveProcesses()
{
	_processes.clear();
	_modules.clear();

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL found = ProcessFirst(&pe);
	while (found)
	{
		_processes.push_back(pe);

		TaskManager mod(TH32CS_SNAPMODULE, pe.th32ProcessID);
		MODULEENTRY32 me;
		me.dwSize = sizeof(MODULEENTRY32);

		ModuleList mods;
		BOOL modFound = mod.ModuleFirst(&me);
		while (modFound) 
		{
			mods.push_back(me);
			modFound = mod.ModuleNext(&me);
		}
		_modules[pe.th32ProcessID] = mods;

		found = ProcessNext(&pe);
	}
}

BOOL TaskManager::GetProcessCmdLine(DWORD pId, LPTSTR imagePath, LPTSTR cmdLine, DWORD bufsize)
{
	System::SetDebugPrivilegies(TRUE);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pId);
	if (!hProcess)
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pId);
		if (hProcess)
		{
			QueryFullProcessImageName(hProcess, 0, imagePath, &bufsize);
			return TRUE;
		}
		//std::wstring error = System::GetLastErrorMessage(GetLastError());
		//wcscpy_s(cmdLine, error.size() * sizeof(TCHAR), error.c_str());
		return FALSE;
	}

	BOOL result = FALSE;
	typedef NTSTATUS(NTAPI *pNtQueryInformationProcess)(
		IN  HANDLE ProcessHandle,
		IN  PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN  ULONG ProcessInformationLength,
		OUT PULONG ReturnLength    OPTIONAL
		);
	pNtQueryInformationProcess NtQueryInformationProcess =
		(pNtQueryInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION pbi;
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
	if (!NtQueryInformationProcess(hProcess, PROCESSINFOCLASS::ProcessBasicInformation,
		&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL))
	{
		PEB peb;
		SIZE_T bytes;
		ZeroMemory(&peb, sizeof(peb));
		if (ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB), &bytes))
		{
			RTL_USER_PROCESS_PARAMETERS ProcParams;
			ZeroMemory(&ProcParams, sizeof(ProcParams));
			if (ReadProcessMemory(hProcess, peb.ProcessParameters, &ProcParams, sizeof(ProcParams), &bytes))
			{
				if (bufsize >= ProcParams.CommandLine.Length &&
					ReadProcessMemory(hProcess, ProcParams.CommandLine.Buffer, cmdLine, ProcParams.CommandLine.Length, &bytes))
				{
					ReadProcessMemory(hProcess, ProcParams.ImagePathName.Buffer, imagePath, ProcParams.ImagePathName.Length, &bytes);
					result = TRUE;
				}
			}
		}
	}
	CloseHandle(hProcess);
	System::SetDebugPrivilegies(FALSE);

	return result;
}