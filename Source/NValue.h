#pragma once
#include <string>

class NValue {
public:
	enum ValueType {
		VT_EMPTY = 0,
		VT_BOOL,
		VT_INT,
		VT_INT64,
		VT_DOUBLE,
		VT_STRING,
		VT_PTR
	};

	struct PTR 
	{
		void* ptr;
		int len;
	};


public:
	NValue();
	NValue(bool value);
	NValue(int value);
	NValue(int64_t value);
	NValue(double value);
	NValue(const std::wstring& value);
	NValue(LPCSTR value);
	NValue(LPCWSTR value);
	NValue(void* value, int len);
	NValue(const NValue& value);
	
	virtual ~NValue();

	NValue& operator =(bool value);
	NValue& operator =(int value);
	NValue& operator =(int64_t value);
	NValue& operator =(double value);
	NValue& operator =(const std::wstring& value);
	NValue& operator =(LPCSTR value);
	NValue& operator =(LPCWSTR value);
//	NValue& operator =(void* value);
	NValue& operator =(const NValue& value);

	bool operator ==(bool right) const;
	bool operator ==(int right) const;
	bool operator ==(int64_t right) const;
	bool operator ==(double right) const;
	bool operator ==(const std::wstring& right) const;
	bool operator ==(LPCSTR right) const;
	bool operator ==(LPCWSTR right) const;
	bool operator ==(const NValue& right) const;

	bool operator !=(bool right) const;
	bool operator !=(int right) const;
	bool operator !=(int64_t right) const;
	bool operator !=(double right) const;
	bool operator !=(const std::wstring& right) const;
	bool operator !=(LPCSTR right) const;
	bool operator !=(LPCWSTR right) const;
	bool operator !=(const NValue& right) const;

	operator bool() const;
	operator int() const;
	operator int64_t() const;
	operator double() const;
// 这三个类型转换有误，原因未知
//	operator LPCSTR();
 	operator LPCWSTR();
// 	operator std::wstring();

	bool boolValue() const;
	int intValue() const;
	int64_t int64Value() const;
	double doubleValue() const;
	std::wstring wstringValue();
	std::string stringValue();
	void* ptrValue() const;
	int ptrLength() const;

	bool isEmpty() const;
	NValue::ValueType type() const;

//	void CacheWstr(const std::wstring& wstr);
	void ToString();

	void Init();
	void clear();

	NValue::ValueType _vt;
	struct {
		bool bVal;
		int iVal;
		int64_t lVal;
		double dVal;
		PTR* pVal;
		std::wstring strVal;
	} _data;

//	mutable std::wstring* _strVal{ nullptr };
};

static const NValue nvEmpty;

