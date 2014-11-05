#pragma once
namespace Registry
{
	static const DWORD MaxValueName = 16383;
	std::wstring GetValue(HKEY key, LPCTSTR subKey, LPCTSTR param);
	std::map<std::wstring, std::wstring> GetValues(HKEY hkey, LPCTSTR subKey);
};

