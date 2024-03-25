#pragma once

template<typename T>
class Singleton
{
public:
	~Singleton(){ }

	//T(T const&) = delete;
	//void operator=(T const&) = delete;

	static T* getInstance()
	{
		static T instance;
		return &instance;
	}

private:
	Singleton() {}
};

