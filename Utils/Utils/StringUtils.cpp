#include "Utils.h"
#include "StringUtils.h"
#include <codecvt>

namespace Utils {

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

	std::string WCharToChar(const std::wstring& wstr)
	{
		using WConvert = std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>;
		WConvert converter(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));
		std::string str = converter.to_bytes(wstr);
		return str;
	}

	std::wstring CharToWChar(const std::string& str)
	{
		using WConvert = std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>;
		WConvert converter(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));
		std::wstring wstr = converter.from_bytes(str);
		return wstr;
	}

	std::string WCharToUTF8(const std::wstring& wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(wstr);
	}

	std::wstring UTF8ToWChar(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.from_bytes(str);
	}
}
