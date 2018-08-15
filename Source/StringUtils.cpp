#include "stdafx.h"
#include "Utils.h"
#include <algorithm>

namespace Utils {

	namespace StringUtils {

		std::string UnicodeToAnsi(const std::wstring& wstr)
		{
			int len = WideCharToMultiByte(CP_ACP, NULL, wstr.c_str(), -1, NULL, 0, NULL, FALSE);
			if (len <= 0) return std::string("");
			char* buffer = (char*)malloc((len + 1) * sizeof(char));
			memset(buffer, 0, (len + 1) * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buffer, len, NULL, NULL);
			buffer[len] = '\0';
			std::string str(buffer);
			free(buffer);
			return str;
		}

		std::wstring AnsiToUnicode(const std::string& str)
		{
			int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
			if (len <= 0) return std::wstring(L"");
			wchar_t* buffer = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
			wmemset(buffer, 0, len + 1);
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, len);
			buffer[len] = '\0';
			std::wstring wstr(buffer);
			free(buffer);
			return wstr;
		}

		std::string UnicodeToUTF8(const std::wstring& wstr)
		{
			int len = WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), -1, NULL, 0, NULL, FALSE);
			if (len <= 0) return std::string("");
			char* buffer = (char*)malloc((len + 1) * sizeof(char));
			memset(buffer, 0, (len + 1) * sizeof(char));
			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, len, NULL, NULL);
			buffer[len] = '\0';
			std::string u8str(buffer);
			free(buffer);
			return u8str;
		}

		std::wstring UTF8ToUnicode(const std::string& u8str)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, u8str.c_str(), -1, NULL, 0);
			if (len <= 0) return std::wstring(L"");
			wchar_t* buffer = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
			wmemset(buffer, 0, len + 1);
			MultiByteToWideChar(CP_UTF8, 0, u8str.c_str(), -1, buffer, len);
			buffer[len] = '\0';
			std::wstring wstr(buffer);
			free(buffer);
			return wstr;
		}

		std::string AnsiToUTF8(const std::string& str)
		{
			std::wstring wstr = AnsiToUnicode(str);
			if (wstr.empty()) return std::string("");
			return UnicodeToUTF8(wstr);
		}

		std::string UTF8ToAnsi(const std::string& u8str)
		{
			std::wstring wstr = UTF8ToUnicode(u8str);
			if (wstr.empty()) return std::string("");
			return UnicodeToAnsi(wstr);
		}

		namespace WString {
			int toInt(const std::wstring& wstr, size_t const mode)
			{
				wchar_t* end;
				int result = ::wcstol(wstr.c_str(), &end, mode);
				return result;
			}

			double toDouble(const std::wstring& wstr)
			{
				wchar_t* end;
				double result = ::wcstod(wstr.c_str(), &end);
				return result;
			}

			std::wstring fromInt(int n)
			{
				wchar_t buffer[32] = { 0 };
				swprintf_s(buffer, 32, L"%d", n);
				return std::wstring(buffer);
			}

			std::wstring fromDouble(double d, size_t const s)
			{
				wchar_t format[16] = { 0 };
				swprintf_s(format, 16, L"%%.%df", s);
				wchar_t buffer[64] = { 0 };
				swprintf_s(buffer, 64, format, d);
				return std::wstring(buffer);
			}

			std::wstring toUpper(std::wstring wstr)
			{
				std::transform(wstr.begin(), wstr.end(), wstr.begin(), ::toupper);
				return wstr.c_str();
			}
			std::wstring toLower(std::wstring wstr)
			{
				std::transform(wstr.begin(), wstr.end(), wstr.begin(), ::tolower);
				return wstr.c_str();
			}

			std::vector<std::wstring> Split(const std::wstring& wstr, const std::wstring& sep)
			{
				std::vector<std::wstring> vec;
				wchar_t *next_token = NULL;
				wchar_t* p = wcstok_s((LPTSTR)wstr.c_str(), sep.c_str(), &next_token);
				while (p)
				{
					vec.push_back(std::wstring(p));
					p = wcstok_s(NULL, sep.c_str(), &next_token);
				}

				return vec;
			}

			std::wstring Replace(const std::wstring& wstr, const std::wstring& repalceStr, const std::wstring& destStr)
			{
				size_t pos = wstr.find(repalceStr);
				if (pos == std::wstring::npos) return wstr;

				int len1 = repalceStr.length();
				int len2 = destStr.length();

				std::wstring temp = wstr;
				while (pos != std::wstring::npos)
				{
					temp.replace(pos, len1, destStr.c_str());
					pos = temp.find(repalceStr, pos + len2);
				}

				return temp;
			}

			bool StartsWith(const std::wstring& wstr, const std::wstring& startStr, bool ignoreCase)
			{
				if (startStr.length() > 0 && startStr.length() <= wstr.length())
				{
					if (ignoreCase)
					{
						return _wcsnicmp(wstr.c_str(), startStr.c_str(), startStr.length()) == 0;
					}
					else
					{
						return wcsncmp(wstr.c_str(), startStr.c_str(), startStr.length()) == 0;
					}
				}

				return false;
			}

			bool EndsWith(const std::wstring& wstr, const std::wstring& endStr, bool ignoreCase)
			{
				int len1 = wstr.length();
				int len2 = endStr.length();

				if (len2 > 0 && len2 <= len1)
				{
					if (ignoreCase)
					{
						return _wcsicmp(wstr.c_str() + (len1 - len2), endStr.c_str()) == 0;
					}
					else
					{
						return wcscmp(wstr.c_str() + (len1 - len2), endStr.c_str()) == 0;
					}
				}

				return false;
			}

			std::wstring Trim(const std::wstring& wstr, const wchar_t* chars)
			{
				std::wstring result = wstr;

				if (wstr.empty()) return result;

				if (chars == nullptr)
				{
					chars = L" \n\r\t";
				}

				size_t pos = result.find_first_not_of(chars);
				if (pos != std::wstring::npos)
				{
					result.erase(0, pos);
					pos = result.find_last_not_of(chars);
					if (pos != std::wstring::npos)
					{
						result.erase(pos + 1);
					}
				}
				else
				{
					result.clear();
				}

				return result;
			}
		}

		namespace String {
			int toInt(const std::string& str, size_t const mode)
			{
				char* end;
				int result = ::strtol(str.c_str(), &end, mode);
				return result;
			}

			double toDouble(const std::string& str)
			{
				char* end;
				double result = ::strtod(str.c_str(), &end);
				return result;
			}

			std::string fromInt(int n)
			{
				char buffer[32] = { 0 };
				sprintf_s(buffer, 32, "%d", n);
				return std::string(buffer);
			}

			std::string fromDouble(double d, size_t const s)
			{
				char format[16] = { 0 };
				sprintf_s(format, 16, "%%.%df", s);
				char buffer[64] = { 0 };
				sprintf_s(buffer, 64, format, d);
				return std::string(buffer);
			}

			std::string toUpper(std::string str)
			{
				std::transform(str.begin(), str.end(), str.begin(), ::toupper);
// 				std::transform(str.begin(), str.end(), str.begin(),
// 					[](unsigned char c) -> unsigned char { return ::toupper(c); });
				return str.c_str();
			}
			std::string toLower(std::string str)
			{
				std::transform(str.begin(), str.end(), str.begin(), ::tolower);
				return str.c_str();
			}

			std::vector<std::string> Split(const std::string& str, const std::string& sep)
			{
				std::vector<std::string> vec;
				char *next_token = NULL;
				char* p = ::strtok_s((LPSTR)str.c_str(), sep.c_str(), &next_token);
				while (p)
				{
					vec.push_back(std::string(p));
					p = ::strtok_s(NULL, sep.c_str(), &next_token);
				}

				return vec;
			}

			std::string Replace(const std::string& str, const std::string& repalceStr, const std::string& destStr)
			{
				size_t pos = str.find(repalceStr);
				if (pos == std::string::npos) return str;

				int len1 = repalceStr.length();
				int len2 = destStr.length();

				std::string temp = str;
				while (pos != std::string::npos)
				{
					temp.replace(pos, len1, destStr.c_str());
					pos = temp.find(repalceStr, pos + len2);
				}

				return temp;
			}

			bool StartsWith(const std::string& str, const std::string& startStr, bool ignoreCase)
			{
				if (startStr.length() > 0 && startStr.length() <= str.length())
				{
					if (ignoreCase)
					{
						return _strnicmp(str.c_str(), startStr.c_str(), startStr.length()) == 0;
					}
					else
					{
						return strncmp(str.c_str(), startStr.c_str(), startStr.length()) == 0;
					}
				}

				return false;
			}

			bool EndsWith(const std::string& str, const std::string& endStr, bool ignoreCase)
			{
				int len1 = str.length();
				int len2 = endStr.length();

				if (len2 > 0 && len2 <= len1)
				{
					if (ignoreCase)
					{
						return _stricmp(str.c_str() + (len1 - len2), endStr.c_str()) == 0;
					}
					else
					{
						return strcmp(str.c_str() + (len1 - len2), endStr.c_str()) == 0;
					}
				}

				return false;
			}

			std::string Trim(const std::string& str, const char* chars)
			{
				std::string result = str;

				if (str.empty()) return result;

				if (chars == nullptr)
				{
					chars = " \n\r\t";
				}

				size_t pos = result.find_first_not_of(chars);
				if (pos != std::wstring::npos)
				{
					result.erase(0, pos);
					pos = result.find_last_not_of(chars);
					if (pos != std::wstring::npos)
					{
						result.erase(pos + 1);
					}
				}
				else
				{
					result.clear();
				}

				return result;
			}
		}
	}
}
