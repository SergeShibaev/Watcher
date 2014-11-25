#pragma once

namespace System {

	class SysInfo
	{
		std::wstring _OSName;
		std::wstring _OSBuildLab;
		std::wstring _OSProductName;
		std::wstring _OSVersion;
		std::wstring _AppInitDlls;
		std::wstring _processorID;
		std::map<std::wstring, std::wstring> _KnownDlls;

		void ReadOSName();
		void ReadOSBuildLab();
		void ReadProductName();
		void ReadOSVersion();
		void ReadAppInitDLLs();
		void ReadKnownDLLs();
		void ReadProcessorID();

	public:
		SysInfo()
		{
			ReadOSName();
			ReadOSBuildLab();
			ReadProductName();
			ReadOSVersion();
			ReadAppInitDLLs();
			ReadKnownDLLs();
			ReadProcessorID();
		}

		std::wstring GetOSName() const { return _OSName; }
		std::wstring GetOSBuildLab() const { return _OSBuildLab; }
		std::wstring GetProductName() const { return _OSProductName; }
		std::wstring GetOSVersion() const { return _OSVersion; }
		std::wstring GetAppInitDLLs() const { return _AppInitDlls; }
		std::map<std::wstring, std::wstring> GetKnownDLLs() const { return _KnownDlls; }
		std::wstring GetProcessorID() const { return _processorID; }
	};

	std::wstring GetLastErrorMessage(DWORD errCode);
	void ErrorExit(const std::wstring& reason);
	bool SetDebugPrivilegies(BOOL enable);
	bool GetFileName(HANDLE hFile, std::wstring& filename);
}