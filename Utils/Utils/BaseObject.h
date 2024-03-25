#pragma once
#include "NValue.h"
#include <vector>
#include <map>

class BaseObject {
public:
	BaseObject();
	~BaseObject();

	virtual bool SetAttribute(const std::wstring& name, const NValue& value);
	virtual NValue GetAttribute(const std::wstring& name, const NValue& defaultValue = nvEmpty) const;
	virtual bool ClearAttribute();
	virtual bool RemoveAttribute(const std::wstring& name);
	virtual const std::vector<std::wstring> AllAttributes() const;

protected:
	virtual void onAttributeChanged(const std::wstring& name);
	virtual void onVariableChanged(const std::wstring& name);

	std::map<std::wstring, NValue>* _attrs;
};

#define DECLARE_VAR(type, name, defaultValue)		\
protected:                                          \
type _##name##_ = defaultValue;						\
public:                                             \
virtual void set##name(type value) {                \
    _##name##_ = value;                             \
}													\
virtual type get##name() const {                    \
    return _##name##_;                              \
}

#define DECLARE_VAR_INT(name, value)				\
protected:                                          \
int _##name##_ = value;								\
public:                                             \
virtual void set##name(int value) {                 \
    _##name##_ = value;                             \
}													\
virtual int get##name() const {                     \
    return _##name##_;                              \
}

#define DECLARE_VAR_BOOL(name)			        	\
protected:                                          \
BOOL _##name##_ = false;							\
public:                                             \
virtual void set##name(BOOL value) {                \
    _##name##_ = value;                             \
}													\
virtual BOOL is##name() const {                     \
    return _##name##_;                              \
}

#define DECLARE_VAR_CSTRING(name)				    \
protected:                                          \
CString _##name##_ = _T("");						\
public:                                             \
virtual void set##name(CString value) {             \
    _##name##_ = value;                             \
}													\
virtual CString get##name() const {                 \
    return _##name##_;                              \
}

#define DECLARE_VAR_FLOAT(name)				        \
protected:                                          \
float _##name##_ = 0.0;								\
public:                                             \
virtual void set##name(float value) {               \
    _##name##_ = value;                             \
}													\
virtual float get##name() const {                   \
    return _##name##_;                              \
}


#define DECLARE_VAR_bool(name)				        \
protected:                                          \
bool _##name##_ = false;							\
public:                                             \
virtual void set##name(bool value) {                \
    _##name##_ = value;                             \
}													\
virtual bool is##name() const {                     \
    return _##name##_;                              \
}


// attribute declare macro
#define DECLARE_ATTR_BOOL(name)			            \
virtual bool set##name(bool value) {                \
    return SetAttribute(L#name, value);             \
}													\
virtual bool is##name() const {                     \
    return (int)GetAttribute(L#name);               \
}

#define DECLARE_ATTR_INT(name)                      \
virtual bool set##name(int value) {                 \
    return SetAttribute(L#name, value);             \
}													\
virtual int get##name() const {                     \
    return (int)GetAttribute(L#name);               \
}


#define DECLARE_ATTR_STRING(name)			        \
virtual bool set##name(const std::wstring& value) { \
    return SetAttribute(L#name, value);             \
}													\
virtual std::wstring get##name() const {            \
    return (LPCTSTR)GetAttribute(L#name);           \
}
