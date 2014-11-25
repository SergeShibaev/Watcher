// Watcher.cpp: ���������� ����� ����� ��� ����������.
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

// ���������� ����������:
HINSTANCE hInst;								// ������� ���������
TCHAR szTitle[MAX_LOADSTRING];					// ����� ������ ���������
TCHAR szWindowClass[MAX_LOADSTRING];			// ��� ������ �������� ����
HWND hDlg;										// ������� ���� �������

enum Tasks { TASK_WATCHER, TASK_TASKMAN };

// ��������� ���������� �������, ���������� � ���� ������ ����:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	MainDlgProc(HWND, UINT, WPARAM, LPARAM);
VOID ResizeControls(HWND hDlg);
VOID RunTaskManager();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���������� ��� �����.
	MSG msg;
	HACCEL hAccelTable;

	// ������������� ���������� �����
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WATCHER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ��������� ������������� ����������:
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

	

	// ���� ��������� ���������:
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
//  �������: MyRegisterClass()
//
//  ����������: ������������ ����� ����.
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
//   �������: InitInstance(HINSTANCE, int)
//
//   ����������: ��������� ��������� ���������� � ������� ������� ����.
//
//   �����������:
//
//        � ������ ������� ���������� ���������� ����������� � ���������� ����������, � �����
//        ��������� � ��������� �� ����� ������� ���� ���������.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��������� ���������� ���������� � ���������� ����������

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

	switch (taskID)
	{
	case TASK_WATCHER:
		table.AddTextColumnL(L"����������", 0, 250);
		break;
	case TASK_TASKMAN:
		table.AddTextColumnL(L"�������", 0, 150);
		table.AddTextColumnR(L"PID", 1, 50);
		table.AddTextColumnL(L"������ �������", 2, 400);
		table.AddTextColumnL(L"��������", 3, 250);
		table.AddTextColumnL(L"��������", 4, 150);
		table.AddTextColumnL(L"������", 5, 120);
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
		// ��������� ����� � ����:
		switch (wmId)
		{
		case IDM_OPEN:
			InitMainDialog(TASK_WATCHER);
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
		// TODO: �������� ����� ��� ���������...
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

	GUIControl table(GetDlgItem(hDlg, IDC_TABLE));
	table.Maximize();
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