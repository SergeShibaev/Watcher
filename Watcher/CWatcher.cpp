#include "stdafx.h"
#include "CWatcher.h"
#include "System.h"

Watcher::Watcher(const std::wstring& filename) : _filename(filename)
{
	if (!System::SetDebugPrivilegies(true))
	{
		System::ErrorExit(L"Не удалось установить привилегии для отладки приложения. Дальнейшая работа невозможна");
		return;
	}

	Debug();
}

Watcher::~Watcher()
{
	System::SetDebugPrivilegies(false);
}

void Watcher::Debug()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, &_filename[0], NULL, NULL, FALSE, DEBUG_PROCESS, NULL, NULL, &si, &pi))
		System::ErrorExit(L"Не удалось подключиться к процессу");

	HANDLE hProcess = pi.hProcess;
	DWORD flags;
	DEBUG_EVENT event;
	ZeroMemory(&event, sizeof(DEBUG_EVENT));
	while (GetHandleInformation(hProcess, &flags))
	{
		if (!WaitForDebugEvent(&event, INFINITE))
			System::ErrorExit(L"Отладчик не смог обработать очередное событие");

		EventInfo ev(event.dwDebugEventCode);
		DWORD status = DBG_CONTINUE;
		switch (event.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			ev.info = new ProcessInfo(event);
			_processList.emplace(event.dwProcessId, dynamic_cast<ProcessInfo*>(ev.info));
			hProcess = event.u.CreateProcessInfo.hProcess;
			break;
		case LOAD_DLL_DEBUG_EVENT:
			ev.info = new LibraryInfo(event);
			_memory.emplace((DWORD)event.u.LoadDll.lpBaseOfDll, dynamic_cast<LibraryInfo*>(ev.info));
			break;
		case UNLOAD_DLL_DEBUG_EVENT:
			ev.info = _memory[(DWORD)event.u.UnloadDll.lpBaseOfDll];
			_memory.erase((DWORD)event.u.UnloadDll.lpBaseOfDll);
			break;
		case EXCEPTION_DEBUG_EVENT:
			ev.info = new ExceptionInfo(event);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			status = DBG_TERMINATE_PROCESS;
			_processList[event.dwProcessId]->SetExitCode(event.u.ExitProcess.dwExitCode);
			ev.info = _processList[event.dwProcessId];
			break;
		case CREATE_THREAD_DEBUG_EVENT:
			ev.info = new ThreadInfo(event);
			_threadList.emplace(event.dwThreadId, dynamic_cast<ThreadInfo*>(ev.info));
			break;
		case EXIT_THREAD_DEBUG_EVENT:
			status = DBG_TERMINATE_THREAD;
			_threadList[event.dwThreadId]->SetExitCode(event.u.ExitThread.dwExitCode);
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			ev.info = new DebugStringInfo(event, hProcess);
			break;
		case RIP_EVENT:
			ev.info = new RIPInfo(event);
			break;
		default: break;
		}

		_events.push_back(ev);
		ContinueDebugEvent(event.dwProcessId, event.dwThreadId, status);
	}
}