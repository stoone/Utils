#include "BaseObject.h"


BaseObject::BaseObject()
{
	_attrs = nullptr;
}


BaseObject::~BaseObject()
{
	if (_attrs)
	{
		delete _attrs;
		_attrs = nullptr;
	}
}


bool BaseObject::SetAttribute(const std::wstring& name, const NValue& value)
{
	if (_attrs == nullptr)
		_attrs = new std::map<std::wstring, NValue>();

	if (_attrs && (*_attrs)[name] != value)
	{
		(*_attrs)[name] = value;
		this->onAttributeChanged(name);
	}

	return true;
}

NValue BaseObject::GetAttribute(const std::wstring& name, const NValue& defaultValue) const
{
	if (_attrs)
	{
		return (*_attrs)[name];
	}

	return defaultValue;
}

bool BaseObject::ClearAttribute()
{
	if (_attrs)
	{
		_attrs->clear();
		this->onAttributeChanged(L"");
	}
	return true;
}

bool BaseObject::RemoveAttribute(const std::wstring& name)
{
	if (_attrs)
	{
		if (_attrs->find(name) != _attrs->end())
		{
			_attrs->erase(name);
			this->onAttributeChanged(name);
		}
	}
	return false;
}

const std::vector<std::wstring> BaseObject::AllAttributes() const
{
	std::vector<std::wstring> attrNames;
	if (_attrs)
	{
		for (const auto& item : *_attrs)
		{
			attrNames.push_back(item.first);
		}
	}

	return attrNames;
}

void BaseObject::onAttributeChanged(const std::wstring& name)
{

}

void BaseObject::onVariableChanged(const std::wstring& name)
{

}