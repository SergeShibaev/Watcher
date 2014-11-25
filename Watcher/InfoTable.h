#pragma once
#include <CommCtrl.h>

class InfoTable
{
	const static DWORD TextCol = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	HWND _hTable;
	void AddColumn(UINT mask, int fmt, int cx, LPTSTR pszText, int iSubitem);
	void AddTextColumn(LPTSTR text, int iSubItem, int width, DWORD fmt) { AddColumn(TextCol, fmt, width, text, iSubItem); }
public:
	InfoTable() { }
	InfoTable(HWND hTable) : _hTable(hTable)
	{
		while (ListView_DeleteColumn(_hTable, 0));
		DeleteAllItems();
		ShowWindow(_hTable, SW_NORMAL);
	}
	HWND GetHandle() { return _hTable; }
	void AddTextColumnL(LPTSTR text, int iSubItem, int width) { AddTextColumn(text, iSubItem, width, LVCFMT_LEFT); }
	void AddTextColumnR(LPTSTR text, int iSubItem, int width) { AddTextColumn(text, iSubItem, width, LVCFMT_RIGHT); }
	void AddTextColumnC(LPTSTR text, int iSubItem, int width) { AddTextColumn(text, iSubItem, width, LVCFMT_CENTER); }
	void InsertItem(size_t item, std::wstring pszText);
	void InsertSubitem(size_t item, DWORD subItem, std::wstring pszText);
	void AppendItem(std::wstring pszText);
	void AppendItem(std::wstring pszText, DWORD value);
	void AppendItem(std::wstring itemText, std::wstring subitemText);
	DWORD GetItemCount() { return ListView_GetItemCount(_hTable); }
	void DeleteAllItems() { ListView_DeleteAllItems(_hTable); }
	void Update() { UpdateWindow(_hTable); }
};

std::wstring ValueAsStr(DWORD value);