#include "Utils.h"


namespace Utils {
	namespace RegUtils {

#define MAX_DATA_LEN	5120

		bool IsKeyExist(HKEY hRoot, const std::wstring& strSubKey, const std::wstring& keyName)
		{
			bool exist = false;
			HKEY hKey = NULL;
			DWORD dwDispositon = REG_OPENED_EXISTING_KEY;
			LSTATUS ls = ::RegOpenKeyEx(hRoot, strSubKey.c_str(), 0, KEY_READ, &hKey);
			if (ls == ERROR_SUCCESS)
			{
				DWORD dwType = REG_SZ;
				DWORD dwDataLen = 60;
				TCHAR buffer[MAX_PATH] = { 0 };
				ls = RegQueryValueEx(hKey, keyName.c_str(), NULL, &dwType, (LPBYTE)buffer, &dwDataLen);
				if (ls == ERROR_SUCCESS || ls == ERROR_MORE_DATA)
				{
					exist = true;
				}
			}
			
			return exist;
		}

		NValue GetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName)
		{
			NValue value;
			HKEY hSubKey = NULL;
			LSTATUS ls = ERROR_SUCCESS;

			if (subKey.empty())
				hSubKey = hRoot;
			else 
				ls = RegOpenKeyEx(hRoot, subKey.c_str(), 0, KEY_READ, &hSubKey);

			if (ERROR_SUCCESS == ls)
			{
				DWORD dwType = REG_NONE;
				DWORD dwDataLen = MAX_DATA_LEN;
				BYTE buffer[MAX_DATA_LEN] = { 0 };
				ls = RegQueryValueEx(hSubKey, keyName.c_str(), NULL, &dwType, buffer, &dwDataLen);
				if (ERROR_SUCCESS == ls)
				{
					if (dwType == REG_SZ)
						value = (LPCWSTR)buffer;
					else if (dwType == REG_DWORD)
						value = (int)(*(DWORD*)buffer);
					else if (dwType == REG_BINARY && dwDataLen > 0 && dwDataLen < MAX_DATA_LEN)
					{
						value = buffer;
					}
				}

				if (hSubKey != hRoot)
				{
					RegCloseKey(hSubKey);
				}
			}

			return value;
		}

		bool SetRegValue(HKEY hRoot, const std::wstring& subKey, const std::wstring& keyName, NValue& value)
		{
			HKEY hSubKey = NULL;
			LSTATUS ls = ERROR_SUCCESS;

			if (subKey.empty())
			{
				hSubKey = hRoot;
			}
			else
			{
				ls = RegOpenKeyEx(hRoot, subKey.c_str(), 0, KEY_ALL_ACCESS, &hSubKey);
				if (ls != ERROR_SUCCESS)
				{
					ls = RegCreateKeyEx(hRoot, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL);
				}
			}
			if (ERROR_SUCCESS == ls)
			{
				if (value.type() == NValue::VT_STRING)
				{
					std::wstring wstr = value.wstringValue();
					LPCWSTR data = value.wstringValue().c_str();
					DWORD len = (_tcslen(data) + 1) * sizeof(TCHAR);
					ls = RegSetValueEx(hSubKey, keyName.c_str(), NULL, REG_SZ, (BYTE*)data, len);
				}
				else if (value.type() == NValue::VT_INT)
				{
					DWORD data = (int)value;
					DWORD len = sizeof(DWORD);
					ls = RegSetValueEx(hSubKey, keyName.c_str(), NULL, REG_DWORD, (BYTE*)&data, len);
				}
				else if (value.type() == NValue::VT_PTR)
				{
					BYTE* data = (BYTE*)value.ptrValue();
					DWORD len = value.ptrLength();
					ls = RegSetValueEx(hSubKey, keyName.c_str(), NULL, REG_BINARY, data, len);
				}
				else if (value.type() == VT_EMPTY)
				{
					RegDeleteValue(hSubKey, keyName.c_str());
				}
				else
				{
					ls = ERROR_BADKEY;
				}

				if (hSubKey != hRoot)
				{
					RegCloseKey(hSubKey);
				}
			}
			return false;
		}

	}
}