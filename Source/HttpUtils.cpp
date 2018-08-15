#include "stdafx.h"
#include "Utils.h"

// 每次读取1K 
#define HTTP_BUF_SIZE    1024

#pragma comment(lib, "WinINet.lib") 

namespace Utils {
	namespace HttpUtils {
		
		void HttpClient::ParseUrl()
		{
			if (m_strUrl.IsEmpty()) return;


			URL_COMPONENTS uc;
			RtlZeroMemory(&uc, sizeof(URL_COMPONENTS));
			uc.dwStructSize = sizeof(URL_COMPONENTS);

			TCHAR szHost[INTERNET_MAX_HOST_NAME_LENGTH] = { 0 };
			uc.lpszHostName = szHost;
			uc.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;

			TCHAR szPath[INTERNET_MAX_PATH_LENGTH] = { 0 };
			uc.lpszUrlPath = szPath;
			uc.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;

			TCHAR szUser[INTERNET_MAX_USER_NAME_LENGTH] = { 0 };
			uc.lpszUserName = szUser;
			uc.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;

			TCHAR szPwd[INTERNET_MAX_PASSWORD_LENGTH] = { 0 };
			uc.lpszPassword = szPwd;
			uc.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;

			TCHAR szExtra[INTERNET_MAX_PATH_LENGTH] = { 0 };
			uc.lpszExtraInfo = szExtra;
			uc.dwExtraInfoLength = INTERNET_MAX_PATH_LENGTH;

			TCHAR szScheme[INTERNET_MAX_SCHEME_LENGTH] = { 0 };
			uc.lpszScheme = szScheme;
			uc.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;

			if (InternetCrackUrl(m_strUrl, m_strUrl.GetLength(), 0, &uc))
			{
				m_iPort = uc.nPort;
				m_strHost = uc.lpszHostName;
				m_strUser = uc.lpszUserName;
				m_strPwd = uc.lpszPassword;
				m_strPath = uc.lpszUrlPath;
				m_strExtraInfo = uc.lpszExtraInfo;
				m_iScheme = uc.nScheme;
			}
			else
			{
				OutputDebugString(_T("Url Parse Failed!"));
			}
		}

		void HttpClient::Close()
		{
			if (m_hInternet != NULL)
			{
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			if (m_hSession != NULL)
			{
				InternetCloseHandle(m_hSession);
				m_hSession = NULL;
			}
			if (m_hRequest != NULL)
			{
				InternetCloseHandle(m_hRequest);
				m_hRequest = NULL;
			}
		}


		BOOL HttpClient::isAcceptInvalidCert()
		{
			return TRUE;
		}

		bool HttpClient::Request()
		{
			BOOL bRet = FALSE;

			m_hInternet = InternetOpen(m_strUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			if (m_hInternet == NULL)
			{
				return false;
			}

			m_hSession = InternetConnect(m_hInternet, m_strHost, m_iPort, m_strUser, m_strPwd, INTERNET_SERVICE_HTTP, INTERNET_FLAG_RELOAD, NULL);
			if (m_hSession == NULL)
			{
				return false;
			}

			DWORD flags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_UI;

			if (m_iScheme == INTERNET_SCHEME_HTTPS)
			{
				flags |= INTERNET_FLAG_SECURE;
			}

			CString strFile = m_strPath + m_strExtraInfo;
			CString strParam = getRequestParameter();
			if (!strParam.IsEmpty() && GetMethod() == _T("GET"))
			{
				strFile = strFile + _T("?") + strParam;
			}
			m_hRequest = HttpOpenRequest(m_hSession, GetMethod(), strFile, NULL, NULL, 0, flags, 0);
			if (m_hRequest == NULL)
			{
				return false;
			}

			if (m_iScheme == INTERNET_SCHEME_HTTPS && isAcceptInvalidCert())
			{
				DWORD dwFlags;
				DWORD dwBuffLen = sizeof(dwFlags);
				bRet = InternetQueryOption(m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
				if (!bRet) return false;

				dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_REVOCATION;

				bRet = InternetSetOption(m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
				if (!bRet) return false;
			}

			CString strHeader = getRequestHeaders();
			if (GetMethod() == _T("POST"))
			{
				if (strParam.IsEmpty())
				{
					bRet = HttpSendRequest(m_hRequest, strHeader, strHeader.GetLength(), NULL, 0);
				}
				else
				{
					std::string data = Utils::StringUtils::UnicodeToUTF8((LPCTSTR)strParam);   // 这里是视实际编码类型而定，这里以UTF8为例

					int len = data.length();

					bRet = HttpSendRequest(m_hRequest, strHeader, strHeader.GetLength(), (LPVOID)data.c_str(), len);
				}
			}
			else
			{
				bRet = HttpSendRequest(m_hRequest, strHeader, strHeader.GetLength(), NULL, 0);
			}

			return bRet;
		}


		bool HttpClient::DownloadFile(const CString& url, const CString& fileName)
		{
			BOOL ret = FALSE;

			m_strUrl = url;
			m_strMethod = L"GET";

			Close();
			ParseUrl();

			if (Request())
			{
				DWORD dwContentLength;
				GetContentLength(dwContentLength);    // the size of the file, can be used in progress

				DWORD dwLen;
				ret = FetchToFile(fileName, &dwLen);				
			}

			return ret;
		}

		BOOL HttpClient::FetchToFile(LPCTSTR file, DWORD* readLen)
		{
			if (readLen) *readLen = 0;

			if (!CheckHttpStatus()) return FALSE;
			
			BYTE buf[HTTP_BUF_SIZE];

			FILE* pFile = _wfsopen(file, L"wb", SH_DENYWR);
			if (pFile == NULL)  return FALSE;

			DWORD len = 0;
			DWORD totalBytes = 0;

			BOOL ret = true;
			do
			{
				if (m_bStop)
					break;

				ret = read(buf, HTTP_BUF_SIZE, &len);

				if (ret && len > 0)
				{
					totalBytes += len;

					if (readLen) *readLen = totalBytes;

					fwrite(buf, sizeof(BYTE), len, pFile);
				}

			} while (ret && len > 0);

			if (pFile)
				fclose(pFile);

			if (!ret || m_bStop)
			{
				::DeleteFile(file);
			}

			return ret;
		}

		BOOL HttpClient::read(BYTE* buf, DWORD bufLen, DWORD* readLen)
		{
			if (m_hRequest == NULL) return FALSE;
			if (readLen) *readLen = 0;

			if (!CheckHttpStatus()) return FALSE;

			DWORD readBytes = 1, totalBytes = 0;
			BOOL bRet = true;

			while (bRet && totalBytes < bufLen && readBytes > 0)
			{
				if (m_bStop) break;

				bRet = InternetReadFile(m_hRequest, buf + totalBytes, bufLen - totalBytes, &readBytes);

				if (bRet && readBytes > 0)
				{
					totalBytes += readBytes;
				}

				if (totalBytes == bufLen)
				{
					break;
				}
			}

			if (readLen) *readLen = totalBytes;

			return bRet;
		}


		BOOL HttpClient::CheckHttpStatus()
		{
			DWORD cs = sizeof(DWORD);
			DWORD code;
			BOOL ret = HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &code, &cs, NULL);
			if (ret)
			{
				// 可以分析返回不同的code时的原因
			}

			return ret;
		}

		BOOL HttpClient::GetContentLength(DWORD& cl) const
		{
			DWORD lenSize = sizeof(DWORD);
			BOOL ishttpqueryinfo = FALSE;
			return HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &cl, &lenSize, NULL);
		}

		CString HttpClient::GetMethod() const
		{
			return m_strMethod;
		}

		void HttpClient::Stop()
		{
			m_bStop = TRUE;
		}


		void HttpClient::AddParam(const CString& key, const CString& value)
		{
			params.insert(std::make_pair(key, value));
		}
		void HttpClient::AddHeader(const CString& key, const CString& value)
		{
			headers.insert(std::make_pair(key, value));
		}

		CString HttpClient::getRequestParameter() const
		{
			if (params.empty())  return _T("");

			CString strParam;
			for (auto iter = params.begin(); iter != params.end(); iter++)
			{
				strParam = strParam + iter->first + _T("=") + iter->second + _T("&");
			}

			strParam = strParam.Left(strParam.GetLength() - 1);

			return strParam;
		}

		CString HttpClient::getRequestHeaders() const
		{
			if (headers.empty()) return _T("");

			CString strHeader;
			for (auto iter = headers.begin(); iter != headers.end(); iter++)
			{
				strHeader = strHeader + iter->first + _T("=") + iter->second + _T("&");
			}

			strHeader = strHeader.Left(strHeader.GetLength() - 1);

			return strHeader;
		}
	}

	
}