#pragma once
#include <string>
#include <vector>
#include <map>
#include <WinInet.h>
#include "NValue.h"

namespace Utils {

	namespace StringUtils {
		std::string UnicodeToAnsi(const std::wstring& wstr);
		std::wstring AnsiToUnicode(const std::string& str);

		std::string UnicodeToUTF8(const std::wstring& wstr);
		std::wstring UTF8ToUnicode(const std::string& u8str);

		std::string AnsiToUTF8(const std::string& str);
		std::string UTF8ToAnsi(const std::string& u8str);

		namespace WString {
			// wstring
			int toInt(const std::wstring& wstr, size_t const mode = 10);
			double toDouble(const std::wstring& wstr);

			std::wstring fromInt(int n);
			std::wstring fromDouble(double d, size_t const s = 2);

			std::wstring toUpper(std::wstring wstr);
			std::wstring toLower(std::wstring wstr);

			std::vector<std::wstring> Split(const std::wstring& wstr, const std::wstring& sep);
			std::wstring Replace(const std::wstring& wstr, const std::wstring& repalceStr, const std::wstring& destStr);

			bool StartsWith(const std::wstring& wstr, const std::wstring& startStr, bool ignoreCase = false);
			bool EndsWith(const std::wstring& wstr, const std::wstring& endStr, bool ignoreCase = false);

			std::wstring Trim(const std::wstring& wstr, const wchar_t* chars = nullptr);
		}

		namespace String {
			// string
			int toInt(const std::string& str, size_t const mode = 10);
			double toDouble(const std::string& str);

			std::string fromInt(int n);
			std::string fromDouble(double d, size_t const s = 2);

			std::string toUpper(std::string wstr);
			std::string toLower(std::string wstr);

			std::vector<std::string> Split(const std::string& wstr, const std::string& sep);
			std::string Replace(const std::string& wstr, const std::string& repalceStr, const std::string& destStr);

			bool StartsWith(const std::string& wstr, const std::string& startStr, bool ignoreCase = false);
			bool EndsWith(const std::string& wstr, const std::string& endStr, bool ignoreCase = false);

			std::string Trim(const std::string& str, const char* chars = nullptr);
		}
	}

	namespace FilePathUtils {
		std::wstring FullPath(const std::wstring& path, const std::wstring& fileName);
		std::wstring ParentPath(const std::wstring& fileFullName);
		std::wstring FileName(const std::wstring& fileFullName, bool bIncludeExt = true);
		int SplitPath(const std::wstring& fileFullName, std::wstring& path, std::wstring& fileName);
		int SplitPathEx(const std::wstring& fileFullName, std::wstring& path, std::wstring& driver, std::wstring& fileName, std::wstring& extension);
		std::wstring TrimSlash(std::wstring& path);
		std::wstring GetCurrentPath();
	}

	namespace FileUtils {

		// return 0: not exist; 1, file; 2, folder
		int FileExists(const std::wstring& file);

		// if file is folder, will not delete it if it is not empty, and will not delete files recursive
		bool DeleteFileForce(const std::wstring& file, bool bForce = false);
		bool DeletePath(const std::wstring& path);
		bool CreatePath(const std::wstring& path);
		bool ReversePath(const std::wstring& path, std::vector<std::wstring>& vec, bool bRecursive = true);
		bool CopyPath(const std::wstring& src, const std::wstring& dst);
		bool MovePath(const std::wstring& src, const std::wstring& dst);

		bool CopyDirectory(const std::wstring& src, const std::wstring& dst);

		//复制文件或文件夹, 使用SHFileOperation，可以设置参数来定义各种行为
		bool SHCopyFile(const std::wstring& src, const std::wstring& dst);

		size_t FileSize(const std::wstring& file);

		FILETIME GetFileLastWriteTime(const std::wstring& file);
		FILETIME GetFileCreateTime(const std::wstring& file);

		bool WriteStringToFile(const std::wstring& file, const std::wstring& data);
		bool ReadStringFromFile(const std::wstring& file, std::wstring& data);
		bool WriteDataToFile(const std::wstring& file, const std::string& data);
		bool ReadDataFromFile(const std::wstring& file, std::string& data);

		bool WriteLine(const std::string& file, const std::string& data);
		bool ReadLine(const std::string& file, std::string& data);
	}

	namespace TimeUtils {
		time_t TimetFromFileTime(FILETIME ftime);
		FILETIME TimetToFileTime(time_t time);

		time_t TimetFromSystemTime(SYSTEMTIME sTime);
		SYSTEMTIME TimetToSystemTime(time_t time);

		FILETIME SystemTimeToFileTime(SYSTEMTIME sTime);
		SYSTEMTIME SysTemTimeFromFileTime(FILETIME fTime);

		time_t GetCurrentSystemTime();
		time_t GetCurrentSystemTimeUTC();

		std::wstring FormatTime(time_t time, const std::wstring& format = L"%Y-%m-%d %H:%M:%S");
	}

	namespace JsonUtils {
		std::string GetItemValue(const std::string& jsonString, const std::string& item);

	}

	namespace ZipUtils {
		bool ZipDirectory(const std::wstring& directory, const std::wstring& zipName = L"");
		bool UnzipFile(const std::wstring& zipName, const std::wstring& path = L"");

		bool ZipFile(const std::wstring& fileName, const std::wstring& zipName = L"");
//		bool UnzipFile(const std::wstring& zipName, const std::wstring& path);


		// 用于同一目录下的文件和文件夹进行压缩
		// files and zipname are all fullname 
		bool ZipFiles(const std::vector<std::wstring>& files, const std::wstring& zipName, const std::wstring& basePath, bool quitIfFail = true);
	}

	namespace RegUtils {
		bool IsKeyExist(HKEY hRoot, const std::wstring& strSubKey, const std::wstring& keyName);
		NValue GetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName);
		bool SetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName, NValue& value);

	}

	namespace HttpUtils {
		class HttpClient {
		public:
			HttpClient()
			{
			};
			HttpClient(const CString& url, const CString& userAgent = _T("")) :
				m_strUrl(url), m_strUserAgent(userAgent)
			{

			}
			~HttpClient()
			{
				Close();
			};

		public:
			void AddParam(const CString& key, const CString& value);
			void AddHeader(const CString& key, const CString& value);
			bool DownloadFile(const CString& url, const CString& fileName);
//			bool GetResponse(const CString& url);
			void Stop();
		private:
			void Close();
			void ParseUrl();
			BOOL isAcceptInvalidCert();
			bool Request();
			CString GetMethod() const;
			CString getRequestParameter() const;
			CString getRequestHeaders() const;
			BOOL GetContentLength(DWORD& cl) const;
			BOOL FetchToFile(LPCTSTR file, DWORD* readLen);
			BOOL CheckHttpStatus();
			BOOL read(BYTE* buf, DWORD bufLen, DWORD* readLen);

			CString m_strUserAgent;
			CString m_strMethod;
			CString m_strUrl;
			CString m_strHost, m_strUser, m_strPwd, m_strPath, m_strExtraInfo;
			int m_iPort, m_iScheme;

			BOOL m_bStop = FALSE;

			std::map<CString, CString> headers;
			std::map<CString, CString> params;

			HINTERNET m_hSession{ NULL };
			HINTERNET m_hInternet{ NULL };
			HINTERNET m_hRequest{ NULL };

		};
	}

}

