#include "StdAfx.h"
#include "FileInfo.h"

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Dbghelp.lib")

void FileInfo::ClearFileInfoParams()
{
	_params[COMPANY_NAME] = FileInfoParams(L"CompanyName", L"");
	_params[FILE_DESCRIPTION] = FileInfoParams(L"FileDescription", L"");
	_params[FILE_VERSION] = FileInfoParams(L"FileVersion", L"");
	_params[INTERNAL_NAME] = FileInfoParams(L"InternalName", L"");
	_params[LEGAL_COPYRIGHT] = FileInfoParams(L"LegalCopyright", L"");
	_params[LEGAL_TRADEMARKS] = FileInfoParams(L"LegalTradeMarks", L"");
	_params[ORIGINAL_FILENAME] = FileInfoParams(L"OriginalFilename", L"");
	_params[PRODUCT_NAME] = FileInfoParams(L"ProductName", L"");
	_params[PRODUCT_VERSION] = FileInfoParams(L"ProductVersion", L"");
	_params[COMMENTS] = FileInfoParams(L"Comments", L"");
	_params[AUTHOR] = FileInfoParams(L"Author", L"");
}

BOOL FileInfo::ReadFileVersionInfo()
{
	PLONG infoBuffer;  
	DWORD infoSize;    

	struct LANGANDCODEPAGE { 
		WORD wLanguage;
		WORD wCodePage;
	} *pLangCodePage;

	TCHAR paramNameBuf[256]; 
	TCHAR *paramValue;       
	UINT paramSz;            

	infoSize = GetFileVersionInfoSize(_fileName.c_str(), NULL);
	if ( infoSize > 0 )
	{
		infoBuffer = (PLONG) malloc(infoSize);

		if ( 0 != GetFileVersionInfo(_fileName.c_str(), NULL, infoSize, infoBuffer) )
		{
			UINT cpSz;

			if ( VerQueryValue(infoBuffer, L"\\VarFileInfo\\Translation", (LPVOID*) &pLangCodePage, &cpSz) )                       
			{
				for (size_t cpIdx = 0; cpIdx < (int)(cpSz/sizeof(struct LANGANDCODEPAGE)); cpIdx++ )
				{
					for (ULONG paramIdx=0; paramIdx<_params.size(); paramIdx++)
					{
						StringCchPrintf(paramNameBuf, sizeof(paramNameBuf)/sizeof(TCHAR), L"\\StringFileInfo\\%04x%04x\\%s",
							pLangCodePage[cpIdx].wLanguage,  
							pLangCodePage[cpIdx].wCodePage,  
							_params[paramIdx].first.c_str());

						if ( VerQueryValue(infoBuffer, paramNameBuf, (LPVOID*)&paramValue, &paramSz))
							_params[paramIdx].second = paramValue;
					}
				}
			}
		}

		free(infoBuffer);

		return TRUE;
	}

	return FALSE;
}

BOOL FileInfo::ReadFileInfoByHandle()
{
	HANDLE hFile = CreateFile(_fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ZeroMemory(&_hfi, sizeof(BY_HANDLE_FILE_INFORMATION));
	if (hFile != INVALID_HANDLE_VALUE)
	{		
		GetFileInformationByHandle(hFile, &_hfi);
		CloseHandle(hFile);

		return TRUE;
	}

	return FALSE;
}

std::wstring FileInfo::FileTimeToString(const FILETIME& ft) const
{
	if (ft.dwHighDateTime == 0 && ft.dwLowDateTime == 0)
		return L"";

	SYSTEMTIME st;
	TCHAR str[20];

	FileTimeToSystemTime(&ft, &st);
	StringCchPrintf(str, 20, L"%02d.%02d.%04d", st.wDay, st.wMonth, st.wYear);

	return str;
}

UINT64 FileInfo::GetFileSize() const
{
	return (_hfi.nFileSizeHigh << 16) | _hfi.nFileSizeLow;
}

std::wstring FileInfo::FileSizeToStr() const
{
	UINT64 size = GetFileSize();
	if (size == 0)
		return L"";

	TCHAR str[32];
	StringCchPrintf(str, sizeof(str)/sizeof(TCHAR), L"%u", size);

	return str;
}
std::wstring FileInfo::FileSizeToStrK() const
{
	UINT64 size = GetFileSize();
	if (size == 0)
		return L"";

	TCHAR str[65];
	StringCchPrintf(str, sizeof(str)/sizeof(TCHAR), L"%uK", size >> 10);

	return str;
}

void ExtractFileNameAndPath(const std::wstring& fullName, LPWSTR filePath, LPWSTR fileName)
{
	TCHAR	drive[5] = { 0 }, dir[MAX_PATH] = { 0 }, fname[MAX_PATH] = { 0 }, ext[MAX_PATH] = { 0 };
	_wsplitpath_s(fullName.c_str(), drive, dir, fname, ext);
	StringCchPrintf(filePath, MAX_PATH, L"%s%s", drive, dir);
	StringCchPrintf(fileName, MAX_PATH, L"%s%s", fname, ext);
	if (filePath[wcslen(filePath) - 1] == '\\')
		filePath[wcslen(filePath) - 1] = '\0';
}

std::wstring FileInfo::Serialize(const std::string separator, const BOOL sysPath) const
{
	std::wstring wsSeparator(separator.begin(), separator.end());
	std::wstring result = L"";
	TCHAR path[MAX_PATH] = { 0 }, name[MAX_PATH] = { 0 };
	if (sysPath)
		ExtractFileNameAndPath(GetSystemFileName(), path, name);
	else
		ExtractFileNameAndPath(GetFileName(), path, name);
	
	result += name;
	result += wsSeparator;
	result += path;
	result += wsSeparator;
	result += FileSizeToStr();
	result += wsSeparator;
	result += GetProductVersion();
	result += wsSeparator;
	result += GetCreationDate();
	result += wsSeparator;
	result += GetLastWriteTime();
	result += wsSeparator;
	result += GetFileDescription();
	result += wsSeparator;
	result += GetCompanyName();

	return result;
}
