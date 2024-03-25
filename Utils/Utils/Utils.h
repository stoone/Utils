#pragma once
#include <string>
#include <vector>
#include <map>
//#include <WinInet.h>
#include "NValue.h"
#include "Utility.h"
#include <functional>
#include "FileUtils.h"
#include "StringUtils.h"
#include "NBuffer.h"
#include "BMPUtils.h"
#include "TimeUtils.h"
#include "TaskManager.h"
#include "LogUtils.h"
#include "win.h"
#include "STLUtils.h"


#define RELEASE_POINT(p) if (p)           \\
                         {                \\
							delete p;     \\
							p = nullptr;  \\
						 }

#define RELEASE_ARRAY(p)  if (p)          \\
						  {               \\
							delete[] p;   \\
							p = nullptr;  \\
                          }

#define lock_mutex(mtx)    std::lock_guard<std::mutex> lk(mtx)
#define lock_rmutex(rmtx)  std::lock_guard<std::recursive_mutex> lk(rmtx)

namespace Utils {	

	using WString = Utils::StringUtils<std::wstring, wchar_t>;
	using String = Utils::StringUtils<std::string, char>;

//	namespace JsonUtils {
//		std::string GetItemValue(const std::string& jsonString, const std::string& item);
//
//	}
//
//	namespace ZipUtils {
//		bool ZipDirectory(const std::wstring& directory, const std::wstring& zipName = L"");
//		bool UnzipFile(const std::wstring& zipName, const std::wstring& path = L"");
//
//		bool ZipFile(const std::wstring& fileName, const std::wstring& zipName = L"");
////		bool UnzipFile(const std::wstring& zipName, const std::wstring& path);
//
//
//		// 用于同一目录下的文件和文件夹进行压缩
//		// files and zipname are all fullname 
//		bool ZipFiles(const std::vector<std::wstring>& files, const std::wstring& zipName, const std::wstring& basePath, bool quitIfFail = true);
//	}

	//namespace RegUtils {
	//	bool IsKeyExist(HKEY hRoot, const std::wstring& strSubKey, const std::wstring& keyName);
	//	NValue GetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName);
	//	bool SetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName, NValue& value);

	//}
/*
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
*/

}

