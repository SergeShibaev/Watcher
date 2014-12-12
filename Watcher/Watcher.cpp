// Watcher.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Watcher.h"
#include "CWatcher.h"
#include "TaskManager.h"
#include "HttpSender.h"
#include "GUIControl.h"
#include "InfoTable.h"
#include "FileInfo.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна
HWND hDlg;										// главное окно диалога

enum Tasks { TASK_WATCHER, TASK_TASKMAN, MAX_TASK };

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	MainDlgProc(HWND, UINT, WPARAM, LPARAM);

VOID ResizeControls(HWND hDlg);
VOID RunTaskManager();
VOID RunWatcher();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WATCHER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, SW_MAXIMIZE))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WATCHER));

	//Watcher watcher(L"C:\\Windows\\write.exe");
	//ShellExecute(NULL, L"open", L"http://sergeshibaev.ru", NULL, NULL, SW_NORMAL);

	//HttpSender sender(L"sergeshibaev.ru");
	//sender.SendRequest(L"watcher/watcher.php", "OS=6.2.9200&test=string+string");

	//watcher.Save(L"log.txt");

	

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WATCHER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WATCHER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

VOID InitMainDialog(DWORD taskID)
{
	InfoTable table(GetDlgItem(hDlg, IDC_TABLE));
	GUIControl log(GetDlgItem(hDlg, IDC_LIST1));
	log.Show();

	switch (taskID)
	{
	case TASK_WATCHER:
		table.AddTextColumnL(L"Библиотека", 0, 150);
		table.AddTextColumnL(L"Путь", 1, 250);
		table.AddTextColumnL(L"Описание", 2, 250);
		table.AddTextColumnL(L"Копирайт", 3, 150);
		table.AddTextColumnL(L"Версия", 4, 120);
		table.AddTextColumnL(L"KnownDLLs", 5, 75);
		break;
	case TASK_TASKMAN:
		table.AddTextColumnL(L"Процесс", 0, 150);
		table.AddTextColumnR(L"PID", 1, 50);
		table.AddTextColumnL(L"Строка запуска", 2, 400);
		table.AddTextColumnL(L"Описание", 3, 250);
		table.AddTextColumnL(L"Копирайт", 4, 150);
		table.AddTextColumnL(L"Версия", 5, 120);
		break;
	default:
		table.DeleteAllItems();
	}
	
	ShowWindow(hDlg, SW_MAXIMIZE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		hDlg = CreateDialog(NULL, MAKEINTRESOURCE(IDD_MAINDIALOG), hWnd, MainDlgProc);
		InitMainDialog(TASK_WATCHER);
		break;
	case WM_SIZE:
		ResizeControls(hDlg);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_OPEN:
			RunWatcher();
			break;
		case IDM_TASKMAN:
			RunTaskManager();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK MainDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_SIZE:
		ResizeControls(hWnd);
		break;
	case WM_COMMAND:
		break;
	}
	return (INT_PTR)FALSE;
}

VOID ResizeControls(HWND hDlg)
{
	GUIControl main(hDlg);
	main.Maximize();

	GUIControl log(GetDlgItem(hDlg, IDC_LIST1), main.width(), 200);
	log.Move(0, main.height() - log.height());

	GUIControl table(GetDlgItem(hDlg, IDC_TABLE), main.width(), main.height() - log.height());
	table.Move(0, 0);
}

VOID RunTaskManager()
{
	TaskManager tm;
	tm.GetActiveProcesses();

	InfoTable table(GetDlgItem(hDlg, IDC_TABLE));
	InitMainDialog(TASK_TASKMAN);

	for (auto p : tm.GetProcesses())
	{
		TCHAR imageName[MAX_PATH] = { 0 };
		TCHAR cmdLine[10000] = { 0 };

		TaskManager::GetProcessCmdLine(p.th32ProcessID, imageName, cmdLine, sizeof(cmdLine)/sizeof(TCHAR));
		std::wstring fileName = (imageName[0]) ? imageName : p.szExeFile;
		if (!cmdLine[0])
			wcscpy_s(cmdLine, fileName.size() * sizeof(TCHAR), fileName.c_str());
		FileInfo fi(fileName);

		DWORD line = table.GetItemCount();
		table.AppendItem(p.szExeFile);
		table.InsertSubitem(line, 1, ValueAsStr(p.th32ProcessID));
		table.InsertSubitem(line, 2, cmdLine);
		table.InsertSubitem(line, 3, fi.GetFileDescription());
		table.InsertSubitem(line, 4, fi.GetCompanyName());
		table.InsertSubitem(line, 5, fi.GetProductVersion());
	}
}

BOOL GetOpenFileName(LPTSTR fileName)
{
	TCHAR curDir[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, curDir);

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Executable files (*.exe)\0*.exe\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = L"Выбор файла";
	ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FORCESHOWHIDDEN;

	GetOpenFileName(&ofn);

	SetCurrentDirectory(curDir);

	if (ofn.lpstrFile[0] != '\0')
		return TRUE;
	else
		return FALSE;
}

VOID RunWatcher()
{
	TCHAR fileName[MAX_PATH] = { 0 };
	if (!GetOpenFileName(fileName))
		return;

	InfoTable table(GetDlgItem(hDlg, IDC_TABLE));
	InitMainDialog(TASK_WATCHER);
	
	HWND hList = GetDlgItem(hDlg, IDC_LIST1);
	ListBox_ResetContent(hList);

	Watcher watcher(fileName);
	auto future = std::async(std::launch::async, [&] { watcher.Debug(); });

	size_t cnt = 0;
	while (future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
	{
		std::unique_lock<std::mutex> lk(watcher.M);
		watcher.Cond.wait(lk, [&] { return watcher.GetEvents().size() > cnt; });
		while (cnt < watcher.GetEvents().size())
			ListBox_AddString(hList, watcher.GetEvents()[cnt++]->What(true).c_str());
		UpdateWindow(hList);
	}

	System::SysInfo si;
	for (auto lib : watcher.GetLibraries())
	{
		TCHAR path[MAX_PATH], name[MAX_PATH];
		ExtractFileNameAndPath(lib, path, name);
		DWORD line = table.GetItemCount();

		table.AppendItem(name);
		table.InsertSubitem(line, 1, path);

		FileInfo fi(lib);
		table.InsertSubitem(line, 2, fi.GetFileDescription());
		table.InsertSubitem(line, 3, fi.GetCompanyName());
		table.InsertSubitem(line, 4, fi.GetProductVersion());
	}
	
	watcher.Save(L"plw.log");
}