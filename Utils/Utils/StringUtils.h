#pragma once
#include <string>
#include <algorithm>
#include <stdarg.h>
#include "win.h"

namespace Utils {
	std::string UnicodeToAnsi(const std::wstring& wstr);
	std::wstring AnsiToUnicode(const std::string& str);
	std::string UnicodeToUTF8(const std::wstring& wstr);
	std::wstring UTF8ToUnicode(const std::string& u8str);
	std::string AnsiToUTF8(const std::string& str);
	std::string UTF8ToAnsi(const std::string& u8str);

	std::string WCharToChar(const std::wstring& wstr);
	std::wstring CharToWChar(const std::string& str);
	std::string WCharToUTF8(const std::wstring& wstr);
	std::wstring UTF8ToWChar(const std::string& str);

// 	template<typename charset>
// 	class StringUtility {
// 	public:
// 		typedef std::basic_string<charset> Str;
// 
// 		std::vector<Str> vec;
// 	};

// 	typedef StringUtils<std::wstring, wchar_t> WStr;
// 	typedef StringUtils<std::string, char> Str;

	template<typename T, typename N>
	class StringUtils {
	public:
		static int toInt(const T& str, size_t const mode = 10);
		static double toDouble(const T& wstr);
		static float toFloat(const T& str);

		static T fromInt(int n);
		static T fromDouble(double d, size_t const s = 2);

		static T toUpper(T str);
		static T toLower(T str);

		static std::vector<T> Split(const T& str, const T& sep);
		static T Replace(const T& str, const T& repalceStr, const T& destStr);

		static bool StartsWith(const T& str, const T& startStr, bool ignoreCase = false);   // ignoreCase-false:区分大小写
		static bool EndsWith(const T& str, const T& endStr, bool ignoreCase = false);    // ignoreCase-false:区分大小写

		static T Trim(const T& str, const N* chars = nullptr);
		static std::wstring Format(const wchar_t* fmt, ...);
		static std::string Format(const char* fmt, ...);

		static size_t hash(T& str);
	};

	template<typename T, typename N>
	int StringUtils<T, N>::toInt(const T& str, size_t const mode)
	{
		N* end = nullptr;
		int result = ::_tcstol(str.c_str(), &end, mode);
		return result;
	}

	template<typename T, typename N>
	double StringUtils<T, N>::toDouble(const T& str)
	{
		N* end = nullptr;
		double result = ::_tcstod(str.c_str(), &end);
		return result;
	}

	template<typename T, typename N>
	float StringUtils<T, N>::toFloat(const T& str)
	{
		N* end = nullptr;
		float result = ::_tcstof(str.c_str(), &end);
		return result;
	}

	template<typename T, typename N>
	T StringUtils<T, N>::fromInt(int n)
	{
		N buffer[32] = { 0 };
		_stprintf_s(buffer, 32, L"%d", n);
		return T(buffer);
	}

	template<typename T, typename N>
	T StringUtils<T, N>::fromDouble(double d, size_t const s)
	{
		N format[16] = { 0 };
		_stprintf_s(format, 16, L"%%.%df", s);
		N buffer[64] = { 0 };
		_stprintf_s(buffer, 64, format, d);
		return T(buffer);
	}

	template<typename T, typename N>
	T StringUtils<T, N>::toUpper(T str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str.c_str();
	}
	template<typename T, typename N>
	T StringUtils<T, N>::toLower(T str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str.c_str();
	}

/*
	template<typename T, typename N>
	std::vector<T> StringUtils<T, N>::Split(const T& str, const T& sep)
	{
		std::vector<T> vec;
		wchar_t *next_token = NULL;
		wchar_t* p = _tcstok_s((N*)str.c_str(), sep.c_str(), &next_token);
		while (p)
		{
			vec.push_back(T(p));
			p = _tcstok_s(NULL, sep.c_str(), &next_token);
		}

		return vec;
	}
*/
	template<typename T, typename N>
	std::vector<T> StringUtils<T, N>::Split(const T& str, const T& sep)
	{
		std::vector<T> vec;
		T s = str;
		int pos = s.find(sep);
		if (pos == T::npos)
			return vec;

		while (pos != T::npos)
		{
			T subStr = s.substr(0, pos);
			if (!subStr.empty())
				vec.push_back(subStr);
			s = s.substr(pos + 1);
			pos = s.find(sep);
		}

		if (!s.empty())
			vec.push_back(s);

		return vec;
	}

	template<typename T, typename N>
	T StringUtils<T, N>::Replace(const T& wstr, const T& repalceStr, const T& destStr)
	{
		size_t pos = wstr.find(repalceStr);
		if (pos == T::npos) return wstr;

		int len1 = repalceStr.length();
		int len2 = destStr.length();

		T temp = wstr;
		while (pos != T::npos)
		{
			temp.replace(pos, len1, destStr.c_str());
			pos = temp.find(repalceStr, pos + len2);
		}

		return temp;
	}

	template<typename T, typename N>
	bool StringUtils<T, N>::StartsWith(const T& str, const T& startStr, bool ignoreCase)
	{
		int len1 = str.length();
		int len2 = startStr.length();
		if (len2 > 0 && len2 <= len1)
		{
			T str1 = str;
			T str2 = startStr;
			if (ignoreCase)
			{
				str1 = toLower(str);
				str2 = toLower(startStr);
			}
			// yes
			if (str1.compare(0, len2, str2) == 0)
			{
				return true;
			}
		}

		return false;
	}

	template<typename T, typename N>
	bool StringUtils<T, N>::EndsWith(const T& str, const T& endStr, bool ignoreCase)
	{
		int len1 = str.length();
		int len2 = endStr.length();

		if (len2 > 0 && len2 <= len1)
		{
			T str1 = str;
			T str2 = endStr;
			if (ignoreCase)
			{
				str1 = toLower(str);
				str2 = toLower(endStr);
			}
			// yes
			if (str1.compare(str1.size() - len2, len2, str2) == 0)
			{
				return true;
			}
		}

		return false;
	}

	template<typename T, typename N>
	T StringUtils<T, N>::Trim(const T& wstr, const N* chars)
	{
		T result = wstr;

		if (wstr.empty()) return result;

		if (chars == nullptr)
		{
			chars = L" \n\r\t";
		}

		size_t pos = result.find_first_not_of(chars);
		if (pos != T::npos)
		{
			result.erase(0, pos);
			pos = result.find_last_not_of(chars);
			if (pos != T::npos)
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

	template<typename T, typename N>
	size_t StringUtils<T, N>::hash(T& str)
	{
		std::hash<T> _hash;
		return _hash(str);
	}

	template<typename T, typename N>
	std::wstring StringUtils<T, N>::Format(const wchar_t* fmt, ...)
	{
		std::wstring strResult = L"";
		if (nullptr != fmt)
		{
			va_list args;
			va_start(args, fmt);
			size_t len = _vsctprintf(fmt, args) + 1;
			std::vector<wchar_t> buffer(len, '\0');
			int nSize = 0;
			nSize = _vsntprintf_s(&buffer[0], buffer.size(), len, fmt, args);
			if (nSize > 0)
			{
				strResult = &buffer[0];
			}
			va_end(args);
		}
		return strResult;
	}

	template<typename T, typename N>
	std::string StringUtils<T, N>::Format(const char* fmt, ...)
	{
		std::string strResult = "";
		if (nullptr != fmt)
		{
			va_list args;
			va_start(args, fmt);
			size_t len = _vscprintf(fmt, args) + 1;
			std::vector<char> buffer(len, '\0');
			int nSize = 0;
			nSize = _vsnprintf_s(&buffer[0], buffer.size(), len, fmt, args);
			if (nSize > 0)
			{
				strResult = &buffer[0];
			}
			va_end(args);
		}
		return strResult;
	}

}
