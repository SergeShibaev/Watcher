#pragma once
#include "DebugInfo.h"

class EventInfo
{
public:
	DWORD code;
	SYSTEMTIME time;
	DebugInfo *info;

	EventInfo(DWORD eventcode) : code(eventcode)
	{
		GetLocalTime(&time);
	}
};

class Watcher
{
	std::wstring _filename;
	std::vector<EventInfo> _events;
	std::map<DWORD, ProcessInfo*> _processList;
	std::map<DWORD, ThreadInfo*> _threadList;
	std::map<DWORD, DebugInfo*> _memory;

	void Debug();
public:
	Watcher(const std::wstring& filename);
	~Watcher();
};

