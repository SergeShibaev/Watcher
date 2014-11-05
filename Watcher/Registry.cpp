#include "stdafx.h"
#include "Registry.h"

std::wstring Registry::GetValue(HKEY key, LPCTSTR subKey, LPCTSTR param)
{
	HKEY hKey;
	DWORD bytes = 0;
	std::wstring value;

	if (RegOpenKeyEx(key, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS &&
		RegQueryValueEx(hKey, param, 0, NULL, NULL, &bytes) == ERROR_SUCCESS && bytes > 0)
	{
		value.resize(bytes / sizeof(TCHAR));
		RegQueryValueEx(hKey, param, 0, NULL, (LPBYTE)&value[0], &bytes);
	}

	return value.c_str();
}

std::map<std::wstring, std::wstring> Registry::GetValues(HKEY key, LPCTSTR subKey)
{
	HKEY hKey;
	DWORD amount = 0;
	std::map<std::wstring, std::wstring> values;

	if (RegOpenKeyEx(key, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS &&
		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &amount, NULL, NULL, NULL, NULL) == ERROR_SUCCESS && amount > 0)
	{
		for (DWORD n = 0; n < amount; ++n)
		{
			DWORD cchValue = MaxValueName;
			TCHAR value[MaxValueName];
			DWORD cbData = 1000;
			TCHAR data[1000];
			if (RegEnumValue(hKey, n, value, &cchValue, 0, NULL, (LPBYTE)data, &cbData) != ERROR_SUCCESS)
				break;
			values[value] = data;
		}
	}

	return values;
}