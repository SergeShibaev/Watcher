#pragma once

class DebugInfo
{
	DWORD _processId;
	DWORD _threadId;
public:
	DebugInfo(DWORD p, DWORD t) 
		: _processId(p), _threadId(t) {}
	std::wstring GetIDs() const;
	virtual ~DebugInfo() {}
	virtual std::wstring Print() const = 0;
	virtual std::wstring Explain() const = 0;
};

class ProcessInfo : public DebugInfo
{
	DWORD _imageBase;
	DWORD _startAddress;
	std::wstring _imageName;
	WORD _unicode;
	DWORD _exitCode;
	HANDLE _handle;
public:
	ProcessInfo(const DEBUG_EVENT& event);
	void SetExitCode(DWORD code) { _exitCode = code; }
	DWORD GetExitCode() const { return _exitCode; }

	std::wstring Print() const override;
	std::wstring Explain() const override { return L""; }
};

class ThreadInfo : public DebugInfo
{
	DWORD _localBase;
	DWORD _startAddress;
	DWORD _exitCode;
public:
	ThreadInfo(const DEBUG_EVENT& event)
		: DebugInfo(event.dwProcessId, event.dwThreadId),
		_localBase((DWORD)event.u.CreateThread.lpThreadLocalBase), 
		_startAddress((DWORD)event.u.CreateThread.lpStartAddress),
		_exitCode(0) {}
	void SetExitCode(DWORD code) { _exitCode = code; }
	DWORD GetExitCode() const { return _exitCode; }

	std::wstring Print() const override;
	std::wstring Explain() const override { return L""; }
};

class LibraryInfo : public DebugInfo
{
	DWORD _baseAddr;
	std::wstring _imageName;
	WORD _unicode;
public:
	LibraryInfo(const DEBUG_EVENT& event);

	std::wstring Print() const override;
	std::wstring Explain() const override { return L""; }
};

class ExceptionInfo : public DebugInfo
{
	EXCEPTION_DEBUG_INFO _item;
	std::map<DWORD, LPCTSTR> _excInfo;
public:
	ExceptionInfo(const DEBUG_EVENT& event) 
		: DebugInfo(event.dwProcessId, event.dwThreadId), _item(event.u.Exception) 
	{
		_excInfo[EXCEPTION_ACCESS_VIOLATION] = L"У процесса недостаточно прав для получения доступа к виртуальному адресу";
		_excInfo[EXCEPTION_ARRAY_BOUNDS_EXCEEDED] = L"Попытка прочитать данные за пределами объявленного массива";
		_excInfo[EXCEPTION_BREAKPOINT] = L"Установлена точка останова (breakpoint)";
		_excInfo[EXCEPTION_DATATYPE_MISALIGNMENT] = L"DATATYPE_MISALIGNMENT";
		_excInfo[EXCEPTION_FLT_DENORMAL_OPERAND] = L"FLT_DENORMAL_OPERAND";
		_excInfo[EXCEPTION_FLT_DIVIDE_BY_ZERO] = L"Попытка деления на ноль";
		_excInfo[EXCEPTION_FLT_INEXACT_RESULT] = L"FLT_INEXACT_RESULT";
		_excInfo[EXCEPTION_FLT_INVALID_OPERATION] = L"Некорректная операция вычисления с плавающей точкой";
		_excInfo[EXCEPTION_FLT_OVERFLOW] = L"EXCEPTION_FLT_OVERFLOW";
		_excInfo[EXCEPTION_FLT_STACK_CHECK] = L"Переполнение стека при выполнении операции с плаваюшей точкой";
		_excInfo[EXCEPTION_FLT_UNDERFLOW] = L"EXCEPTION_FLT_UNDERFLOW";
		_excInfo[EXCEPTION_ILLEGAL_INSTRUCTION] = L"Попытка выполнить недопустимую команду";
		_excInfo[EXCEPTION_IN_PAGE_ERROR] = L"Процесс пытается получить доступ к несуществующей странице памяти";
		_excInfo[EXCEPTION_INT_DIVIDE_BY_ZERO] = L"Попытка деления целого числа на ноль";
		_excInfo[EXCEPTION_INT_OVERFLOW] = L"Целочисленное переполнение";
		_excInfo[EXCEPTION_INVALID_DISPOSITION] = L"Установлен некорректный обработчик исключений";
		_excInfo[EXCEPTION_NONCONTINUABLE_EXCEPTION] = L"Процесс пытается продолжить выполнение после генерации аварийного исключения";
		_excInfo[EXCEPTION_PRIV_INSTRUCTION] = L"Процесс пытается выполнить инструкцию, недопустимую в данном машинном режиме";
		_excInfo[EXCEPTION_SINGLE_STEP] = L"Подан сигнал о выполнении очередной команды";
		_excInfo[EXCEPTION_STACK_OVERFLOW] = L"Стек переполнен";
	}

	std::wstring Print() const override;
	std::wstring Explain() const override { return L""; }
};

class DebugStringInfo : public DebugInfo
{
	std::wstring _data;
public:
	DebugStringInfo(const DEBUG_EVENT& event, HANDLE h);

	std::wstring Print() const override { return _data; }
	std::wstring Explain() const override { return L""; }
};

class RIPInfo : public DebugInfo
{
	RIP_INFO _data;
public:
	RIPInfo(const DEBUG_EVENT& event) 
		: DebugInfo(event.dwProcessId, event.dwThreadId), _data(event.u.RipInfo) { }

	std::wstring Print() const override { return L""; }
	std::wstring Explain() const override { return L""; }
};