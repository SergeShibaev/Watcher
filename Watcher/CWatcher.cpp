#include "stdafx.h"
#include "CWatcher.h"
#include "System.h"
#include "Logger.h"
#include "FileInfo.h"

Watcher::Watcher(const std::wstring& filename) : _filename(filename)
{
	if (!System::SetDebugPrivilegies(true))
		System::ErrorExit(L"Не удалось установить привилегии для отладки приложения. Дальнейшая работа невозможна");
}

std::wstring EventInfo::What(bool full) const
{
	TCHAR str[1000];
	try {
		std::wstring desc = full ? _descFull.at(code) : _desc.at(code);
		StringCchPrintf(str, sizeof(str), L"%02d:%02d:%02d.%03d  %s %s   %s",
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
			desc.c_str(), info->Print().c_str(), info->GetIDs().c_str());
	}
	catch (std::out_of_range)
	{
		return L"Unknown event code";
	}
	return str;
}

void Watcher::Save(const std::wstring& fileName)
{
	System::SysInfo sysInfo;
	Logger log(fileName);
	const std::wstring delimiter(80, '-');
	log.Add(L"Process Launch Watcher 2.0  by Serge Shibaev  (http://sergeshibaev.ru)");
	log.Add(delimiter);
	log.Add(L"Processor: " + sysInfo.GetProcessorID() + L"\n");
	log.Add(L"OS: " + sysInfo.GetOSName() + L" (" + sysInfo.GetOSVersion() + L")");
	log.Add(L"Product: " + sysInfo.GetProductName());
	log.Add(L"BuildLab: " + sysInfo.GetOSBuildLab() + L"\n");
	log.Add(L"AppInit_DLLs: " + sysInfo.GetAppInitDLLs());
	log.Add(delimiter);
	log.Add(L"[ E] Exception     / Сгенерировано исключение");
	log.Add(L"[ T] CreateThread  / Создан новый поток");
	log.Add(L"[ P] CreateProcess / Создан новый процесс");
	log.Add(L"[-T] ExitThread    / Удален поток");
	log.Add(L"[-P] ExitProcess   / Удален процесс");
	log.Add(L"[ L] Load DLL      / Загружена библиотека");
	log.Add(L"[-L] Unload DLL    / Выгружена библиотека");
	log.Add(L"[DS] Debug String  / Получена отладочная информация");
	log.Add(L"[ R] RIP Event\n");
	log.Add(L"B: Base, A: Address, P: Process, T: Thread");
	log.Add(std::wstring(50, '-'));
	for (auto ev : _events)
	{
		log.Add(ev->What(FALSE));
	}

	log.Add(L"\nБиблиотеки: файл - путь - размер - версия - создан - модифицирован - описание - копирайт");
	log.Add(delimiter);
	for (auto lib : _libList)
	{
		FileInfo fi(lib);
		log.Add(fi.Serialize(" - ", FALSE));
	}

	log.Add(L"\nKnown DLLs");
	log.Add(std::wstring(10, '-'));
	auto knownDlls = sysInfo.GetKnownDLLs();
	log.Add(L"DllDirectory: " + knownDlls[L"DllDirectory"]);
	log.Add(L"DllDirectory32: " + knownDlls[L"DllDirectory32"] + L"\n");

	for (auto lib : knownDlls)
	{
		if (lib.first == L"DllDirectory" || lib.first == L"DllDirectory32")
			continue;

		WIN32_FIND_DATA fd;
		std::wstring fileName = knownDlls[L"DllDirectory"] + L"\\" + lib.second;
		if (FindFirstFile(fileName.c_str(), &fd) == INVALID_HANDLE_VALUE)
			log.Add(fileName + L" - файл не найден");
		else
		{
			FileInfo fi(fileName);
			log.Add(fi.Serialize(" - ", FALSE));
			_libList.insert(fileName);
		}
	}

	log.Add(L"\nПроцессы: файл - путь - размер - версия - создан - модифицирован - описание - копирайт");
	log.Add(delimiter);
	for (auto p : _processList)
	{
		FileInfo fi(p.second->GetName());
		log.Add(fi.Serialize(" - ", FALSE));
	}

	log.Save();
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
	while (GetHandleInformation(hProcess, &flags))
	{
		ZeroMemory(&event, sizeof(DEBUG_EVENT));
		if (!WaitForDebugEvent(&event, INFINITE))
			System::ErrorExit(L"Отладчик не смог обработать очередное событие");

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
			_libList.insert(std::dynamic_pointer_cast<LibraryInfo>(ev.info).get()->GetName());
			_memory[event.u.LoadDll.lpBaseOfDll] = ev.info;
			break;
		case UNLOAD_DLL_DEBUG_EVENT:
			ev.info = _memory[event.u.UnloadDll.lpBaseOfDll];
			_memory.erase(event.u.UnloadDll.lpBaseOfDll);
			break;
		case EXCEPTION_DEBUG_EVENT:
			status = DBG_EXCEPTION_NOT_HANDLED;
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

		std::unique_lock<std::mutex> locker(M);
		_events.push_back(std::make_shared<EventInfo>(ev));
		Cond.notify_one();
		ContinueDebugEvent(event.dwProcessId, event.dwThreadId, status);
	}
}