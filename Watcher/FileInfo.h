#pragma once
#include <Strsafe.h>
#include <WinVer.h>
#include <DbgHelp.h>

class FileInfo
{
	enum {
		COMPANY_NAME, FILE_DESCRIPTION, FILE_VERSION, INTERNAL_NAME, LEGAL_COPYRIGHT, LEGAL_TRADEMARKS,
		ORIGINAL_FILENAME, PRODUCT_NAME, PRODUCT_VERSION, COMMENTS, AUTHOR, FILE_INFO_COUNT
	};

	typedef std::pair<std::wstring, std::wstring> FileInfoParams;
	std::map<DWORD, FileInfoParams> _params;
	BY_HANDLE_FILE_INFORMATION _hfi;
	std::wstring _fileName;

	void ClearFileInfoParams();
	BOOL ReadFileInfoByHandle();
	BOOL ReadFileVersionInfo();
	std::wstring FileTimeToString(const FILETIME& ft) const;	
public:
	FileInfo::FileInfo(const std::wstring& fileName) : _fileName(fileName)
	{
		ClearFileInfoParams();
		ReadFileVersionInfo();	
		ReadFileInfoByHandle();
	}

	UINT64 GetFileSize() const;
	std::wstring FileSizeToStr() const;
	std::wstring FileSizeToStrK() const;
	std::wstring GetSystemFileName() const
	{
		std::wstring sysFileName = _fileName;
		//SysInfo::ReplaceSystemPaths(sysFileName);

		return sysFileName;
	}
	std::wstring GetFileName() const { return _fileName; }
	std::wstring GetCreationDate() const { return FileTimeToString(_hfi.ftCreationTime); }
	std::wstring GetLastWriteTime() const { return FileTimeToString(_hfi.ftLastWriteTime); }
	std::wstring GetCompanyName() const { return _params.at(COMPANY_NAME).second; }
	std::wstring GetFileDescription() const { return _params.at(FILE_DESCRIPTION).second; }
	std::wstring GetProductVersion() const { return _params.at(PRODUCT_VERSION).second; }
	std::wstring Serialize(const std::string separator, const BOOL sysPath) const;	
};
