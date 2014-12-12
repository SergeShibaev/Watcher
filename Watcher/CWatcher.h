#pragma once
#include "DebugInfo.h"

class EventInfo
{
	std::map<DWORD, std::wstring> _desc;
	std::map<DWORD, std::wstring> _descFull;
public:
	DWORD code;
	SYSTEMTIME time;
	std::shared_ptr<DebugInfo> info;

	EventInfo(DWORD eventcode) : code(eventcode)
	{
		GetLocalTime(&time);
		_desc[EXCEPTION_DEBUG_EVENT] = L"[ E]";
		_desc[CREATE_THREAD_DEBUG_EVENT] = L"[ T]";
		_desc[CREATE_PROCESS_DEBUG_EVENT] = L"[ P]";
		_desc[EXIT_THREAD_DEBUG_EVENT] = L"[-T]";
		_desc[EXIT_PROCESS_DEBUG_EVENT] = L"[-P]";
		_desc[LOAD_DLL_DEBUG_EVENT] = L"[ L]";
		_desc[UNLOAD_DLL_DEBUG_EVENT] = L"[-L]";
		_desc[OUTPUT_DEBUG_STRING_EVENT] = L"[DS]";
		_desc[RIP_EVENT] = L"[ R]";

		_descFull[EXCEPTION_DEBUG_EVENT] = L"Исключение";
		_descFull[CREATE_THREAD_DEBUG_EVENT] = L"Запущен поток";
		_descFull[CREATE_PROCESS_DEBUG_EVENT] = L"Создан процесс";
		_descFull[EXIT_THREAD_DEBUG_EVENT] = L"Уничтожен поток";
		_descFull[EXIT_PROCESS_DEBUG_EVENT] = L"Уничтожен процесс";
		_descFull[LOAD_DLL_DEBUG_EVENT] = L"Загружена библиотека";
		_descFull[UNLOAD_DLL_DEBUG_EVENT] = L"Выгружена библиотека";
		_descFull[OUTPUT_DEBUG_STRING_EVENT] = L"Отладочная информация";
		_descFull[RIP_EVENT] = L"RIP";
	}

	std::wstring What(bool full) const;
	std::wstring Explain() const;
};

class Watcher
{
	typedef std::vector<std::shared_ptr<EventInfo> > EventList;
	typedef std::set<std::wstring> LibList;
public:
	Watcher(const std::wstring& filename);
	void Debug();
	void Save(const std::wstring& fileName);
	EventList GetEvents() const { return _events; }
	LibList GetLibraries() const { return _libList; }
	~Watcher();

	std::mutex M;
	std::condition_variable Cond;
private:
	Watcher();
	Watcher(const Watcher&);
	Watcher& operator=(const Watcher&);

	std::wstring _filename;
	EventList _events;
	std::map<DWORD, std::shared_ptr<ProcessInfo> > _processList;
	LibList _libList;
	std::map<DWORD, std::shared_ptr<ThreadInfo> > _threadList;
	std::map<LPVOID, std::shared_ptr<DebugInfo> > _memory;

};