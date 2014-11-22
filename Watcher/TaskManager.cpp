#include "stdafx.h"
#include "TaskManager.h"

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

		std::vector<MODULEENTRY32> mods;
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