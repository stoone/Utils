#include <iostream>
#include "utils/Utils.h"
#include "model.h"
#include <array>
#include ".\Utils\Singleton.h"
#include <ctime>
#include <memory>
#include <unordered_map>

//void f()
//{
//	model::getInstance()->print(5);
//	model::getInstance()->print(6);
//	model::getInstance()->print(7);
//	model::getInstance()->print(8);
//}

class object;
using single_object = Singleton<object>;

class object
{
public:
	object()
	{
		std::cout << "create object" << std::endl;
	}
	~object()
	{
		std::cout << "decreate object" << std::endl;
	}
	void print(int n)
	{
		printf("this is n = %d\r\n", n);
	}
private:
	
};


template<typename T>
void f(const T& param)
{

}

void ff(int* array)
{
	for (int i = 0; i < 10; i++)
	{
		printf("%d\r\n", array[i]);
	}
}

void fff(std::array<int, 10>& array)
{
	for (int i = 0; i < 10; i++)
	{
		printf("%d\r\n", array.at(i));
	}
}

std::unique_ptr<object> get()
{
	std::unique_ptr<object> p(new object());
	return p;
}

std::shared_ptr<object> getp()
{
	std::shared_ptr<object> p(new object());
	return p;
}

struct object_info {
	std::string id;
	int num;
	std::string name;
	int type;
};

int main()
{
//	auto p = get();

	struct object_info obj;
	obj.id = "1234";
	obj.num = 10;
	obj.name = "bird";
	obj.type = 0;
	

	std::unordered_map<int, int> map;
	map[1] = 1;
	map[5] = 5;
	map[2] = 2;
	map[4] = 4;

	for (const auto& item : map)
	{
		std::cout << item.first << ":" << item.second << std::endl;
	}

	std::unordered_map<std::string, int> m;
	m["111"] = 1;
	m["000"] = 0;
	m["555"] = 5;
	m["333"] = 3;
	m["222"] = 2;

	for (const auto& item : m)
	{
		std::cout << item.first << ":" << item.second << std::endl;
	}

	int ccc = getchar();
	return 0;

	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	v.resize(2);

	auto pp = getp();

	int cc = getchar();
	return 0;

	Singleton<object>::getInstance()->print(5);
	Singleton<object>::getInstance()->print(6);
	Singleton<object>::getInstance()->print(7);
	Singleton<object>::getInstance()->print(8);


	std::array<int, 10> a = { 1,2,3,4,5,6 };
	for (auto& item : a)
	{
		printf("%d\r\n", item);
	}

	int n = 10;
	for (auto iter = a.begin(); iter != a.end(); iter++)
	{
		*iter = n;
		n++;
	}

	for (auto& item : a)
	{
		printf("%d\r\n", item);
	}

	for (int i = 0; i < 10; i++)
	{
		a[i] = i + 20;
	}

	fff(a);
	std::string s("main.log");
	LogUtils::getInstance()->init(LOG_LEVEL::LOG_LEVEL_DEBUG, s);

	LOGD(L"number = %d", 5);
	LOGD("string is %s", "123");

	int c = getchar();

	return 0;
}