#include "stdafx.h"
#include "CWatcher.h"
#include "System.h"
#include "Logger.h"
#include "FileInfo.h"

Watcher::Watcher(const std::wstring& filename) : _filename(filename)
{
	if (!System::SetDebugPrivilegies(true))
	{
		System::ErrorExit(L"�� ������� ���������� ���������� ��� ������� ����������. ���������� ������ ����������");
		return;
	}

	Debug();
}

std::wstring EventInfo::What() const
{
	TCHAR str[1000];
	try {
		StringCchPrintf(str, sizeof(str), L"%02d:%02d:%02d.%03d  [%s] %s   %s",
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
			desc.at(code).c_str(), info->Print().c_str(), info->GetIDs().c_str());
	}
	catch (std::out_of_range)
	{
		return L"Unknown event code";
	}
	return str;
}

Watcher::~Watcher()
{
	System::SetDebugPrivilegies(false);

	Logger log(L"log.txt");
	std::wstring delimiter(L"___________________________________________________\n");
	log.Add(L"Process Launch Watcher 2.0\n");
	log.Add(delimiter);
	log.Add(System::GetOSName());
	log.Add(delimiter);
	log.Add(L"[ E] Exception     / ������������� ����������");
	log.Add(L"[CT] CreateThread  / ������ ����� �����");
	log.Add(L"[CP] CreateProcess / ������ ����� �������");
	log.Add(L"[ET] ExitThread    / ������ �����");
	log.Add(L"[EP] ExitProcess   / ������ �������");
	log.Add(L"[LD] Load DLL      / ��������� ����������");
	log.Add(L"[UD] Unload DLL    / ��������� ����������");
	log.Add(L"[DS] Debug String  / �������� ���������� ����������");
	log.Add(L"[ R] RIP Event\n");
	log.Add(L"B: Base, A: Address, P: Process, T: Thread");
	log.Add(delimiter);
	for (auto ev : _events)
	{
		log.Add(ev->What());
	}

	log.Add(L"\n��������: ���� - ���� - ������ - ������ - ������ - ������������� - �������� - ��������");
	log.Add(delimiter);
	for (auto p : _processList)
	{
		FileInfo fi(p.second->GetName());
		log.Add(fi.Serialize(" - ", FALSE));
	}
	
	log.Add(L"\n����������: ���� - ���� - ������ - ������ - ������ - ������������� - �������� - ��������");
	log.Add(delimiter);
	for (auto lib : _libList)
	{
		FileInfo fi(lib);
		log.Add(fi.Serialize(" - ", FALSE));
	}

	log.Save();
}

void Watcher::Debug()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, &_filename[0], NULL, NULL, FALSE, DEBUG_PROCESS, NULL, NULL, &si, &pi))
		System::ErrorExit(L"�� ������� ������������ � ��������");

	HANDLE hProcess = pi.hProcess;
	DWORD flags;
	DEBUG_EVENT event;
	ZeroMemory(&event, sizeof(DEBUG_EVENT));
	while (GetHandleInformation(hProcess, &flags))
	{
		if (!WaitForDebugEvent(&event, INFINITE))
			System::ErrorExit(L"�������� �� ���� ���������� ��������� �������");

		EventInfo ev(event.dwDebugEventCode);
		DWORD status = DBG_CONTINUE;
		switch (event.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			ev.info = _processList[event.dwProcessId] = std::make_shared<ProcessInfo>(event);
			hProcess = event.u.CreateProcessInfo.hProcess;
			break;
		case LOAD_DLL_DEBUG_EVENT:
			ev.info = std::make_shared<LibraryInfo>(event);
			_libList.emplace(std::dynamic_pointer_cast<LibraryInfo>(ev.info).get()->GetName());
			_memory[event.u.LoadDll.lpBaseOfDll] = ev.info;
			break;
		case UNLOAD_DLL_DEBUG_EVENT:
			ev.info = _memory[event.u.UnloadDll.lpBaseOfDll];
			_memory.erase(event.u.UnloadDll.lpBaseOfDll);
			break;
		case EXCEPTION_DEBUG_EVENT:
			ev.info = std::make_shared<ExceptionInfo>(event);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			status = DBG_TERMINATE_PROCESS;
			_processList[event.dwProcessId]->SetExitCode(event.u.ExitProcess.dwExitCode);
			ev.info = _processList[event.dwProcessId];
			break;
		case CREATE_THREAD_DEBUG_EVENT:
			ev.info = _threadList[event.dwThreadId] = std::make_shared<ThreadInfo>(event);
			break;
		case EXIT_THREAD_DEBUG_EVENT:
			status = DBG_TERMINATE_THREAD;
			_threadList[event.dwThreadId]->SetExitCode(event.u.ExitThread.dwExitCode);
			ev.info = _threadList[event.dwThreadId];
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			ev.info = std::make_shared<DebugStringInfo>(event, hProcess);
			break;
		case RIP_EVENT:
			ev.info = std::make_shared<RIPInfo>(event);
			break;
		default: break;
		}

		_events.push_back(std::make_shared<EventInfo>(ev));
		ContinueDebugEvent(event.dwProcessId, event.dwThreadId, status);
	}
}