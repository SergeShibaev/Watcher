#include "StdAfx.h"
#include "InfoTable.h"

void InfoTable::AddColumn(UINT mask, int fmt, int cx, LPTSTR pszText, int iSubitem)
{
	LVCOLUMN col;
	ZeroMemory(&col, sizeof(col));
	col.mask = mask;
	col.fmt = fmt;  
	col.cx = cx;
	col.pszText = pszText;
	col.iSubItem = iSubitem;
	ListView_InsertColumn(_hTable, col.iSubItem, &col);
}

void InfoTable::InsertItem(size_t iItem, std::wstring pszText)
{
	LVITEM item;
	ZeroMemory(&item, sizeof(item));

	item.mask = LVIF_TEXT;
	item.iItem = static_cast<int>(iItem);
	item.pszText = (LPWSTR)pszText.c_str();
	ListView_InsertItem(_hTable, &item);
}

void InfoTable::InsertSubitem(size_t iItem, DWORD iSubItem, std::wstring pszText)
{
	LVITEM subitem;
	ZeroMemory(&subitem, sizeof(subitem));
	subitem.mask = LVIF_TEXT;
	subitem.iItem = static_cast<int>(iItem);
	subitem.iSubItem = iSubItem;
	subitem.pszText = (LPWSTR)pszText.c_str();
	ListView_InsertItem(_hTable, &subitem);
	ListView_SetItemText(_hTable, iItem, subitem.iSubItem, subitem.pszText);
}

void InfoTable::AppendItem(std::wstring pszText)
{
	InsertItem(GetItemCount(), pszText);
}

void InfoTable::AppendItem(std::wstring itemText, std::wstring subitemText)
{
	AppendItem(itemText);
	InsertSubitem(GetItemCount()-1, 1, subitemText);
}

void InfoTable::AppendItem(std::wstring pszText, DWORD value)
{
	WCHAR strValue[100];
	if (value > 15)
		StringCchPrintf(strValue, 100, L"%d   (0x%X)", value, value);
	else
		StringCchPrintf(strValue, 100, L"%d", value);
	AppendItem(pszText);
	InsertSubitem(GetItemCount()-1, 1, strValue);
}

std::wstring ValueAsStr(DWORD value)
{
	WCHAR str[10];
	StringCchPrintf(str, 10, L"%d", value);
	return str;
}