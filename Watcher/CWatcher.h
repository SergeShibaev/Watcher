#pragma once
#include "DebugInfo.h"

class EventInfo
{
	std::map<DWORD, std::wstring> desc;
public:
	DWORD code;
	SYSTEMTIME time;
	std::shared_ptr<DebugInfo> info;

	EventInfo(DWORD eventcode) : code(eventcode)
	{
		GetLocalTime(&time);
		desc[EXCEPTION_DEBUG_EVENT] = L" E";
		desc[CREATE_THREAD_DEBUG_EVENT] = L" T";
		desc[CREATE_PROCESS_DEBUG_EVENT] = L" P";
		desc[EXIT_THREAD_DEBUG_EVENT] = L"-T";
		desc[EXIT_PROCESS_DEBUG_EVENT] = L"-P";
		desc[LOAD_DLL_DEBUG_EVENT] = L" L";
		desc[UNLOAD_DLL_DEBUG_EVENT] = L"-L";
		desc[OUTPUT_DEBUG_STRING_EVENT] = L"DS";
		desc[RIP_EVENT] = L" R";
	}

	std::wstring What() const;
};

class Watcher
{
	std::wstring _filename;
	std::vector<std::shared_ptr<EventInfo> > _events;
	std::map<DWORD, std::shared_ptr<ProcessInfo> > _processList;
	std::set<std::wstring> _libList;
	std::map<DWORD, std::shared_ptr<ThreadInfo> > _threadList;
	std::map<LPVOID, std::shared_ptr<DebugInfo> > _memory;

	void Debug();
public:
	Watcher(const std::wstring& filename);
	void Save(const std::wstring& fileName);
	~Watcher();
};