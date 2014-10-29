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
		desc[CREATE_THREAD_DEBUG_EVENT] = L"CT";
		desc[CREATE_PROCESS_DEBUG_EVENT] = L"CP";
		desc[EXIT_THREAD_DEBUG_EVENT] = L"ET";
		desc[EXIT_PROCESS_DEBUG_EVENT] = L"EP";
		desc[LOAD_DLL_DEBUG_EVENT] = L"LD";
		desc[UNLOAD_DLL_DEBUG_EVENT] = L"UD";
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
	std::map<DWORD, std::shared_ptr<ThreadInfo> > _threadList;
	std::map<DWORD, std::shared_ptr<DebugInfo> > _memory;

	void Debug();
public:
	Watcher(const std::wstring& filename);
	~Watcher();
};

