#include "stdafx.h"
#include "NValue.h"
#include "Utils.h"
#include <algorithm>

NValue::~NValue()
{
	clear();
}

NValue::NValue()
{
	Init();
}

NValue::NValue(bool value)
{
	Init();
	_vt = VT_BOOL;
	_data.bVal = value;
}

NValue::NValue(int value)
{
	Init();
	_vt = VT_INT;
	_data.iVal = value;
}

NValue::NValue(int64_t value)
{
	Init();
	_vt = VT_INT64;
	_data.lVal = value;
}

NValue::NValue(double value)
{
	Init();
	_vt = VT_DOUBLE;
	_data.dVal = value;
}

// void NValue::CacheWstr(const std::wstring& wstr)
// {
// 	_data.strVal = new TCHAR[wstr.length() + 1];
// 	memset(_data.strVal, 0, sizeof(TCHAR) * (wstr.length() + 1));
// 	_tcscpy_s(_data.strVal, wstr.length() + 1, wstr.c_str());
// }

void NValue::Init()
{
	_vt = VT_EMPTY;
	_data.bVal = false;
	_data.iVal = 0;
	_data.lVal = 0;
	_data.dVal = 0.0;
	_data.pVal = nullptr;
	_data.strVal = L"";
}

void NValue::clear()
{
	Init();
}

NValue::NValue(const std::wstring& value)
{
	Init();
	_vt = VT_STRING;
	_data.strVal = value;
}

NValue::NValue(LPCSTR value)
{
	Init();
	_vt = VT_STRING;

	std::wstring wstr = Utils::StringUtils::AnsiToUnicode(value);
	_data.strVal = wstr;
}

NValue::NValue(LPCWSTR value)
{
	Init();
	_vt = VT_STRING;
	_data.strVal = value;
}

NValue::NValue(void* value, int len)
{
	Init();
	_vt = VT_PTR;
	_data.strVal = nullptr;
	_data.pVal->ptr = value;
	_data.pVal->len = len;
//	_strVal = nullptr;
}

NValue::NValue(const NValue& value)
{
	Init();
	this->_vt = value._vt;
	this->_data = value._data;
}

NValue& NValue::operator =(bool value)
{
	clear();
	_vt = VT_BOOL;
	_data.bVal = value;

	return *this;
}

NValue& NValue::operator =(int value)
{
	clear();
	_vt = VT_INT;
	_data.iVal = value;

	return *this;
}

NValue& NValue::operator =(int64_t value)
{
	clear();
	_vt = VT_INT64;
	_data.lVal = value;

	return *this;
}
NValue& NValue::operator =(double value)
{
	clear();
	_vt = VT_DOUBLE;
	_data.dVal = value;

	return *this;
}

NValue& NValue::operator =(const std::wstring& value)
{
	clear();
	_vt = VT_STRING;
	_data.strVal = value;

	return *this;
}

NValue& NValue::operator =(LPCSTR value)
{
	clear();
	_vt = VT_STRING;

	std::wstring wstr = Utils::StringUtils::AnsiToUnicode(value);
	_data.strVal = wstr;

	return *this;
}

NValue& NValue::operator =(LPCWSTR value)
{
	clear();
	_vt = VT_STRING;
	_data.strVal = value;

	return *this;
}

// NValue& NValue::operator =(void* value)
// {
// 	_vt = VT_PTR;
// 	_data.pVal->ptr = value;
// 
// 	return *this;
// }

NValue& NValue::operator =(const NValue& value)
{
	clear();
	_vt = value._vt;
	_data = value._data;

	return *this;
}

bool NValue::operator ==(bool right) const
{
	return _data.bVal == right;
}

bool NValue::operator ==(int right) const
{
	return _data.iVal == right;
}

bool NValue::operator ==(int64_t right) const
{
	return _data.lVal == right;
}

bool NValue::operator ==(double right) const
{
	return _data.dVal == right;
}

bool NValue::operator ==(const std::wstring& right) const
{
	return _data.strVal.compare(right) == 0;
}

bool NValue::operator ==(LPCSTR right) const
{
	std::wstring wstrRight = Utils::StringUtils::AnsiToUnicode(right);
	return _data.strVal.compare(wstrRight) == 0;
}

bool NValue::operator ==(LPCWSTR right) const
{
	return _data.strVal.compare(std::wstring(right)) == 0;
}

bool NValue::operator ==(const NValue& right) const
{
	if (this->_vt != right._vt)  return false;

	if (this->_vt == VT_EMPTY && right._vt == VT_EMPTY) return true;

	if (this->_vt == VT_BOOL)
		return this->_data.bVal == right._data.bVal;
	else if (_vt == VT_INT)
		return this->_data.iVal == right._data.iVal;
	else if (_vt == VT_INT64)
		return this->_data.lVal == right._data.lVal;
	else if (_vt == VT_DOUBLE)
		return this->_data.dVal == right._data.dVal;
	else if (_vt == VT_PTR)
		return this->_data.pVal == right._data.pVal;
	else if (_vt == VT_STRING)
		return _data.strVal.compare(right._data.strVal) == 0;

	return false;
}

bool NValue::operator !=(bool right) const
{
	return _data.bVal != right;
}

bool NValue::operator !=(int right) const
{
	return _data.iVal != right;
}

bool NValue::operator !=(int64_t right) const
{
	return _data.lVal != right;
}

bool NValue::operator !=(double right) const
{
	return _data.dVal != right;
}

bool NValue::operator !=(const std::wstring& right) const
{
	return _data.strVal.compare(right) != 0;
}

bool NValue::operator !=(LPCSTR right) const
{
	std::wstring wstr = Utils::StringUtils::AnsiToUnicode(right);
	return _data.strVal.compare(wstr) != 0;
}

bool NValue::operator !=(LPCWSTR right) const
{
	return _data.strVal.compare(std::wstring(right)) != 0;
}

bool NValue::operator !=(const NValue& right) const
{
	return !(*this == right);
}

bool isNum(const std::wstring& wstr)
{
	return _tcsspn(wstr.c_str(), L".0123456789") == _tcslen(wstr.c_str());
}

NValue::operator bool() const
{
	return boolValue();
}

NValue::operator int() const
{
	return intValue();
}

NValue::operator int64_t() const
{
	return int64Value();
}

NValue::operator double() const
{
	return doubleValue();
}

/*
NValue::operator LPCSTR()
{
	ToString();
//	std::wstring wstr = stringValue();
	std::string str = Utils::StringUtils::UnicodeToAnsi(_data.strVal);
	return str.c_str();
}

NValue::operator std::wstring()
{
	return wstringValue();
}
*/
NValue::operator LPCWSTR()
{
	ToString();
	return _data.strVal.c_str();
}

bool NValue::boolValue() const
{
	if (_vt == VT_STRING)
	{
		if (_data.strVal.empty())
		{
			return false;
		}
		else
		{
			std::wstring wstr(_data.strVal);
			transform(wstr.begin(), wstr.end(), wstr.begin(), ::tolower);
			if (wstr.compare(L"false") == 0 || wstr.compare(L"0") == 0)
				return false;
			else
				return true;
		}
	}

	return this->intValue() ? true : false;
}



int NValue::intValue() const
{
	int v = 0;
	switch (_vt)
	{
		case VT_BOOL:
			v = _data.bVal ? 1 : 0;
			break;
		case VT_INT:
			v = _data.iVal;
			break;
		case VT_DOUBLE:
			v = (int)_data.dVal;
			break;
		case VT_INT64:
			v = (int)_data.lVal;
			break;
		case VT_STRING:
			if (isNum(_data.strVal))
				v = _ttoi(_data.strVal.c_str());
			break;
		default:
			break;
	}

	return v;
}
int64_t NValue::int64Value() const
{
	int64_t v = 0;
	switch (_vt)
	{
		case NValue::VT_BOOL:
			v = _data.bVal ? 1 : 0;
			break;
		case NValue::VT_INT:
			v = (int64_t)_data.iVal;
			break;
		case NValue::VT_INT64:
			v = _data.lVal;
			break;
		case NValue::VT_DOUBLE:
			v = (int64_t)_data.dVal;
			break;
		case NValue::VT_STRING:
			if (isNum(_data.strVal))
				v = _ttoi(_data.strVal.c_str());
			break;
		default:
			break;
	}

	return v;
}

double NValue::doubleValue() const
{
	double v = 0;
	switch (_vt)
	{
		case NValue::VT_BOOL:
			v = _data.bVal ? 1 : 0;
			break;
		case NValue::VT_INT:
			v = (double)_data.iVal;
			break;
		case NValue::VT_INT64:
			v = (double)_data.lVal;
			break;
		case NValue::VT_DOUBLE:
			v = _data.dVal;
			break;
		case NValue::VT_STRING:
			if (isNum(_data.strVal))
				v = _ttof(_data.strVal.c_str());
			break;
		default:
			break;
	}

	return v;
}

std::wstring NValue::wstringValue()
{
	ToString();
	std::wstring wstr = _data.strVal;
	return _data.strVal;
}

std::string NValue::stringValue()
{
	ToString();
	std::string str = Utils::StringUtils::UnicodeToAnsi(_data.strVal);
	return str;
}

void* NValue::ptrValue() const
{
	if (_vt == VT_PTR)
		return _data.pVal->ptr;

	return nullptr;
}

int NValue::ptrLength() const
{
	if (_vt == VT_PTR)
		return _data.pVal->len;

	return 0;
}


void NValue::ToString()
{
	if (_vt == VT_STRING) return;

	std::wstring wstr;
	switch (_vt)
	{
		case NValue::VT_BOOL:
			wstr = _data.bVal ? L"true" : L"false";
			break;
		case NValue::VT_INT:
		{
			TCHAR buffer[64] = { 0 };
			_itot(_data.iVal, buffer, 10);
			wstr = buffer;
		}
			break;
		case NValue::VT_INT64:
		{
			TCHAR buffer[64] = { 0 };
			wsprintf(buffer, L"%lld", _data.lVal);
			wstr = buffer;
		}
			break;
		case NValue::VT_DOUBLE:
		{
			TCHAR buffer[60] = { 0 };
			wsprintf(buffer, L"%f", _data.dVal);
			wstr = buffer;
		}
			break;
		default:
			break;
	}

	_data.strVal = wstr;
}

bool NValue::isEmpty() const
{
	return _vt == VT_EMPTY;
}

NValue::ValueType NValue::type() const
{
	return _vt;
}


